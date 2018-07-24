#include"api.hpp"
#include"chat_server.h"

namespace server
{

int ChatServer::Start(const std::string& ip,short port)
{
    // 创建一个UDP的服务器
    _sock = socket(AF_INET,SOCK_DGRAM,0);
    if(_sock < 0)
    {
        perror("socket");
        return -1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    int ret = bind(_sock,(sockaddr*)&addr,sizeof(addr));
    if(ret < 0)
    {
        perror("bind");
        return -1;
    }
    printf("Server Init OK\n");

    // 创建两个线程作为生产者和消费者
    pthread_t productor , consumer;
    pthread_create(&productor,NULL,Product,this);
    pthread_create(&consumer,NULL,Consume,this);
    // 使用join可防止主线程提前退出
    pthread_join(productor,NULL);
    pthread_join(consumer,NULL);

    return 0;
}

void* ChatServer::Product(void* arg)
{
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    // 循环的从socket中读取数据
    // 把读到的数据写到队列中
    while(1)
    {
        server->RecvMsg();

    }
    return NULL;
}

void* ChatServer::Consume(void* arg)
{
    ChatServer* server = reinterpret_cast<ChatServer*>(arg);
    // 循环的从队列中读取数据
    // 根据数据进行广播处理
    while(1)
    {
        server->BroadMsg();

    }
    return NULL;
}

int ChatServer::RecvMsg()
{
    sockaddr_in peer;
    socklen_t len = sizeof(peer);
   // 从 sock 中读取数据 
   char buf[1024 * 5] = {0};
   ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,(sockaddr*)&peer,&len);
   if(read_size < 0)
   {
       perror("recfrom");
       return -1;
   }
   // 读到json格式的字符串
   buf[read_size] = '\0';

   // 把数据放到阻塞队列中
   Context context;
   context.msg = buf;
   context.addr = peer;
   _data_pool.PushBack(context);
   return 0;
}

int ChatServer::BroadMsg()
{
    // 从队列中取出一个元素(字符串)
    Context context;
    _data_pool.PopFront(&context);
    // 对字符串进行反序列化(可能要根据应用层协议
    //          字段来决定是否要更新在线好友列表)
    Data data;
    data.StringToData(context.msg); 
    // 此处的更新好友列表，需要以sockaddr_in结构作为key进行操作
    // 但是此处在BroadCast函数内部不能直接拿到客户端的sockaddr_in
    // 为了解决此问题，让生产者把peer信息通过队列也传给消费者线程
    //          如果用户发送的上线报文，则把用户加入好友列表
    //          如果发的是下线报文，则把用户从好友列表删除
    if(data.cmd == "quit")
    {
        // 下线报文
        int ret = _online_list.erase(context.addr);
        if(ret > 0)
        {
           printf("User [%s:%d] logout OK\n",inet_ntoa(context.addr.sin_addr),ntohs(context.addr.sin_port));
        }
        else
        {
           printf("User [%s:%d] logout failed\n",inet_ntoa(context.addr.sin_addr),ntohs(context.addr.sin_port));
        }
    }
    else
    {
        // 其他值为上线报文
        auto ret =  _online_list.insert(context.addr);
        if(ret.second)
        {
            printf("User [%s:%d] login OK\n",inet_ntoa(context.addr.sin_addr),ntohs(context.addr.sin_port));
        }
    }
    // 遍历在线好友列表，把数据发送给每一个好友
    for(auto item : _online_list) 
    {
        // item 即 sockaddr_in
        SendMsg(context.msg,item);
    }
    return 0;
}

void ChatServer::SendMsg(const std::string& msg,sockaddr_in addr)
{
    ssize_t write_size = sendto(_sock,msg.data(),msg.size(),0,(sockaddr*)&addr,sizeof(addr));
    if(write_size < 0)
    {
        perror("sendto");
        return;
    }
}

}// end server


int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("Usage ./chat_server [ip] [port]\n");
        return 1;
    }
    server::ChatServer server;
    server.Start(argv[1],atoi(argv[2]));
    return 0;
}

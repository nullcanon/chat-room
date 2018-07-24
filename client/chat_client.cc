#include"chat_client.h"

namespace client
{

int ChatClient::Init(const std::string& ip,short port)
{
    // 创建一个UDP的客户端
    _sock = socket(AF_INET,SOCK_DGRAM,0);
    if(_sock < 0)
    {
        perror("socket");
        return -1;
    }

    // 客户端不需要绑定端口号
    // 为了后序的sendto,在此提前把服务器的地址结构准备好
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    _server_addr.sin_port = htons(port);
    return 0;
}


void ChatClient::SetUserInfo(const std::string& name)
{
    _name = name;
}

void ChatClient::SendMsg(const std::string& msg)
{
    // 客户端给服务器发送数据，先构造一个Data结构体
    // 调用Data结构体的序列化方法，获取到字符串
    // 再把字符串发送给服务器
    server::Data data;
    data.name = _name;
    data.msg = msg;
    data.cmd = "";
    if(data.msg == "quit")
    {
        data.cmd = "quit";
    }
    std::string data_str;
    data.DataToString(&data_str);
    sendto(_sock,data_str.data(),data_str.size(),0,(sockaddr*)&_server_addr,sizeof(_server_addr));
}

void ChatClient::RecvMsg(server::Data* data)
{
    char buf[1024*5] = {0};
    ssize_t read_size = recvfrom(_sock,buf,sizeof(buf)-1,0,NULL,NULL);
    if(read_size < 0)
    {
        perror("recvfrom");
        return;
    }
    buf[read_size] = '\0';
    data->StringToData(buf);
}
    
}// end client



/***************客户端测试代码*****************/

#include<iostream>

void Run(const char* ip,short port)
{
    // 创建client::ChatClient 对象
    // 对对象进行初始化
    // 设置用户信息
    // 从标准输入读用户输入，发送到服务器上
    // 尝试从服务器读数据，并打印到标准输出上
    client::ChatClient client;
    int ret = client.Init(ip,port);
    if(ret < 0)
    {
        printf("Init failed\n");
        return;
    }
    std::cout<<"请输入昵称："<<std::endl;
    std::string name;
    std::cin>>name;
    client.SetUserInfo(name);
    while(1)
    {
        std::cout<<"请输入发送的消息："<<std::endl;
        std::string msg;
        std::cin>>msg;
        client.SendMsg(msg);

        server::Data data;
        client.RecvMsg(&data);
        std::cout<<data.name<<":"<<data.msg<<std::endl;
    }
}

int main(int agrc,char* argv[])
{
    if(agrc != 3)
    {
        printf("Usage ./chat_client [ip] [port]\n");
        return 1;
    }
    Run(argv[1],atoi(argv[2]));
    return 0;
}

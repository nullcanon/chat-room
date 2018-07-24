#pragma once

#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<sys/types.h>

#include<string>
#include<set>

#include"block_queue.hpp"

namespace server
{

struct Setcom
{

    inline bool operator()(const sockaddr_in& lhs,const sockaddr_in& rhs)
    {
        if(lhs.sin_addr.s_addr < rhs.sin_addr.s_addr)
            return true;
        if(lhs.sin_addr.s_addr > rhs.sin_addr.s_addr)
            return false;

        // ip相等比较端口号
        if(lhs.sin_port < rhs.sin_port)
            return true;
        return false;
    }

};

// 一个请求到处理完中间过程所有的数据
struct Context
{
    sockaddr_in addr; // 客户端的地址和端口
    std::string msg;
};

class ChatServer
{
public:
    int Start(const std::string& ip,short port);
    int RecvMsg();
    int BroadMsg();
private:
    static void* Product(void* arg);
    static void* Consume(void* arg);
    void SendMsg(const std::string& msg,sockaddr_in addr);
private:
    int _sock;
    BlockingQueue<Context> _data_pool;  // 队列作为生产者消费者模型的交易场所
    std::set<sockaddr_in,Setcom> _online_list;    // 在线好友列表
};
    
}// end server

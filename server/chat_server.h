#pragma once

#include<sys/socket.h>
#include<arpa/inet.h>

#include<string>
#include<set>

#include"block_queue.hpp"

namespace server
{

class ChatServer
{
public:
    int Start();
    int RecvMsg();
    int BroadMsg();
private:
    int _sock;
    BlockingQueue<std::string> _data_pool;  // 队列作为生产者消费者模型的交易场所
    std::set<sockaddr_in> _online_list;    // 在线好友列表
};
    
}// end server

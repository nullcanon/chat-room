#pragma once
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cstdio>
#include<cstdlib>

#include<string>

#include"../server/api.hpp"

namespace client
{

// 交互客户端，不含界面
class ChatClient
{
public:
    // 初始化客户端(scoket)
    int Init(const std::string& ip,short port);

    // 设置用户信息
    void SetUserInfo(const std::string& name);

    // 向服务器发送数据
    void SendMsg(const std::string& name);

    // 从服务器获取数据
    void RecvMsg(server::Data* data);

private:
    int _sock;                  // 客户端的socket
    sockaddr_in _server_addr;   // 服务器的ip和端口号
    std::string _name;          // 用户的名字,每次发送数据带name
};

}// end client

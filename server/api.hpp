#pragma once

#include<string>

#include<jsoncpp/json/json.h>

// 这个文件定义了服务器给客户端的接口
// 即应用层协议的描述

namespace server
{

// 该结构体既是客户端给服务器发送请求的信息
// 也是服务器给客户端的信息对应的结构
struct Data
{
    std::string name;   // 用户昵称
    std::string msg;    // 用户发送消息的正文
    std::string cmd;    // 用户上下线的控制数据

    // 序列化,使用jsoncpp
    void DataToString(std::string* output)
    {
        Json::Value root;
        root["name"] = name;
        root["msg"] = msg;
        root["cmd"] = cmd;
        Json::FastWriter writer;
        *output = writer.write(root);
    }

    // 反序列化
    void StringToData(const std::string& input)
    {
        Json::Value root;
        Json::Reader reader;
        reader.parse(input,root);
        name = root["name"].asString();
        msg = root["msg"].asString();
        cmd = root["cmd"].asString();
    }
};

}// end server

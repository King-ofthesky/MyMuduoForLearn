#pragma once
#include<functional>
#include<memory>
class Buffer;
class Timestamp;
class TcpConnection;

using TcpConnectionPtr =  std::shared_ptr<TcpConnection>;
using ConnetionCallBack = std::function<void(const TcpConnectionPtr&)>;
using WriteCompleteCallBack = std::function<void(const TcpConnectionPtr&)>;
using CloseCallBack = std::function<void(const TcpConnectionPtr&)>;
//高水位的回调函数  也就是客户端发送数据过快时 到达高水位线时调用的
using HighWaterMarkCallBack = std::function<void (const TcpConnectionPtr&,size_t)>;
/*
    自己写错了  应该引用智能指针  而不是类  参数不匹配了  debug1h
*/
// using MessageCallBack = std::function<void(const TcpConnection&, Buffer*, Timestamp)>;

// typedef std::function<void (const TcpConnectionPtr&,
//                             Buffer*,
//                             Timestamp)> MessageCallback;

using MessageCallBack =  std::function<void (const TcpConnectionPtr&,Buffer*,Timestamp)> ;




//Acceptor  
//accept函数获取新链接后执行的 回调函数
using NewConnectionCallback = std::function<void(int sockfd,const InetAddress &inetaddress)>;
#pragma once

#include"noncopyable.h"
#include"InetAddress.h"

class InetAddress;
class Socket:noncopyable{

public:
    explicit Socket(int fd)
        :sockfd_(fd)
    {
        
    }

    ~Socket();
    int fd()const{return sockfd_;};
    void bindInetAddress(const InetAddress inetaddress);
    void listen();
    int accept(InetAddress *peeraddr);


    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};
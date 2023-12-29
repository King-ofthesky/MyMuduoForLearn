#include"Socket.h"
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/tcp.h>
#include"Logger.h"
Socket::~Socket()
{

}

void Socket::bindInetAddress(const InetAddress inetaddress)  //绑定 socket  目前支持ipv4
{
    if(0!=::bind(sockfd_,(struct sockaddr *)inetaddress.getSockaddr(),sizeof(sockaddr_in)))
    {
        //Log()
    }
}

void Socket::listen()
{
    ::listen(sockfd_,0);
}
int Socket::accept(InetAddress *peeraddr)
{
    sockaddr_in addr;
    // union ipaddr
    // {
    //    sockaddr_in addr4;
    //    sockaddr_in6 addr6;
    // };

    // ipaddr addr;
    socklen_t len;
    bzero(&addr,sizeof(addr));
    int connfd = ::accept(sockfd_,(sockaddr*)&addr,&len);
    if(connfd>=0){
        peeraddr->setSockaddr(addr);
    }
    return connfd;
}
//关闭写端 触发EPOLLHUP
void Socket::shutdownWrite()  
{
    if(::shutdown(sockfd_,SHUT_WR)){
            //ERROR()
    }
}

void Socket::setTcpNoDelay(bool on)
{//协议级别
    int opt = on ? 1 :0;
    ::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(opt));
}

void Socket::setReuseAddr(bool on)
{//socket级别
    int opt = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}

void Socket::setReusePort(bool on)
{
    int opt = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
}

void Socket::setKeepAlive(bool on)
{
    int opt = on ? 1 :0;
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
}


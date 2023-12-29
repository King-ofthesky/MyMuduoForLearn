#include"Acceptor.h"
#include"Logger.h"

static int createNonblocking()    //创建一个非阻塞fd
{
    int sockfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
    if(sockfd<0){
        //LOG_FATAL()
    }else{
        return sockfd;
    }
    return -1;
}
//
Acceptor::Acceptor(EventLoop *eventloop,const InetAddress &inetaddress,bool reuseport)
            :loop_(eventloop),
            acceptSocket_(createNonblocking()),
            acceptChannel_(loop_,acceptSocket_.fd()),
            listenning_(true)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindInetAddress(inetaddress);//bind  函数
    //TCPServer->start()
    acceptChannel_.setReadCallBack(std::bind(&Acceptor::handleRead,this));//类的成员函数 需要一个类对象的指针
}

Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen()
{
    listenning_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}
void Acceptor::handleRead() //获取新用户连接
{
    InetAddress peerAddr;//客户端地址
    int connfd = acceptSocket_.accept(&peerAddr);//通过accept函数 获取一个连接的客户端ip和port和fd  
    if(connfd >=0)  //有链接
    { 
        if(newConnectiomCallback_){   //当获取一个链接有 尝试执行回调函数
            newConnectiomCallback_(connfd,peerAddr);//执行建立新连接的回调函数
        }else{
            ::close(connfd);//没有回调函数直接结束
        }
    }
    else{//
        //LOG_ERROR()
        if(errno = EMFILE ){//fd到上限
            //LOG_ERROR
        }
    }
}
#include"Channel.h"

#include<sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN |EPOLLPRI;
const int Channel::KwriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *Loop,int fd):
loop_(Loop),
fd_(fd),
events_(0),
revents_(0),
index_(-1),
tied_(-1),
eventHandling_(false),
addedToEventLoop_(false){

}
//channel析构函数  在一个事件循环中有多个channel
//channel属于某个eventloop 
Channel::~Channel(){
    //断言判断 是否被所属的loop析构
    assert(!eventHandling_);
    assert(!addedToEventLoop_);
    if(loop_){

    }
}

//建立新连接时调用
void Channel::tie(const std::shared_ptr<void>& shard_p){
    //tie是weak 弱智能指针  shard_p是
    tie_ = shard_p;
    //绑定后tied 置为true;
    tied_ = true;
}

/*
当改变channel所表示的fd的events事件后   update负责在poller中更改fd对应的事件 epoll_ctl
EventLoop-> ChannelList->Channel
EventLoop->looper
        Channel 本身无法向poller中注册事件
        需要通过指针调用looper中的函数 向looper注册fd的events事件
*/              
void Channel::update(){
    addedToEventLoop_ = true;
    //add...
    loop_->updateChannel(this);
}
//EventLoop-> ChannelList->Channel
//在channel所属的eventloop中 删除channel
void Channel::remove(){
    assert(Channel::isNoneEvent());
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime){
    std::shared_ptr<void> guard;
    if(tied_)
    {
        //尝试把弱智能指针提示为强智能指针
        //失败就不执行
        /*
        智能指针复习 : unique_ptr  只能指向一个对象，即当它指向其他对象时，之前所指向的对象会被摧毁 可以通过移动语义 转移所有权
        shard_ptr   有个引用计数  为0时自动析构
        weak_ptrweak_ptr可以从一个shared_ptr或者另一个weak_ptr对象构造，获得资源的观测权。但weak_ptr没有共享资源，
        它的构造不会引起指针引用计数的增加
        */
        guard  =tie_.lock();
        if(guard)
        {
            Channel::handleEventWithGuard(receiveTime);
        }
    }else 
    {
        Channel::handleEventWithGuard(receiveTime);
    }
}

//根据接受的事件执行相应的回调操作
//通过revents_ 控制
void Channel::handleEventWithGuard(Timestamp receiveTime){
    //与操作 EPOLLHUP 有一端断开连接   发生异常了 执行关闭回调函数
    //Logger::LOG_INFO("Channel::handleEventWithGuard :%d",revents_);
    
    if(revents_ & EPOLLHUP && !(revents_ & EPOLLIN)){
        //
        if(closeCallBack_){
            closeCallBack_();
        }
    }

    //发生异常了就执行 异常处理的回调函数
    if(revents_ &EPOLLERR ){
        if(errorCallBack_){
            errorCallBack_();
        }
    }

    //可读事件发生 或者紧急的可读事件
    if(revents_ & (EPOLLIN | EPOLLPRI)){
        if(readEventCallBack_){
            readEventCallBack_(receiveTime);
        }
    }

    //可写事件发生
    if(revents_ & EPOLLOUT){
        if(writeEventCallBack_){
            writeEventCallBack_();
        }
    }

}

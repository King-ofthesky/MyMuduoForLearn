#pragma once 

#include<vector>
#include<sys/epoll.h>
#include<stdlib.h>
#include<string.h>

#include"Poller.h"
#include"Channel.h"
#include"Logger.h"
class EpollPoller:public  Poller
{

    /*
    epoll使用  
    epoll_creat
    epoll_ctl   add/mod/del
    epoll_wait

    
    
    
    */
public:


EpollPoller(EventLoop* loop);
~EpollPoller()override; //基类为虚函数


//基类抽象方法
Timestamp poll(int timeOutMs,ChannelList *activeChannelList) override;

void updateChannel(Channel *channel) override;

void removeChannel(Channel *channel) override;



protected:


private:
using Eventlist = std::vector<epoll_event> ;

int epoll_fd_;
Eventlist eventlist_;
static const int kInitEventListSize = 16;//vector初始长度
//填写活跃连接
void fillActiveChannels(int numevents,ChannelList* activechannellist)const;
//更新channel通道

void update(int operation,Channel* channel);



};
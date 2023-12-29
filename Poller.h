#pragma once
#include<map>
#include<vector>
#include<unordered_map>
#include<iterator>

#include"Channel.h"
#include"noncopyable.h"
//抽象类 抽象poll和epoll
//多路事件分发器  io复用模块

class Poller:noncopyable{
    public:
    using ChannelList = std::vector<Channel*>;
    ChannelList channels_;

    Poller(EventLoop *loop);

    virtual ~Poller();

    //给io复用保留统一的接口
    virtual Timestamp poll(int timeOutMs,ChannelList *activeChannelList) = 0;

    virtual void updateChannel(Channel *channel) = 0;

    virtual void removeChannel(Channel *channel) = 0;
    //判断channel 是否在当前 Poller中
    bool hasChannel(Channel *channel) const;

    static Poller* newDefaultPoller(EventLoop *loop);


    protected:
    using   ChannelMap = std::map<int,Channel*>;
    //int表示 fd，     fd属于一个channel
    //typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channelmap_;

    private:

    EventLoop *ownerloop_;
};
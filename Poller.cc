#include"Poller.h"

Poller::Poller(EventLoop *loop)
    :ownerloop_(loop)
{

}

bool Poller::hasChannel(Channel *channel) const
{  

    auto it= channelmap_.find( channel->fd());
    //std::map<int,Channel*>::iterator it;
    if(it != channelmap_.end()){
        return it !=channelmap_.end() && it->second == channel;
    } 
    return true;
}

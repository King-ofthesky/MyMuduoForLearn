#include"EpollPoller.h"
#include<errno.h>
#include"Channel.h"

const int knew = -1;
const int Kadded = 1;
const int kdeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop):
    Poller(loop),
    epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)),//创建epoll
    eventlist_(kInitEventListSize)
{
    if(epoll_fd_ <0){  //创建失败
        exit(-1);
    }   
}

EpollPoller::~EpollPoller(){
    // ::close(epoll_fd_);
}


//基类抽象方法
Timestamp EpollPoller::poll(int timeOutMs,ChannelList *activeChannelList) { // epoll_wait
    // LOG_ERROR

    // fd  数组起始地址  数组长度  timeout
    int numEvents = ::epoll_wait(epoll_fd_,&(*eventlist_.begin()),static_cast<int>(eventlist_.size()),timeOutMs);  
    /*
    这段代码分为两个部分：
    eventlist_.begin()：eventlist_ 是一个 std::vector<> 类型的容器，begin() 是该容器提供的成员函数，用于返回指向容器中第一个元素的迭代器。
    此处使用 eventlist_.begin() 可以获取到指向 eventlist_ 容器中第一个元素的迭代器。&(*eventlist_.begin())：*eventlist_.begin() 解引用了 
    eventlist_.begin() 返回的迭代器，即获取到了 eventlist_.begin() 迭代器指向的元素；而 & 操作符则取得该元素的地址。整个表达式的作用是获取到
    eventlist_ 容器中第一个元素的地址。综合起来，这段代码的目的是获取 eventlist_ 容器中第一个元素的地址。这样可以通过这个地址来访问或修改 
    eventlist_ 容器中的第一个元素。


    这段代码分为两个部分：
    static_cast<int>(eventlist_.size())
    eventlist_.size()：eventlist_ 是一个 std::vector<> 类型的容器，size() 是该容器提供的成员函数，用于返回容器中元素的数量。这行代码的作用
    是获取 eventlist_ 容器中的元素数量。static_cast<int>(eventlist_.size())：static_cast<int> 是 C++ 中的类型转换操作符，它将括号内的表达式
    转换为指定的类型。在此代码中，它将 eventlist_.size() 的结果转换为 int 类型。由于 size() 返回的是无符号整数类型 std::size_t，使用 
    static_cast<int> 可以将其转换为有符号整数 int，以便进行后续的操作。综合起来，这段代码的目的是获取 eventlist_ 容器中元素的数量，并将其转换为
    有符号整数类型 int。这样可以方便地在后续的代码中使用该数量进行各种计算和逻辑判断。
    */

    int saveerr = errno;
    if(numEvents>0){
        // LOG_INFO
        fillActiveChannels(numEvents,activeChannelList);
        if(numEvents == eventlist_.size()){
            eventlist_.resize(numEvents * 2);
        }

    }else if(numEvents == 0){//没事件发生
        //LOG_DEBUG

    }else{
        //判断错误情况
        if(saveerr != EINTR){ // 外部中断
            errno = saveerr;

        }
    }
    return Timestamp::now();
}
//对应eoll_ctl
void EpollPoller::updateChannel(Channel *channel) {
    const int index = channel->index();

    if(index == knew || index == kdeleted){
        int fd = channel->fd();
        //LOG_INFO()

        if(index == knew){ // 刚创建的 channel
            channels_[fd] =channel; //向vector 中添加fd
        }else{   // index == deleted
            //assert(false);

        }
        channel->setIndex(Kadded);  //刚注册的事件  添加到epoll
        update(EPOLL_CTL_ADD,channel);

    }else{


        int fd = channel->fd();
        (void)fd;
        if(channel->isNoneEvent()){  //对任何事件不感兴趣
            update(EPOLL_CTL_DEL,channel);   //从epoll中删除
            channel->setIndex(kdeleted);
        }else{
            update(EPOLL_CTL_MOD,channel);
        }
    }
    

    
}

void EpollPoller::removeChannel(Channel *channel) {  //把channel 从epoll中删除
    int fd = channel->fd();
    int index = channel->index();

    //根据fd获取到channel_的迭代器  
    auto it = std::find_if(channels_.begin(), channels_.end(), 
        [fd](const Channel* ch) {   //lambda 表达式 判断fd是否是属于当前channel
         return ch->fd() == fd;
        }   
    );

    //删除
    if(it != channels_.end())
    {
        channels_.erase(it);//
    }

    if(index == Kadded){
        update(EPOLL_CTL_DEL,channel); // 从epoll中删除
    
    }

    channel->setIndex(knew);
}




//填写活跃连接
void EpollPoller::fillActiveChannels(int numevents,ChannelList* activechannellist)const{
    for(int i = 0;i < numevents;++i){
        Channel *channel = static_cast<Channel*>(eventlist_[i].data.ptr);//从epoll_event中得到 
        channel->set_revents(eventlist_[i].events);
        activechannellist->push_back(channel);
    }
}
//更新channel通道
void EpollPoller::update(int operation,Channel* channel){
    struct epoll_event  event;
    memset(&event,0,sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if(::epoll_ctl(epoll_fd_,operation,fd,&event)<0){
        if(operation == EPOLL_CTL_DEL){
            // LOG_ERROR
        }else{
            // LOG_ERROR
        }
    }
}


#include"EventLoopThread.h"



EventLoopThread::EventLoopThread(const ThreadInitCallBack &cb ,const std::string &name)
                :loop_(nullptr)
                ,exiting_(false)
                ,thread_(std::bind(&EventLoopThread::threadFunc,this))
                ,mutex_()
                ,cond_()
                ,callback_(cb)
{

}



EventLoopThread::~EventLoopThread(){
    exiting_ = true;
    /**/
    if(loop_ != nullptr){
        loop_->quit();
        thread_.join();
    }
}


EventLoop* EventLoopThread::startLoop(){
    /*
    本质上是调用thraed()方法 创建一个线程  线程函数为threadFunc  每次start都是启动一个新的线程
    */
    thread_.start();//启动线程

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        //从threadFunc中获取lopp
        while(loop == nullptr){
             cond_.wait(lock);
        }
        loop = loop_;//在threadFunc中创建的loop变量
    }
    return loop;
}
void EventLoopThread::threadFunc(){
    EventLoop loop;  //一个独立的loop  对应上面的线程
    if(callback_)
    {
        callback_(&loop);
    }
    

   {
    std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();//通知一个等待在mutex上的
   }
   loop.loop();//在此处进行事件循环  如果执行下去则说明循环结束
   std::unique_lock<std::mutex> lock(mutex_);
   loop_=nullptr;
}
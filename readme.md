one loop per thread 模型
 
Eventloopthread 是对线程封装的抽象  创建这个对象 等价与创建一个新线程 和eventloop

EventloopthreadPoll 是线程池 管理Eventloopthread, 可以根据cpu核心数量自动创建线程池


一个eventloop中有一个poller  和多个channel

一个poller中监听多个channel、

cannel中封装了 sockfd   和 感兴趣的 event, 包括 EPOLLIN  EPOLLOUT  EPOLLERR事件

对外放编程的类包括TcpServer TcpConnection


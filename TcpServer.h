//
// Created by erick on 1/1/19.
//

#ifndef FINALTASK_TCPSERVER_H
#define FINALTASK_TCPSERVER_H


#include "TcpSocket.h"
#include "user_data.h"
#include "ThreadPool.h"
#include "ThreadSafeQueue.h"

#include <sys/epoll.h>
#include <netinet/ip.h>
#include <memory>
#include <vector>
#include <fstream>
#include <string>

/*
 *  封装一个TcpServer类型
 */

class TcpServer {
public:
    explicit TcpServer(int threadNum = DEFAULT_THREAD_NUM,
                       int maxWaiter = MAX_WAITER_NUM);

    ~TcpServer();

    bool listen(int _port = 8001);



    // 在这里进入事件的主循环，可以自行设置时间
    bool startService(int timeout = -1);

protected:
    // 非阻塞模式设置
    void setnonblocking(int fd); 

    // 新的连接
    void newConnection();

    // 用户离开
    void clientLeft(int fd);

    // 用户发来消息
    void receiveClientMsg(int fd, std::string str);

    static void setStop(int sig);

    struct sockaddr_in serv_addr; // 服务器信息
    std::unique_ptr<TcpSocket> m_tcpSocket;

    epoll_event m_epollEvents[MAX_EVENTS];  // epoll事件队列
    int m_epfd;   // epoll的fd
    int m_listen_sockfd;
    
    std::mutex m_mtxFileStream;  // 文件锁，用于互斥文件
    std::condition_variable m_condFile;
    
    std::mutex m_mtxNewConn;

    std::unique_ptr<ThreadPool> m_upThreadPool;
};


#endif //FINALTASK_TCPSERVER_H

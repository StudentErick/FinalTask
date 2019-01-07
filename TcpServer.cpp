//
// Created by erick on 1/1/19.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <functional>
#include <signal.h>
#include <fcntl.h>
#include "TcpServer.h"

TcpServer::TcpServer(int threadNum, int maxWaiter) {
    m_tcpSocket = std::make_unique<TcpSocket>(maxWaiter);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1024);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_listen_sockfd = m_tcpSocket->getSockFD();
    m_epfd = epoll_create1(0);

    m_upThreadPool = std::make_unique<ThreadPool>(threadNum);

    clientCnt = 0;
    msgCnt = 0;

    serverStop = false;

    signal(SIGINT, setStop);

    fileStream.open("./log.txt", std::ios::app);
}

bool TcpServer::listen(int _port) {
    return m_tcpSocket->bindPort(_port) && m_tcpSocket->listenOn();
}

TcpServer::~TcpServer() {
  fileStream.close();
}

bool TcpServer::startService(int timeout) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.data.fd = m_listen_sockfd;
    ev.events = EPOLLIN | EPOLLET;    
    if (epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listen_sockfd, &ev) < 0) {
        return false;
    }

    setnonblocking(m_listen_sockfd);

    std::cout << "service started, 'Ctrl+C' to end server...\n";

    // 服务器的监听循环
    while (!serverStop) {
        std::time_t res = std::time(nullptr);
        std::string str(std::ctime(&res));

        int nfds = epoll_wait(m_epfd, m_epollEvents, MAX_EVENTS, timeout);

        if (nfds < 0) {
            return false;
        }
        for (int i = 0; i < nfds; ++i) {
            if (m_epollEvents[i].data.fd == m_listen_sockfd) {
                // 新来的连接
                //std::cout << "NEW\n";
                m_upThreadPool->enqueue(std::bind(&TcpServer::newConnection, this));
                //newConnection();
            } else if (m_epollEvents[i].events & EPOLLRDHUP) {
                // 客户端主动断开连接
                int fd = m_epollEvents[i].data.fd;
                // std::cout << "LEFT: " << fd << std::endl;
                m_upThreadPool->enqueue(std::bind(&TcpServer::clientLeft, this, fd));
            } else if (m_epollEvents[i].events & EPOLLIN) {
                // 客户端发来新的消息
                //std::cout << "---------------MSG\n";
                int fd = m_epollEvents[i].data.fd;
                
                time_t rawtime;
                struct tm *timeinfo;
                char buffer[80];
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
                std::string str(buffer);
                
                  
                
                m_upThreadPool->enqueue(std::bind(&TcpServer::receiveClientMsg, this, fd, str));
            }
        }
    }

    return true;
}

void TcpServer::newConnection() {
    sockaddr_in client_address;
    bzero(&client_address, sizeof(client_address));
    socklen_t client_addrlen = sizeof(client_address);
    int connfd = -1;
    //m_mtxNewConn.lock();
    while ((connfd = accept(m_listen_sockfd, (struct sockaddr *) &client_address, &client_addrlen)) > 0) {
        /*if (connfd < 0) {
            std::cout << "newConnection() accept() failed\n";
            return;
        }*/
        // 设置非阻塞模式！！！！！ mark一下
        setnonblocking(connfd);
        epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = connfd;
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        int ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, connfd, &ev);
        if (ret < 0) {
            std::cout << "newConnection() epoll_ctl() error\n";
        } else {
            ++clientCnt;
            std::cout << "register a new client\n";
        }
    }
    //m_mtxNewConn.unlock();
}

void TcpServer::clientLeft(int fd) {
    if (epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, m_epollEvents) < 0) {
        std::cout << "clientLeft() epoll_ctl() error\n";
    } else {
        std::cout << "a client left\n";
    }
}

void TcpServer::receiveClientMsg(int fd, std::string str) {
    char buf[MAX_CHAR_BUFFER];
    memset(buf, 0, MAX_CHAR_BUFFER);
    m_mtxFileStream.lock();
    //std::unique_lock<std::mutex> lock(m_mtxFileStream);
    while (recv(fd, buf, MAX_CHAR_BUFFER, MSG_DONTWAIT) > 0) {
      /*
         *  在这里处理实际的数据
         *  先测试输出
         */
        //m_mtxFileStream.lock();
        /*
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
        std::string str(buffer);*/
        //m_mtxFileStream.lock();
        
        std::string s(buf);
        //s += str + " " + std::string(buf); 
	      fileStream << s;
        std::cout << s;
        //m_mtxFileStream.unlock();    
        memset(buf, 0, MAX_CHAR_BUFFER);
    }
    ++msgCnt;
    m_mtxFileStream.unlock();
}

void TcpServer::setStop(int sig) {
    if (sig == SIGINT) {
        serverStop = true;
    }
    std::cout << "clientCnt = " << clientCnt << std::endl;
    std::cout << "msgCnt = " << msgCnt << std::endl;

    clientCnt = 0;
    msgCnt = 0;
    fileStream.close();
    std::cout << "server stop...\n";
    exit(0);
}

void TcpServer::setnonblocking(int fd) {
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, F_SETFL, new_option);
}

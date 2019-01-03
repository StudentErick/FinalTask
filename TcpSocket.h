//
// Created by erick on 1/1/19.
//

#ifndef FINALTASK_TCPSOCKET_H
#define FINALTASK_TCPSOCKET_H


#include <string>
#include <netinet/ip.h>

/*
 * 封装socket的基本操作
 */

class TcpSocket {
public:
    explicit TcpSocket(int _maxWaiter = 5);

    ~TcpSocket();

    bool bindPort(int _port);

    bool setServerInfo(const std::string &ip, int _port);

    bool listenOn();

    bool connectToHost();

    unsigned int getPort() const;

    inline int getSockFD() const { return m_sockfd; }

private:
    int m_sockfd{ -1 };
    struct sockaddr_in serv_addr;  // 客户端的socket表示远程服务器信息，服务器的socket表示自己的信息
    int m_maxWaiter{ 5 };
};


#endif //FINALTASK_TCPSOCKET_H

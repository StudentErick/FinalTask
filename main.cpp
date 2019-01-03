#include <iostream>
#include "TcpServer.h"
#include "user_data.h"

int main() {
    clientCnt = 0;
    msgCnt = 0;
    TcpServer tcpServer;
    if (!tcpServer.listen(8001)) {
        std::cout << "listen error\n";
        return 0;
    }
    if (!tcpServer.startService()) {
        std::cout << "startService error\n";
        return 0;
    }
    return 0;
}
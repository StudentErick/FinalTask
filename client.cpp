#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <strings.h>
#include <arpa/inet.h>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>
#include <functional>
#include <string>
#include <string.h>

const int MAX_CLIENT_NUM = 20;
const int MIN_PORT = 1024;
const int MAX_PORT = 65535;
const int MAX_THREAD_NUMS = 6;
const int THREAD_NUM = 4;
const int THREAD_CLIENT_NUM = 250;
const int TIME = 10;
const int TIME_VAL = 100;
const int MSG_NUM = 50;
const char data[THREAD_CLIENT_NUM][5] = {"aaaa", "bbbb", "cccc", 
                                         "dddd", "eeee", "ffff", 
                                         "gggg", "hhhh", "iiii", "kkkk"};

int create_client(const char *ip, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr) < 0) {
        return -1;
    }

    if (connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        return -1;
    }

    return fd;
}

int main(int argc, char *argv[]) {

    const char *ip = "127.0.0.1";
    int port = 8001;

    std::atomic_int msgCnt, clientCnt;
    msgCnt = 0;
    clientCnt = 0;

    std::vector<std::thread> threads;
    std::cout << "Thread num = " << THREAD_NUM << std::endl;
    for (int i = 0; i < THREAD_NUM; ++i) {
        threads.emplace_back([&msgCnt, &ip, &port, &clientCnt, i]() {
            std::vector<int> fdVec(THREAD_CLIENT_NUM);
            // 创建客户端
            for (int j = 0; j < THREAD_CLIENT_NUM; ++j) {
                fdVec[j] = create_client(ip, port);
                ++clientCnt;
            }
            
            
            // 每个客户端发送指定的消息个数
            for (int j = 0; j < MSG_NUM; ++j) {
              for (int k = 0; k < THREAD_CLIENT_NUM; ++k) {
                std::string msg;
                msg += "Thread num: " + std::to_string(i) + 
                       ", fd num: " + std::to_string(fdVec[k]) +
                       ", msg num: " + std::to_string(j) + 
                       ", msg: " + std::string(data[k]) + "\n";
                if (send(fdVec[j], msg.data(), msg.length(), 0) > 0) {
                  std::cout << msg;
                  ++msgCnt;
                } else {
                  std::cout << "send error\n";
                }
              }
              std::this_thread::sleep_for(std::chrono::milliseconds(TIME_VAL));
            }
        });
    }

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    std::cout << "clientCnt = " << clientCnt << std::endl;
    std::cout << "msgCnt = " << msgCnt << std::endl;

    return 0;
}

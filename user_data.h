//
// Created by erick on 1/1/19.
//

#ifndef FINALTASK_USER_DATA_H
#define FINALTASK_USER_DATA_H

#include <fstream>

const unsigned int MAX_PORT = 65535;
const unsigned int MIN_PORT = 1024;
const unsigned int MAX_EVENTS = 5000;
const unsigned int MAX_CHAR_BUFFER = 1000;
const unsigned int MAX_WAITER_NUM = 1000;
const unsigned int DEFAULT_THREAD_NUM = 4;

static int clientCnt = 0;
static int msgCnt = 0;
static bool serverStop = false;

static std::fstream fileStream;

#endif //FINALTASK_USER_DATA_H

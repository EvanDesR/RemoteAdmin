
#ifndef main_h
#define main_h
#include <WinSock2.h>
#include <mutex>
const timeval msgHandlerTimeout; // 50ms timeout for recv
extern fd_set serverListeningSocketAsSet;
extern std::mutex vectorAndMapAccessMutex;

#endif // !STRUCTDEFINITIONS_H

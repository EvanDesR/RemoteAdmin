#ifndef SERVERMANAGEMENT_H
#define SERVERMANAGEMENT_H
#include <winsock2.h> // For SOCKET type, if needed

void forwardToAll(char buff[4096], int buffSize, SOCKET& fromsocket);
void sendMsg(clientSocketInformation destinationSocket, char* buff);
int howManySocketsInListOfClientSocketInformation();
void incomingHandler(SOCKET &servSocket);
void msgRecv();
//void sendMsg(clientSocketInformation& destinationSocket, char* buff);
#endif SERVERMANAGEMENT_H
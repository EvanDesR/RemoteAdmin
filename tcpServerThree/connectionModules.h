#ifndef CONNECTIONMODULES_H
#define CONNECTIONMODULES_H
//#include "sharedObjects.h"

int cmdModule(SOCKET& clientPassedToCmd, std::string passedString); //pass by ref
void forwardToAll(char buff[4096], int buffSize, SOCKET& fromsocket); //exists strictly for testing purposes, this will be removed and placed in connectionModules before release
void changeAlias(SOCKET& targetSocket); //pass by ref


#endif
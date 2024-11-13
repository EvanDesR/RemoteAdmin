
// Contains clientSocketInformation struct definition, messageInformation definition
//It also includes an extern vectorOfClientSocketInformation

#ifndef SHAREDOBJECTS_H
#define SHAREDOBJECTS_h
//#include "sharedObjects.h"
#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <sys/types.h>
#include <unordered_map>
#include <array>
#pragma comment(lib, "ws2_32.lib") //winsock library file

struct clientSocketInformation; 

struct messageInformation
{
	char serverSent[4096];
	char replyFromClient[4096];
	short int msgID;
	bool waiting;
	bool messageSentByServer;
	bool simpleTCPFromCient;
	

	messageInformation(char* serverSendingMsg, int sizeOfArray,bool isServerSend)
	{
		waiting = 1;
		for (int i = 0; i < sizeOfArray; i++)
		{
			serverSent[i] = serverSendingMsg[i];
		}
		if (isServerSend == 1)
			messageSentByServer = TRUE;
		else
		{
			messageSentByServer = FALSE;
		}
	}
	messageInformation()
	{
		bool waiting = true;
	}
	messageInformation(bool yes)
	{
		bool waiting = true;
		bool messageSentByServer = true;
	}



};

struct clientSocketInformation
{
	std::string alias;
	FD_SET socketOfClientAsSet;
	SOCKET socketOfClient{};
	bool isReadable(FD_SET);
	bool isDisconnected(FD_SET, bool, int);
	void closeConnection(int);
	std::vector<messageInformation> messages;
	void logClientReply(clientSocketInformation& socketForKey, char serverSendingMsg[4096]);
	void logSent(clientSocketInformation&, char storeThis[4096]);
	//Constructor
	clientSocketInformation(SOCKET socketOfClientInput)
	{
		FD_ZERO(&socketOfClientAsSet); //clear fd_set which will hold a single socket 
		socketOfClient = socketOfClientInput;
		FD_SET(socketOfClientInput, &socketOfClientAsSet);
		std::cout << "testing \n";

	}
	clientSocketInformation()
	{
	}
};



extern std::vector<clientSocketInformation> vectorOfClientSocketInformation; //it is actually defined in maincpp

extern std::unordered_map<SOCKET, clientSocketInformation> hashMap;



#endif // !1

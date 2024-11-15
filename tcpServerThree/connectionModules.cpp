#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <array>
#pragma comment(lib, "ws2_32.lib") //winsock library file
#include <string>
#include <iostream>
#include <regex>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include "main.h"
#include "sharedObjects.h"
#include "SERVERMANAGEMENT.h"


int cmdModule(SOCKET& clientPassedToCmd, std::string passedString) //pass by ref
{
	
	char commandInput[4096] = {0};

    std::size_t copyLength = passedString.length(); // Copy the entire length of passedString
    passedString.copy(commandInput, copyLength);
    commandInput[copyLength] = '\0'; //null termination after the copied length
   // std::cout << "sending " << commandInput;
    char* sendbuf = commandInput;
    send(hashMap[clientPassedToCmd].socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
	SOCKET lazySolution = clientPassedToCmd;
	int ohGodThisIsABadIdea = 0;
	for (auto incre = passedString.begin(); incre < passedString.end(); incre++) //going out  of bounds. Probably problem with .length()
	{

		commandInput[ohGodThisIsABadIdea] = passedString[ohGodThisIsABadIdea];
		std::cout << commandInput[ohGodThisIsABadIdea]<<" ";
		ohGodThisIsABadIdea++;
	}
	std::cout << "exited loop \n";
	hashMap[clientPassedToCmd].log(hashMap[clientPassedToCmd], commandInput);
	hashMap[clientPassedToCmd].messages.back().waiting = TRUE;
	hashMap[clientPassedToCmd].messages.back().messageSentByServer = TRUE;
  //  send(clientObjectPassedToCmd.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
    return 4;
}

void forwardToAll(char buff[4096], int buffSize, SOCKET& fromsocket) //exists strictly for testing purposes.
{
	char* sendbuf = buff;

	for(auto iterator = hashMap.begin(), end = hashMap.end(); iterator != end; iterator++)
	{
		
		if ((iterator->first) != fromsocket) //We can use the key value (first), for the checking because the key value is the sockFD as int (sockfd is actually uint64)
		{
			std::cout << "frwrding msg from socket " << fromsocket << " to " << (iterator->first) << "\n";
			send((iterator->first), sendbuf, (int)strlen(sendbuf), 0);
		}
	}

}


void changeAlias(SOCKET& targetSocket) //pass by ref
{

	std::getline(std::cin, hashMap[targetSocket].alias);
	return;
}
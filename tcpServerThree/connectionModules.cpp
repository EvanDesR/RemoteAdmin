#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
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
    char commandInput[4096] = { 0 }; // Initialize the entire array to null characters
    std::size_t copyLength = passedString.length(); // Copy the entire length of passedString
    passedString.copy(commandInput, copyLength);
    commandInput[copyLength] = '\0'; //null termination after the copied length
    std::cout << "sending " << commandInput;
    char* sendbuf = commandInput;
    send(hashMap[clientPassedToCmd].socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
	//clientObjectPassedToCmd.logClientReply(clientObjectPassedToCmd, commandInput); 

  //  send(clientObjectPassedToCmd.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
    return 4;
}

void forwardToAll(char buff[4096], int buffSize, SOCKET& fromsocket) //exists strictly for testing purposes.
{
	int socketIncrementer = 0; //0 owrked
	int amountOfSocketsInvectorOfClientSocketInformation = vectorOfClientSocketInformation.size();
	char* sendbuf = buff;

	while (amountOfSocketsInvectorOfClientSocketInformation > socketIncrementer)
	{
		if (vectorOfClientSocketInformation[socketIncrementer].socketOfClient != fromsocket)
		{
			std::cout << "frwrding msg from socket " << fromsocket << " to socket: " << vectorOfClientSocketInformation[socketIncrementer].socketOfClient << "\n";
			send(vectorOfClientSocketInformation[socketIncrementer].socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
		}
		socketIncrementer++;
	}

}


void changeAlias(SOCKET& targetSocket) //pass by ref
{

	std::getline(std::cin, hashMap[targetSocket].alias);
	return;
}
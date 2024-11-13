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
#include <unordered_map>
#include <thread>
#include <mutex>
#pragma comment(lib, "ws2_32.lib") //winsock library file
const timeval msgHandlerTimeout = { 0,  10 }; // 50ms timeout for recv
struct addrinfo hints, * server; 
struct fd_set serverListeningSocketAsSet;
#include "sharedObjects.h"
#include "serverManagement.h"
#include "connectionModules.h"
#include "inputAPI.h"
std::unordered_map<SOCKET, clientSocketInformation> hashMap;
std::vector<clientSocketInformation> vectorOfClientSocketInformation; //
std::mutex vectorAndMapAccessMutex;


SOCKET servSetup()
{
	//Initialize winsock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
		std::cerr << "Cant start winsock error: " << wsResult << "\n";


	memset(&hints, 0, sizeof(hints)); //ensures hints struct is clear.

	hints.ai_family = AF_INET; //fill hints struct with information clarifying our use of getaddrinfo
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	std::cout << getaddrinfo("localhost", "40343", &hints, &server); //uses the provided information to populate the server addrinfo struct

	SOCKET serverListeningSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol); //create socket object using server addrinfo struct

	int bindStatus = bind(serverListeningSocket, server->ai_addr, server->ai_addrlen); //bind socket object to machine
	if (bindStatus != -1) //bindstatus check, error would return -1
	{
		std::cout << "server is binded to 40343" << "\n";
	}

	listen(serverListeningSocket, '120');
	return serverListeningSocket;
}





void main()
{
	SOCKET servSocket = servSetup(); //Creates socket object and binds it to port
	
	
	std::thread handlerThread (incomingHandler,std::ref( servSocket));
	std::thread cliParseThread(cliParser);
	
	
	while (true) // Threads are children of main(), so they will be aborted if end of main() is reached
	{

	}
}




//todo

//rework logReply, as currently all calls to logReply refererence the objects fields in a verbose,
// its call syntax is complicated by it being a member of messageInformation, meaning every call to it must be via a messageObejct



//Utilize more .cpp files for better organization, and clearer distinction between the seperate operations of this program.

//CLI Parser to allow interpretation, and execution of modules based on the server admin CLI input.

//Further expand on module concept






//DONE

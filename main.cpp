#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "msgOperations.cpp"

#pragma comment(lib, "ws2_32.lib") //winsock library file
//structs initializations
struct addrinfo hints,*serv; //ipv4 only
struct fd_set listOfClients,referenceServ,testMe, sendList;
int counterForIntakeOfNewClients{};
const timeval msgHandlerTimeout = { 0,  50 }; // 50ms timeout for recv



/*
functions

incomingConnectHandler: Checks if the server programs listening port, has a queue of clients wanting to establish a socket connection.
						Accepts the connection, and adds the resulting sockfd to the file descriptor set listOfClients.Also it creates an object using the same sockfd.
						It adds that object into the listOfClientsInformation array.




						The listOfClients functions as simply that, a file descriptor set with every client sockfd. Winsock fds canot be
						stored in arrays, which necessitates the use of file descriptor set. A limitation of this is the rigidity of fd_set.




						The listOfClientsInformation is an array of objects. 
						At a high level, by putting each client into its own fd_set, it enables checking socket status at the individual socket level
						rather than at the group level. Using objects also opens the door for future advanced features like a permission system, and file transfer.

			

						clientsInformation OBJECT
						Each socket will be held in a clientsInformation object (which is inturn held in the above listOfClientsInformation)
						This object holds an fd_set which holds only the sockets file descriptor. The objects can call on some basic
						functionalities(ie isReadable) which will create a more legible, code base.

*/











struct clientSocketInformation
{
	char username[32];
	FD_SET socketOfClientAsSet;
	SOCKET socketOfClient{};
	bool isAdmin{};
	void initializeSet(SOCKET, FD_SET);
	bool isReadable(FD_SET socketOfClientAsSet);
		
		//Constructor
	clientSocketInformation(SOCKET socketOfClientInput, bool isAdminInput)
		{
		ZeroMemory(username, 32);
		FD_ZERO(&socketOfClientAsSet); //clear fd_set which will hold a single socket 
		socketOfClient = socketOfClientInput;
		FD_SET(socketOfClientInput, &socketOfClientAsSet);
		isAdmin = isAdminInput;
		std::cout << "testing \n";

		}
	clientSocketInformation()
	{
	}

	
};



bool clientSocketInformation::isReadable(FD_SET clientSocketSet)
{
	FD_SET cloneOfClientSocketSet = clientSocketSet;
	bool resultOfReadTest = select(NULL,&cloneOfClientSocketSet, NULL, NULL, &msgHandlerTimeout);
	return resultOfReadTest;
}


clientSocketInformation listOfClientSocketInformation[120];

 int portServ{ 7878 };




int servSetup()
{
	//Initialize winsock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
		std::cerr << "Cant start winsock error: " << wsResult << "\n";


	memset(&hints, 0, sizeof(hints)); //ensures hints struct is clear.

	hints.ai_family = AF_INET; //fill hints struct with information clarifying how we implement to use winsock2
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	std::cout << getaddrinfo("localhost", "40343", &hints, &serv); //?????????????????
	
	SOCKET serverListeningSocket = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol); //create socket object that adheres to hints struct

	int bindStatus = bind(serverListeningSocket,serv->ai_addr ,serv->ai_addrlen); //bind socket object to machine
	if (bindStatus != -1) //bindstatus check, error would return -1
	{
		std::cout << "server is binded to 40343" << "\n";
	}

	FD_SET(serverListeningSocket, &referenceServ); //enters serverListeningSocket into referenceServ socket set,
	listen(serverListeningSocket, '100');
	return serverListeningSocket;
}




bool incomingConnectHandler(SOCKET servSocket)
{
	fd_set copyOfReferenceServ = referenceServ; //select() alters whatever set is provided as arg, using a copy of original set ensures
						 						//We can maintain intended integrity of referenceServ, which only holds our servers listen socket.
	
	int isReadable = select(NULL, &copyOfReferenceServ, NULL, NULL, &msgHandlerTimeout); // If any socket in copyofReferenceServ, has readable content it will return true
																					//As we only have our servers listening socket, we are effectively checking if any inbound connection queue 
	if (isReadable!=0)
	{
		std::cout << "attempting to establish socket connection with " << isReadable << "\n";

		// std::cout << "Listening socket, appears to have " << res <<" connection.Running clientHandler()";
		SOCKET clientAndServerCommunicationSocket = accept(servSocket, NULL, NULL); //accept blocks until connection is made. That is why we did the above inbound queue check

		FD_SET(clientAndServerCommunicationSocket, &listOfClients); //Sockfd of client is added to listOfClients fd set
	//	clientSocketInformation currentIntake{ clientAndServerCommunicationSocket,false};
		listOfClientSocketInformation[counterForIntakeOfNewClients] = { clientAndServerCommunicationSocket,false };
		counterForIntakeOfNewClients++;
	}
	return false;
}




void forwardMsg(char buff[4096], int buffSize,fd_set sendList, SOCKET fromsocket)
{
	//fd_set writable = listOfClients;
	int x{ 0 }; //0 owrked
	int initialSendListQuantity = sendList.fd_count;
	char* sendbuf = buff;
	fd_set sendCheck{};

	while (initialSendListQuantity >= x)
	{
		FD_ZERO(&sendCheck);
		SOCKET sendCheckSocket = sendList.fd_array[x];
		FD_SET(sendCheckSocket, &sendCheck);
		std::cout << "testing " << sendCheck.fd_array[x] << " for ownership \n";
		if ( sendCheckSocket != fromsocket)
		{
			std::cout << "frwrding msg from socket " << fromsocket << " to socket: " << sendList.fd_array[x] << "\n";
			send(sendList.fd_array[x], sendbuf, (int)strlen(sendbuf), 0);
		}
		x++;
	}

}

void msgRecv()
{
	fd_set listOfReadableClients = listOfClients;
	fd_set clientAddressBook = listOfClients;
	sendList = listOfClients;
	char buff[4096];
	int buffSize = sizeof(buff);
	int totalSockCount = listOfClients.fd_count;
	//read a copy of list of clients fd set for readability, return a altered fd with states
	//fd_count of non-altered listOfClients, return total amount of socks (totalSockCount)
	int i = 0;

	//Instructions Below

	while( i < totalSockCount)
	{
	

		
		if(listOfClientSocketInformation[i].isReadable(listOfClientSocketInformation[i].socketOfClientAsSet) != 0)
		{
			//std::cout << listOfClientSocketInformation[i].isReadable(listOfClientSocketInformation[i].socketOfClientAsSet) << "!!!!!!!!!!!!!!!!!!!!!!!!!!! \n";
			ZeroMemory(buff, 4096);

			std::cout << "hang on recv \n";
			SOCKET socketToTest = listOfClientSocketInformation[i].socketOfClient; //Copies socketOfClient from listOfclientSocketInformation object array

			int inbound = recv(socketToTest, buff, 4096, 0); //hangs until inboundSocket has data!, if we can intelligently select what filedescriptor inboundSocket is this wont be a proble,
			std::cout << "segments received from:" << "from sock:" << i << "\n";
			std::cout << "msg content: " << buff << "\n";
			forwardMsg(buff, buffSize,sendList, listOfClientSocketInformation[i].socketOfClient);
			ZeroMemory(buff, 4096);


		}
		i++;

	}



}




void main()
{
	int servSocket = servSetup(); //Creates socket object and binds it to port
	while (true)
	{

		incomingConnectHandler(servSocket);
		msgRecv();
	}
}




//todo
//regex scan buffers first characters for /cmd
//prepare code for a proper github release, and make a really compelling logic graph.
//investigate file sharing over winsock
//Find a purpose for this project, other than as a technical demonstration. 

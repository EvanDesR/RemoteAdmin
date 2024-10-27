#include "needed.h"

//structs initializations
struct addrinfo hints,*serv; //ipv4 only
struct fd_set listOfClients,testMe, serverListeningSocketAsSet;
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



struct messageInformation
{
	char bufferForMsgContent[4096];
	short int msgID;


};







struct clientSocketInformation
{
	char username[32];
	FD_SET socketOfClientAsSet;
	SOCKET socketOfClient{};
	bool isAdmin{};
	bool isReadable(FD_SET socketOfClientAsSet);
	bool isDisconnected(FD_SET socketOfClientAsSet);

		
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


bool clientSocketInformation::isDisconnected(FD_SET clientSocketSet)
{
	FD_SET cloneOfClientSocketSet = clientSocketSet;
	bool resultOfReadTest = select(NULL, &cloneOfClientSocketSet, NULL, NULL, &msgHandlerTimeout);
	if (resultOfReadTest = true)
	{
		char recvBuff[4096];
		ZeroMemory(recvBuff, 4096);
		int a = recvBuff[53];
		int amountOfNullBeforeInput = 0;
		for (int increment = 1; increment < sizeof(recvBuff); increment++)
		{
			int buffCharAsANSI = recvBuff[increment];
			if (buffCharAsANSI == 0)
			{
				amountOfNullBeforeInput++;
			}
		}
		return (amountOfNullBeforeInput < 4095)?  true :  false; //yay finally, I finally thought to use the ternary operator!
	}
}





clientSocketInformation listOfClientSocketInformation[120];


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

	listen(serverListeningSocket, '100');
	return serverListeningSocket;
}

int howManySocketsInListOfClientSocketInformation()
{

	int i = 0;

	while (i < (sizeof(listOfClientSocketInformation) / sizeof(clientSocketInformation[1])) && listOfClientSocketInformation[i].socketOfClient != 0 && listOfClientSocketInformation[i].isDisconnected(listOfClientSocketInformation[i].socketOfClientAsSet) != 1)
	{ //ALERT: I added isDisconnected into the above while loop, as I want to ensure the validity of listOfClientSocketInformation, and not have this function return an inaccurate amount of sockets in the set. 
		i++;
	}
	return i;
}



void incomingConnectHandler(SOCKET servSocket)
{
	SOCKET serverSock = servSocket;
	FD_SET(serverSock, &serverListeningSocketAsSet);
	int isThereIncomingConnection = select(NULL, &serverListeningSocketAsSet, NULL, NULL, &msgHandlerTimeout); // If any socket in serverListeningSocketAsSet, has readable content it will return true
																					//As we only have our servers listening socket, we are effectively checking if any inbound connection queue 
	if (isThereIncomingConnection!=0)
	{
		std::cout << "attempting to establish socket connection with \n";
		SOCKET clientAndServerCommunicationSocket = accept(servSocket, NULL, NULL); //accept blocks until connection is made. That is why we did the above inbound queue check
		FD_SET(clientAndServerCommunicationSocket, &listOfClients); //Sockfd of client is added to listOfClients fd set
	listOfClientSocketInformation[howManySocketsInListOfClientSocketInformation()] = {clientAndServerCommunicationSocket,false};

	}


}





void forwardMsg(char buff[4096], int buffSize, SOCKET fromsocket)
{
	int socketIncrementer = 0; //0 owrked
	int amountOfSocketsInListOfClientSocketInformation = howManySocketsInListOfClientSocketInformation();
	char* sendbuf = buff;

	while (amountOfSocketsInListOfClientSocketInformation >= socketIncrementer)
	{

		std::cout << "testing " << listOfClientSocketInformation[socketIncrementer].socketOfClient << " for ownership \n";
		if ( listOfClientSocketInformation[socketIncrementer].socketOfClient != fromsocket)
		{
			std::cout << "sockets in listOfClientSocketInformation" << howManySocketsInListOfClientSocketInformation() << "\n";

			std::cout << "frwrding msg from socket " << fromsocket << " to socket: " << listOfClientSocketInformation[socketIncrementer].socketOfClient << "\n";
			send(listOfClientSocketInformation[socketIncrementer].socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
		}
		socketIncrementer++;
	}

}


void msgRecv()
{
	char buff[4096];
	int buffSize = sizeof(buff);
	int totalSockCount = howManySocketsInListOfClientSocketInformation();
	//read a copy of list of clients fd set for readability, return a altered fd with states
	//fd_count of non-altered listOfClients, return total amount of socks (totalSockCount)
	int i = 0;

	//Instructions Below

	while( i < totalSockCount)
	{
	

		
		if(listOfClientSocketInformation[i].isReadable(listOfClientSocketInformation[i].socketOfClientAsSet) != 0 )
		{
			ZeroMemory(buff, 4096);
			SOCKET socketToTest = listOfClientSocketInformation[i].socketOfClient; //Copies socketOfClient from listOfclientSocketInformation object array

			int inbound = recv(socketToTest, buff, 4096, 0); //hangs until inboundSocket has data!, if we can intelligently select what filedescriptor inboundSocket is this wont be a proble,
			std::cout << "receiving from sock:" << i << "\n";

			if (listOfClientSocketInformation[i].isDisconnected(listOfClientSocketInformation[i].socketOfClientAsSet) != true)
			{
				forwardMsg(buff, buffSize, listOfClientSocketInformation[i].socketOfClient);
				std::ofstream tcpLogOut{ "tcpLogs.txt",std::ios::app };
				tcpLogOut << buff;
				tcpLogOut << sizeof(buff);
				tcpLogOut << "---";
			}
			else
			{
				std::cout << "socket " << listOfClientSocketInformation[i].socketOfClient << " has disconnected. They should be removed from listOfClientInformation accordingly. \n";
			}
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

//listOfClientSocketInformation cleanup. If isDisconnected returns true, the socket should be removed from the list.

//Multi-threading so server admin inputs can be taken concurrently, alongside other operations.

//Utilize more .cpp files for better organization, and clearer distinction between the seperate operations of this program.

//CLI Parser to allow interpretation, and execution of modules based on the server admin CLI input.

//Further expand on module concept






//DONE
// fd_set sendlist should be entirely retired, as it has been replaced, and expanded on in functionality by the object array listOfClientInformation.
	//sendlist.fdcount provides amount of file descriptors in the set. It is used as a stop condition for a while loop incrementer. 
	// Once i can provide an int value, of how many objects are in the listOfClientInformation array, only then can sendList be removed.



//isDisconnected should also remove the client socket from clientSocketInformation, and close the socket from our side.
	// it probably should rearrange the listOfClientInformation array, to ensure its remains contiguous in index (i hope this makes sense.)

//prepare code for a proper github release, and make a really compelling logic graph.

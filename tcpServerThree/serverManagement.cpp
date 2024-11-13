//Home of all things that turn command instructions into action.
#include "main.h"
#include "sharedObjects.h"
//std::vector<clientSocketInformation> vectorOfClientSocketInformation;



void clientSocketInformation::logClientReply(clientSocketInformation& client, char serverSendingMsg[4096])
{
	std::cout << "sf sfd" << "\n";

	vectorAndMapAccessMutex.lock();

	if (client.messages.size() == 0) // meaning no message has ever been exchanged.
	{
		std::cout << "entered nomesagehistory" << "\n";

		client.messages.push_back({}); //push_back alongside null constructor. before "back" ensures that we do not attempt to access an empty vector
		for (int i = 0; i <= (int)strlen(serverSendingMsg); i++)
		{
			client.messages.back().replyFromClient[i] = serverSendingMsg[i];

		}
	}
	else if (client.messages.size() > 0 && client.messages.back().messageSentByServer == TRUE) //if vector has values, and if recent message sent is awaiting a clientReply.
	{
		std::cout << "entered logclientreply" << "\n";

		for (int i = 0; i <= (int)strlen(serverSendingMsg); i++)
		{
			client.messages.back().replyFromClient[i] = serverSendingMsg[i];

		}

		//std::ofstream tcpLogOut{ "tcpLogs.txt",std::ios::app };
		//hashMap[socketForKey].messages.push_back
	}
	else//if vector has values, and if recent message sent is awaiting a clientReply.
	{
		client.messages.push_back({}); //push_back alongside null constructor. before "back" ensures that we do not attempt to access an empty vector
		for (int i = 0; i <= (int)strlen(serverSendingMsg); i++)
		{
			client.messages.back().replyFromClient[i] = serverSendingMsg[i];
		}
		client.messages.back().simpleTCPFromCient = TRUE;
		std::cout << "simple message logged" << "\n";
	}


	std::ofstream tcpLogOut{ "tcpLogs.txt",std::ios::app };

	tcpLogOut << "---" << "\r\n";
	tcpLogOut << "From:" << client.socketOfClient << "\r\n";
	tcpLogOut << "Sent/Reply history is: " << (client.messages.size()) << "\r\n";
	tcpLogOut << "simple message?:" << client.messages.back().simpleTCPFromCient << "\n";
	tcpLogOut << "message contents: " << serverSendingMsg << "\r\n";
	tcpLogOut << "---" << "\r\n";
	vectorAndMapAccessMutex.unlock();


	std::cout << "unlock done" << "\n";

}



void clientSocketInformation::logSent(clientSocketInformation& client, char serverSendingMsg[4096])
{
	//pushback to make an element in the vector (so its not a empty vector)
	for (int i = 0; i <= (int)strlen(serverSendingMsg); i++)
	{
	//FromClient[i] = serverSendingMsg[i];

	}
	vectorAndMapAccessMutex.lock();
	client.messages.push_back({ serverSendingMsg,sizeof(serverSendingMsg),TRUE });
	vectorAndMapAccessMutex.unlock();

	std::ofstream tcpLogOut{ "tcpLogs.txt",std::ios::app };

	tcpLogOut << "---" << "\r\n";
	tcpLogOut << "Reply from:" << client.socketOfClient << "\r\n";
	tcpLogOut << "sent/reply history is: " << (client.messages.size()) << "\r\n";
	tcpLogOut << "message contents: " << serverSendingMsg << "\r\n";
	tcpLogOut << "---" << "\r\n";


}



bool clientSocketInformation::isReadable(FD_SET clientSocketSet)
{
	FD_SET cloneOfClientSocketSet = clientSocketSet;
	bool resultOfReadTest = select(NULL, &cloneOfClientSocketSet, NULL, NULL, &msgHandlerTimeout);
	return resultOfReadTest;
}
/**/

bool clientSocketInformation::isDisconnected(FD_SET clientSocketSet, bool closeBadConnect, int index) //4095 loops is huge time waste.
{
	FD_SET cloneOfClientSocketSet = clientSocketSet;
	bool resultOfReadTest = select(NULL, &cloneOfClientSocketSet, NULL, NULL, &msgHandlerTimeout);
	if (resultOfReadTest == true)
	{
		char recvBuff[4096];
		ZeroMemory(recvBuff, 4096);
		int amountOfNullBeforeInput = 0;
		for (int increment = 1; increment < sizeof(recvBuff); increment++)
		{
			char buffCharAsANSI = recvBuff[increment];
			if (buffCharAsANSI == NULL)
			{
				std::cout << buffCharAsANSI << "\n";
				amountOfNullBeforeInput++;

			}
		}
		std::cout << "amount of null before input is... " << amountOfNullBeforeInput << "\n";
		return (amountOfNullBeforeInput > 4090) ? true : false; //yay finally, I finally thought to use the ternary operator!
	}
}
//bool clientSocketInformation::closeConnection(int socketToRemove)
//{
	//std::cout << "removing clientSocketInformation object " << listOfClientSocketInformation[socketToRemove].socketOfClient << "from listOfClientSocketInformation";

//}


void clientSocketInformation::closeConnection(int elementAddress)
{
	if (vectorOfClientSocketInformation.size() > 1)
	{

		int deletedSockFDNum = vectorOfClientSocketInformation[elementAddress].socketOfClient;
		vectorAndMapAccessMutex.lock();
		hashMap.erase(elementAddress);
		vectorOfClientSocketInformation.erase(vectorOfClientSocketInformation.begin() + elementAddress);
		vectorAndMapAccessMutex.unlock();
		std::cout << "socket " << deletedSockFDNum << "/" << elementAddress << " Disconnected! They been removed from vectorOfClientInformation. \n";

	}
	else
	{
		vectorOfClientSocketInformation.clear();
		std::cout << "vectorOfClientSocketInformation has been entirely wiped. size " << vectorOfClientSocketInformation.size() << "\n";
	}
	// dd errror checking eventually
}




void forwardToAll(char buff[4096], int buffSize, SOCKET& fromsocket)
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



/* void sendMsg(clientSocketInformation& destinationSocket, char* buff)
{

	char* sendbuf = buff;
	forwardToAll(sendbuf, (int)strlen(sendbuf), destinationSocket.socketOfClient);
	send(destinationSocket.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
}
*/




int howManySocketsInListOfClientSocketInformation()
{
	return vectorOfClientSocketInformation.size();
}



void incomingHandler(SOCKET &servSocket)
{
	//SOCKET serverSock = servSocket;
	int i{};
	char buff[4096];
	char* pointToArray = buff;
	int buffSize;
	while (true)
	{
		i = 0;
		//char* pointToArray = buff;
		buffSize = sizeof(buff);		
		FD_SET(servSocket, &serverListeningSocketAsSet);
		int isThereIncomingConnection = select(NULL, &serverListeningSocketAsSet, NULL, NULL, &msgHandlerTimeout); // If any socket in serverListeningSocketAsSet, has readable content it will return true
		//As we only have our servers listening socket, we are effectively checking if any inbound connection queue 
		
		if (isThereIncomingConnection > 0)
		{
			//int i = 0;

			std::cout << "attempting to establish socket connection with \n";
			SOCKET clientAndServerCommunicationSocket = accept(servSocket, NULL, NULL); //accept blocks until connection is made. That is why we did the above inbound queue check
			clientSocketInformation newClientObject = { clientAndServerCommunicationSocket };
			vectorOfClientSocketInformation.push_back(newClientObject);
			hashMap.insert({ newClientObject.socketOfClient, newClientObject });
			std::cout << "new connection from" << newClientObject.socketOfClient << "\n";
			//hashMap.
		}	//vectorOfClientSocketInformation.insert

		int totalSockCount = howManySocketsInListOfClientSocketInformation(); // this is ass.


		while (i < totalSockCount)
		{
			//	if (hashMap[i].isReadable(hashMap[i].socketOfClientAsSet) == 0)

			if (vectorOfClientSocketInformation[i].isReadable(vectorOfClientSocketInformation[i].socketOfClientAsSet) != 0)
			{
				ZeroMemory(buff, 4096);
				SOCKET socketToTest = vectorOfClientSocketInformation[i].socketOfClient; //Copies socketOfClient from listOfclientSocketInformation object array
				int inbound = recv(socketToTest, buff, 4096, 0); //hangs until inboundSocket has data!, if we can intelligently select what filedescriptor inboundSocket is this wont be a proble,
				forwardToAll(buff, buffSize, socketToTest);
				std::cout << totalSockCount << " AND I = " << i << "\n";

				if (inbound == 0 || inbound == -1) //Controls the possibily of read data that is indicative of a socket being shutdown
				{
					vectorOfClientSocketInformation[i].closeConnection(i);
					break;
				}
				else if (inbound != 0)
				{
					std::cout << "operating on a reply \n";
					SOCKET destinationSocket = vectorOfClientSocketInformation[i].socketOfClient;
					hashMap[i].logClientReply(hashMap[i], buff);
				}


					//vectorOfClientSocketInformation[i].logClientReply(hashMap[destinationSocket], buff);
					//hashMap[socketToTest].logClientReply(hashMap[destinationSocket], buff);
					//std::cout << "reply from client:" << vectorOfClientSocketInformation[i].messages.back().replyFromClient << "\n";
			}
			i++;
		}



	}

}





/* OVERLOADED VERSION of sendMsg FOR SENDING TO A SOCKGROUP
void sendMsg(char buff[4096], clientSocketInformation& destinationSocket)
{
	char* sendbuf = buff;

	destinationSocket.messages[destinationSocket.messages.size()].storeMessage(buff);

	std::cout << "message logged in " << destinationSocket.messages.size() << "sized vector. the message is" << destinationSocket.messages.back().serverSent << "\n";
	send(destinationSocket.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
}

*/
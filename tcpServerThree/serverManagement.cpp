//Home of all things that turn command instructions into action.
#include "main.h"
#include "sharedObjects.h"
#include "connectionModules.h"



void clientSocketInformation::logClientReply(clientSocketInformation& client, char serverSendingMsg[4096])
{
	std::cout << "log client reply entered" << "\n";

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



void clientSocketInformation::closeConnection(SOCKET& socketToClose) //once vector is removed, this should take SOCKET as arg.
{
	if (hashMap.size() > 1)
	{
		int socketToDelete = socketToClose;
		vectorAndMapAccessMutex.lock();
		hashMap.erase(socketToClose);
		vectorAndMapAccessMutex.unlock();
		std::cout << "socket " << socketToDelete << " Disconnected! They been removed from hashMap. \n";

	}
	else
	{
		vectorAndMapAccessMutex.lock();
		hashMap.clear();
		vectorAndMapAccessMutex.unlock();
		std::cout << "hashMap has been entirely wiped. size " << hashMap.size() << "\n";
	}
	// dd errror checking eventually
}



void incomingHandler(SOCKET &servSocket) //checks and adds any inbound connection to hashMap and vector, and then iterates through vector to check if any sockets are readable.
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
			SOCKET clientAndServerCommunicationSocket = accept(servSocket, NULL, NULL); //accept will block until a connection is made. That is why we use IsThereincoming select() to check first
			clientSocketInformation newClientObject = { clientAndServerCommunicationSocket };
			//vectorOfClientSocketInformation.push_back(newClientObject);
			hashMap.insert({ newClientObject.socketOfClient, newClientObject });
			std::cout << "new connection from" << newClientObject.socketOfClient << "\n";
			//hashMap.
		}	//vectorOfClientSocketInformation.insert


		//while (i < totalSockCount)
		for (auto it = hashMap.begin(), end = hashMap.end(); it != end; it++)//it is a iterator. This is replacing vectorOfClientSocketInformation loop as its faster and clean(er)
		{
			//SOCKET currentTest = it->second.socketOfClient;
			it->second.isReadable(it->second.socketOfClientAsSet);
			if (it->second.isReadable(it->second.socketOfClientAsSet) != 0) //REPLACE W: UNSORTED MAP FOR LOOP ITERATOR
			{
				ZeroMemory(buff, 4096);
				SOCKET socketToTest = it->second.socketOfClient;
				int inbound = recv(socketToTest, buff, 4096, 0); //hangs until inboundSocket has data!, if we can intelligently select what filedescriptor inboundSocket is this wont be a proble,
				forwardToAll(buff, buffSize, socketToTest);

				if (inbound == 0 || inbound == -1) //Controls the possibily of read data that is indicative of a socket being shutdown
				{
					//vectorOfClientSocketInformation[i].closeConnection(i);
					hashMap.erase(it->second.socketOfClient);
					break;
				}
				else if (inbound != 0 && inbound != -1)
				{
					std::cout << "operating on a reply \n";
					//SOCKET destinationSocket = vectorOfClientSocketInformation[i].socketOfClient;
					hashMap[socketToTest].logClientReply(hashMap[socketToTest], buff); //socketToTest is used as an integer, for the key value. hashMap holds clientSocketInformation object as the value corresponding to key.
				}
				else
				{
					std::cout << "Error of some sort with recv() calls return value????";
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
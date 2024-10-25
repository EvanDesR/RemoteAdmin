/*

void forwardMsg(char buff[4096], int buffSize, fd_set sendList, SOCKET fromsocket)
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
		if (sendCheckSocket != fromsocket)
		{
			std::cout << "frwrding msg from socket " << fromsocket << " to socket: " << sendList.fd_array[x] << "\n";
			send(sendList.fd_array[x], sendbuf, (int)strlen(sendbuf), 0);
		}
		x++;
	}

}

*/
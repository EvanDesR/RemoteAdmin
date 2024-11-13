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

void listAllSockets()
{
	for (auto& x : hashMap)
	{
		//test = x.second.
		std::cout <<"SOCKET:" << x.second.socketOfClient << "\n";
		std::cout <<"ALIAS" << x.second.alias << "\n";
		std::cout << "Message History:" << x.second.messages.size() << "\n";
		std::cout << "---------------------------- \n";

	}
}
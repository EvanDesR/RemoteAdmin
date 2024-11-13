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


int cmdModule(clientSocketInformation& clientObjectPassedToCmd, std::string passedString) //pass by ref
{
    char commandInput[4096] = { 0 }; // Initialize the entire array to null characters
    std::size_t copyLength = passedString.length(); // Copy the entire length of passedString
    passedString.copy(commandInput, copyLength);
    commandInput[copyLength] = '\0'; // Explicit null-termination after the copied length
    std::cout << "sending " << commandInput;
    char* sendbuf = commandInput;

    send(clientObjectPassedToCmd.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
    //forward the rest to 

  //  send(clientObjectPassedToCmd.socketOfClient, sendbuf, (int)strlen(sendbuf), 0);
    return 4;
}
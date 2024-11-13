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
#include "connectionModules.h"
#include "serverModules.h"  



//Connection module designation, is for any action that acts solely upon one clientSocketInformation 
//It does not implicitly mmean that it has to be related to sending information. It can be for something 
//as simple as listing messageHisotry of that partiuclar clientobject
//typedef match_results<std::string::const_iterator> smatch;
std::regex validConnection("(/connection)\\s{1,3}(\\d{2,4})\\s{1,3}([/A-Zi]\\w+)");//check if /connectoin sockfd /module
std::regex validServer("(/server)\\s{1,3}([/A-Zi]\\w+)"); //check if /server sockfd /module 
std::regex sockfdParser("(\\d{2,4})");
std::regex allFlags(R"(\/\S+)"); //regex for every /char234sHere
//use to exclude everything b4 2nd flag!!!!!!!!!!!!!!!!!!!!!!!!!



int changeAlias(clientSocketInformation& clientObjectPassedToCmd) //pass by ref
{
    std::getline(std::cin, clientObjectPassedToCmd.alias);
    return 3;
}


void connectionParser(std::string input)
{
    std::smatch matches;

    std::regex_search(input, matches, sockfdParser);
    std::string sockfdParsed = matches[0]; //sockFD is stored!
    std::size_t pos{};
    int sockInt{ std::stoi(sockfdParsed,&pos) };
    SOCKET inputSocketParsed = sockInt;
    if (hashMap.find(sockInt) != hashMap.end())
    {
        std::cout << "THE PROVIDED SOCKET IS VALID \n";
        //assign that object to ptr



        std::cout << "Extracted sockfd value: " << sockInt << std::endl; //sockfd is parsed

        std::regex moduleFlagRegex("(/\\w+)$");

        std::string moduleFlagParsed;
        std::string moduleFlagParsedLower;

        std::sregex_iterator begin(input.begin(), input.end(), allFlags), end;
        int flagCounter = 0;
        for (auto it = begin; it != end; ++it) {
            flagCounter++;
            if (flagCounter == 2) {
                moduleFlagParsed = it->str(); //module Flag is stored!
                std::cout << "moduleFlagParsed: " << moduleFlagParsed << "\n";
                // Outputs: "Found: /flag2"
            }
        }

        if (moduleFlagParsed == "/cmd")
        {
            //remove everything until /cmd
            std::size_t position = input.find("/cmd");
            std::string passToModule = input.substr(position);
            std::cout << "passing " << passToModule << " to cmd module" << "\n";
            cmdModule(hashMap.at(inputSocketParsed),passToModule);
        }
        else if (moduleFlagParsed == "/changealias")
        {
            std::size_t position = input.find("/changealias");
            std::string passToModule = input.substr(position);
            std::cout << "passing " << passToModule << " to changeAlias module" << "\n";
            changeAlias(hashMap[inputSocketParsed]);
            std::cout << "new alias is: " << hashMap[inputSocketParsed].alias << " (ACCESSED VIA HASHMAP REFERNCE)";

            //update vector call
        }
        else if (moduleFlagParsed == "/reconscript")
        {
            std::size_t position = input.find("/reconscript");
            std::string passToModule = input.substr(position);
            std::cout << "passing " << passToModule << " to reconscript module" << "\n";
        }
        else
        {
            std::cout << "error no cmd known as: " << moduleFlagParsed << " found! \n \n";

            std::cout << "Valid Options are \n";
            std::cout << "/cmd \n";
            std::cout << "/changealais \n";
            std::cout << "/reconscript \n";


        }

    }
    else
    {
        std::cout << "invalid socket provided : " << inputSocketParsed << "\n";
    }

}

void serverParser(std::string input)
{
    std::cout << "entered server parser \n";
    std::smatch matches;
    //   std::string moduleFlagParsedLower;

    std::regex moduleFlagRegex("(/\\w+)$");

    std::string moduleFlagParsed;
    std::string moduleFlagParsedLower;

    std::sregex_iterator begin(input.begin(), input.end(), allFlags), end;
    int flagCounter = 0;
    for (auto it = begin; it != end; ++it) {
        flagCounter++;
        if (flagCounter == 2) {
            moduleFlagParsed = it->str(); //module Flag is stored!
            std::cout << "moduleFlagParsed: " << moduleFlagParsed << "\n";
            // Outputs: "Found: /flag2"
        }
    }

    if (moduleFlagParsed == "/listallsockets")
    {
        //remove everything until /cmd
        std::size_t position = input.find("/listallsockets");
        std::string passToModule = input.substr(position);
        std::cout << "passing " << passToModule << " to listallsockets module" << "\n";
        listAllSockets();
    }
    else if (moduleFlagParsed == "/stats")
    {
        std::size_t position = input.find("/stats");
        std::string passToModule = input.substr(position);
        std::cout << "passing " << passToModule << " to stats module" << "\n";
    }
    else if (moduleFlagParsed == "/save")
    {
        std::size_t position = input.find("/save");
        std::string passToModule = input.substr(position);
        std::cout << "passing " << passToModule << " to save module" << "\n";
    }
    else
    {
        std::cout << "error no cmd known as: " << moduleFlagParsed << " found! \n";

        std::cout << "error no cmd known as: " << moduleFlagParsed << " found! \n \n";

        std::cout << "Valid Options are \n";
        std::cout << "/listallsockets \n";
        std::cout << "/stats \n";
        std::cout << "/save \n";

    }

}






void cliParser()
{
    while (true)
    {
        std::string testString;
        std::getline(std::cin, testString);



        std::transform(testString.begin(), testString.end(), testString.begin(), [](unsigned char c) {return std::tolower(c); });
        vectorAndMapAccessMutex.lock();
        if (std::regex_search(testString, validConnection)) //
        {
            std::cout << testString << "| connection type module request passes CLI Parser \n";
            connectionParser(testString);
        }
        else if (std::regex_search(testString, validServer))
        {
            std::cout << testString << "| server type module request passes CLI Parser \n";
            serverParser(testString);

        }
        else
        {
            std::cout << "inital input syntax error! ( Must be /server, or /connection ) \n";
        }
        vectorAndMapAccessMutex.unlock();

        std::cout << "------- \n";
    }
}

void xmlParser()
{

}
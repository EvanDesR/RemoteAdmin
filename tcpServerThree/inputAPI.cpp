#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <ws2def.h>
#include <stdlib.h>
#include <time.h>
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

std::regex validConnection("(/connection)\\s{1,3}(\\d{2,4})\\s{1,3}([/A-Zi]\\w+)");//check if /connectoin sockfd /module
std::regex validServer("(/server)\\s{1,3}([/A-Zi]\\w+)"); //check if /server sockfd /module 
std::regex sockfdParser("(\\d{2,4})");
std::regex allFlags(R"(\/\S+)"); //regex for every /char234sHere
std::regex moduleFlagRegex("(/\\w+)$");

//use to exclude everything b4 2nd flag!!!!!!!!!!!!!!!!!!!!!!!!!






void connectionParser(std::string input)
{
    std::smatch matches; //string match result container.
    std::regex_search(input, matches, sockfdParser); //search input with sockfdParser, and put results in matches
    //std::string sockfdParsed = matches[0]; //sockFD is stored!
    std::size_t pos{};
    int sockAsInt{ std::stoi(matches[0],&pos)}; //sockAsInt is the string at match[0], turned into a integer
    SOCKET inputSocketParsed = sockAsInt; //inputSocketParsed is assigned the value of sockAsInt. (SOCKET is actually a type_def for a uint_64.)  
    std::string moduleFlagParsed;
    std::string moduleFlagParsedLower;


    if (hashMap.find(sockAsInt) != hashMap.end()) // if an entry exists in hashMap which uses SockAsInt as a key. If sockAsInt does not exist as a key in hashMap hashMap.find() will return an iterator to hashMap.end.
    {
        std::cout << "Regex extracted sock value: " << sockAsInt << std::endl;


        std::sregex_iterator begin(input.begin(), input.end(), allFlags), end; //to be honest I hardly understand this line of code. It creates an iterator for the beginning of input
        int flagCounter = 0;
        for (auto it = begin; it != end; ++it) {
            flagCounter++;
            if (flagCounter == 2) {
                moduleFlagParsed = (it->str()); //I dont even know i this must be cast into str()? as its of type auto. It should already be a string.?
                std::cout << "moduleFlagParsed: " << moduleFlagParsed << "\n";
                // Outputs: "Found: /flag2"
            }
        }

        if (moduleFlagParsed == "/cmd")
        {
            std::size_t position = input.find("/cmd");
            std::string passToModule = input.substr(position);       //creates new string with everything after /cmd
            std::cout << "passing " << passToModule << " to cmd module" << "\n";
            cmdModule(hashMap.at(inputSocketParsed).socketOfClient,passToModule);
        }
        else if (moduleFlagParsed == "/changealias")
        {
            std::size_t position = input.find("/changealias");
            std::string passToModule = input.substr(position);
            std::cout << "passing " << passToModule << " to changeAlias module" << "\n";
            changeAlias(hashMap[inputSocketParsed].socketOfClient);
            std::cout << "new alias is: " << hashMap[inputSocketParsed].alias << " (ACCESSED VIA HASHMAP REFERNCE)";

        }
        else if (moduleFlagParsed == "/forwardtoall")
        {
            std::size_t position = input.find("/forwardtoall");
            std::string passToModule = input.substr(position);
            std::cout << "passing " << passToModule << " to forwardToAll module" << "\n";
            //forwardToAll(hashMap[inputSocketParsed]);
            std::cout << "forwardToAll() parameters require a rework as its not optimized for hashMap centric design";

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

            std::cout << "Valid /connection Options are \n";
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
    std::smatch matches;
    //   std::string moduleFlagParsedLower;
    //size_t = input.find()
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

        std::cout << "Valid /server Options are \n";
        std::cout << "/listallsockets \n";
        std::cout << "/stats \n";
        std::cout << "/save \n";

    }

}






void cliParser()
{
    while (true)
    {
        std::string moduleFlag;
        std::string testString;
        std::getline(std::cin, testString);
        std::sregex_iterator begin(testString.begin(), testString.end(), allFlags), end;

        int flagCounter = 0;
        for (auto it = begin; it != end; ++it) {
            flagCounter++;
            if (flagCounter == 2) {
                moduleFlag = it->str(); //module Flag is stored!
                std::cout << "moduleFlagParsed: " << moduleFlag << "\n";
                // Outputs: "Found: /flag2"
            }
        }
        std::string strAfterModule = testString.substr((testString.find(moduleFlag)+moduleFlag.length())); //creating string of everything after module by using the match for moduleFlag + moduleFlag length.
        std::string StrToTheModule = testString.substr(0,(testString.find(moduleFlag)+ moduleFlag.length())); //creating string of everything up to, AND including the module flag
        std::transform(StrToTheModule.begin(), StrToTheModule.end(), StrToTheModule.begin(), [](unsigned char c) {return std::tolower(c); }); //make StrToTheModule all lowercase
        testString = StrToTheModule.append(strAfterModule); //join them back together.
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

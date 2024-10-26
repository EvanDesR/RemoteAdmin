# RemoteAdmin
Hooby/School project, remote administrator tool. (Work in progress). Expect multiple updates in the coming days.

Built with C++, using the winapi (winsock2). The server accepts incoming TCP socket requests, and will forward data from any of the established socket, to all other established sockets. With an already established object based backbone, I have made preparations to build this into a larger remote administration program: integrating file transferring, cmd execution, and a plug, and play module system which will allow users greater customization of functionality, and basic automations. 

## Objects

#### clientInformation

Storing the socket, of a single connection. The clientInformation object, holds the socket as a file descriptor of type SOCKET, and as a fd_set. While having only a single file descriptor in a file descriptor set (fd_set) may seem strange, it is quite important for legible code, considering how often winsock2's SELECT() function is used. SELECT() only takes fd_set as input, and overwrites the fd_set content with the status of the file descriptor (ie if we use select() to test for readability, the response code will be written over the corresponding sockets in the set). 

---

## Functions

#### servSetup() - Prepares winsock and opens listening socket on server

Initalizes winsock2, and performs prerequisite operations.

Populates hint struct, which clarifies the specifications of the sockets we will use.

Creates listening socket, and binds listening socket to 4043.

Adds serverListeningSocket to referenceServ fd_set which currently acts as a globally accessible holder for the servers listening socket. (referenceServ will be reworked in next update)

Uses winsock2's listen function, to set the socket into listening mode, and allow a inbound queue of 100 clients.



##### incomingConnectHandler() - Accept inbound socket connection attempts, then creating a clientInformation object, to add to listOfClientSocketInformation array. ((Note: Change from bool to void

Creates a copy of fd_set referenceServ (which houses the SOCKET object, of the listening socket)

Uses Select() to check if the listening port is readable (ie a connection queue exists)

If a connection queue exists , it will accept() the connection, which returns a SOCKET that we will name clientAndServerCommunicationSocket

The SOCKET ClientAndServerCommunicationSocket is added to fd_set listOfClients

A new clientSocketInformnation object is instantiated with SOCKET clientAndServerCommunicationSocket, in the listOfClientSocketInformation object array. (This array of clientInformation objects is, and will be the main way to access sockets, and their program assigned attributes.)

counterForIntakeOfNewClients is incremeneted +1, this int is used when we above created a new object in the array. Note: This will soon be replaced by something simpler, something along the lines of (totalArraysSize/objectSize)+1). 


#### msgRecv() - Check if any of our clients, on listOfClientSocketInformation have sent us data to read

Loop that increments through listOfClientInformation, calling the isReadable object function to check if the socket has readable data, waiting to be seen. The program checks readability first, because recv() hangs until the socket has data to process.

  If the socket has readable data, the program will wipe the message storage buffer, then using recv to read from the socket, and populate the buffer with the data.

  It then calls forwardMsg(), and provides the following args: buffer, bufferSize, sendList, and the socket from which the data was received (NOTE: sendList should be removed, and replaced. Its some lazy code).

  

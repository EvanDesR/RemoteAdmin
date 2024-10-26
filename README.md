# RemoteAdmin
Hooby/School project, remote administrator tool. (Work in progress). Expect multiple updates in the coming weeks.

Built with C++, using the winapi (winsock2). The server accepts incoming TCP socket requests, and will forward data from any of the established socket, to all other established sockets. With an already established object based backbone, I have made preparations to build this into a larger remote administration program: integrating file transferring, cmd execution, and a plug, and play module system which will allow users greater customization of functionality, and basic automations. 

## Meet The Objects

#### clientInformation

Storing the socket, of a single connection. The clientInformation object, holds the socket as a file descriptor of type SOCKET, and as a fd_set. While having only a single file descriptor in a file descriptor set (fd_set) may seem strange, it is quite important for legible code, considering how often winsock2's SELECT() function is used. SELECT() only takes fd_set as input, and overwrites the fd_set content with the status of the file descriptor (ie if we use select() to test for readability, the response code will be written over the corresponding sockets in the set). 


---
## Functions

####servSetup()

Initalizes winsock2, and performs prerequisite operations.
Populates hint struct, which clarifies the specifications of the sockets we will use.
Creates listening socket, and binds listening socket to 4043.
Adds serverListeningSocket to referenceServ fd_set which currently acts as a globally accessible holder for the servers listening socket. (referenceServ will be reworked in next update)
Uses winsock2's listen function, to set the socket into listening mode, and allow a inbound queue of 100 clients.



#####incomingConnectionHandler()


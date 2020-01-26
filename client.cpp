//
//  client.cpp
//  PiMessage
//
//  Created by Luke Korsman on 1/23/20.
//  Copyright © 2020 Luke Korsman. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// Creates a socket connection to a specific server
int createSocket(addrinfo *serverStruct);

// Sends a text based message to specified socket
void sendMessage(int clientSocket, std::string message);

// Receives a message from specified socket
void getResponse(int socketFD);

int main(int argc, const char * argv[]) 
{
	struct addrinfo addrInfoHolder;
	struct addrinfo *serverStruct;
	int socketFD;
	int addrSuccess; 
	std::string portNumber = "8866";
	std::string domainName = "192.168.0.104";
	std::string message;
	char *hostChar = NULL;
	char *portChar = NULL;
	
	if (argc != 2) 
	{
		fprintf(stderr, "Must provide 2 arguments");
		exit(EXIT_FAILURE);
	}
	
	message = argv[1];
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&addrInfoHolder, 0, sizeof(struct addrinfo));
	addrInfoHolder.ai_family = AF_INET;
	addrInfoHolder.ai_socktype = SOCK_STREAM;
	addrInfoHolder.ai_flags = 0;
	addrInfoHolder.ai_protocol = 0;
	
	hostChar = new char[domainName.length()];
	strcpy(hostChar, domainName.c_str());
	portChar = new char[portNumber.length()];
	strcpy(portChar, portNumber.c_str());
	
	addrSuccess = getaddrinfo(hostChar, portChar, &addrInfoHolder, &serverStruct);
	if (addrSuccess != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrSuccess));
		exit(EXIT_FAILURE);
	}
	
	socketFD = createSocket(serverStruct); 
	sendMessage(socketFD, message);
	getResponse(socketFD);
	
	return 0;
}

int createSocket(addrinfo *serverStruct)
{
	struct addrinfo *getAddrInfoResultList;
	int clientSocket = -1;
	
	// Loop through linked list of addrinfo structs until we find right IP address
	for (getAddrInfoResultList = serverStruct; getAddrInfoResultList != NULL; 
		  getAddrInfoResultList = getAddrInfoResultList->ai_next) 
	{
		clientSocket = socket(getAddrInfoResultList->ai_family, 
									 getAddrInfoResultList->ai_socktype,
									 getAddrInfoResultList->ai_protocol);
		if (clientSocket == -1)
			continue;
		int conn = connect(clientSocket, getAddrInfoResultList->ai_addr, getAddrInfoResultList->ai_addrlen);
		if (conn != -1)
			break;
		
		close(clientSocket);
	}
	
	// If did not succeed with getaddrinfo() call, exit program
	if (getAddrInfoResultList == NULL) 
	{
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	return clientSocket;
}

void sendMessage(int clientSocket, std::string message)
{
	size_t bytesLeft;
	size_t bytesSent;
	ssize_t requestLength = message.length();
	char sendBuffer[requestLength];
	
	strcpy(sendBuffer, message.c_str());			
	bytesLeft = requestLength;
	
	std::cerr << message;
	std::cerr << "\n";
	
	while (bytesLeft > 0) {
		bytesSent = send(clientSocket, sendBuffer, requestLength, 0);
		if (bytesSent < 0) 
			fprintf(stderr, "ERROR writing to socket");
		bytesLeft = bytesLeft - bytesSent;
	}
}

void getResponse(int socketFD)
{
	ssize_t nread;
	const long BUFFER_SIZE = 1000;
	char receiveBuffer[BUFFER_SIZE];

	nread = recv(socketFD, receiveBuffer, BUFFER_SIZE, 0);
	 
	if (nread == -1) 
	{
		fprintf(stderr, "ERROR reading from socket");
	}
	else
	{
		for (unsigned int i = 0; i < (long)nread; i++) 
		{
			std::cout << receiveBuffer[i];
		}
		std::cout << std::endl;
	}
}

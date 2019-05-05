/*********************************************************
 * Program: chatclient.c
 * Author: Joel Huffman
 * Last updated: 5/04/2019
 * Sources: https://media.oregonstate.edu/media/t/0_hh05nevl
 * https://beej.us/guide/bgnet/html/single/bgnet.html
 * https://github.com/Huffmajo/CS344/tree/master/program4
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/***********************************************************
 * Function: StdError(err)
 * Accepts a string. Sends that string to standard error and
 * exits with value 1.
 ***********************************************************/
void StdError(const char* string) 
{ 
	fprintf(stderr, string); 
	exit(1); 
}

/***********************************************************
 * Function: void SetupConnection()
 * Handles the socket creation and connection to the server.
 * Sources: This function is mostly pulled from my CS344
 * Porject 4 code with some modifications. URL to my private
 * repo is listed in the program header. Access is available
 * upon request.
 ***********************************************************/
int SetupConnection(char* username, char* hostname, int portNumber)
{
	int socketFD;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	
	// setup server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));

	// create network socket
	serverAddress.sin_family = AF_INET;

	// assign submitted portnumber
	//portNumber = atoi(argv[2]);
	//portNumber = atoi(portNum);

	// store the port number
	serverAddress.sin_port = htons(portNumber);	

	serverHostInfo = gethostbyname(hostname);
	if (serverHostInfo == NULL)
	{
		StdError("CLIENT: ERROR, no such host\n");
	}

	// copy the address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

	// setup client socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFD < 0)
	{
		StdError("CLIENT: ERROR opening socket\n");
	}

	// connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		StdError("Could not contact server on selected port\n");
	}

	return socketFD;
}

/***********************************************************
 * Function: void SendAndRecv(int socketFD, char* username)
 * Manages all the sending and recieving of messages between
 * client and server. Keeps conversation going until the
 * connection is terminated by either side.
 * Sources: This function is mostly pulled from my CS344 
 * Project 4 code with some modifications. URL to my private 
 * repo is listed in the program header. Access is available 
 * upon request. 
 ***********************************************************/
void SendAndRecv(int socketFD, char* username)
{
	// declare some needed 
	int  charsRead, charsSent;
	int bufferSize = 512;
	char inputBuffer[bufferSize];
	char outputBuffer[bufferSize];

	// send username as initial handshake, throw error if something goes wrong
	if (charsSent = send(socketFD, username, strlen(username), 0) < 0)
	{
		StdError("CLIENT: ERROR sending client username\n");
	}

	while (1)
	{
		// clear buffers
		memset(inputBuffer, '\0', sizeof(inputBuffer));
		memset(outputBuffer, '\0', sizeof(outputBuffer));

		// let user type message to server
		printf("%s> ", username);
		fgets(inputBuffer, sizeof(inputBuffer), stdin);
		strtok(inputBuffer, "\n");

		// if escape message exit chat
		if (strcmp(inputBuffer, "\\quit") == 0)
		{
			send(socketFD, inputBuffer, strlen(inputBuffer), 0);
			printf("Disconnecting from server\n");
			break;
		}

		// send message to server, throw error if it isn't sent
		if (charsSent = send(socketFD, inputBuffer, strlen(inputBuffer), 0) < 0)
		{
			StdError("CLIENT: ERROR sending data to server\n");
		}

		// receive message from server
		if (charsRead = recv(socketFD, outputBuffer, sizeof(outputBuffer), 0) < 0)
		{
			StdError("CLIENT: ERROR receiving data from server\n");			
		}

		// if quit command received, close connection and exit loop
		if (strcmp(outputBuffer, "\\quit") == 0)
		{
			printf("***Server has terminated connection***\n");
			break;
		}

		// print received message from server
		printf("Server> %s\n", outputBuffer);
	}

	printf("Now exiting chatclient\n");

	// close socket
	close(socketFD);
}

int main(int argc, char* argv[])
{
	// greet user
	printf("***Welcome to ChatClient***\n");

	// throw error if wrong amount of arguments
	if (argc != 3)
	{
		StdError("Specify address and port number with chatclient call.\n");
	}

	// get username from user
	char username[512];
	int letterCount = 0;
	int hasSpace = 0;

	while (letterCount > 10 || letterCount < 1 || hasSpace)
	{
		// prompt user and get input
		printf("Enter your username. It must be one word and have a maximum of 10 characters\n");
		fgets(username, sizeof(username), stdin);

		// reset flags
		int i = 0;
		letterCount = 0;
		hasSpace = 0;

		// ensure username is right length and only one word
		while (username[i] != '\n')
		{
			letterCount++;
			if (username[i] == ' ')
			{
				hasSpace = 1;
			}
			i++;
		}
	}

	// remove trailing newline 
	strtok(username, " \n");	

	// create socket and connect to server	
	int socketFD = SetupConnection(username, argv[1], atoi(argv[2]));

	// send and receive messages with server
	SendAndRecv(socketFD, username);
}

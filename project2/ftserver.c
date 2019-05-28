/*********************************************************
 * Program: ftserver.c
 * Author: Joel Huffman
 * Last updated: 5/27/2019
 * Sources:
 * https://github.com/Huffmajo/CS344/tree/master/program4 *private repo, access available upon request
 * https://github.com/Huffmajo/CS372/tree/master/project1 *private repo, access available upon request
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
 * Function: serverSetup(portNum)
 * Accepts an int. Sets up server at the given port number.
 * Returns int of server socket.
 ***********************************************************/
int serverSetup(int portNum)
{
	int listenSocketFD, 
	    optval;
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress; 

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNum); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket

	// error if socket fails
	if (listenSocketFD < 0) 
	{
		stderror("ERROR opening socket\n");
	}

	// can re-use ports 
	optval = 1;
	setsockopt(listenSocketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	{
		stderror("ERROR on binding\n");
	}

	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	return listenSocketFD;
}

int main(int argc, char* argv[])
{
	int serverSocket;
	pid_t pid;
	sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
	int bufferSize = 150000;
	char buffer[bufferSize];

	// greet user
	printf("***Welcome to File Transfer Server***\n");

	// throw error if wrong amount of arguments
	if (argc != 2)
	{
		StdError("Specify port number with ftserver call.\n");
	}

	// create/setup server socket
	serverSocket = setupServer(atoi(argv[1]));

	// keep server listening until CTRL+C
	while(1)
	{
		// get size of the address for the connecting client
		sizeOfClientInfo = sizeof(clientAddress);

		// establish connection
		establishedConnectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo)
		if (establishedConnectionFD < 0)
		{
			// print error, but don't exit
			fprintf(stderr, "ERROR on accept\n");
		}

		// fork off child process for connection
		pid = fork();

		// error with fork
		if (pid == -1)
		{
			fprintf(stderr, "Fork error\n");
		}

		// child process
		else if (pid == 0)
		{

		}

		// parent process
		else 
		{
			// close socket to client
			close(establishedConnectionFD); 			
		}
	}
}

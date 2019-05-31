/*********************************************************
 * Program: ftserver.c
 * Author: Joel Huffman
 * Last updated: 5/30/2019
 * Sources:
 * https://github.com/Huffmajo/CS344/tree/master/program4 *private repo, access available upon request
 * https://github.com/Huffmajo/CS372/tree/master/project1 *private repo, access available upon request
 * https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

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

/***********************************************************
 * Function: receiveData(socket, buffer)
 * Accepts an int as a socket. Reads sent data from socket 
 * to a buffer. Then returns that buffer.
 ***********************************************************/
char* receiveData(int socket)
{
	char buffer[150000];
	memset(buffer, '\0', sizeof(buffer));

	// read data from socket to buffer
	int charsRead = recv(socket, &buffer, sizeof(buffer), 0);

	if (charsRead < 0)
	{
		fprintf(stderr, "ERROR receiving data on %d\n", socket);
	}

	return buffer;
}

/***********************************************************
 * Function: sendData(socket, buffer)
 * Accepts an int and a string. Sends data in buffer to 
 * provided socket.
 ***********************************************************/
void sendData(int socket, char* buffer)
{
	int charsSent = send(socket, buffer, strlen(buffer), 0);

	if (charsSent < 0)
	{
		fprintf(stderr, "ERROR sending data on %d\n, socket);
	}
}

/***********************************************************
 * Function: getDirListing()
 * Gets directory listing of current working directory and 
 * returns it in string form.
 ***********************************************************/
char* getDirListing()
{
	char buffer[150000];
	memset(buffer, '\0', sizeof(buffer));

	DIR* d;
	struct dirent* dir;
	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) 
		{
			strcat(buffer, d->d_name);
			strcat(buffer, "\t");
		}

		closedir(d);
	}
	return buffer;	
}

/***********************************************************
 * Function: sendFile(socket, filename)
 * Accepts an int and a string. Reads content of filename 
 * and sends those contents to client through provided 
 * socket. If file doesn't exist, "File not found" is sent to
 * client.
 ***********************************************************/
void sendFile(int socket, char* filename)
{
	char buffer[150000];
	memset(buffer, '\0', sizeof(buffer));

	File* fp;

	// attempt to open file 
	fp = fopen(filename, "r");

	// let client know if file does not exist
	if (fp == NULL)
	{
		strcpy(buffer, "File not found\n");
	}

	// otherwise read and send file contents
	else
	{
		fgets(buffer, sizeof(buffer), fp);
	}

	fclose(fp);

	sendData(socket, buffer);
}

int main(int argc, char* argv[])
{
	int serverSocket;
	int clientSocket;
	pid_t pid;
	sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
	int bufferSize = 150000;
	char buffer[bufferSize];
	char* command;
	char* filename;

	// greet user
	printf("***Welcome to File Transfer Server***\n");

	// throw error if wrong amount of arguments
	if (argc != 2)
	{
		StdError("Specify port number with ftserver call.\n");
	}

	// create/setup server socket
	serverSocket = setupServer(atoi(argv[1]));

	// print confirmation of server setup
	printf("Server open on port %d"\n, atoi(argv[1]));

	// keep server listening until CTRL+C
	while(1)
	{
		// get size of the address for the connecting client
		sizeOfClientInfo = sizeof(clientAddress);

		// establish connection
		establishedConnectionFD = accept(serverSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo)
		if (establishedConnectionFD < 0)
		{
			// print error, but don't exit
			fprintf(stderr, "ERROR on control accept\n");
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
			// get TCP data portNum
			int dataPortNum = atoi(receiveData(establishedConnectionPD));

			// get client host
			char* clientHost = receiveData(establishedConnectionPD);

			// get command from client
			command = receiveData(establishedConnectionPD));

			// get filename from client
			filename = receiveData(establishedConnectionPD));

			// let user know if command is invalid
			if (strcmp(commandPrefix, "-l") != 0 && strcmp(commandPrefix, "-g") != 0)
			{
				sendData(establsihedConnectionPD, "Invalid command\n");
			}

			else
			{
				// form TCP data connection with client	
				clientSocket = setupServer(atoi(dataPortNum));
				int establishedDataConnectionFD = accept(clientSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo)

				if (establishedDataConnectionFD < 0)
				{
					// print error, but don't exit
					fprintf(stderr, "ERROR on data accept\n");
				}

				// print confirmation of server setup
				printf("Connection from %s\n", clientHost);

				// send directory listing if properly requested
				if (strcmp(commandPrefix, "-l") == 0)
				{
					// get current directory listing
					buffer = getDirListing();

					// send to client
					sendData(EstablishedDataConnectionFD, buffer);
				}

				// send file if properly requested
				else if (strcmp(commandPrefix, "-g") == 0)
				{
					// send file contents or message if file is not found
					sendFile(establishedDataConnectionFD, filename); 
				}
			
				//close data connection
				close(establishedDataConnectionFD);
			}
		}

		// parent process
		else 
		{
			// nothing needed here
		}

		// close data connection
		
	}
}

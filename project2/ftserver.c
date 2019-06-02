/*********************************************************
 * Program: ftserver.c
 * Author: Joel Huffman
 * Last updated: 6/2/2019
 * Sources:
 * https://github.com/Huffmajo/CS344/tree/master/program4 *private repo, access available upon request
 * https://github.com/Huffmajo/CS372/tree/master/project1 *private repo, access available upon request
 * https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 *********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <dirent.h>

/***********************************************************
 * Function: stderror(err)
 * Accepts a string. Sends that string to standard error and
 * exits with value 1.
 ***********************************************************/
void stderror(const char* string) 
{ 
	fprintf(stderr, string); 
	exit(1); 
}

/***********************************************************
 * Function: serverSetup(portNum)
 * Accepts an int. Connects at the given port number.
 * Returns int of server socket.
 ***********************************************************/
int serverSetup(int portNum)
{
	int listenSocketFD, optval;
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
void receiveData(int socket, char* buffer)
{
	// clear out buffer before writing to it
	memset(buffer, '\0', sizeof(buffer));

	// read data from socket into buffer
	int charsRead;
	charsRead = recv(socket, buffer, 50000, 0);

	if (charsRead < 0)
	{
		fprintf(stderr, "ERROR receiving data on %d\n", socket);
	}
	printf("Received %s\n", buffer);
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
		fprintf(stderr, "ERROR sending data on %d\n", socket);
	}

	printf("Sent %s\n", buffer);
}

/***********************************************************
 * Function: getDirListing(buffer)
 * Gets directory listing of current working directory and 
 * returns it in string form.
 ***********************************************************/
char* getDirListing(char* buffer)
{
	// clear buffer before writing to it
	memset(buffer, '\0', sizeof(buffer));

	struct dirent* dir;
	DIR* d;
	d = opendir(".");
	if (d) {
		while ((dir = readdir(d)) != NULL) 
		{
			strcat(buffer, dir->d_name);
			strcat(buffer, "\n");
		}

	}
	closedir(d);
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

	FILE* fp;

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

/***********************************************************
 * Function: parseMessage(socket, filename)
 * Accepts a string message and several params used for 
 * output. Splits message at delimiters to distinuguish info 
 * from one another. Returns dataport number
 ***********************************************************/
int parseMessage(char* message, char* clientHost, char* command, char* filename)
{
	// separate data port number
	char* token = strtok(message, "!");
	int dataPort = atoi(token);

	// separate client host name
	token = strtok(NULL, "!");
	strcpy(clientHost, token);	

	// separate command
	token = strtok(NULL, "!");
	strcpy(command, token);	

	// separate filename only if command is -g
	if (strcmp(command, "-g") == 0)
	{
		token = strtok(NULL, "!");
		strcpy(filename, token);	
	}

	return dataPort;
}

int main(int argc, char* argv[])
{
	int serverSocket;
	int clientSocket;
	int clientPortNum;
	pid_t pid;
	sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
	int bufferSize = 50000;
	char buffer[bufferSize];
	char* command;
	char* filename;

	// greet user
	printf("***Welcome to File Transfer Server***\n");

	// throw error if wrong amount of arguments
	if (argc != 2)
	{
		stderror("Specify port number with ftserver call.\n");
	}

	// get port number from arguments
	clientPortNum = atoi(argv[1]);

	// create/setup server socket
	serverSocket = serverSetup(clientPortNum);

	// print confirmation of server setup
	printf("Server open on port %d\n", clientPortNum);

	// keep server listening until CTRL+C
	while(1)
	{
		printf("Waiting for client to connect\n");

		// get size of the address for the connecting client
		sizeOfClientInfo = sizeof(clientAddress);

		// establish connection
		int establishedConnectionFD = accept(serverSocket, NULL, NULL);
		if (establishedConnectionFD < 0)
		{
			// print error, but don't exit
			fprintf(stderr, "ERROR on control accept\n");
		}

		printf("client connected on %d\n", clientPortNum);

		// fork off child process for connection
		pid = fork();

		// error with fork
		if (pid == -1)
		{
			fprintf(stderr, "Fork error\n");
			printf("FORK ERROR\n");
		}

		// child process
		else if (pid == 0)
		{
			int dataPortNum;
			char clientHost[50000];
			char command[50000];
			char filename[50000];

			printf("In child process\n");

			// get grouped client data
			receiveData(establishedConnectionFD, buffer);

			// split up data at delimiters
			dataPortNum = parseMessage(buffer, clientHost, command, filename);

			// TEST PRINTS
			printf("dataPort: %d\nclientHost: %s\ncommand: %s\nfilename: %s\n", dataPortNum, clientHost, command, filename);

			// check for command validity
			if (strcmp(command, "-l") == 0 || strcmp(command, "-g") == 0)
			{
				// send valid command message
				strcpy(buffer, "Valid command");
				sendData(establishedConnectionFD, buffer);				

				// needed for pythons stream-style sending
				receiveData(establishedConnectionFD, buffer);

				// form TCP data connection with client	
				clientSocket = serverSetup(dataPortNum);
				int establishedDataConnectionFD = accept(clientSocket, NULL, NULL);

				if (establishedDataConnectionFD < 0)
				{
					// print error, but don't exit
					fprintf(stderr, "ERROR on data accept\n");
				}

				// print confirmation of server setup
				printf("Connection from %s\n", clientHost);

				// send directory listing if properly requested
				if (strcmp(command, "-l") == 0)
				{
					// get current directory listing
					getDirListing(buffer);

					// send to client
					sendData(establishedDataConnectionFD, buffer);
				}

				// send file if properly requested
				else if (strcmp(command, "-g") == 0)
				{
					// send file contents or message if file is not found
					sendFile(establishedDataConnectionFD, filename); 
				}
			
				//close data connection
				close(establishedDataConnectionFD);
			}

			// otherwise send invalid command message to client
			else
			{
				strcpy(buffer, "Invalid command");
				sendData(establishedConnectionFD, buffer);
			}
		}

		// parent process
		else 
		{
			// nothing needed here
		}

		// close connection
		//close(establishedConnectionFD);
	}
}

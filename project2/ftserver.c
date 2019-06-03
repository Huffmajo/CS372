/*********************************************************
 * Program: ftserver.c
 * Author: Joel Huffman
 * Last updated: 6/2/2019
 * Sources:
 * https://github.com/Huffmajo/CS344/tree/master/program4 *private repo, access available upon request
 * https://github.com/Huffmajo/CS372/tree/master/project1 *private repo, access available upon request
 * https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 * https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
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

#define BUFFERSIZE 150000

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
 * Accepts an int as a socket and char* as a string. Reads 
 * sent data from socket to the buffer. 
 ***********************************************************/
void receiveData(int socket, char* buffer)
{
	// clear out buffer before writing to it
	memset(buffer, '\0', sizeof(buffer));

	// read data from socket into buffer
	int charsRead;
	charsRead = recv(socket, buffer, BUFFERSIZE, 0);

	if (charsRead < 0)
	{
		fprintf(stderr, "ERROR receiving data on %d\n", socket);
	}
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
 * Function: sendFile(socket, filename, clientHost, dataPortNum)
 * Accepts an int, 2 strings and another int. Reads content 
 * of filename and sends those contents to client through the 
 * provided socket. If the file doesn't exist, "File not found" 
 * is sent to the client.
 ***********************************************************/
void sendFile(int socket, char* filename, char* clientHost, int dataPortNum)
{
	char buffer[BUFFERSIZE + 1];
	memset(buffer, '\0', sizeof(buffer));

	// attempt to open file 
	FILE* fp = fopen(filename, "r");

	// let client know if file does not exist
	if (fp == NULL)
	{
		strcpy(buffer, "File not found");

		// print what is happening
		printf("File not found. Sending error message to %s:%d\n", clientHost, dataPortNum);

		sendData(socket, buffer);
	}

	// otherwise read and send file contents
	else
	{
		size_t newLen = fread(buffer, sizeof(char), BUFFERSIZE, fp);
		if ( ferror( fp ) != 0 )
		{
			fputs("Error reading file", stderr);
    		} 
		else 
		{
			buffer[newLen++] = '\0'; /* Just to be safe. */
		}

		// print what is happening
		printf("Sending \"%s\" to %s:%d\n", filename, clientHost, dataPortNum);

		sendData(socket, buffer);
	}
	fclose(fp);

	//close data connection
	close(socket);
}

/***********************************************************
 * Function: parseMessage(message, clientHost, command, filename)
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

/***********************************************************
 * Function: dataResponse(command, clientHost, filename, dataPortNum, clientSocket, establishedConnectionFD)
 * Accepts 3 strings and 3 ints. Creates and connects to the 
 * TCP data socket if needed. Then sends either the directory
 * listing or file contents to the client.
 ***********************************************************/
void dataResponse(char* command, char* clientHost, char* filename, int dataPortNum, int clientSocket, int establishedConnectionFD)
{
	char buffer[BUFFERSIZE];

	// check for command validity
	if (strcmp(command, "-l") == 0 || strcmp(command, "-g") == 0)
	{
		// send valid command message
		strcpy(buffer, "Valid command");
		sendData(establishedConnectionFD, buffer);				

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
			// print what is happening
			printf("List directory requested on port %d.\n", dataPortNum);

			// get current directory listing
			getDirListing(buffer);

			// print what is happening
			printf("Sending directory contents to %s:%d\n", clientHost, dataPortNum);

			// send to client
			sendData(establishedDataConnectionFD, buffer);

			close(establishedDataConnectionFD);
		}

		// send file if properly requested
		else if (strcmp(command, "-g") == 0)
		{
			// print what is happening
			printf("File \"%s\" requested on port %d.\n", filename, dataPortNum);

			// send file contents or message if file is not found
			sendFile(establishedDataConnectionFD, filename, clientHost, dataPortNum);
 
			close(establishedDataConnectionFD);
		}
	}

	// otherwise send invalid command message to client
	else
	{
		strcpy(buffer, "Invalid command");
		sendData(establishedConnectionFD, buffer);
	}

	close(establishedConnectionFD);
}


int main(int argc, char* argv[])
{
	int serverSocket;
	int clientSocket;
	int clientPortNum;
	pid_t pid;
	sockaddr_in clientAddress;
	socklen_t sizeOfClientInfo;
	char buffer[BUFFERSIZE];
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
	printf("Server open on %d\n", clientPortNum);

	// keep server listening until CTRL+C
	while(1)
	{
		// establish connection
		int establishedConnectionFD = accept(serverSocket, NULL, NULL);
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
			printf("FORK ERROR\n");
		}

		// child process
		else if (pid == 0)
		{
			int dataPortNum;
			char clientHost[BUFFERSIZE];
			char command[BUFFERSIZE];
			char filename[BUFFERSIZE];

			// get grouped client data
			receiveData(establishedConnectionFD, buffer);

			// split up data at delimiters
			dataPortNum = parseMessage(buffer, clientHost, command, filename);

			// print client's hostname
			printf("Connection from %s\n", clientHost);

			// handle client request
			dataResponse(command, clientHost, filename, dataPortNum, clientSocket, establishedConnectionFD);
		}

		// close connection
		close(establishedConnectionFD);
	}
}

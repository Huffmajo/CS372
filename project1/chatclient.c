/*********************************************************
 * Program: chatclient.c
 * Author: Joel Huffman
 * Last updated: 4/30/2019
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
 * Function: stderror(err)
 * Accepts a string. Sends that string to standard error and
 * exits with value 1.
 ***********************************************************/
void stderror(const char* string) 
{ 
	fprintf(stderr, string); 
	exit(1); 
}


int main(int argc, char* argv[])
{
	// greet user
	printf("***Welcome to ChatClient***\n");

	// throw error if wrong amount of arguments
	if (argc != 3)
	{
		stderror("Specify address and port number with chatclient call.\n");
	}

	// have user enter username
	printf("Choose your username. It must be 1-10 characters and one word");
	char* username;
	scanf("%s", username);

}

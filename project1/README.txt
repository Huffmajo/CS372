1. Ensure you have permissions to execute the program files:

	chmod +x chatclient.c chatserve.py makefile

2. Run the makefile and compile chatclient.c into something you can execute: 

	make

3. Spin up the server with whatever port number you'd like to test on:

	python3 chatserve.py [PORTNUM]

   Example: 

	python 3 chatserve.py 45600 


4. Spin up the client with the hostname you are testing this on (all testing performed on flip1.engr.oregonstate.edu) and the port number used above:
	
	chatclient [HOSTNAME] [PORTNUM]

  Example:

	chatclient flip1.engr.oregonstate.edu 45600

NOTES:	- Messages sent need to alternate between client and server, starting with the client.
	- Enter "/quit" as a message from either source to close the connection.
	- Server must receive a SIGINT (ctrl + c) to exit.	

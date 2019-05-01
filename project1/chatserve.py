##########################################################
# Program: chatserve.py
# Author: Joel Huffman
# Last updated: 4/30/2019
# Sources: https://media.oregonstate.edu/media/t/0_hh05nevl
# https://docs.python.org/release/2.6.5/library/internet.html
# https://beej.us/guide/bgnet/html/single/bgnet.html
##########################################################

from socket import *
import sys

# 
def SendAndRecv(connectionSocket):

	# get username from client
	clientUserName = connectionSocket.recv(1024)
	
	# take turns receiving and sending ad infinitum
	while 1:

		# get message from client
		recvMsg = connectionSocket.recv(1024)
	
		# print received message
		f"{clientUserName}> {recvMsg}"

		# let user type message to send to client
		print("Server> ")
		sendMsg = input()

		# allow disconnection if quit command is used
		if sendMsg == "\quit":
			print("Connection closed")
			break

		# otherwise, send the message to client
		connectionSocket.send(sendMsg)		
	

# main function
if __name__ == "__main__":
	# greet user
	print ("***Welcome to ChatServe***")

	# check that potential port number was included in call
	if len(sys.argv) != 2:
		print ("Specify port number with chatserve.py call.")
		exit(1)

	# get port number
	serverPort = sys.argv[1]

	# verify port number
	if serverPort.isdigit() == False:
		print ("Included argument is not an integer. Please call charserve.py with a port number from 1024-65535.")
		exit(1)

	# get hostname
	serverHost = gethostname()

	# setup server socket
	serverSocket = socket(AF_INET, SOCK_STREAM)

	# bind socket to port number
	serverSocket.bind((serverHost, int(serverPort)))

	# listen at bound port for client
	serverSocket.listen(1)

	print ("The server is ready to receive")
	
	# keep connection open until ctrl + c interrupt
	while 1:
		connectionSocket, addr = serverSocket.accept()
		f"Connected on {addr}"

		SendAndRecv(connectionSocket)	

		connectionSocket.close()

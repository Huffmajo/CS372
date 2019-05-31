#!/usr/bin/python3
##########################################################
# Program: ftclient.py
# Author: Joel Huffman
# Last updated: 5/30/2019
# Sources: https://media.oregonstate.edu/media/t/0_hh05nevl - Most of socket and connection setup
# https://docs.python.org/release/2.6.5/library/internet.html
# https://beej.us/guide/bgnet/html/single/bgnet.html
# https://www.tutorialspoint.com/python3/python_networking.htm - Great for translating to Python3
# https://github.com/Huffmajo/CS372/tree/master/project1
##########################################################

from socket import *
import sys

# 
def setupClient(portNum):
	

# encode and send data
def send(socket, msg):
	socket.send(msg.encode('utf-8'))	

# receive and decode data
def receive(socket):
	msg = (socket.recv(150000)).decode()
	return msg

# main function
if __name__ == "__main__":

	# greet user
	print ("***Welcome to File Transfer Client***")

	# check that potential port number was included in call
	if len(sys.argv) > 5 or len(sys.argv) < 4:
		print ("Specify server host, server port number, command, filename (if command is -g) and data port number with chatserve.py call.")
		exit(1)

	# get the submitted arguments
	serverHost = int(sys.argv[1])
	serverPortNum = int(sys.argv[2])
	command = int(sys.argv[3])

	# if requesting directory listing, data port number is next
	if command == "-l":
		dataPortNum = int(sys.argv[4])
		filename = NULL

	# otherwise expect a filename before the data port number		
	else:
		filename = int(sys.argv[4])
		dataPortNum = int(sys.argv[5])

	# connect to server
	clientSocket = socket(AF_INET, SOCK_STREAM)

	# bind socket to port number
	# MIGHT NEED TO REPLACE '' WITH serverHost
	clientSocket.bind(('', int(serverPortNum)))

	connectionSocket, addr = clientSocket.accept()

	# send data port number
	send(connectionSocket, dataPortNum)

	# send client host
	clientHost = gethostname()
	send(connectionSocket, clientHost)

	# send command
	send(connectionSocket, dataPortNum)

	# send filename
	send(connectionSocket, dataPortNum)

	# receive command response
	# MIGHT HAVE TO DO THIS OVER THE DATA CONNECTION
	response = receive(connectionSocket)

	if response == "File not found":
		print("%s:%s says FILE NOT FOUND" % serverHost, str(serverPortNum))
	elif command == "-l":
		print("Receiving directory structure from %s:%s" % serverHost, str(dataPortNum))
		print(response)
	elif command == "-g":
		print("File transfer complete")

	# close control connection
	connectionSocket.close()

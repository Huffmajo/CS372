#!/usr/bin/python3
##########################################################
# Program: ftclient.py
# Author: Joel Huffman
# Last updated: 5/30/2019
# Sources: https://media.oregonstate.edu/media/t/0_hh05nevl - Most of socket and connection setup
# https://docs.python.org/release/2.6.5/library/internet.html
# https://beej.us/guide/bgnet/html/single/bgnet.html
# https://www.tutorialspoint.com/python3/python_networking.htm - Great for translating to Python3
# https://github.com/Huffmajo/CS372/tree/master/project1 *access to private repo is available upon request
##########################################################

from socket import *
# import socket
import sys
import time
# 
def setupClient(host, portNum):
	clientSocket = socket(AF_INET, SOCK_STREAM)
	try:
		clientSocket.connect((host, portNum))
	except:
		print("Error connecting to %s:%s" % (host, str(portNum)))
	error("Connected to %s:%s" % (host, str(portNum)), 1)
	return clientSocket

# encode and send data
def send(socket, msg):
	msg = str(msg)
#	msg = msg.encode('utf-8')
	socket.send(msg.encode())
	print("Sent %s" % msg)


# receive and decode data
def receive(socket, size):
	msg = socket.recv(size)
	msg = msg.decode()
	print("Received %s" % msg)
	return msg

# main function
if __name__ == "__main__":

	# greet user
	print ("***Welcome to File Transfer Client***")

	# check if valid number of arguments
	if len(sys.argv) > 6 or len(sys.argv) < 5:
		print ("Specify server host, server port number, command, filename (if command is -g) and data port number with chatserve.py call.")
		exit(1)

	# get the submitted arguments
	serverHost = sys.argv[1] + ".engr.oregonstate.edu"
	serverPortNum = int(sys.argv[2])
	command = sys.argv[3]

	# if requesting directory listing, data port number is next and no filename is needed
	if command == "-l":
		dataPortNum = int(sys.argv[4])
		filename = ""
		commandResponse = "Valid command"

	# if get file command, expect a filename before the data port number		
	elif command == "-g":
		filename = sys.argv[4]
		dataPortNum = int(sys.argv[5])
		commandResponse = "Valid command"

	
	# otherwise tell user to it's an invalid command
	else:
		filename = ""
		dataPortNum = ""
		commandResponse = "Valid command"

	# TEST PRINTS
	print("Server host: %s" % serverHost)
	print("Server port #: %s" % serverPortNum)
	print("Command: %s" % command)
	print("Data port #: %s" % dataPortNum)
	print("Filename: %s" % filename)

	# connect to server
	connectionSocket = setupClient(serverHost, serverPortNum);

	# send data port number
	send(connectionSocket, dataPortNum)

	# needed for pythons stream-style sending
	garbage = receive(connectionSocket, 10)

	# send client host
	clientHost = gethostname()
	send(connectionSocket, clientHost)

	# needed for pythons stream-style sending
	garbage = receive(connectionSocket, 10)

	# send command
	send(connectionSocket, command)

	# needed for pythons stream-style sending
	garbage = receive(connectionSocket, 10)

	# send filename
	send(connectionSocket, filename)

	# receive if server accepts command
	commandResponse = receive(connectionSocket, 1000)

	# needed for pythons stream-style sending
	send (connectionSocket, garbage)

	# continue with function if command is deemed valid
	if commandResponse == "Valid command":
		time.sleep(.1)

		# connect to server for data communication
		dataSocket = setupClient(serverHost, dataPortNum);

		# receive command response
		# **NEED TO DO THIS OVER THE DATA CONNECTION**
		response = receive(dataSocket, 50000)

		if response == "File not found":
			print("%s:%s says FILE NOT FOUND" % (serverHost, str(dataPortNum)))
		elif command == "-l":
			print("Receiving directory structure from %s:%s" % (serverHost, str(dataPortNum)))
			print(response)
		elif command == "-g":
			print("File transfer complete")
		dataSocket.close()

	# otherwise, close sockets
	else:
		print("Invalid command: %s. Valid commands are -l and -g." % command)

	# close both connections
	connectionSocket.close()

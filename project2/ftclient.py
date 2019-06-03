#!/usr/bin/python3
##########################################################
# Program: ftclient.py
# Author: Joel Huffman
# Last updated: 6/2/2019
# Sources: https://media.oregonstate.edu/media/t/0_hh05nevl - Most of socket and connection setup
# https://docs.python.org/release/2.6.5/library/internet.html
# https://beej.us/guide/bgnet/html/single/bgnet.html
# https://www.tutorialspoint.com/python3/python_networking.htm - Great for translating to Python3
# https://github.com/Huffmajo/CS372/tree/master/project1 *access to private repo is available upon request
# https://www.geeksforgeeks.org/python-string-split/
# https://www.tutorialspoint.com/python3/python_files_io.htm
# https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data/17668009#17668009
# https://stackoverflow.com/questions/8772142/converting-to-upper-case-which-way-is-more-pythonic
##########################################################

from socket import *
# import socket
import sys
import time

BUFFERSIZE = 150000

##########################################################
# Function: setupClient(host, portNum)
# Accepts a hostname and port number. Creates a socket at
# that port and connects to the host at that socket.
##########################################################
def setupClient(host, portNum):
	clientSocket = socket(AF_INET, SOCK_STREAM)
	try:
		clientSocket.connect((host, portNum))
	except:
		print("Error connecting to %s:%s" % (host, str(portNum)))
		exit(1)
	return clientSocket

##########################################################
# Function: send(socket, msg)
# Accepts a socket and message to be sent. Converts the
# message into a string, encodes it and sends it through
# the socket.
##########################################################
def send(socket, msg):
	msg = str(msg)
	socket.send(msg.encode())

##########################################################
# Function: receive(socket, size)
# Accepts a socket and an int. Receives a message through
# the socket of the given size. Then decodes the message 
# before returning it.
##########################################################
def receive(socket, size):
	msg = socket.recv(size)
	msg = msg.decode()
	return msg

##########################################################
# Function: handleResponse(response, filename, serverHost, dataPortNum, dataSocket)
# Accepts response string from server, filename, server 
# host name, data port number and data socket. Prints the
# directory listing sent from the server or copies content
# from chosen file depending on the command.
##########################################################
def handleResponse(response, filename, serverHost, dataPortNum, dataSocket):
	if response == "File not found":
		print("%s:%s says FILE NOT FOUND" % (serverHost, str(dataPortNum)))
		dataSocket.close()

	elif command == "-l":

		print("Receiving directory structure from %s:%s" % (serverHost, str(dataPortNum)))
		print(response)
		dataSocket.close()

	elif command == "-g":

		# if filename doesn't have an extension, append _copy to it's name
		if filename.find(".") == -1:
			filenameCopy = filename + "_copy"

		# otherwise, add _copy to filename before extension
		else:
			parts = filename.split('.', 1)
			filenameCopy = parts[0] + "_copy." + parts[1]

		# loop until valid choice is selected
		choice = "X"
		while choice != "Y" and choice != "N":

			# let user choose to create new file or not
			choice = input("File \"%s\" already exists. Save as \"%s\" Y/N?" % (filename, filenameCopy))
			choice = choice.upper()

		if choice == "Y": 
			# make copy of file with updated name
			copiedFile = open(filenameCopy, "w")
			
			# inject copied contents into new file
			copiedFile.write(response)

			# close new file
			copiedFile.close()

			print("File transfer complete")
			
		elif choice == "N":
			print("File transfer cancelled")

		dataSocket.close()



##########################################################
# Function: main
# Accepts 5 or 6 arguments from CLI including server host
# name, control port number, command, (optionally) filename
# and data port number. Controls main workflow of program
# from here and directs when to run other functions. 
##########################################################
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
	filename = ""
	dataPortNum = 0

	# if requesting directory listing, data port number is next and no filename is needed
	if command == "-l":
		dataPortNum = int(sys.argv[4])
		filename = ""

	# if get file command, expect a filename before the data port number		
	elif command == "-g":
		filename = sys.argv[4]
		dataPortNum = int(sys.argv[5])

	# connect to server
	connectionSocket = setupClient(serverHost, serverPortNum);

	# condense info into a single delimited string and send
	message = str(dataPortNum) + "!" + gethostname() + "!" + command + "!" + filename
	send(connectionSocket, message)

	time.sleep(.5)

	# receive if server accepts command
	commandResponse = receive(connectionSocket, BUFFERSIZE)

	# continue with function if command is deemed valid
	if commandResponse == "Valid command":
		time.sleep(.5)

		# connect to server for data communication
		dataSocket = setupClient(serverHost, dataPortNum)

		# receive command response
		response = receive(dataSocket, BUFFERSIZE)
	
		# apply appropriate actions per command
		handleResponse(response, filename, serverHost, dataPortNum, dataSocket)

	# otherwise, close sockets
	else:
		print("Invalid command: %s. Valid commands are -l and -g." % command)

	# close connection
	connectionSocket.close()

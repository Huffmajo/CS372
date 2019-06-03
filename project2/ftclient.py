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
# https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
##########################################################

from socket import *
# import socket
import sys
import time

# create and connect to socket
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

	# otherwise it's an invalid command
	else:
		filename = ""
		dataPortNum = 0
		commandResponse = "Invalid command"

	# connect to server
	connectionSocket = setupClient(serverHost, serverPortNum);

	# condense info into a single delimited string
	message = str(dataPortNum) + "!" + gethostname() + "!" + command + "!" + filename

	# send condesnsed client info
	send(connectionSocket, message)

	time.sleep(.5)

	# receive if server accepts command
	commandResponse = receive(connectionSocket, 50001)

	# continue with function if command is deemed valid
	if commandResponse == "Valid command":
		time.sleep(.5)

		# connect to server for data communication
		dataSocket = setupClient(serverHost, dataPortNum)

		print("Connecting @ %d" % dataPortNum)

		# receive command response
		# **NEED TO DO THIS OVER THE DATA CONNECTION**
		response = receive(dataSocket, 50001)

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

	# otherwise, close sockets
	else:
		print("Invalid command: %s. Valid commands are -l and -g." % command)

	# close connection
	connectionSocket.close()

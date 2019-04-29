##########################################################
# Program: chatserve.py
# Author: Joel Huffman
# Last updated: 4/28/2019
# Sources: https://media.oregonstate.edu/media/t/0_hh05nevl
# https://docs.python.org/release/2.6.5/library/internet.html
# https://beej.us/guide/bgnet/html/single/bgnet.html
##########################################################

from socket import *

##########################################################
# Function: setupServer()
# 
def setupServer():
	# get port number

	while 1:
		print "Please enter an integer between 1024-65535 to use as a port number:"

		serverPort = input()
		if (serverPort.isdigit() == false):
			print "Input is not an integer. Try again."
		elif (serverPort > 65535 or serverPort < 1024):
			print "Port number is not in the accepted range. Try again."
		else:
			break

# greet user
print "***Welcome to ChatServe***"

serverPort

while 1:
	while 1:
		setupServer();

	


	serverSocket = socket(AF_INET, SOCK_STREAM)
	serverSocket.bind(('', serverport))
	serverSocket.listen(1)
	print "The server is ready to receive"
	# while 1:
	#	connectionSocket, addr = serverSocket.accept()
	#	sentence = connectionSocket.recv(1024)
	#	capitalizedSentence = sentence.upper()
	#	connectionSocket.send(capitalizedSentence)
	#	connectionSocket.close()

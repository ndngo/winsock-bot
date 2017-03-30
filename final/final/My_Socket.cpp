#include "My_Socket.h"

MySocket::MySocket(SocketType socketType, std::string ip, unsigned int port, ConnectionType connectionType, unsigned int size) {
	//Constructor that configures the socket


}

MySocket::~MySocket() {
	//Closes Sockets
	//Cleans up memory
}

void MySocket::ConnectTCP() {
	//Ack Syn Ack - Checks ConnectionType
}

void MySocket::DisconnectTCP() {
	//Fin Ack Fin Ack
}

void MySocket::SendData(const char * buffer, int size) {
	//Send block of data
}

int MySocket::GetData(char * buffer) {
	//Receives Data
}

std::string MySocket::GetIPAddr() {
	//Return IP addr of MySocket
}

void MySocket::SetIPAddr(std::string ip) {
	//Change default IP addr
}

void MySocket::SetPort(int port) {
	// PURPLE - Changes the default port number
}

int MySocket::GetPort() {
	//Returns port number
}

SocketType MySocket::GetType() {
	//Returns SocketType
}

void MySocket::SetType(SocketType socketType) {
	//Changes the default SocketsType
}

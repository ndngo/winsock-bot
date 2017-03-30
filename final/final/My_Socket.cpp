#include "My_Socket.h"

MySocket::MySocket(SocketType socketType, std::string ip, unsigned int port, ConnectionType _connectionType, unsigned int size) {
	//Constructor that configures the socket
	connectionType = _connectionType;
	mySocket = socketType;
	Port = port;
	IPAddr = ip;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(Port);
	SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
	if(mySocket == SERVER) {
		if (connectionType == TCP) {
			WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (WelcomeSocket == INVALID_SOCKET) {
				WSACleanup();
			}


			if (bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
			{
				closesocket(WelcomeSocket);
				WSACleanup();
			}

			if (listen(WelcomeSocket, 1) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
			}

		}
		else if (connectionType == UDP)
		{
			WelcomeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (WelcomeSocket == INVALID_SOCKET) {
				WSACleanup();
			}


			if (bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
			{
				closesocket(WelcomeSocket);
				WSACleanup();
			}

		}
	}else if (mySocket == CLIENT)
	{
		ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectionSocket == INVALID_SOCKET) {
			WSACleanup();
		}

		if ((connect(ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(ConnectionSocket);
			WSACleanup();
		}
	}
	if (size > 0)
	{
		Buffer = new char[size];
	}
	else {
		Buffer = new char[DEFAULT_SIZE];
	}

}

MySocket::~MySocket() {
	//Closes Sockets
	//Cleans up memory
	closesocket(this->ConnectionSocket);
	closesocket(this->WelcomeSocket);
	delete this->Buffer;
	Buffer = nullptr;
}

void MySocket::ConnectTCP() {


	if (GetType() == TCP) {
		if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(this->ConnectionSocket);
			WSACleanup();
			std::cout << "Could not connect to the server" << std::endl;
			std::cin.get();
			exit(0);
		}
	}

	//Ack Syn Ack - Checks ConnectionType
	// handshake
}

void MySocket::DisconnectTCP() {
	//Fin Ack Fin Ack

	// disconnect
}

void MySocket::SendData(const char * buffer, int size) {
	//Send block of data
}

int MySocket::GetData(char * buffer) {
	//Receives Data
}

std::string MySocket::GetIPAddr() {
	//Return IP addr of MySocket
	return IPAddr;
}

void MySocket::SetIPAddr(std::string ip) {
	//Change default IP addr
	IPAddr = ip;
	SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
}

void MySocket::SetPort(int port) {
	// PURPLE - Changes the default port number
	Port = port;
	SvrAddr.sin_port = htons(Port);
}

int MySocket::GetPort() {
	//Returns port number
	return Port;
}

SocketType MySocket::GetType() {
	//Returns SocketType
	return mySocket;
}

void MySocket::SetType(SocketType socketType) {
	//Changes the default SocketsType
	mySocket = socketType;
}

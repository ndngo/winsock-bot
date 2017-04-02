#include "MySocket.h"

MySocket::MySocket(SocketType socketType, std::string ip, unsigned int port, ConnectionType connectType, unsigned int size) {
	//Constructor that configures the socket
	// initialization list
	connectionType = connectType;
	mySocket = socketType;
	Port = port;
	IPAddr = ip;
	
	if (size > DEFAULT_SIZE) {
		MaxSize = DEFAULT_SIZE;
	} else {
		MaxSize = size;
	}
	Buffer = new char[MaxSize];

	// if invalid size, set size to DEFAULT_SIZE
	
	
	// INIT WINSOCK DLLS
	WSADATA wsaData;

	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		exit(0);
	}

	// SOCKET ADDR
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_port = htons(Port);



	if(mySocket == SERVER) {
		SvrAddr.sin_addr.s_addr = INADDR_ANY;
		 //MAKE SOCKET
		if (connectionType == TCP) {
			WelcomeSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (WelcomeSocket == INVALID_SOCKET) {
				WSACleanup();
			}

			// BIND
			if (bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
			}

			// LISTEN

			if (listen(WelcomeSocket, 1) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
			}

			// ACCEPT

			if ((ConnectionSocket = accept(WelcomeSocket, NULL, NULL)) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
				return;
			}

		} else if (connectionType == UDP) {
			SvrAddr.sin_addr.s_addr = INADDR_ANY;
			WelcomeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (WelcomeSocket == INVALID_SOCKET) {
				WSACleanup();
			}


			if (bind(WelcomeSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
				closesocket(WelcomeSocket);
				WSACleanup();
			}

		}
	} else if (mySocket == CLIENT) {
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
		//MAKE SOCKET
		ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectionSocket == INVALID_SOCKET) {
			WSACleanup();
		}
	}
}

MySocket::~MySocket() {
	//Closes Sockets
	//Cleans up memory
	delete Buffer;
	Buffer = nullptr;
	WSACleanup();
}

void MySocket::ConnectTCP() {
	if (GetType() == TCP) {
		char buffer[32];
		if ((connect(this->ConnectionSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
			closesocket(this->ConnectionSocket);
			WSACleanup();
			std::cout << "Could not connect to the server" << std::endl;
			std::cin.get();
			exit(0);
		}
	}
}

void MySocket::DisconnectTCP() {
	// disconnect
	closesocket(this->ConnectionSocket);
	closesocket(this->WelcomeSocket);
}

void MySocket::SendData(const char * buffer, int size) {
	//Send block of data

	if (connectionType == TCP) {
		send(ConnectionSocket, buffer, size, 0);
	} else if (connectionType == UDP) {
		sendto(ConnectionSocket, buffer, size, 0, (sockaddr *)&SvrAddr, sizeof(SvrAddr));
	}
}

int MySocket::GetData(char * buffer) {
	//Receives Data
	int addr_len = sizeof(SvrAddr);
	int size = 0;
	if (connectionType == TCP) {
		size = recv(ConnectionSocket, Buffer, MaxSize, 0);
	}
	else if (connectionType == UDP) {
		size = recvfrom(ConnectionSocket, Buffer, MaxSize, 0, (sockaddr *)&SvrAddr, &addr_len);
	}
	memcpy(buffer, Buffer, size);
	return size;
}

std::string MySocket::GetIPAddr() {	return IPAddr; }

void MySocket::SetIPAddr(std::string ip) {
	//Change default IP addr
	if (!bTCPConnect) {
		IPAddr = ip;
		SvrAddr.sin_addr.s_addr = inet_addr(IPAddr.c_str());
	} else {
		std::cout << "Unable to set IP address. Connection already established." << std::endl;
	}
	// if connected return err msg
}

void MySocket::SetPort(int port) {
	// PURPLE - Changes the default port number
	Port = port;
	SvrAddr.sin_port = htons(Port);
}

int MySocket::GetPort() { return Port; }

SocketType MySocket::GetType() { return mySocket; }

void MySocket::SetType(SocketType socketType) {	mySocket = socketType; }

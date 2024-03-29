#pragma once
#ifndef _MYSOCKET_H
#define _MYSOCKET_H
#include "Pkt_Def.h"

enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };
const int DEFAULT_SIZE = 256;

class MySocket {
	char * Buffer;
	SOCKET WelcomeSocket; // TCP/IP
	SOCKET ConnectionSocket; // TCP and UDP
	struct sockaddr_in SvrAddr; 
	SocketType mySocket;
	std::string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bTCPConnect; // is connected?
	int MaxSize; // max size of Buffer

public:
	MySocket(SocketType, std::string, unsigned int, ConnectionType, unsigned int);
	~MySocket();
	void ConnectTCP();
	void DisconnectTCP();
	void SendData(const char *, int);
	int GetData(char *);
	std::string GetIPAddr();
	void SetIPAddr(std::string);	
	void SetPort(int); // PURPLE
	int GetPort();
	SocketType GetType();
	void SetType(SocketType);
};

#endif
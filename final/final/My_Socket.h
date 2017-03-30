#pragma once
#ifndef _MY_SOCKET_H
#define _MY_SOCKET_H
#include "Pkt_Def.h"

enum SocketType { CLIENT, SERVER };
enum ConnectionType { TCP, UDP };
const int DEFAULT_SIZE;

class MySocket {
	char * Buffer;
	SOCKET WelcomeSocket; // TCP/IP
	SOCKET ConnectionSocket; // TCP and UDP
	sockaddr_in SvrAddr;
	SocketType mySocket;
	std::string IPAddr;
	int Port;
	ConnectionType connectionType;
	bool bTCPConnect;
	int MaxSize;
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
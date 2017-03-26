#pragma once
#ifndef _PKT_H_
#define _PKT_H_
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>


enum CmdType { DRIVE, SLEEP, STATUS, ARM, CLAW, ACK, NACK };
    
const int FORWARD = 1;
const int BACKWARD = 2;
const int RIGHT = 3;
const int LEFT = 4;
const int UP = 5;
const int DOWN = 6;
const int OPEN = 7;
const int CLOSE = 8;

struct Header { // 8 bytes
	unsigned int PktCount; // 4 bytes	
	unsigned char Drive : 1; // 1 bytes
	unsigned char Status : 1;
	unsigned char Sleep : 1;
	unsigned char Arm : 1;
	unsigned char Claw : 1;
	unsigned char Ack : 1;
	unsigned char pad : 2;
	//
	unsigned char length; // 1 byte, size of whole packet in bytes
	//
};

const int HEADERSIZE = sizeof(Header) - 2; // compiler allocates 8 bytes but only uses 6
// is for serialization purpose to ensure no unused gaps

struct CmdPacket {
	Header head; // 6 bytes
	char * Data; // depends on bit flags
	unsigned char CRC; // 1 bytes
};

struct MotorBody {
	unsigned char Direction;
	unsigned char Duration;
};

class PktDef {
	CmdPacket cmdPacket;
	char * RawBuffer;
public:
	PktDef();
	PktDef(char *);
	void SetCmd(CmdType);
	void SetBodyData(char *, int);
	void SetPktCount(int);
	CmdType GetCmd();
	bool GetAck();
	int GetLength();
	char * GetBodyData();
	int GetPktCount();
	bool CheckCRC(char *, int);
	void CalcCRC();
	char * GenPacket();
	~PktDef();
};

#endif
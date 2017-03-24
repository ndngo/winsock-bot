#pragma once
#ifndef _PKT_H_
#define _PKT_H_
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

namespace robocom {

	enum CmdType { DRIVE, SLEEP, ARM, CLAW, ACK };
    
	const int FORWARD = 1;
	const int BACKWARD = 2;
	const int RIGHT = 3;
	const int LEFT = 4;
	const int UP = 5;
	const int DOWN = 6;
	const int OPEN = 7;
	const int CLOSE = 8;
	const int HEADERSIZE;

	struct Header {
		unsigned int PktCount;
		unsigned char Drive : 1;
		unsigned char Status : 1;
		unsigned char Sleep : 1;
		unsigned char Arm : 1;
		unsigned char Claw : 1;
		unsigned char Ack : 1;
		unsigned char pad : 2;
		unsigned char length;
	};

	struct CmdPacket {
		Header head;
		char * Data;
		unsigned char CRC;
	};

	struct MotorBody {
		unsigned char Direction;
		unsigned char Duration;
	};
	class PktDef {
		CmdPacket cmdPacket;
		char * RawBuffer;
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
	};
}
#endif
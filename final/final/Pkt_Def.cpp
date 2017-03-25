#include "Pkt_Def.h"

PktDef::PktDef() {
	cmdPacket.head.PktCount = 0;
	cmdPacket.head.Drive = 0;
	cmdPacket.head.Status = 0;
	cmdPacket.head.Sleep = 0;
	cmdPacket.head.Arm = 0;
	cmdPacket.head.Claw = 0;
	cmdPacket.head.Ack = 0;
	cmdPacket.head.pad = 0;
	cmdPacket.head.length = 0;
	cmdPacket.Data = nullptr;
	cmdPacket.CRC = 0;
	RawBuffer = nullptr;
}
PktDef::PktDef(char * raw) {
	char * p = raw; // 
	
	// header
	memcpy(&cmdPacket.head.PktCount, p, sizeof(int)); // pktcount
	p += sizeof(int);
	cmdPacket.head.Drive = (*p & 0x01);
	cmdPacket.head.Status = ((*p >> 1) & 0x01);
	cmdPacket.head.Sleep = ((*p >> 2) & 0x01);
	cmdPacket.head.Arm = ((*p >> 3) & 0x01);
	cmdPacket.head.Claw = ((*p >> 4) & 0x01);
	cmdPacket.head.Ack = ((*p >> 5) & 0x01);
	cmdPacket.head.pad = 0x00;
	p += sizeof(char);
	memcpy(&cmdPacket.head.length, p, sizeof(char));
	p += sizeof(char);
	
	// body
	RawBuffer = new char[cmdPacket.head.length];
	memcpy(cmdPacket.Data, p, cmdPacket.head.length);
	p += cmdPacket.head.length;
	
	// tail
	memcpy(&cmdPacket.CRC, p, sizeof(char));
}
void PktDef::SetCmd(CmdType cmdtype) {
	if (cmdtype == DRIVE) {
		cmdPacket.head.Drive = 1;
		cmdPacket.head.length = 2;
	}
	else if (cmdtype == SLEEP) {
		cmdPacket.head.Sleep = 1;
		cmdPacket.head.length = 0;
	}
	else if (cmdtype == ARM) {
		cmdPacket.head.Arm = 1;
		cmdPacket.head.length = 2;
	}
	else if (cmdtype == CLAW) {
		cmdPacket.head.Claw = 1;
		cmdPacket.head.length = 2;
	}
	else if (cmdtype == ACK) {
		cmdPacket.head.Ack = 1;
		cmdPacket.head.length = 2;
	}
}

void PktDef::SetBodyData(char * raw, int size) {
	cmdPacket.Data = new char[size];
	memcpy(cmdPacket.Data, raw, size);
}

void PktDef::SetPktCount(int count) {
	cmdPacket.head.PktCount = count;
}

CmdType PktDef::GetCmd() {
	// pointer to beginning of bitfields
	
	char * p = (char *)&cmdPacket.head.PktCount + sizeof(int);
	if (*p & 0x01) {
		return DRIVE;
	}
	else if ((*p >> 2) & 0x01) {
		return SLEEP;
	}
	else if ((*p >> 3) & 0x01) {
		return ARM;
	}
	else if ((*p >> 4) & 0x01) {
		return CLAW;
	}
	else if ((*p >> 5) & 0x01) {
		return ACK;
	}
	else {
		return ERR;
	}
}
bool PktDef::GetAck() {
	// TODO CHEKC ACK BIT
	char * p = (char *)&cmdPacket.head.PktCount + sizeof(int);
	return (*p >> 5) & 0x01;
}
int PktDef::GetLength() {
	return cmdPacket.head.length;
}
char * PktDef::GetBodyData() {
	return cmdPacket.Data;
}
int PktDef::GetPktCount() {
	return cmdPacket.head.PktCount;
}
bool PktDef::CheckCRC(char * raw, int size) {
	// counts number of 1's for CRC?
	// if calc-crc = crc return true else false
	unsigned char count = 0;
	for (int i = 0; i < size; i++) {// loop through byte header
		for (int j = 0; j < 8; j++) {// loop bits
			((*raw >> j) & 0x01) ? count++ : count;
		}
	}
	return count == cmdPacket.CRC;
}
void PktDef::CalcCRC() {
	char * p = (char *)&cmdPacket.head.PktCount;
	unsigned char count = 0;
	for (int i = 0; i < sizeof(Header); i++) {// loop through byte header
		for (int j = 0; j < 8; j++) {// loop bits
			((*p >> j) & 0x01) ? count++ : count;
		}
	}

	for (int i = 0; i < cmdPacket.head.length; i++) {// loop through byte body
		for (int j = 0; j < 8; j++) {// loop bits
			((*p >> j) & 0x01) ? count++ : count;
		}
	}

	for (int i = 0; i < sizeof(char); i++) {// loop through byte body
		for (int j = 0; j < 8; j++) {// loop bits
			((*p >> j) & 0x01) ? count++ : count;
		}
	}

	cmdPacket.CRC = count; // calcuate CRC
}

char * PktDef::GenPacket() {
	int size = sizeof(Header) + cmdPacket.head.length + sizeof(unsigned char);
	RawBuffer = new char[size];
	memset(RawBuffer, 0, size);
	char * p = RawBuffer;

	// header
	memcpy(p, &cmdPacket.head, sizeof(Header));
	p += sizeof(Header);

	// body
	memcpy(p, cmdPacket.Data, cmdPacket.head.length);
	p += cmdPacket.head.length;

	// tail
	memcpy(p, &cmdPacket.CRC, sizeof(char));

	return RawBuffer;
}

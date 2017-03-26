#include "Pkt_Def.h"

/**
 * Sets the PktDef object to an empty safe state
 */
PktDef::PktDef() {
	cmdPacket.head.PktCount = 0;
	cmdPacket.head.Drive = 0;
	cmdPacket.head.Status = 0;
	cmdPacket.head.Sleep = 0;
	cmdPacket.head.Arm = 0;
	cmdPacket.head.Claw = 0;
	cmdPacket.head.Ack = 0;
	cmdPacket.head.pad = 0;
	cmdPacket.head.length = HEADERSIZE + sizeof(cmdPacket.CRC);
	cmdPacket.Data = nullptr;
	cmdPacket.CRC = 0;
	RawBuffer = nullptr;
}

/**
 * Reads in a raw buffer and parses it into a PktDef object
 * param: raw an address to where the raw data to be parsed is stored
 */
PktDef::PktDef(char * raw) {
	char * p = raw;
	char * bitfields;
	
	// header
	memcpy(&cmdPacket.head.PktCount, p, sizeof(int));
	p += sizeof(int);

	bitfields = p;
	cmdPacket.head.Drive = (*p & 0x01);
	cmdPacket.head.Status = ((*p >> 1) & 0x01);
	cmdPacket.head.Sleep = ((*p >> 2) & 0x01);
	cmdPacket.head.Arm = ((*p >> 3) & 0x01);
	cmdPacket.head.Claw = ((*p >> 4) & 0x01);
	cmdPacket.head.Ack = ((*p >> 5) & 0x01);
	cmdPacket.head.pad = 0x00;
	p += sizeof(char);

	memcpy(&cmdPacket.head.length, p, sizeof(char));
	int size = 0; // determine size of body based on cmdType
	if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == SLEEP) {
		size = 0;
	}
	else if (GetCmd() == ACK) {
		size = 0;
	}

	//	0000 0010
	// 0000 0001
	else if (((*bitfields >> 1) & 0x01)) {
		size = cmdPacket.head.length - HEADERSIZE - sizeof(cmdPacket.CRC);
	}
	p += sizeof(char);
	
	// body
	cmdPacket.Data = new char[size];
	memcpy(cmdPacket.Data, p, size);
	p += size;
	
	// tail
	memcpy(&cmdPacket.CRC, p, sizeof(char));
}

/**
 * Sets the bitfields to represent the passed in CmdType
 * and unsets the configuration from previous cmdtypes 
 * param: cmdtype the CmdType
 */
void PktDef::SetCmd(CmdType cmdtype) {
	if (cmdtype == DRIVE) {
		cmdPacket.head.Drive = 1;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 0;
		cmdPacket.head.Arm = 0;
		cmdPacket.head.Claw = 0;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(char);
	}
	else if (cmdtype == SLEEP) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 1;
		cmdPacket.head.Arm = 0;
		cmdPacket.head.Claw = 0;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 0 + sizeof(char);
	}
	else if (cmdtype == ARM) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 0;
		cmdPacket.head.Arm = 1;
		cmdPacket.head.Claw = 0;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(char);
	}
	else if (cmdtype == CLAW) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 0;
		cmdPacket.head.Arm = 0;
		cmdPacket.head.Claw = 1;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(char);
	}
	else if (cmdtype == ACK) {
		cmdPacket.head.Ack = 1;
		cmdPacket.head.length = 0;
	}
}

void PktDef::SetBodyData(char * raw, int size) {
	cmdPacket.Data = new char[size];
	memcpy(cmdPacket.Data, raw, size);
}

void PktDef::SetPktCount(int count) {
	cmdPacket.head.PktCount = count;
}

/**
 * Checks the bitfield configuration and returns its associated CmdType
 * return: the CmdType associated with the bitfield configuration, or
 * returns ERR if the bitfield is improperly configured
 */
CmdType PktDef::GetCmd() {
	char * p = (char *)&cmdPacket.head.PktCount + sizeof(int);
	if		(*p & 0x01)		   { return DRIVE; }
	else if ((*p >> 1) & 0x01) { return STATUS; }
	else if ((*p >> 2) & 0x01) { return SLEEP; }
	else if ((*p >> 3) & 0x01) { return ARM; }
	else if ((*p >> 4) & 0x01) { return CLAW; }
	else if ((*p >> 5) & 0x01) { return ACK; }
//	else { return ERR; }
}

bool PktDef::GetAck() {
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

/**
 * Calculates the CRC of the data stored in the passed in memory location
 * and compares it to the known CRC value.
 * return: true if the calculated and the known CRC values match
 * otherwise false
 */
bool PktDef::CheckCRC(char * raw, int size) {
	// counts number of 1's for CRC?
	// if calc-crc = crc return true else false
	// create another pointer and increment it for raw
	char * p = raw;
	unsigned char count = 0;
	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < 8; j++) {
			((*p >> j) & 0x01) ? count++ : count;
		}
		p++;
	}
	return (count == *p);
}

/**
 * Calculates the CRC of the cmdPacket and stores it in its CRC field
 */
void PktDef::CalcCRC() {
	char * p = (char *)&cmdPacket;
	unsigned char count = 0;
	for (int i = 0; i < HEADERSIZE; i++) {
		for (int j = 0; j < 8; j++) {
			((*p >> j) & 0x01) ? count++ : count;
		}
		p++;
	}

	int size = 0; // determine size of body based on cmdType
	if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == SLEEP) {
		size = 0;
	}
	p = cmdPacket.Data;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 8; j++) {
			((*p >> j) & 0x01) ? count++ : count;
		}
		p++;
	}
	/*p = (char *)&cmdPacket.CRC;
	for (int i = 0; i < sizeof(char); i++) {
		for (int j = 0; j < 8; j++) {
			((*p >> j) & 0x01) ? count++ : count;
		}
		p++;
	}*/
	cmdPacket.CRC = count; // calcuate CRC
}

char * PktDef::GenPacket() {
	int size = 0; // determine size of body based on cmdType
	if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == SLEEP) {
		size = 0;
	}
	RawBuffer = new char[cmdPacket.head.length];
	memset(RawBuffer, 0, cmdPacket.head.length);
	char * p = RawBuffer;

	// header
	memcpy(p, &cmdPacket.head, HEADERSIZE);
	p += HEADERSIZE;

	// body
	memcpy(p, cmdPacket.Data, size);
	p += size;

	// tail
	memcpy(p, &cmdPacket.CRC, sizeof(char));

	return RawBuffer;
}

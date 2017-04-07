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
	memcpy(&cmdPacket.head.PktCount, p, sizeof(cmdPacket.head.PktCount));
	p += sizeof(cmdPacket.head.PktCount);

	bitfields = p;
	cmdPacket.head.Drive = (*p & 0x01);
	cmdPacket.head.Status = ((*p >> 1) & 0x01);
	cmdPacket.head.Sleep = ((*p >> 2) & 0x01);
	cmdPacket.head.Arm = ((*p >> 3) & 0x01);
	cmdPacket.head.Claw = ((*p >> 4) & 0x01);
	cmdPacket.head.Ack = ((*p >> 5) & 0x01);
	cmdPacket.head.pad = 0x00;
	p += sizeof(char);

	memcpy(&cmdPacket.head.length, p, sizeof(cmdPacket.head.length));
	int size = 0; // determine size of body based on cmdType

	// ACK automatically makes size 0 regardless of the other bits
	// , check for ACK bit first

	if (GetCmd() == ACK || GetCmd() == SLEEP || GetCmd() == NACK) {
		size = 0;
	} else if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == STATUS) {
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
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
	}
	else if (cmdtype == SLEEP) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 1;
		cmdPacket.head.Arm = 0;
		cmdPacket.head.Claw = 0;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + sizeof(cmdPacket.CRC);
	}
	else if (cmdtype == ARM) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 0;
		cmdPacket.head.Arm = 1;
		cmdPacket.head.Claw = 0;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
	}
	else if (cmdtype == CLAW) {
		cmdPacket.head.Drive = 0;
		cmdPacket.head.Status = 0;
		cmdPacket.head.Sleep = 0;
		cmdPacket.head.Arm = 0;
		cmdPacket.head.Claw = 1;
		cmdPacket.head.Ack = 0;
		cmdPacket.head.length = HEADERSIZE + 2 + sizeof(cmdPacket.CRC);
	}
	else if (cmdtype == ACK) {
		cmdPacket.head.Ack = 1;
		cmdPacket.head.length = HEADERSIZE + sizeof(cmdPacket.CRC);
	}
}

void PktDef::SetBodyData(char * raw, int size) {
	delete cmdPacket.Data;
	cmdPacket.Data = new char[size];
	cmdPacket.head.length = HEADERSIZE + size + sizeof(cmdPacket.CRC);
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
	char * p = (char *)&cmdPacket + sizeof(cmdPacket.head.PktCount);

	// check ack bit first before anything else because the presence of
	// ack means it is an ack bit regardless of the other bits
	// check ack using GetAck() method, also ACK is not a cmd
	// if      ((*p >> 5) & 0x01) { return ACK; }
	if	    (*p & 0x01)		   { return DRIVE; }
	else if ((*p >> 1) & 0x01) { return STATUS; }
	else if ((*p >> 2) & 0x01) { return SLEEP; }
	else if ((*p >> 3) & 0x01) { return ARM; }
	else if ((*p >> 4) & 0x01) { return CLAW; }
	else { return NACK; }
}

bool PktDef::GetAck() {
	char * p = (char *)&cmdPacket.head.PktCount + sizeof(cmdPacket.head.PktCount);
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
	if (GetCmd() == ACK || GetCmd() == SLEEP || GetCmd() == NACK) {
		size = 0;
	}
	else if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == STATUS) {
		size = cmdPacket.head.length - HEADERSIZE - sizeof(cmdPacket.CRC);
	}

	p = cmdPacket.Data;
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 8; j++) {
			((*p >> j) & 0x01) ? count++ : count;
		}
		p++;
	}

	cmdPacket.CRC = count;
}

/**
 * Serializes the packet
 */
char * PktDef::GenPacket() {
	int size = 0; // determine size of body based on cmdType
	if (GetCmd() == ACK || GetCmd() == SLEEP || GetCmd() == NACK) {
		size = 0;
	}
	else if (GetCmd() == DRIVE || GetCmd() == ARM || GetCmd() == CLAW) {
		size = 2;
	}
	else if (GetCmd() == STATUS) {
		size = cmdPacket.head.length - HEADERSIZE - sizeof(cmdPacket.CRC);
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
	memcpy(p, &cmdPacket.CRC, sizeof(cmdPacket.CRC));

	return RawBuffer;
}

PktDef::~PktDef() {
	delete cmdPacket.Data;
	delete RawBuffer;
	RawBuffer = nullptr;
	cmdPacket.Data = nullptr;
}
#include "Pkt_Def.h"
namespace robocom {
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
	RawBuffer = raw;
	// TODO bitshiefitng

}
void PktDef::SetCmd(CmdType cmdtype) {
	if (cmdtype == DRIVE) {
		cmdPacket.head.Drive = 1;
	}
	else if (cmdtype == SLEEP) {
		cmdPacket.head.Sleep = 1;
	}
	else if (cmdtype == ARM) {
		cmdPacket.head.Arm = 1;
	}
	else if (cmdtype == CLAW) {
		cmdPacket.head.Claw = 1;
	}
	else if (cmdtype == ACK) {
		cmdPacket.head.Ack = 1;
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
	if (/* cmdPacket.head.Drive = 1;*/) {
		// TODO BIT CHECKING
		return DRIVE;
	}
	else if (/*cmdPacket.head.Sleep = 1*/) {

		return SLEEP;
	}
	else if (/*cmdPacket.head.Arm = 1*/) {

		return ARM;
	}
	else if (/*cmdPacket.head.Claw = 1*/) {

		return CLAW;
	}
	else if (/*cmdPacket.head.Ack = 1*/) {

		return ACK;
	}
}
bool PktDef::GetAck() {
	// TODO CHEKC ACK BIT
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
}
void PktDef::CalcCRC() {
	cmdPacket.CRC = 1; // calcuate CRC
}
char * PktDef::GenPacket() {
	RawBuffer = new char[cmdPacket.head.length]; // check if use length here
	// TODO copy object member variables, the header
}
}
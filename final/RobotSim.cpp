#pragma once
#include "MySocket.h"
#include <thread> 
#include <iostream>
#include <iomanip>
#include <string>

bool ExeComplete = false;

void command(std::string ip, int port) {
	char * receiveData;

	int command = 0;
	char * ptr;
	int time = 0;
	int count = 0; 
	
	MySocket CommandSocket(SocketType::CLIENT, ip, port, ConnectionType::TCP, 100);
	
	// 3 way handshake
	CommandSocket.ConnectTCP();
	while (!ExeComplete) {
		enum choice {ADVANCE = 1, RETREAT, SINISTER, DEXTER, RAISE, LOWER, RELEASE, HOLD, ZZZ};
		// packet create
		std::cout << "options:\n1-forward\n2-backward\n3-left\n4-right\n5-up\n6-down\n7-open\n8-close\n9-sleep" << std::endl;
		command = 0;
		std::cout << "Enter command: ";
		std::cin >> command;
		std::cout << "Enter duration: ";
		std::cin >> time;
		MotorBody DriveCmd;
		PktDef txPacket;
		
		if (command == ADVANCE) { // "forward"
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = FORWARD;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == RETREAT) { // "backward"
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = BACKWARD;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == SINISTER)	{	// "left"
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = LEFT;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == DEXTER) { // "right"
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = RIGHT;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == RAISE) { // up
			txPacket.SetCmd(ARM);
			DriveCmd.Direction = UP;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == LOWER) { // down
			txPacket.SetCmd(ARM);
			DriveCmd.Direction = DOWN;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == RELEASE) { // open
			txPacket.SetCmd(CLAW);
			DriveCmd.Direction = OPEN;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
		} else if (command == HOLD) { // close
			txPacket.SetCmd(CLAW);
			DriveCmd.Direction = CLOSE;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
			std::cin.clear();
			std::cin.ignore(2000, '\n');
			
		} else if (command == ZZZ) { // sleep
			std::cout << "Going to sleep...";
			txPacket.SetCmd(SLEEP);
			

			// do not disconnect when sleep packet is sent
			// wait for ACK(SLEEP) before cleanup
			// ExeComplete = true;
			// CommandSocket.DisconnectTCP();
		} else {
			// invalid command
			std::cout << "Invalid command\n";
			std::cin.clear();
			std::cin.ignore(2000, '\n');
			command = 0;
		}
		if (command != 0) {
			txPacket.SetPktCount(count++);
			txPacket.CalcCRC();
			ptr = txPacket.GenPacket();

			CommandSocket.SendData(ptr, txPacket.GetLength());

			// receive data from robot
			// crc check, check calculated crc, with given crc, calculate crc with packet, given crc in buffer
			receiveData = new char[256];
			int dataSize = CommandSocket.GetData(receiveData);
			PktDef rxPacket(receiveData);
			char * p = (char *)&rxPacket + sizeof(int);

			if (!rxPacket.CheckCRC(receiveData, dataSize)) {
				std::cout << "Invalid CRC" << std::endl;
			} else if (*p == 0) {
				std::cout << "NACK received" << std::endl;
				CommandSocket.SendData(ptr, txPacket.GetLength());
				int dataSize = CommandSocket.GetData(receiveData);
				rxPacket.CheckCRC(receiveData, dataSize);
				rxPacket.SetBodyData(receiveData, dataSize);
			} else if (rxPacket.GetAck()) { std::cout << "\nACK received" << std::endl; }
			// process packet only if CRC is good
			// wait for ackx`
			// while bad ACK or is a NACK, resend cmd packet
			// check if packet is well formed before checking if ACK
			// while recv packet from robot is !ACK, then resend txPacket, until at what point should give up?
			// for 7 times
			// while (!rxPacket.CheckCRC(receiveData, dataSize) || !rxPacket.GetAck()) {
			// while packet is well formed (check CRC), and packet is not an acknowledgement of the send cmd packet,
			// and having only tried resending less than 7 times
			// keep resending

			// check this code what is this
			//

			/*
			for (unsigned i = 0; i < 7 && (!rxPacket.CheckCRC(receiveData, dataSize) || !rxPacket.GetAck()); i++) {
				std::cout << "CRC validation failed. Resending command packet..." << std::endl;
				CommandSocket.SendData(ptr, txPacket.GetLength());
				int dataSize = CommandSocket.GetData(receiveData);
				rxPacket.CheckCRC(receiveData, dataSize);
				rxPacket.SetBodyData(receiveData, dataSize);
			}*/
			// the cmd packet is well formed and robot has replied with ACK(cmd)
			// if ACK(SLEEP) was received, begin cleanup process

			if (rxPacket.GetCmd() == SLEEP) {
				CommandSocket.DisconnectTCP();
				ExeComplete = true;
			}
		}
	}
}
void telemetry(std::string ip, int port) {


	MySocket TelemetrySocket(SocketType::CLIENT, ip, port, ConnectionType::TCP, 100);
	TelemetrySocket.ConnectTCP();


	// receive data from robot
	// crc check, check calculated crc, with given crc, calculate crc with packet, given crc in buffer
	char * recvData = new char[256];
	int size = 0;

	while (!ExeComplete) {
		memset(recvData, 0, sizeof(recvData));
		size = TelemetrySocket.GetData(recvData);
		PktDef rxPacket(recvData);

		// if packet is well formed and has status bit set
		if (!rxPacket.CheckCRC(recvData, size)) {
			std::cout << "Invalid CRC\n";
		} else if (!(rxPacket.GetCmd() == STATUS)) {
			std::cout << "STATUS bit not set\n";
		} else {
			// psilay RAW
			char * p = recvData;
			std::cout << "RECEIVING DATA PACKET" << std::endl << "RAW Data Bytes: ";
			for (int i = 0; i < size; i++) {
				std::cout << std::hex << (unsigned int)*(p++) << ",";
			}

			// display sonar reading, arm reading body
			char * q = rxPacket.GetBodyData();

 			int bodysize = rxPacket.GetLength() - HEADERSIZE - sizeof(char);
			std::cout << std::dec << "\nSonar Value: " << (unsigned int) *q << std::endl;
			q += sizeof(short);

			// arms
			short armpos = *(short *)q;
 			std::cout << std::dec << "Arm Position: " << armpos;
			q += sizeof(short);

			// bits
			
			std::cout << "\nStatus Bits:\nDrive: " << (int)(*q & 0x01);

			std::cout << "\nArm is: ";
			if ((*q >> 1) & 0x01) { std::cout << "UP"; }
			if ((*q >> 2) & 0x01) { std::cout << "DOWN"; }
			std::cout << "\nClaw is: ";
			if ((*q >> 3) & 0x01) { std::cout << "OPEN"; }
			if ((*q >> 4) & 0x01) { std::cout << "CLOSED"; }
		
			*q = (*q & 0x1F);

			std::cout << "\nPacket count: " << std::dec << rxPacket.GetPktCount() << std::endl;
		}
	}
	TelemetrySocket.DisconnectTCP();
}

int main()
{
	// get uinput from main then pass into threads

	std::string ip = "";
	std::cout << "Enter IP address: ";
	std::cin >> ip;

	int portcmd = 0;
	std::cout << "Enter command port number: ";
	std::cin >> portcmd;

	int porttele = 0;
	std::cout << "Enter telemetry port number: ";
	std::cin >> porttele;
	
	std::thread CommandThread(command, ip, portcmd);
	std::thread TelemetryThread(telemetry, ip, porttele);

	CommandThread.detach();
	TelemetryThread.detach();

	for(;;)
	std::cin.get();
	return 0;
}
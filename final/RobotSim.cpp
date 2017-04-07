#pragma once
#include "MySocket.h"
#include <thread> 
#include <iostream>
#include <string>

bool ExeComplete = false;

void command() {
	char * receiveData;

	std::string ipAddress = "";
	std::cout << "Enter command IP address: ";
	std::cin >> ipAddress;

	int portNumber;
	std::cout << std::endl << "Enter command port number: ";
	std::cin >> portNumber;

	std::string command = "";
	char * ptr;
	int time = 0;
	int count = 0; 
	
	// Create MySocket obj
	MySocket CommandSocket(SocketType::CLIENT, ipAddress, portNumber, ConnectionType::TCP, 100);
	
	// 3 way handshake
	CommandSocket.ConnectTCP();
	while (!ExeComplete) {

		// packet create
		std::cout << "Enter command: ";
		std::cin >> command;
		std::cout << "Enter duration: ";
		std::cin >> time;
		MotorBody DriveCmd;
		PktDef txPacket;
		if (command == "forward") {
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = FORWARD;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "backward") {
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = BACKWARD;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "left")	{	
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = LEFT;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "right") {
			txPacket.SetCmd(DRIVE);
			DriveCmd.Direction = RIGHT;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "up") {
			txPacket.SetCmd(ARM);
			DriveCmd.Direction = UP;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "down") {
			txPacket.SetCmd(ARM);
			DriveCmd.Direction = DOWN;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "open") {
			txPacket.SetCmd(CLAW);
			DriveCmd.Direction = OPEN;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "close") {
			txPacket.SetCmd(CLAW);
			DriveCmd.Direction = CLOSE;
			DriveCmd.Duration = time;
			txPacket.SetBodyData((char*)&DriveCmd, 2);
		}
		else if (command == "sleep") {
			txPacket.SetCmd(SLEEP);

			// do not disconnect when sleep packet is sent
			// wait for ACK(SLEEP) before cleanup
			// ExeComplete = true;
			// CommandSocket.DisconnectTCP();
		}
		txPacket.SetPktCount(count++);
		txPacket.CalcCRC();
		ptr = txPacket.GenPacket();

		CommandSocket.SendData(ptr, txPacket.GetLength());
		
		// receive data from robot
		// crc check, check calculated crc, with given crc, calculate crc with packet, given crc in buffer
		receiveData = new char[256];
		int dataSize = CommandSocket.GetData(receiveData);
		PktDef rxPacket(receiveData);
		
		// process packet only if CRC is good
		// wait for ack
		// while bad ACK or is a NACK, resend cmd packet
		// check if packet is well formed before checking if ACK
		// while recv packet from robot is !ACK, then resend txPacket, until at what point should give up?
		// for 7 times
		// while (!rxPacket.CheckCRC(receiveData, dataSize) || !rxPacket.GetAck()) {
		// while packet is well formed (check CRC), and packet is not an acknowledgement of the send cmd packet,
		// and having only tried resending less than 7 times
		// keep resending
		for (unsigned i = 0; i < 7 && (!rxPacket.CheckCRC(receiveData, dataSize) || !rxPacket.GetAck()); i++) {
			CommandSocket.SendData(ptr, txPacket.GetLength());
			int dataSize = CommandSocket.GetData(receiveData);
			rxPacket.CheckCRC(receiveData, dataSize);
			rxPacket.SetBodyData(receiveData,dataSize);
		}
		
		// the cmd packet is well formed and robot has replied with ACK(cmd)
		// if ACK(SLEEP) was received, begin cleanup process

		if (rxPacket.GetCmd == SLEEP) {
			CommandSocket.DisconnectTCP();
			ExeComplete = true;
		}

	}


}
void telemetry() {
	std::string ipAddress = "";
	std::cout << "Enter telemetry IP address: ";
	std::cin >> ipAddress;
	
	int portNumber = 0;
	std::cout << "Please input port number for telemetry: ";
	std::cin >> portNumber;

	MySocket TelemetrySocket(SocketType::CLIENT, ipAddress, portNumber, ConnectionType::TCP, 100);
	TelemetrySocket.ConnectTCP();

}
//
//void task2()
//{
//	char * receiveData;
//	int dataSize;
//	std::cout << "Please input I.P Address : ";
//	std::string ipAddress = "";
//	std::cin >> ipAddress;
//	int portNumber;
//	std::cout << std::endl << "Please input port : ";
//	std::cin >> portNumber;
//	MySocket TelemetrySocket(SocketType::CLIENT, ipAddress, portNumber, ConnectionType::TCP, 100);
//	TelemetrySocket.ConnectTCP();
//	dataSize = TelemetrySocket.GetData(receiveData);
//	PktDef packet2(receiveData);
//	
//	packet2.CheckCRC(receiveData, dataSize);
//	
//}



int main()
{

	std::thread CommandThread(command);
	//std::thread TelemetryThread(task2);
	
	//TelemetryThread.detach();
	CommandThread.detach();
	std::cin.get();
	return 0;
}
#pragma once
#include "MySocket.h"
#include <thread> 
#include <iostream>
#include <string>

bool ExeComplete = false;



void task1()
{
	char * receiveData;
	std::cout << "Please input I.P Address : ";
	std::string ipAddress = "";
	std::cin >> ipAddress;
	int portNumber;
	std::cout << std::endl << "Please input port : ";
	std::cin >> portNumber;
	std::string command;
	char * ptr;
	int time;
	int count = 0; 
	MySocket CommandSocket(SocketType::CLIENT, ipAddress, portNumber, ConnectionType::TCP, 100);
	CommandSocket.ConnectTCP();
	while (!ExeComplete) {
		std::cout << "Please enter a command and duration :" << std::endl;
		std::cin >> command;
		std::cin >> time;
		MotorBody DriveCmd;
	
		PktDef packet;
			
			if (command == "forward")
			{
				packet.SetCmd(DRIVE);
				DriveCmd.Direction = FORWARD;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "backward")
			{
				packet.SetCmd(DRIVE);
				DriveCmd.Direction = BACKWARD;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "left")
			{	
				packet.SetCmd(DRIVE);
				DriveCmd.Direction = LEFT;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "right")
			{
				packet.SetCmd(DRIVE);
				DriveCmd.Direction = RIGHT;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "up")
			{
				packet.SetCmd(ARM);
				DriveCmd.Direction = UP;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "down")
			{
				packet.SetCmd(ARM);
				DriveCmd.Direction = DOWN;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "open")
			{
				packet.SetCmd(CLAW);
				DriveCmd.Direction = OPEN;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "close")
			{
				packet.SetCmd(CLAW);
				DriveCmd.Direction = CLOSE;
				DriveCmd.Duration = time;
				packet.SetBodyData((char*)&DriveCmd, 2);
			}
			else if (command == "sleep")
			{
				packet.SetCmd(SLEEP);
				ExeComplete = true;
				CommandSocket.DisconnectTCP();
			}


		
		packet.SetPktCount(count++);
		packet.CalcCRC();
		ptr = packet.GenPacket();

		CommandSocket.SendData(ptr, packet.GetLength());
		receiveData = new char[256];
		int dataSize = CommandSocket.GetData(receiveData);
		packet.CheckCRC(receiveData, dataSize);
		PktDef packet2(receiveData);
		if (!packet2.GetAck())
		{
			CommandSocket.SendData(ptr, packet.GetLength());
			int dataSize = CommandSocket.GetData(receiveData);
			packet.CheckCRC(receiveData, dataSize);
			packet2.SetBodyData(receiveData,dataSize);
		}
	}


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

	std::thread CommandThread(task1);
	//std::thread TelemetryThread(task2);
	
	//TelemetryThread.detach();
	CommandThread.detach();
	while (true)
	{

	}
	std::cin.get();
	return 1;
}
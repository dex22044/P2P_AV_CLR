#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Windows::Forms;

ref class ConnectionSystem
{
private:
	TcpListener^ videoServer; //TCP server (video)
	TcpClient^ videoClient; //Client (video)

public:
	NetworkStream^ videoStream; //Stream for video data transmission

	void StartServer();
	void Connect(IPAddress^ remote);
};


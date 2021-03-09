#include "pch.h"
#include "ConnectionSystem.h"

void ConnectionSystem::StartServer() {
	videoServer = gcnew TcpListener(IPAddress::Any, 8090);
	videoServer->Start(1);
	videoClient = videoServer->AcceptTcpClient();
	videoStream = videoClient->GetStream();
}

void ConnectionSystem::Connect(IPAddress^ remote) {
	videoClient = gcnew TcpClient();
	videoClient->Connect(remote, 8090);
	videoStream = videoClient->GetStream();
}
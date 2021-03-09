#include "pch.h"
#include "VideoStreamer.h"
#include "ConnectionSystem.h"
#include <SDL.h>

using namespace System;

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		Console::WriteLine("SDL failed to start. SDL_Error: {0}", gcnew String(SDL_GetError()));
	}

	ConnectionSystem^ connector = gcnew ConnectionSystem();
	Console::Write("Server or client: ");
	String^ mode = Console::ReadLine()->ToLower()->Trim();
	if (mode == "server") {
		connector->StartServer();
		Console::WriteLine("Server started and client connected");
	}
	else if (mode == "client") {
		Console::WriteLine("Where to connect (IPv4 address): ");
		connector->Connect(IPAddress::Parse(Console::ReadLine()));
		Console::WriteLine("Connected");
	}
	else {
		Console::WriteLine("What?\nPress any key to exit.");
		Console::ReadKey();
		return 0;
	}

	VideoStreamer^ vstream = gcnew VideoStreamer();
	vstream->stream = connector->videoStream;
	vstream->InitWindow();
	if (mode == "server")vstream->InitCapture();
	else vstream->InitRemotePlayback();

	Application::Run(); //Idk what this function does, but without it program just breaks

	return 0;
}

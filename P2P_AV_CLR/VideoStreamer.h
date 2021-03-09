#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Threading;
using namespace ScreenCapturerNS;

#include <SDL.h>

ref class VideoStreamer
{
private:
	void ReceiverThreadProcessor(); //Thread function for receiving data
	Thread^ receiverThread;
	SDL_Window* videoDisplayWindow;
	SDL_Renderer* videoDisplayRenderer;
	SDL_Texture* videoDisplayTexture;
	bool isCaptureBusy = false;
	void OnScreenUpdated(System::Object^ sender, ScreenCapturerNS::OnScreenUpdatedEventArgs^ e);
	MemoryStream^ encodingBuffer;
	MemoryStream^ receivingBuffer;
	MemoryStream^ receivingLenBuffer;
	DateTime prevFrameStamp;
	void SendFrame(Bitmap^ frame);

public:
	NetworkStream^ stream; //Stream for data transmission
	void InitWindow();
	void InitCapture();
	void InitRemotePlayback();
};


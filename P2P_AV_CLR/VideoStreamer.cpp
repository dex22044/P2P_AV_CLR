#include "pch.h"
#include "VideoStreamer.h"

void VideoStreamer::ReceiverThreadProcessor() {
	array<unsigned char>^ buffer1 = gcnew array<unsigned char>(4);
	array<unsigned char>^ buffer2 = gcnew array<unsigned char>(65536);
	while (true) {
		receivingLenBuffer = gcnew MemoryStream();
		while (receivingLenBuffer->Length < 4) {
			int len = stream->Read(buffer1, 0, 4);
			receivingLenBuffer->Write(buffer1, 0, len);
		}
		int frameLength = BitConverter::ToInt32(receivingLenBuffer->ToArray(), 0);
		delete receivingLenBuffer;

		receivingBuffer = gcnew MemoryStream();
		while (receivingBuffer->Length < frameLength) {
			int len = stream->Read(buffer2, 0, 65536);
			receivingBuffer->Write(buffer2, 0, len);
		}
		receivingBuffer->Seek(0, SeekOrigin::Begin);
		Bitmap^ frame = gcnew Bitmap(receivingBuffer);
		if (frame != nullptr) {
			System::Drawing::Rectangle imageRect = System::Drawing::Rectangle(0, 0, frame->Width, frame->Height); //Get image bounds
			System::Drawing::Imaging::BitmapData^ lockData = frame->LockBits(imageRect, System::Drawing::Imaging::ImageLockMode::ReadOnly,
				Imaging::PixelFormat::Format32bppArgb); //Lock image for getting raw image pointer

			SDL_UpdateTexture(videoDisplayTexture, NULL, lockData->Scan0.ToPointer(), imageRect.Width * 4); //Copy image to texture
			SDL_RenderCopy(videoDisplayRenderer, videoDisplayTexture, NULL, NULL); //Render that texture
			SDL_RenderPresent(videoDisplayRenderer); //Render renderer

			frame->UnlockBits(lockData); //Unlock image after rendering
		}
		delete frame;
		delete receivingBuffer;
	}
}

void VideoStreamer::InitWindow() {
	videoDisplayWindow = SDL_CreateWindow("P2P_AV_CLR video display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1280, 720, SDL_WINDOW_RESIZABLE); //Create window for image displaying
	videoDisplayRenderer = SDL_CreateRenderer(videoDisplayWindow, -1, SDL_RENDERER_SOFTWARE); //Create window renderer
	videoDisplayTexture = SDL_CreateTexture(videoDisplayRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 1920, 1080); //Create texture for video
}

void VideoStreamer::InitCapture() {
	ScreenCapturer::OnScreenUpdated += gcnew System::EventHandler<ScreenCapturerNS::OnScreenUpdatedEventArgs^>(this, &VideoStreamer::OnScreenUpdated);
	ScreenCapturer::StartCapture(0, 0);
}

void VideoStreamer::InitRemotePlayback() {
	receiverThread = gcnew Thread(gcnew ThreadStart(this, &VideoStreamer::ReceiverThreadProcessor));
	receiverThread->Start();
}

void VideoStreamer::OnScreenUpdated(System::Object^ sender, ScreenCapturerNS::OnScreenUpdatedEventArgs^ e)
{
	if (isCaptureBusy)return;
	isCaptureBusy = true;
	if (e->Bitmap != nullptr) {
		System::Drawing::Rectangle imageRect = System::Drawing::Rectangle(0, 0, e->Bitmap->Width, e->Bitmap->Height); //Get image bounds
		System::Drawing::Imaging::BitmapData^ lockData = e->Bitmap->LockBits(imageRect, System::Drawing::Imaging::ImageLockMode::ReadOnly, 
			Imaging::PixelFormat::Format32bppArgb); //Lock image for getting raw image pointer

		SDL_UpdateTexture(videoDisplayTexture, NULL, lockData->Scan0.ToPointer(), imageRect.Width * 4); //Copy image to texture
		SDL_RenderCopy(videoDisplayRenderer, videoDisplayTexture, NULL, NULL); //Render that texture
		SDL_RenderPresent(videoDisplayRenderer); //Render renderer

		SendFrame(e->Bitmap);

		e->Bitmap->UnlockBits(lockData); //Unlock image after rendering
	}
	isCaptureBusy = false;
}

void VideoStreamer::SendFrame(Bitmap^ frame) {
	encodingBuffer = gcnew MemoryStream(); //Flush ecoding buffer
	if (!true) return; //Reserved for connection check
	frame->Save(encodingBuffer, Imaging::ImageFormat::Jpeg); //Encode image
	DateTime currFrameStamp = DateTime::Now;
	TimeSpan delta = currFrameStamp - prevFrameStamp;
	prevFrameStamp = DateTime::Now;

	stream->Write(BitConverter::GetBytes((int)encodingBuffer->Length), 0, 4);
	encodingBuffer->Seek(0, SeekOrigin::Begin);
	encodingBuffer->CopyTo(stream);
}
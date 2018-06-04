#include "pch.h"
#include "App1Main.h"
#include "Common\DirectXHelper.h"
#include "math.h"       /* sin */

#define PI 3.14159265

using namespace App1;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

#include <iostream>
using namespace std;

// The DirectX 12 Application template is documented at https://go.microsoft.com/fwlink/?LinkID=613670&clcid=0x409

const DWORD dwChunkSize = 256000;
const DWORD maxWord = 2 ^ 16;


union PCMdata {
	BYTE DataBytes[dwChunkSize];
	WORD DataInt[dwChunkSize / 2]; // Word is 16 bit windows format
};



// Loads and initializes application assets when the application is loaded.
App1Main::App1Main()
{
	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
	// JB Audio
	cout << "Test";

	IXAudio2* pXAudio2 = NULL;
	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return;

	Microsoft::WRL::ComPtr<IXAudio2> XAudio2;
	HRESULT hrmv;
	if (FAILED(hrmv = XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		throw Platform::Exception::CreateException(hr);

	IXAudio2MasteringVoice* pMasterVoice = NULL;
	if (FAILED(hrmv = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
		return;
	// Note:TBA   Ensure that all smart pointers to XAUDIO2 objects are fully released before you release the IXAudio2 object.

	// 	Create and Populate an XAUDIO2_BUFFER structure.
	XAUDIO2_BUFFER buffer = { 0 };
//	DWORD dwChunkSize = 25600;
	BYTE * pDataBuffer = new BYTE[dwChunkSize];

//	INT16 * pDataBuffer = new INT16[dwChunkSize/2];
	//	INT * iDataBuffer;
//	iDataBuffer = pDataBuffer;

//a	PCMdata pcm;
//a	BYTE * pDataBuffer;
//a	pDataBuffer = &pcm.DataBytes[0]; // this is used a few rows below to thell the Audio buffer where the data is.
	
//	BYTE DataBuffer[dwChunkSize];
//	DataBuffer[0] = 5;
	//cout << DataBuffer[0];
//	BYTE * pDataBuffer = &DataBuffer[0];

// next two lines don't work - just make noise
//	BYTE DataBuffer[dwChunkSize]=0;
//	BYTE * pDataBuffer = &DataBuffer[0];

//	* (pDataBuffer) = 0;
//	for (byte *p = pDataBuffer;p < pDataBuffer + dwChunkSize;p++) { *p = 127; };

	bool FirstChar = true;
	bool High = true; // switch
	int BytesinBlock = 2;
	int BytesInHalfCycle = 256; // move from low to high or from high to low after this many bytes
	BYTE HighFirstChar  = 0b00001111;
	BYTE HighSecondChar = 0b00000000;
	BYTE LowFirstChar   = 0b00000000;
	BYTE LowSecondChar  = 0b00000000;
	
	for (int i = 0; i < dwChunkSize; i=i+1) {
		// First, set up the two boolean variables which determine what goes in this Byte
		if (i % BytesInHalfCycle == 0) { High = !High; }
		if (i % BytesinBlock == 0) { FirstChar = true; } else { FirstChar = false; }

		//Then put in the appropriate byte
		if (High) {  
			if (FirstChar){ *(pDataBuffer + i) = HighFirstChar; }
			else          { *(pDataBuffer + i) = HighSecondChar; }
		}
		else {
			if (FirstChar) { *(pDataBuffer + i) = LowFirstChar; }
			else           { *(pDataBuffer + i) = LowSecondChar; }
		}
		;
	//	*(pDataBuffer+i) = HighFirstChar;

		
		//*(pDataBuffer + i+1) = 0;
	};//  FOR LOOP

	//sin
	union byteint
	{
		byte b[sizeof int16_t];
		int16_t iValue;
	};
	byteint bi;
	
//	bi.iValue = 1337;
//	for (int i = 0; i<4;i++)
//		destination[i] = bi.b[i];
	float volume;


	for (int i = 0; i < dwChunkSize; i = i + 2) {
		if (i < dwChunkSize * 7 / 8)
		    {
			volume = 256 * 64 * i / dwChunkSize;
		    }
		else {
			volume = volume * .99;
			};
		
		bi.iValue = (volume)* (1 + sin (2* PI * i / BytesInHalfCycle)); // should go from 0 to 256*128
		*(pDataBuffer + i) = bi.b[0];   // first byte should be least significant
		*(pDataBuffer + i+1) = bi.b[1]; //second byte should be most significant
		
	};//  FOR LOOP








	buffer.AudioBytes = dwChunkSize;      // size of the audio buffer in bytes
	buffer.pAudioData = pDataBuffer;  // buffer containing audio data
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

// Populate a WAVEFORMATEX description of the data going into the buffer and put the data in

	WAVEFORMATEX wfx = { 0 };
	wfx.wFormatTag= WAVE_FORMAT_PCM;  // PCM(pulse - code modulated) data in integer format.
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.nAvgBytesPerSec = 88200;
	wfx.nBlockAlign = 2;
	wfx.wBitsPerSample = 16;
	wfx.cbSize = 0;

//a	int j;
//a	for (j = 0; j < dwChunkSize / 2; j++) {
//a		pcm.DataInt[j] = 0;// _byteswap_ushort(j % maxWord); //% is modulus  // byteswap converts Begendian toAMallEndian
//a	}
	//unsigned short _byteswap_ushort(unsigned short value);

	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx))) return;

	if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
		return;

	// start the voice
	if (FAILED(hr = pSourceVoice->Start(0)))
		return;





}

// Creates and initializes the renderers.
void App1Main::CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	// TODO: Replace this with your app's content initialization.
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(deviceResources));

	OnWindowSizeChanged();
}

// Updates the application state once per frame.
void App1Main::Update()
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_sceneRenderer->Update(m_timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool App1Main::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	return m_sceneRenderer->Render();
}

// Updates application state when the window's size changes (e.g. device orientation change)
void App1Main::OnWindowSizeChanged()
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

// Notifies the app that it is being suspended.
void App1Main::OnSuspending()
{
	// TODO: Replace this with your app's suspending logic.

	// Process lifetime management may terminate suspended apps at any time, so it is
	// good practice to save any state that will allow the app to restart where it left off.

	m_sceneRenderer->SaveState();

	// If your application uses video memory allocations that are easy to re-create,
	// consider releasing that memory to make it available to other applications.
}

// Notifes the app that it is no longer suspended.
void App1Main::OnResuming()
{
	// TODO: Replace this with your app's resuming logic.
}

// Notifies renderers that device resources need to be released.
void App1Main::OnDeviceRemoved()
{
	// TODO: Save any necessary application or renderer state and release the renderer
	// and its resources which are no longer valid.
	m_sceneRenderer->SaveState();
	m_sceneRenderer = nullptr;
}

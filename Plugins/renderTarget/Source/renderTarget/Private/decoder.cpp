#include "Decoder.h"

#include "ShadersSendSomethingModule.h"
#include "SendSomethingShaders.h"
#include "GameFramework/GameModeBase.h"
#include <vector>
#include "USingletone.h"

Decoder::Decoder()
{
	
}

Decoder::~Decoder()
{
	isDecodingEnable = false;
	if (DecodingThread)
	{
		DecodingThread->Join();
	}
}

void Decoder::StartDecode()
{
	isDecodingEnable = true;
	DecodingThread = MakeUnique<FThread>(TEXT("Decoding"), [this]()
	{
		DecodeThreadFunc();
	});
}



void Decoder::DecodeThreadFunc()
{
	FWmfMediaHardwareVideoDecodingTextureSamplePool HwTextureSamplePool;
	TSharedPtr<SendSomethingShaders> sendSomethingShaders = FShadersSendSomethingModule::Get().MakeSendSomethingShaders();
	int fps = 24;
	clock_t lastSendTime = clock();
	const long clocsIn10ms = (CLOCKS_PER_SEC / 100);
	const long oneFrame = CLOCKS_PER_SEC / fps;
	//auto texture = GenerateTexture(D3D11Device, 640, 480);
	while (isDecodingEnable)
	{
		if ((clock() - lastSendTime) < oneFrame)
		{
			continue;
		}
		lastSendTime = clock() - ((clock() - lastSendTime) - oneFrame);

		TSharedRef<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe> TextureSample = HwTextureSamplePool.AcquireShared();
		ID3D11Texture2D* SharedTexture = TextureSample->InitializeSourceTexture(
			D3D11Device,
			0,
			0,
			FIntPoint(640, 480),
			PF_NV12,
			EMediaTextureSampleFormat::CharNV12);
		


		D3D11_BOX SrcBox;
		SrcBox.left = 0;
		SrcBox.top = 0;
		SrcBox.front = 0;
		SrcBox.right = 640;
		SrcBox.bottom = 480;
		SrcBox.back = 1;

		TComPtr<IDXGIKeyedMutex> KeyedMutex;
		SharedTexture->QueryInterface(_uuidof(IDXGIKeyedMutex), (void**)&KeyedMutex);

		if (KeyedMutex)
		{
			if (KeyedMutex->AcquireSync(0, 0) == S_OK)
			{
				D3DImmediateContext->CopySubresourceRegion(SharedTexture, 0, 0, 0, 0, texture, 0, &SrcBox);

				KeyedMutex->ReleaseSync(1);
			}
		}
		D3DImmediateContext->Flush();
		if (sendSomethingShaders->RenderTarget == nullptr)
		{
			if (USingletone::renderTargets.IsEmpty() == false)
			{
				USingletone::renderTargets.Dequeue(sendSomethingShaders->RenderTarget);
			}
		}
		else
		{
			sendSomethingShaders->dxTextures2.Enqueue(TextureSample);
		}
	}


}





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

TComPtr<ID3D11Texture2D> GenerateTexture(TRefCountPtr<ID3D11Device> pDevice, int32_t width, int32_t height) {
	TComPtr<ID3D11ShaderResourceView> pSRVTexture;
	TComPtr<ID3D11Texture2D> pTexture;
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_NV12;
		desc.Width = width;
		desc.Height = height;
		desc.BindFlags = 512;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		//You can pass texture data in pInitialData
		pDevice->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)&pTexture);
		pDevice->CreateShaderResourceView(pTexture.Get(), nullptr, (ID3D11ShaderResourceView**)&pSRVTexture);
	}


	TComPtr<ID3D11DeviceContext> pImmediateContext;
	pDevice->GetImmediateContext((ID3D11DeviceContext**)&pImmediateContext);


	std::vector<uint8_t> textureData(4ull * width * height);
	/*for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			auto const currentPixelIndex = ((y * width) + x);

			textureData[4 * currentPixelIndex + 0] = static_cast<uint8_t>(0);
			textureData[4 * currentPixelIndex + 1] = static_cast<uint8_t>(0);
			textureData[4 * currentPixelIndex + 2] = static_cast<uint8_t>(255);
			textureData[4 * currentPixelIndex + 3] = 255;
		}
	}*/

	pImmediateContext->UpdateSubresource(pTexture.Get(), 0, nullptr, std::data(textureData), 4 * width, 0);
	return pTexture;
}

void Decoder::DecodeThreadFunc()
{
	FWmfMediaHardwareVideoDecodingTextureSamplePool HwTextureSamplePool;
	TSharedPtr<SendSomethingShaders> sendSomethingShaders = FShadersSendSomethingModule::Get().MakeSendSomethingShaders();
	int fps = 24;
	clock_t lastSendTime = clock();
	const long clocsIn10ms = (CLOCKS_PER_SEC / 100);
	const long oneFrame = CLOCKS_PER_SEC / fps;
	
	while (isDecodingEnable)
	{
		if ((clock() - lastSendTime) < oneFrame)
		{
			continue;
		}
		lastSendTime = clock() - ((clock() - lastSendTime) - oneFrame);

		auto texture = GenerateTexture(D3D11Device, 640, 480);
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





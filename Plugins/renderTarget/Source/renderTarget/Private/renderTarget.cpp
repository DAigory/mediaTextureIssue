// Copyright Epic Games, Inc. All Rights Reserved.

#include "renderTarget.h"
#include "ShadersSendSomethingModule.h"
#include "USingletone.h"
#include "SendSomethingShaders.h"
#include "GameFramework/GameModeBase.h"
#include <vector>
#include "Utils.h"
#include "decoder.h"
#include "DecodingSettings.h"


#define LOCTEXT_NAMESPACE "FrenderTargetModule"

void FrenderTargetModule::StartupModule()
{
	OnEnterGame = FGameModeEvents::GameModePostLoginEvent.AddRaw(this, &FrenderTargetModule::OnGameModePostLogin);
	OnExitGame = FGameModeEvents::GameModeLogoutEvent.AddRaw(this, &FrenderTargetModule::OnGameModeLogout);

}

void FrenderTargetModule::ShutdownModule()
{
	FGameModeEvents::GameModePostLoginEvent.Remove(OnEnterGame);
	FGameModeEvents::GameModeLogoutEvent.Remove(OnExitGame);
}



void FrenderTargetModule::OnGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("OnGameModePostLogin"));
	if (CreateDevice() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("creating device failed"));
	}
	else
	{		
		
		for (int i = 0; i < GetDefault<UDecodingSettings>()->DecodersCount; i++)
		{
			auto decoder = new Decoder();
			{
				decoder->D3D11Device = D3D11Device;
				decoder->D3DImmediateContext = D3DImmediateContext;
				decoder->DXGIManager = DXGIManager;
				//decoder->texture = GenerateTexture(D3D11Device, 640, 480).Get();
			}
			decoder->StartDecode();
			decoders.Add(decoder);
		}		
	}

}

void FrenderTargetModule::OnGameModeLogout(AGameModeBase* GameMode, AController* Exiting)
{
	for (auto decoder : decoders)
	{
		delete(decoder);
	}
	decoders.Empty();
	USingletone::renderTargets.Empty();
}

bool FrenderTargetModule::CreateDevice()
{
	HRESULT Result = S_OK;

	UINT resetToken = 0;

	if (nullptr == DXGIManager)
	{
		Result = MFCreateDXGIDeviceManager(&resetToken, DXGIManager.GetInitReference());
		if (FAILED(Result))
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to Create DXGI Device Manager: %s"), *ResultToString(Result));
			return false;
		}

		if (GDynamicRHI == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Dynamic RHI"));
			return false;
		}

		if (TCString<TCHAR>::Stricmp(GDynamicRHI->GetName(), TEXT("D3D11")) != 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Dynamic RHI is not D3D11"));
			return false;
		}

		ID3D11Device* PreExistingD3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());

		TComPtr<IDXGIDevice> DXGIDevice;
		PreExistingD3D11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&DXGIDevice);

		TComPtr<IDXGIAdapter> DXGIAdapter(nullptr);
		DXGIDevice->GetAdapter((IDXGIAdapter**)&DXGIAdapter);

		// Create device from same adapter as already existing device
		D3D_FEATURE_LEVEL FeatureLevel;

		uint32 DeviceCreationFlags = 0;

		if (FParse::Param(FCommandLine::Get(), TEXT("d3ddebug")))
		{
			DeviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		}


		Result = D3D11CreateDevice(
			DXGIAdapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			DeviceCreationFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			D3D11Device.GetInitReference(),
			&FeatureLevel,
			D3DImmediateContext.GetInitReference());

		if (FAILED(Result))
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to Create D3D11 Device: %s"), *ResultToString(Result));
			return false;
		}

		if (FeatureLevel < D3D_FEATURE_LEVEL_9_3)
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to Create D3D11 Device with feature level 9.3 or above"));
			return false;
		}

		Result = DXGIManager->ResetDevice(D3D11Device, resetToken);
		if (FAILED(Result))
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to Reset D3D11 Device: %s"), *ResultToString(Result));
			return false;
		}

		ID3D10Multithread* MultiThread = nullptr;

		D3D11Device->QueryInterface(__uuidof(ID3D10Multithread), (void**)&MultiThread);
		if (MultiThread)
		{
			MultiThread->SetMultithreadProtected(true);
			MultiThread->Release();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to activate multi-threading on device: %p"), D3D11Device.GetReference());
			return false;
		}

		UE_LOG(LogTemp, Verbose, TEXT("D3D11 Device Created: %p"), D3D11Device.GetReference());
		return true;
	}
	else
	{
		return true;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FrenderTargetModule, renderTarget)
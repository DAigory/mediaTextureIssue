// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"
THIRD_PARTY_INCLUDES_START
#define WIN32_LEAN_AND_MEAN
#include <mfapi.h>
#pragma warning(push)
#pragma warning(disable: 4005)
#include <d3d11.h>
#include <d3d10.h> 
#include <d3d9.h>
#pragma warning(pop)
#include <dxva2api.h>
THIRD_PARTY_INCLUDES_END
#include "Windows/HideWindowsPlatformTypes.h"

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HAL/Thread.h"


class FEvent;
class IMediaTextureSample;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IMFDXGIDeviceManager;
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DDeviceManager9;
struct IMFSample;
struct IMFTransform;

class FrenderTargetModule : public IModuleInterface
{
public:
	//FrenderTargetModule();
	FDelegateHandle OnEnterGame;
	FDelegateHandle OnExitGame;
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void OnGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnGameModeLogout(AGameModeBase* GameMode, AController* Exiting);
	
	TUniquePtr<FThread> DecodingThread;
	std::atomic<bool> isDecodingEnable = true;
	void DecodeThreadFunc();
	bool CreateDevice();

	TRefCountPtr<IMFDXGIDeviceManager> DXGIManager;
	TRefCountPtr<ID3D11Device> D3D11Device;
	TRefCountPtr<ID3D11DeviceContext> D3DImmediateContext;
};

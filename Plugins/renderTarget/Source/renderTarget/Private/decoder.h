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

#include "HAL/Thread.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IMFDXGIDeviceManager;
struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DDeviceManager9;
struct IMFSample;

class Decoder
{
public:
	Decoder();
	~Decoder();
	

	TUniquePtr<FThread> DecodingThread;
	std::atomic<bool> isDecodingEnable = true;

	ID3D11Texture2D* texture = nullptr;
	void DecodeThreadFunc();
	void StartDecode();

	TRefCountPtr<IMFDXGIDeviceManager> DXGIManager;
	TRefCountPtr<ID3D11Device> D3D11Device;
	TRefCountPtr<ID3D11DeviceContext> D3DImmediateContext;
};



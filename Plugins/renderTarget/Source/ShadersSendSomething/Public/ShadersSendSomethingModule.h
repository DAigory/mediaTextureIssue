// Copyright 2016-2020 Cadic AB. All Rights Reserved.
// @Author	Fredrik Lindh [Temaran] (temaran@gmail.com) {https://github.com/Temaran}
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "RenderGraphResources.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "WmfMediaHardwareVideoDecodingTextureSample.h"

class SendSomethingShaders;

struct FShaderParameters
{
	UTextureRenderTarget2D* RenderTarget;
	TSharedPtr<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe> dxTexture;
};

class SHADERSSENDSOMETHING_API FShadersSendSomethingModule : public IModuleInterface
{
public:	
	TArray<SendSomethingShaders*> shaders;
	static UTextureRenderTarget2D* RenderTarget;
	static TQueue<TSharedPtr<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe>> dxTextures2;
	static TQueue<FShaderParameters> dxTextures;
	static inline FShadersSendSomethingModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FShadersSendSomethingModule>("ShadersSendSomething");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ShadersSendSomething");
	}

	TSharedPtr<SendSomethingShaders> MakeSendSomethingShaders();

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	/*TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	FShaderUsageExampleParameters CachedShaderUsageExampleParameters;
	FDelegateHandle OnPostResolvedSceneColorHandle;
	FCriticalSection RenderEveryFrameLock;
	volatile bool bCachedParametersValid;*/
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	FDelegateHandle OnPostResolvedSceneColorHandle;
	void PostResolveSceneColor_RenderThread(FRHICommandListImmediate& RHICmdList,  class FSceneRenderTargets& SceneContext);
	void Draw_RenderThread(FShaderParameters& DrawParameters);
};

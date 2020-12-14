#pragma once

#include "RHI.h"
#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderTargetPool.h"


#include "RenderUtils.h"
#include "RHIStaticStates.h"
#include "PipelineStateCache.h"
#include "ShaderParameterUtils.h"
#include "MediaShaders.h"
#include "ShadersSendSomethingModule.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "WmfMediaHardwareVideoDecodingTextureSample.h"

class SendSomethingShaders
{
public:
	SendSomethingShaders();
	~SendSomethingShaders();
	TQueue<TSharedPtr<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe>> dxTextures2;
	UTextureRenderTarget2D* RenderTarget;
	FDelegateHandle OnPostResolvedSceneColorHandle;
	void RenderThreadeUpdate(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext);
	void DrawToRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList,  FShaderParameters& DrawParameters);
	static void DrawToRenderTarget_RenderThread2(FRHICommandListImmediate& RHICmdList,  FShaderParameters& DrawParameters);
	static void DrawToRenderTarget_RenderThread3(FRHICommandListImmediate& RHICmdList,  FShaderParameters& DrawParameters);
};



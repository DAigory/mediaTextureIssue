// Copyright 2016-2020 Cadic AB. All Rights Reserved.
// @Author	Fredrik Lindh [Temaran] (temaran@gmail.com) {https://github.com/Temaran}
///////////////////////////////////////////////////////////////////////////////////////

#include "ShadersSendSomethingModule.h"



#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "RHI.h"
#include "GlobalShader.h"
#include "RHICommandList.h"
#include "RenderGraphBuilder.h"
#include "RenderTargetPool.h"
#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "SendSomethingShaders.h"

IMPLEMENT_MODULE(FShadersSendSomethingModule, ShadersSendSomething)

// Declare some GPU stats so we can track them later
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Render, TEXT("sendSomethingShaders: Root Render"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Compute, TEXT("sendSomethingShaders: Render Compute Shader"));
DECLARE_GPU_STAT_NAMED(ShaderPlugin_Pixel, TEXT("sendSomethingShaders: Render Pixel Shader"));

TQueue<FShaderParameters> FShadersSendSomethingModule::dxTextures;
UTextureRenderTarget2D* FShadersSendSomethingModule::RenderTarget = nullptr;
TQueue<TSharedPtr<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe>> FShadersSendSomethingModule::dxTextures2;

void FShadersSendSomethingModule::StartupModule()
{
	OnPostResolvedSceneColorHandle.Reset();
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("renderTarget"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/TutorialShaders"), PluginShaderDir);
}

void FShadersSendSomethingModule::ShutdownModule()
{
	dxTextures.Empty();	
}



void FShadersSendSomethingModule::PostResolveSceneColor_RenderThread(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext)
{
	/*for (auto shader : shaders)
	{
		shader->RenderThreadeUpdate(RHICmdList);
	}*/
}

void FShadersSendSomethingModule::Draw_RenderThread(FShaderParameters& DrawParameters)
{
	check(IsInRenderingThread());

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
	//SendSomethingShaders::DrawToRenderTarget_RenderThread(RHICmdList, DrawParameters);
	

	//QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_Render); // Used to gather CPU profiling data for the UE4 session frontend
	//SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Render); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	//if (!ComputeShaderOutput.IsValid())
	//{
	//	FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(DrawParameters.GetRenderTargetSize(), PF_R32_UINT, FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
	//	ComputeShaderOutputDesc.DebugName = TEXT("ShaderPlugin_ComputeShaderOutput");
	//	GRenderTargetPool.FindFreeElement(RHICmdList, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("ShaderPlugin_ComputeShaderOutput"));
	//}

	//FComputeShaderExample::RunComputeShader_RenderThread(RHICmdList, DrawParameters, ComputeShaderOutput->GetRenderTargetItem().UAV);
	//FPixelShaderExample::DrawToRenderTarget_RenderThread(RHICmdList, DrawParameters, ComputeShaderOutput->GetRenderTargetItem().TargetableTexture);

}

TSharedPtr<SendSomethingShaders> FShadersSendSomethingModule::MakeSendSomethingShaders()
{	
	return MakeShared<SendSomethingShaders>();
}


#include "SendSomethingShaders.h"

#include "ShaderParameterMacros.h"
#include "ShaderParameterStruct.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"
#include "Containers/DynamicRHIResourceArray.h"
#include "Runtime/RenderCore/Public/PixelShaderUtils.h"

class FWmfMediaHardwareVideoDecodingShader2 : public FGlobalShader
{
	DECLARE_TYPE_LAYOUT(FWmfMediaHardwareVideoDecodingShader2, NonVirtual);
public:	
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5)
			|| IsPCPlatform(Parameters.Platform); // to support mobile emulation on PC
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FWmfMediaHardwareVideoDecodingShader2() {}

	FWmfMediaHardwareVideoDecodingShader2(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		TextureY.Bind(Initializer.ParameterMap, TEXT("TextureY"));
		TextureUV.Bind(Initializer.ParameterMap, TEXT("TextureUV"));

		PointClampedSamplerY.Bind(Initializer.ParameterMap, TEXT("PointClampedSamplerY"));
		BilinearClampedSamplerUV.Bind(Initializer.ParameterMap, TEXT("BilinearClampedSamplerUV"));

		ColorTransform.Bind(Initializer.ParameterMap, TEXT("ColorTransform"));
		SrgbToLinear.Bind(Initializer.ParameterMap, TEXT("SrgbToLinear"));
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const FShaderResourceViewRHIRef& InTextureY,
		const FShaderResourceViewRHIRef& InTextureUV,
		const bool InIsOutputSrgb,
		const bool InFilterUV = true
	)
	{
		SetSRVParameter(RHICmdList, ShaderRHI, TextureY, InTextureY);
		SetSRVParameter(RHICmdList, ShaderRHI, TextureUV, InTextureUV);

		SetSamplerParameter(RHICmdList, ShaderRHI, PointClampedSamplerY, TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI());
		SetSamplerParameter(RHICmdList, ShaderRHI, BilinearClampedSamplerUV, InFilterUV ? TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI() : TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI());

		SetShaderValue(RHICmdList, ShaderRHI, ColorTransform, MediaShaders::CombineColorTransformAndOffset(MediaShaders::YuvToSrgbDefault, MediaShaders::YUVOffset8bits));
		SetShaderValue(RHICmdList, ShaderRHI, SrgbToLinear, InIsOutputSrgb ? 1 : 0); // Explicitly specify integer value, as using boolean falls over on XboxOne.
	}

	template<typename TShaderRHIParamRef>
	void SetParameters(
		FRHICommandListImmediate& RHICmdList,
		const TShaderRHIParamRef ShaderRHI,
		const FShaderResourceViewRHIRef& InTextureRGBA,
		const bool InIsOutputSrgb
	)
	{
		SetSRVParameter(RHICmdList, ShaderRHI, TextureY, InTextureRGBA);

		SetSamplerParameter(RHICmdList, ShaderRHI, PointClampedSamplerY, TStaticSamplerState<SF_Point, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI());
		SetSamplerParameter(RHICmdList, ShaderRHI, BilinearClampedSamplerUV, TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI());

		SetShaderValue(RHICmdList, ShaderRHI, ColorTransform, MediaShaders::CombineColorTransformAndOffset(MediaShaders::YuvToSrgbDefault, MediaShaders::YUVOffset8bits));
		SetShaderValue(RHICmdList, ShaderRHI, SrgbToLinear, InIsOutputSrgb ? 1 : 0); // Explicitly specify integer value, as using boolean falls over on XboxOne.
	}

private:

	LAYOUT_FIELD(FShaderResourceParameter, TextureY);
	LAYOUT_FIELD(FShaderResourceParameter, TextureUV);
	LAYOUT_FIELD(FShaderResourceParameter, PointClampedSamplerY);
	LAYOUT_FIELD(FShaderResourceParameter, BilinearClampedSamplerUV);
	LAYOUT_FIELD(FShaderParameter, ColorTransform);
	LAYOUT_FIELD(FShaderParameter, SrgbToLinear);
};


class FHardwareVideoDecodingVS2 : public FWmfMediaHardwareVideoDecodingShader2
{
	DECLARE_GLOBAL_SHADER(FHardwareVideoDecodingVS2);

public:

	/** Default constructor. */
	FHardwareVideoDecodingVS2() {}

	/** Initialization constructor. */
	FHardwareVideoDecodingVS2(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWmfMediaHardwareVideoDecodingShader2(Initializer)
	{
	}
};

class FHardwareVideoDecodingPS2 : public FWmfMediaHardwareVideoDecodingShader2
{
	DECLARE_GLOBAL_SHADER(FHardwareVideoDecodingPS2);

public:

	/** Default constructor. */
	FHardwareVideoDecodingPS2() {}

	/** Initialization constructor. */
	FHardwareVideoDecodingPS2(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FWmfMediaHardwareVideoDecodingShader2(Initializer)
	{ }
};


IMPLEMENT_TYPE_LAYOUT(FWmfMediaHardwareVideoDecodingShader2);
IMPLEMENT_GLOBAL_SHADER(FHardwareVideoDecodingVS2, "/TutorialShaders/Private/MediaHardwareVideoDecoding.usf", "MainVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FHardwareVideoDecodingPS2, "/TutorialShaders/Private/MediaHardwareVideoDecoding.usf", "NV12ConvertPS", SF_Pixel);


class FSimpleScreenVertexBuffer : public FVertexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	void InitRHI()
	{
		TResourceArray<FFilterVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(6);

		Vertices[0].Position = FVector4(-1, 1, 0, 1);
		Vertices[0].UV = FVector2D(0, 0);

		Vertices[1].Position = FVector4(1, 1, 0, 1);
		Vertices[1].UV = FVector2D(1, 0);

		Vertices[2].Position = FVector4(-1, -1, 0, 1);
		Vertices[2].UV = FVector2D(0, 1);

		Vertices[3].Position = FVector4(1, -1, 0, 1);
		Vertices[3].UV = FVector2D(1, 1);

		// Create vertex buffer. Fill buffer with initial data upon creation
		FRHIResourceCreateInfo CreateInfo(&Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};
TGlobalResource<FSimpleScreenVertexBuffer> GSimpleScreenVertexBuffer;

/************************************************************************/
/* A simple passthrough vertexshader that we will use.                  */
/************************************************************************/
class FSimplePassThroughVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSimplePassThroughVS);

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}

	FSimplePassThroughVS() { }
	FSimplePassThroughVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer) { }
};

/**********************************************************************************************/
/* This class carries our parameter declarations and acts as the bridge between cpp and HLSL. */
/**********************************************************************************************/
class FPixelShaderExamplePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FPixelShaderExamplePS);
	SHADER_USE_PARAMETER_STRUCT(FPixelShaderExamplePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	
		SHADER_PARAMETER_TEXTURE(Texture2D<uint>, ComputeShaderOutput)
		SHADER_PARAMETER(FVector4, StartColor)
		SHADER_PARAMETER(FVector4, EndColor)
		SHADER_PARAMETER(FVector2D, TextureSize) // Metal doesn't support GetDimensions(), so we send in this data via our parameters.
		SHADER_PARAMETER(float, BlendFactor)
		END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

// This will tell the engine to create the shader and where the shader entry point is.
//                           ShaderType                            ShaderPath                Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FSimplePassThroughVS, "/TutorialShaders/Private/PixelShader.usf", "MainVertexShader", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FPixelShaderExamplePS, "/TutorialShaders/Private/PixelShader.usf", "MainPixelShader", SF_Pixel);



struct FRHICommandCopyResource2 final : public FRHICommand<FRHICommandCopyResource2>
{
	TComPtr<ID3D11Texture2D> SampleTexture;
	FTexture2DRHIRef SampleDestinationTexture;

	FRHICommandCopyResource2(ID3D11Texture2D* InSampleTexture, FRHITexture2D* InSampleDestinationTexture)
		: SampleTexture(InSampleTexture)
		, SampleDestinationTexture(InSampleDestinationTexture)
	{
	}

	void Execute(FRHICommandListBase& CmdList)
	{
		LLM_SCOPE(ELLMTag::MediaStreaming);
		ID3D11Device* D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
		ID3D11DeviceContext* D3D11DeviceContext = nullptr;

		D3D11Device->GetImmediateContext(&D3D11DeviceContext);
		if (D3D11DeviceContext)
		{
			ID3D11Resource* DestinationTexture = reinterpret_cast<ID3D11Resource*>(SampleDestinationTexture->GetNativeResource());
			if (DestinationTexture)
			{
				TComPtr<IDXGIResource> OtherResource(nullptr);
				SampleTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&OtherResource);

				if (OtherResource)
				{
					HANDLE SharedHandle = nullptr;
					if (OtherResource->GetSharedHandle(&SharedHandle) == S_OK)
					{
						if (SharedHandle != 0)
						{
							TComPtr<ID3D11Resource> SharedResource;
							D3D11Device->OpenSharedResource(SharedHandle, __uuidof(ID3D11Texture2D), (void**)&SharedResource);

							if (SharedResource)
							{
								TComPtr<IDXGIKeyedMutex> KeyedMutex;
								SharedResource->QueryInterface(_uuidof(IDXGIKeyedMutex), (void**)&KeyedMutex);

								if (KeyedMutex)
								{
									// Key is 1 : Texture as just been updated
									// Key is 2 : Texture as already been updated.
									// Do not wait to acquire key 1 since there is race no condition between writer and reader.
									if (KeyedMutex->AcquireSync(1, 0) == S_OK)
									{
										// Copy from shared texture of FWmfMediaSink device to Rendering device
										D3D11DeviceContext->CopyResource(DestinationTexture, SharedResource);
										KeyedMutex->ReleaseSync(2);
									}
									else
									{
										// If key 1 cannot be acquired, another reader is already copying the resource
										// and will release key with 2. 
										// Wait to acquire key 2.
										if (KeyedMutex->AcquireSync(2, INFINITE) == S_OK)
										{
											KeyedMutex->ReleaseSync(2);
										}
									}
								}
							}
						}
					}
				}
			}
			D3D11DeviceContext->Release();
		}
	}
};

SendSomethingShaders::SendSomethingShaders()
{
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &SendSomethingShaders::RenderThreadeUpdate);
	}
}

SendSomethingShaders::~SendSomethingShaders()
{
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}

	dxTextures2.Empty();
	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();
}

void SendSomethingShaders::RenderThreadeUpdate(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext)
{
	TSharedPtr<FWmfMediaHardwareVideoDecodingTextureSample, ESPMode::ThreadSafe> texture;	
	if (dxTextures2.Dequeue(texture))
	{
		FShaderParameters shadersParams;
		{
			shadersParams.dxTexture = texture;
			shadersParams.RenderTarget = RenderTarget;
		}
	
		DrawToRenderTarget_RenderThread(RHICmdList, shadersParams);
	}	
}

void SendSomethingShaders::DrawToRenderTarget_RenderThread3(FRHICommandListImmediate& RHICmdList, FShaderParameters& DrawParameters)
{
	/*IMediaTextureSampleConverter::FConversionHints hints;
	{
		
	}*/
	//DrawParameters.dxTexture->Convert(DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), hints);
}

void SendSomethingShaders::DrawToRenderTarget_RenderThread2(FRHICommandListImmediate& RHICmdList,  FShaderParameters& DrawParameters)
{
	FRHIRenderPassInfo RenderPassInfo(DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), ERenderTargetActions::Clear_Store);
	RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("ShaderPlugin_OutputToRenderTarget"));

	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FSimplePassThroughVS> VertexShader(ShaderMap);
	TShaderMapRef<FPixelShaderExamplePS> PixelShader(ShaderMap);

	// Set the graphic pipeline state.
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	// Setup the pixel shader
	FPixelShaderExamplePS::FParameters PassParameters;	
	//PassParameters.ComputeShaderOutput = ComputeShaderOutput;
	PassParameters.StartColor = FVector4(200, 100, 10, 200) / 255.0f;
	PassParameters.EndColor = FVector4(100, 200, 1, 200) / 255.0f;
	PassParameters.TextureSize = FVector2D(200, 200);
	PassParameters.BlendFactor = 0.5;
	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), PassParameters);

	// Draw
	RHICmdList.SetStreamSource(0, GSimpleScreenVertexBuffer.VertexBufferRHI, 0);
	RHICmdList.DrawPrimitive(0, 2, 1);

	// Resolve render target
	RHICmdList.CopyToResolveTarget(DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture(), DrawParameters.RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());

	RHICmdList.EndRenderPass();
}

void SendSomethingShaders::DrawToRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList,  FShaderParameters& DrawParameters)
{	
	//UE_LOG(LogTemp, Log, TEXT("DrawToRenderTarget_RenderThread"));
	auto TextureSample = DrawParameters.dxTexture;

	const FIntPoint OutputDim = TextureSample->GetDim();
	
	FRHIResourceCreateInfo CreateInfo;
	auto Format = PF_NV12;
	auto MediaTextureSampleFormat = EMediaTextureSampleFormat::CharNV12;
	ETextureCreateFlags CreateFlags = TexCreate_Dynamic | TexCreate_DisableSRVCreation;

	IMediaTextureSampleConverter::FConversionHints hints;
	hints.bOutputSRGB = false;
	hints.NumMips = 1;
	
	check(DrawParameters.RenderTarget != nullptr);
	check(DrawParameters.RenderTarget->GetRenderTargetResource() != nullptr);
	auto InSample = TextureSample;
	auto InDstTexture = DrawParameters.RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();

	TComPtr<ID3D11Texture2D> SampleTexture = InSample->GetSourceTexture();
	ID3D11Device* D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	D3D11Device->GetImmediateContext(&D3D11DeviceContext);
	if (D3D11DeviceContext)
	{
		//FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
		FRHIRenderPassInfo RPInfo(InDstTexture, ERenderTargetActions::DontLoad_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("ConvertTextureFormat"));
		//RHICmdList.EndRenderPass();

		RHICmdList.SetViewport(0, 0, 0.f, InSample->GetDim().X, InSample->GetDim().Y, 1.f);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();

	//	auto SampleDestinationTexture = InDstTexture;

		FTexture2DRHIRef SampleDestinationTexture = InSample->GetOrCreateDestinationTexture();

		if (RHICmdList.Bypass())
		{
			FRHICommandCopyResource2 Cmd(SampleTexture, SampleDestinationTexture);
			Cmd.Execute(RHICmdList);
		}
		else
		{
			new (RHICmdList.AllocCommand<FRHICommandCopyResource2>()) FRHICommandCopyResource2(SampleTexture, SampleDestinationTexture);
		}
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

		TShaderMapRef< FHardwareVideoDecodingVS2 > VertexShader(GlobalShaderMap);
		TShaderMapRef< FHardwareVideoDecodingPS2 > PixelShader(GlobalShaderMap);
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

		FShaderResourceViewRHIRef Y_SRV = RHICreateShaderResourceView(SampleDestinationTexture, 0, 1, PF_G8);
		FShaderResourceViewRHIRef UV_SRV = RHICreateShaderResourceView(SampleDestinationTexture, 0, 1, PF_R8G8);
		VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), Y_SRV, UV_SRV, InSample->IsOutputSrgb());
		PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), Y_SRV, UV_SRV, InSample->IsOutputSrgb());

		RHICmdList.DrawPrimitive(0, 2, 1);
		RHICmdList.CopyToResolveTarget(InDstTexture, DrawParameters.RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());
		RHICmdList.EndRenderPass();		
	}
	D3D11DeviceContext->Release();
}
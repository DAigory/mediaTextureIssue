#pragma once

#include "CoreMinimal.h"
#include "ShadersSendSomethingModule.h"
#include "USingletone.generated.h"

UCLASS(Blueprintable, BlueprintType)
class RENDERTARGET_API USingletone : public UObject
{
	GENERATED_BODY()

public:
	static TQueue<UTextureRenderTarget2D*> renderTargets;
	UFUNCTION(BlueprintCallable, Category = "Singletone Delegates")
		static USingletone* GetUSingletone()
	{
		UE_LOG(LogTemp, Log, TEXT("GetUSingletone"));
		if (!Singleton)
		{
			CreateInstance();
		}
		return Singleton;
	}

	UFUNCTION(BlueprintCallable, Category = "Singletone Delegates")
		static void SetRenderTarget(UTextureRenderTarget2D* target)
	{
		UE_LOG(LogTemp, Log, TEXT("SetRenderTarget"));
		renderTargets.Enqueue(target);		
	}

private:
	static void CreateInstance();
	// The singleton object.
	static USingletone* Singleton;

};


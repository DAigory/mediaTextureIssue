#include "USingletone.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

USingletone* USingletone::Singleton = nullptr;
TQueue<UTextureRenderTarget2D*> USingletone::renderTargets;

void USingletone::CreateInstance()
{
	Singleton = NewObject<USingletone>();
	Singleton->AddToRoot();
}



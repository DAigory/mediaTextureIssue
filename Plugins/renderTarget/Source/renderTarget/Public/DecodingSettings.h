#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InputCoreTypes.h"

#include "DecodingSettings.generated.h"

UCLASS(Blueprintable, BlueprintType,config = PixelStreaming, defaultconfig, meta = (DisplayName = "renderTarget"))
class RENDERTARGET_API UDecodingSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:		
	UPROPERTY(config, EditAnywhere, Category = decoding)
		int32 DecodersCount = 1;
	
	virtual FName GetCategoryName() const override;
#if WITH_EDITOR
	virtual FText GetSectionText() const override;
#endif	

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
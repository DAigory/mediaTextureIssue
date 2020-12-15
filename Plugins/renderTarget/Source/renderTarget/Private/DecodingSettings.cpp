
#include "DecodingSettings.h"

UDecodingSettings::UDecodingSettings(const FObjectInitializer& ObjectInitlaizer)
	: Super(ObjectInitlaizer)
{

}

FName UDecodingSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

#if WITH_EDITOR
FText UDecodingSettings::GetSectionText() const
{
	return NSLOCTEXT("renderTargetPlugin", "renderTargetsSection", "renderTarget");
}
#endif

#if WITH_EDITOR
void UDecodingSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif



#pragma once
#include "CoreMinimal.h"
#include "WorldOpenerSettings.generated.h"

UCLASS(config=EditorPerProjectUserSettings, defaultconfig)
class UWoSettings : public UObject
{
    GENERATED_UCLASS_BODY()

public:

    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TAssetPtr<class UWorld> WorldToOpen;

    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TSet<TAssetPtr<class UWorld>> PreloadWorlds;
    
};

UCLASS(config=EditorPerProjectUserSettings)
class UWoViewConfig : public UObject
{
    GENERATED_UCLASS_BODY()

public:

    UPROPERTY(EditAnywhere, GlobalConfig, Category = "World Opener")
    bool bDrawLine = true;

    UPROPERTY(EditAnywhere, GlobalConfig, Category = "World Opener")
    int32 LoadWarn = 20;

};
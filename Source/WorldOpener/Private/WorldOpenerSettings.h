#pragma once
#include "CoreMinimal.h"
#include "WorldOpenerSettings.generated.h"

USTRUCT()
struct FMapArray
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TArray<TAssetPtr<class UWorld>> Maps;
};

UCLASS(config=EditorPerProjectUserSettings, defaultconfig)
class UWoSettings : public UObject
{
    GENERATED_UCLASS_BODY()

public:

    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TAssetPtr<class UWorld> WorldToOpen;

    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TSet<TAssetPtr<class UWorld>> PreloadWorlds;
    
    UPROPERTY(EditAnywhere, Config, Category = "World Opener", meta=(DisplayThumbnail="false"))
    TMap<TAssetPtr<class ULevelSequence>, FMapArray> SequenceAdditionAssociatedWorld;
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
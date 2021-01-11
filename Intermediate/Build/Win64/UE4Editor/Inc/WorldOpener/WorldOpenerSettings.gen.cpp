// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "WorldOpener/Private/WorldOpenerSettings.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWorldOpenerSettings() {}
// Cross Module References
	WORLDOPENER_API UClass* Z_Construct_UClass_UWoSettings_NoRegister();
	WORLDOPENER_API UClass* Z_Construct_UClass_UWoSettings();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_WorldOpener();
	ENGINE_API UClass* Z_Construct_UClass_UWorld_NoRegister();
	WORLDOPENER_API UClass* Z_Construct_UClass_UWoViewConfig_NoRegister();
	WORLDOPENER_API UClass* Z_Construct_UClass_UWoViewConfig();
// End Cross Module References
	void UWoSettings::StaticRegisterNativesUWoSettings()
	{
	}
	UClass* Z_Construct_UClass_UWoSettings_NoRegister()
	{
		return UWoSettings::StaticClass();
	}
	struct Z_Construct_UClass_UWoSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_WorldToOpen_MetaData[];
#endif
		static const UE4CodeGen_Private::FSoftObjectPropertyParams NewProp_WorldToOpen;
		static const UE4CodeGen_Private::FSoftObjectPropertyParams NewProp_PreloadWorlds_ElementProp;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PreloadWorlds_MetaData[];
#endif
		static const UE4CodeGen_Private::FSetPropertyParams NewProp_PreloadWorlds;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWoSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_WorldOpener,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "WorldOpenerSettings.h" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoSettings_Statics::NewProp_WorldToOpen_MetaData[] = {
		{ "Category", "World Opener" },
		{ "DisplayThumbnail", "false" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
	const UE4CodeGen_Private::FSoftObjectPropertyParams Z_Construct_UClass_UWoSettings_Statics::NewProp_WorldToOpen = { "WorldToOpen", nullptr, (EPropertyFlags)0x0014000000004001, UE4CodeGen_Private::EPropertyGenFlags::SoftObject, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWoSettings, WorldToOpen), Z_Construct_UClass_UWorld_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UWoSettings_Statics::NewProp_WorldToOpen_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWoSettings_Statics::NewProp_WorldToOpen_MetaData)) };
	const UE4CodeGen_Private::FSoftObjectPropertyParams Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds_ElementProp = { "PreloadWorlds", nullptr, (EPropertyFlags)0x0004000000004001, UE4CodeGen_Private::EPropertyGenFlags::SoftObject, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UClass_UWorld_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds_MetaData[] = {
		{ "Category", "World Opener" },
		{ "DisplayThumbnail", "false" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
	const UE4CodeGen_Private::FSetPropertyParams Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds = { "PreloadWorlds", nullptr, (EPropertyFlags)0x0014000000004001, UE4CodeGen_Private::EPropertyGenFlags::Set, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWoSettings, PreloadWorlds), METADATA_PARAMS(Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UWoSettings_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWoSettings_Statics::NewProp_WorldToOpen,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds_ElementProp,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWoSettings_Statics::NewProp_PreloadWorlds,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWoSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWoSettings>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UWoSettings_Statics::ClassParams = {
		&UWoSettings::StaticClass,
		"EditorPerProjectUserSettings",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UWoSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UWoSettings_Statics::PropPointers),
		0,
		0x000000A6u,
		METADATA_PARAMS(Z_Construct_UClass_UWoSettings_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UWoSettings_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UWoSettings()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UWoSettings_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UWoSettings, 4022872447);
	template<> WORLDOPENER_API UClass* StaticClass<UWoSettings>()
	{
		return UWoSettings::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UWoSettings(Z_Construct_UClass_UWoSettings, &UWoSettings::StaticClass, TEXT("/Script/WorldOpener"), TEXT("UWoSettings"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWoSettings);
	void UWoViewConfig::StaticRegisterNativesUWoViewConfig()
	{
	}
	UClass* Z_Construct_UClass_UWoViewConfig_NoRegister()
	{
		return UWoViewConfig::StaticClass();
	}
	struct Z_Construct_UClass_UWoViewConfig_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bDrawLine_MetaData[];
#endif
		static void NewProp_bDrawLine_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bDrawLine;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LoadWarn_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_LoadWarn;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UWoViewConfig_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_WorldOpener,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoViewConfig_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "WorldOpenerSettings.h" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine_MetaData[] = {
		{ "Category", "World Opener" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
	void Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine_SetBit(void* Obj)
	{
		((UWoViewConfig*)Obj)->bDrawLine = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine = { "bDrawLine", nullptr, (EPropertyFlags)0x0010000000044001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UWoViewConfig), &Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine_SetBit, METADATA_PARAMS(Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UWoViewConfig_Statics::NewProp_LoadWarn_MetaData[] = {
		{ "Category", "World Opener" },
		{ "ModuleRelativePath", "Private/WorldOpenerSettings.h" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UWoViewConfig_Statics::NewProp_LoadWarn = { "LoadWarn", nullptr, (EPropertyFlags)0x0010000000044001, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UWoViewConfig, LoadWarn), METADATA_PARAMS(Z_Construct_UClass_UWoViewConfig_Statics::NewProp_LoadWarn_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UWoViewConfig_Statics::NewProp_LoadWarn_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UWoViewConfig_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWoViewConfig_Statics::NewProp_bDrawLine,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UWoViewConfig_Statics::NewProp_LoadWarn,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UWoViewConfig_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UWoViewConfig>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UWoViewConfig_Statics::ClassParams = {
		&UWoViewConfig::StaticClass,
		"EditorPerProjectUserSettings",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UWoViewConfig_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UWoViewConfig_Statics::PropPointers),
		0,
		0x000000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UWoViewConfig_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UWoViewConfig_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UWoViewConfig()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UWoViewConfig_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UWoViewConfig, 2150636705);
	template<> WORLDOPENER_API UClass* StaticClass<UWoViewConfig>()
	{
		return UWoViewConfig::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UWoViewConfig(Z_Construct_UClass_UWoViewConfig, &UWoViewConfig::StaticClass, TEXT("/Script/WorldOpener"), TEXT("UWoViewConfig"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UWoViewConfig);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif

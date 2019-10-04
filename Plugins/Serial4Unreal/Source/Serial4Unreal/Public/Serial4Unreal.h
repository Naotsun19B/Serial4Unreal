// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

// Avoid macro overloading
#include "AllowWindowsPlatformTypes.h"	
#include <Windows.h>
#pragma warning(push)				//
#pragma warning( disable : 4668 )	// Disable warnig (error C4668)
#include <SetupAPI.h>				// '<macro name>' is not defined as a preprocessor macro,
#include <winioctl.h>				// replacing with '0' for '#if/#elif'
#pragma warning(pop)				//
#include "HideWindowsPlatformTypes.h"	

class FSerial4UnrealModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

DEFINE_LOG_CATEGORY_STATIC(Serial4Unreal, Log, All);

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetEnumName((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

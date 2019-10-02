// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Serial4UnrealBPLibrary.h"
#include "Serial4Unreal.h"

USerial4UnrealBPLibrary::USerial4UnrealBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USerial4UnrealBPLibrary::CreateSerialPort(USerialPort* &SerialPort)
{
	SerialPort = NewObject<USerialPort>();
}
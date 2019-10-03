// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Serial4UnrealBPLibrary.h"
#include "Serial4Unreal//Public/Serial4Unreal.h"

USerial4UnrealBPLibrary::USerial4UnrealBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USerial4UnrealBPLibrary::CreateSerialPort(int ComNumber, FPortConfig PortConfig, USerialPort* &SerialPort)
{
	SerialPort = NewObject<USerialPort>();
	SerialPort->comNumber = ComNumber;
	SerialPort->portConfig = PortConfig;
}

FString USerial4UnrealBPLibrary::ConvertPortConfigToString(FPortConfig PortConfig)
{
	return	"Baud rate		: " + FString::FromInt(PortConfig.BaudRate) + '\n' +
			"Byte size		: " + FString::FromInt(PortConfig.ByteSize) + '\n' +
			"Parity bit		: " + GETENUMSTRING("EParityBit", PortConfig.Parity) + '\n' +
			"Stop bit		: " + GETENUMSTRING("EStopBit", PortConfig.Parity) + '\n' +
			"Outx CTS flow	: " + (PortConfig.OutxCtsFlow ? "True" : "False") + '\n' +
			"RTS control	: " + GETENUMSTRING("ERTSControl", PortConfig.RtsControl) + '\n';
}
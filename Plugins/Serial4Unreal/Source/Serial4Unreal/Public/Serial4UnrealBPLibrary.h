// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Serial4Unreal/Public/SerialPort.h"
#include "Serial4UnrealBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/

UCLASS()
class USerial4UnrealBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	// Create a class for serial communication (When disable read buffer process, the event dispatcher is not called)
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal", meta = (AdvancedDisplay = "DisableReadBufferProcess"))
	static void CreateSerialPort(int ComNumber, FPortConfig PortConfig, bool DisableReadBufferProcess, USerialPort* &SerialPort);

	// Get a list of names and numbers of devices connected to the communication port
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	static bool GetComPortList(TMap<int, FString>& List);

	// Convert FPortConfig to FString
	UFUNCTION(BlueprintPure, meta = (BlueprintAutoCast, DisplayName = "ToString(PortConfig)"))
	static FString ConvertPortConfigToString(FPortConfig PortConfig);
};

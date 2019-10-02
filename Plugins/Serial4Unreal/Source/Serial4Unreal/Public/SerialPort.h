// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "AllowWindowsPlatformTypes.h"	//
#include <Windows.h>					//マクロの多重定義を回避
#include "HideWindowsPlatformTypes.h"	//

#include "SerialPort.generated.h"

UENUM(BlueprintType)
enum class EParityBit : uint8
{
	PB_NoParity		UMETA(DisplayName = "NoParity"),
	PB_OddParity	UMETA(DisplayName = "OneParity"),
	PB_EvenParity	UMETA(DisplayName = "EvenParity"),
	PB_MarkParity	UMETA(DisplayName = "MarkParity"),
	PB_SpaceParity	UMETA(DisplayName = "SpaceParity"),
};

UENUM(BlueprintType)
enum class EStopBit : uint8
{
	SB_OneStopBit	UMETA(DisplayName = "OneStopBit"),
	SB_One5StopBits	UMETA(DisplayName = "One5StopBits"),
	SB_TwoStopBits	UMETA(DisplayName = "TwoStopBits"),
};

UENUM(BlueprintType)
enum class ERTSControl : uint8
{
	RC_Disable		UMETA(DisplayName = "Disable"),
	RC_Enable		UMETA(DisplayName = "Enable"),
	RC_Handshake	UMETA(DisplayName = "Handshake"),
	RC_Toggle		UMETA(DisplayName = "Toggle"),
};

USTRUCT(BlueprintType)
struct FPortConfig
{
	GENERATED_USTRUCT_BODY()

public:
	FPortConfig(int baudRate, int byteSize, EParityBit parity, EStopBit stopBits, bool outxCtsFlow, ERTSControl rtsControl)
		: BaudRate(baudRate), ByteSize(byteSize), Parity(parity), StopBits(stopBits), OutxCtsFlow(outxCtsFlow), RtsControl(rtsControl) {}

	FPortConfig() : FPortConfig(9600, 8, EParityBit::PB_NoParity, EStopBit::SB_OneStopBit, false, ERTSControl::RC_Enable) {}

	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		int BaudRate;
	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		int ByteSize;
	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		EParityBit Parity;
	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		EStopBit StopBits;
	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		bool OutxCtsFlow;
	UPROPERTY(BlueprintReadWrite, Category = "PortConfig")
		ERTSControl RtsControl;
};

UCLASS(BlueprintType)
class SERIAL4UNREAL_API USerialPort : public UObject
{
	GENERATED_BODY()
	
private:
	// Raw communication port config
	HANDLE mComPort;

public:
	// Communication port number
	UPROPERTY(BlueprintReadOnly, Category = "Serial4Unreal")
		int comNumber;

	// Communication port config
	UPROPERTY(BlueprintReadWrite, Category = "Serial4Unreal")
		FPortConfig  portConfig;

public:
	// Constructor
	USerialPort() {}

	// Destructor -- Close communication port
	~USerialPort();

	// Initialize port config and Open communication port
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
		void Open();

	// Send multiple bytes of data
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
		virtual int Write(FString Buffer);

	// Return the number of bytes of data accumulated in the receive buffer
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
		virtual int GetAmoutOfDataReceived();

	// Extract 1 byte from thre receive buffer
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
		virtual bool Read(FString& Data);
};

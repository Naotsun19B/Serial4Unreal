// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Serial4Unreal/Private/TickProxy.h"
#include "Serial4Unreal/Public/Serial4Unreal.h"
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
	FPortConfig(int InBaudRate, int InByteSize, EParityBit InParity, EStopBit InStopBits, bool InOutxCtsFlow, ERTSControl InRtsControl)
		: BaudRate(InBaudRate), ByteSize(InByteSize), Parity(InParity), StopBits(InStopBits), OutxCtsFlow(InOutxCtsFlow), RtsControl(InRtsControl) {}

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSerialDelegate, FString, RecivedData);

UCLASS(BlueprintType)
class SERIAL4UNREAL_API USerialPort : public UObject
{
	GENERATED_BODY()
	
private:
	// Raw communication port config
	void* ComPort;

	// A class that performs regular processing of the read buffer
	TSharedPtr<FTickProxy> Tick;

	// Temporary storage variable until all data is received
	FString ReceivedDataBuffer;

public:
	// Communication port number
	UPROPERTY(BlueprintReadOnly, Category = "Serial4Unreal", meta = (ClampMin = -1))
	int ComNumber;

	// Communication port config
	UPROPERTY(BlueprintReadOnly, Category = "Serial4Unreal")
	FPortConfig  PortConfig;

	// Use read buffer process ? (When this is false, the event dispatcher is not called)
	UPROPERTY(BlueprintReadOnly, Category = "Serial4Unreal")
	bool bIsEnableReadBufferProcess;

	// Is serial port open ?
	UPROPERTY(BlueprintReadOnly, Category = "Serial4Unreal")
	bool bIsOpen;

public:
	// Called when some data is received
	UPROPERTY(BlueprintAssignable, Category = "Serial4Unreal")
	FSerialDelegate OnDataRecived;

	// Called when all data has been received
	UPROPERTY(BlueprintAssignable, Category = "Serial4Unreal")
	FSerialDelegate OnReceptionCompleted;

public:
	// Constructor
	USerialPort();

	// Destructor
	~USerialPort();

	// Initialize port config and Open communication port
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	bool Open();

	// Close communication port
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	void Close();

	// Send multiple bytes of data
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	virtual int Write(FString Buffer);

	// Return the number of bytes of data accumulated in the receive buffer
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	virtual int64 GetAmoutOfDataReceived();

	// Extract 1 byte from thre receive buffer
	UFUNCTION(BlueprintCallable, Category = "Serial4Unreal")
	virtual bool Read(FString& Data);

private:
	// Periodic check processing of the read buffer
	void ReadBufferProcess();
};

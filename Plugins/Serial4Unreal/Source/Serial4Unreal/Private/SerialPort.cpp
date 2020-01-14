// Fill out your copyright notice in the Description page of Project Settings.

#include "SerialPort.h"

// Avoid macro overloading
#include "AllowWindowsPlatformTypes.h"	
#include <Windows.h>
#include "HideWindowsPlatformTypes.h"	

USerialPort::USerialPort()
	: Tick(nullptr)
	, ReceivedDataBuffer("")
	, ComNumber(-1)
	, bIsEnableReadBufferProcess(true)
	, bIsOpen(false)
{

}

USerialPort::~USerialPort()
{
	if (bIsOpen)
	{
		Close();
	}
}

bool USerialPort::Open()
{
	if(bIsOpen)
	{
		UE_LOG(Serial4Unreal, Warning, TEXT("The port is already open : COM%d"), ComNumber);
		return false;
	}

	//シリアルポートの構成情報
	DCB Dcb;								
	GetCommState(ComPort, &Dcb);

	Dcb.BaudRate = PortConfig.BaudRate;							//速度
	Dcb.ByteSize = PortConfig.ByteSize;							//データ長
	Dcb.Parity = static_cast<int>(PortConfig.Parity);			//パリティ
	Dcb.StopBits = static_cast<int>(PortConfig.StopBits);		//ストップビット長
	Dcb.fOutxCtsFlow = PortConfig.OutxCtsFlow;					//送信時CTSフロー
	Dcb.fRtsControl = static_cast<int>(PortConfig.RtsControl);	//RTSフロー

	SetCommState(ComPort, &Dcb);	

	//シリアルポートを開く
	TCHAR ComName[10] = { 'C','O','M', ComNumber + '0', '\0' };
	ComPort = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	//ポートが開けなかった場合
	if (ComPort == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//読み取りバッファの定期確認を開始
	if (bIsEnableReadBufferProcess)
	{
		Tick = MakeShareable(new FTickProxy([this] {ReadBufferProcess(); }));
	}
		
	bIsOpen = true;
	return true;
}

void USerialPort::Close()
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), ComNumber);
		return;
	}

	//Tickコンポーネントを破棄
	if (bIsEnableReadBufferProcess)
	{
		Tick.Reset();
	}

	//シリアルポートを閉じる
	CloseHandle(ComPort);

	bIsOpen = false;
}

int USerialPort::Write(FString Buffer)
{
	if (!bIsOpen) 
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), ComNumber);
		return -1;
	}

	//送信するデータ
	DWORD LengthOfSent = Buffer.Len();
	//実際に送信したデータ長
	DWORD NumberOfPut;

	char* Data = TCHAR_TO_ANSI(*Buffer);
	//ポートへ送信
	WriteFile(ComPort, Data, LengthOfSent, &NumberOfPut, NULL);

	return NumberOfPut;
}

int64 USerialPort::GetAmoutOfDataReceived()
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), ComNumber);
		return -1;
	}

	//受信データ数を調べる
	DWORD Errors;
	COMSTAT ComStat;
	ClearCommError(ComPort, &Errors, &ComStat);
	//受信したメッセージ長を取得する
	int LengthOfRecieved = ComStat.cbInQue;

	return static_cast<int64>(LengthOfRecieved);
}

bool USerialPort::Read(FString& Data)
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), ComNumber);
		return false;
	}

	//受信データがない場合は読み込まない
	if (GetAmoutOfDataReceived() < 1)
	{
		return false;
	}

	//受信データ格納用
	char Buffer[1];
	// 実際に受信したバイト数
	DWORD NumberOfRead;

	//データ受信
	bool bResult = ReadFile(ComPort, Buffer, 1, &NumberOfRead, NULL);

	Data = FString(UTF8_TO_TCHAR(Buffer));
	return bResult;
}

void USerialPort::ReadBufferProcess()
{
	FString TempBuffer;
	//何かしらのデータを受信した時
	if (Read(TempBuffer)) 
	{
		TempBuffer = TempBuffer.Left(1);
		OnDataRecived.Broadcast(TempBuffer);
		ReceivedDataBuffer.Append(TempBuffer);
	}
	//すべてのデータの受信が完了した時
	else if (!ReceivedDataBuffer.IsEmpty()) 
	{
		OnReceptionCompleted.Broadcast(ReceivedDataBuffer);
		ReceivedDataBuffer.Empty();
	}
}

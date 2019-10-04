// Fill out your copyright notice in the Description page of Project Settings.

#include "SerialPort.h"

USerialPort::~USerialPort()
{
	//Tickコンポーネントを破棄
	mTick.Reset();
	//シリアルポートを閉じる
	CloseHandle(mComPort);
}

void USerialPort::Open()
{
	//シリアルポートの構成情報
	DCB dcb;								
	GetCommState(mComPort, &dcb);

	dcb.BaudRate = portConfig.BaudRate;							//速度
	dcb.ByteSize = portConfig.ByteSize;							//データ長
	dcb.Parity = static_cast<int>(portConfig.Parity);			//パリティ
	dcb.StopBits = static_cast<int>(portConfig.StopBits);		//ストップビット長
	dcb.fOutxCtsFlow = portConfig.OutxCtsFlow;					//送信時CTSフロー
	dcb.fRtsControl = static_cast<int>(portConfig.RtsControl);	//RTSフロー

	SetCommState(mComPort, &dcb);	

	//シリアルポートを開く
	TCHAR com[10] = { 'C','O','M', comNumber + '0', '\0' };
	mComPort = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	//読み取りバッファの定期確認を開始
	mTick = MakeShareable(new FTickProxy([this] {ReadBufferProcess(); }));
}

int USerialPort::Write(FString Buffer)
{
	//送信するデータ
	DWORD lengthOfSent = Buffer.Len();
	//実際に送信したデータ長
	DWORD numberOfPut;

	char* data = TCHAR_TO_ANSI(*Buffer);
	//ポートへ送信
	WriteFile(mComPort, data, lengthOfSent, &numberOfPut, NULL);

	return numberOfPut;
}

int64 USerialPort::GetAmoutOfDataReceived()
{
	//受信データ数を調べる
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(mComPort, &errors, &comStat);
	//受信したメッセージ長を取得する
	int lengthOfRecieved = comStat.cbInQue;

	return static_cast<int64>(lengthOfRecieved);
}

bool USerialPort::Read(FString& Data)
{
	//受信データがない場合は読み込まない
	if (GetAmoutOfDataReceived() < 1) return false;

	//受信データ格納用
	char buf[1];
	// 実際に受信したバイト数
	DWORD numberOfRead;

	//データ受信
	bool result = ReadFile(mComPort, buf, 1, &numberOfRead, NULL);

	Data = FString(UTF8_TO_TCHAR(buf));
	return result;
}

void USerialPort::ReadBufferProcess()
{
	FString temp;
	//何かしらのデータを受信した時
	if (Read(temp)) 
	{
		temp = temp.Left(1);
		OnDataRecived.Broadcast(temp);
		mReceivedDataBuffer.Append(temp);
	}
	//すべてのデータの受信が完了した時
	else if (!mReceivedDataBuffer.IsEmpty()) 
	{
		OnReceptionCompleted.Broadcast(mReceivedDataBuffer);
		mReceivedDataBuffer.Empty();
	}
}
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

	//�V���A���|�[�g�̍\�����
	DCB Dcb;								
	GetCommState(ComPort, &Dcb);

	Dcb.BaudRate = PortConfig.BaudRate;							//���x
	Dcb.ByteSize = PortConfig.ByteSize;							//�f�[�^��
	Dcb.Parity = static_cast<int>(PortConfig.Parity);			//�p���e�B
	Dcb.StopBits = static_cast<int>(PortConfig.StopBits);		//�X�g�b�v�r�b�g��
	Dcb.fOutxCtsFlow = PortConfig.OutxCtsFlow;					//���M��CTS�t���[
	Dcb.fRtsControl = static_cast<int>(PortConfig.RtsControl);	//RTS�t���[

	SetCommState(ComPort, &Dcb);	

	//�V���A���|�[�g���J��
	TCHAR ComName[10] = { 'C','O','M', ComNumber + '0', '\0' };
	ComPort = CreateFile(ComName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	//�|�[�g���J���Ȃ������ꍇ
	if (ComPort == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//�ǂݎ��o�b�t�@�̒���m�F���J�n
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

	//Tick�R���|�[�l���g��j��
	if (bIsEnableReadBufferProcess)
	{
		Tick.Reset();
	}

	//�V���A���|�[�g�����
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

	//���M����f�[�^
	DWORD LengthOfSent = Buffer.Len();
	//���ۂɑ��M�����f�[�^��
	DWORD NumberOfPut;

	char* Data = TCHAR_TO_ANSI(*Buffer);
	//�|�[�g�֑��M
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

	//��M�f�[�^���𒲂ׂ�
	DWORD Errors;
	COMSTAT ComStat;
	ClearCommError(ComPort, &Errors, &ComStat);
	//��M�������b�Z�[�W�����擾����
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

	//��M�f�[�^���Ȃ��ꍇ�͓ǂݍ��܂Ȃ�
	if (GetAmoutOfDataReceived() < 1)
	{
		return false;
	}

	//��M�f�[�^�i�[�p
	char Buffer[1];
	// ���ۂɎ�M�����o�C�g��
	DWORD NumberOfRead;

	//�f�[�^��M
	bool bResult = ReadFile(ComPort, Buffer, 1, &NumberOfRead, NULL);

	Data = FString(UTF8_TO_TCHAR(Buffer));
	return bResult;
}

void USerialPort::ReadBufferProcess()
{
	FString TempBuffer;
	//��������̃f�[�^����M������
	if (Read(TempBuffer)) 
	{
		TempBuffer = TempBuffer.Left(1);
		OnDataRecived.Broadcast(TempBuffer);
		ReceivedDataBuffer.Append(TempBuffer);
	}
	//���ׂẴf�[�^�̎�M������������
	else if (!ReceivedDataBuffer.IsEmpty()) 
	{
		OnReceptionCompleted.Broadcast(ReceivedDataBuffer);
		ReceivedDataBuffer.Empty();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SerialPort.h"

USerialPort::~USerialPort()
{
	if (bIsOpen) Close();
}

void USerialPort::Open()
{
	//�V���A���|�[�g�̍\�����
	DCB dcb;								
	GetCommState(mComPort, &dcb);

	dcb.BaudRate = portConfig.BaudRate;							//���x
	dcb.ByteSize = portConfig.ByteSize;							//�f�[�^��
	dcb.Parity = static_cast<int>(portConfig.Parity);			//�p���e�B
	dcb.StopBits = static_cast<int>(portConfig.StopBits);		//�X�g�b�v�r�b�g��
	dcb.fOutxCtsFlow = portConfig.OutxCtsFlow;					//���M��CTS�t���[
	dcb.fRtsControl = static_cast<int>(portConfig.RtsControl);	//RTS�t���[

	SetCommState(mComPort, &dcb);	

	//�V���A���|�[�g���J��
	TCHAR com[10] = { 'C','O','M', comNumber + '0', '\0' };
	mComPort = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	//�ǂݎ��o�b�t�@�̒���m�F���J�n
	if (bIsEnableReadBufferProcess)
		mTick = MakeShareable(new FTickProxy([this] {ReadBufferProcess(); }));

	bIsOpen = true;
}

void USerialPort::Close()
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), comNumber);
		return;
	}

	//Tick�R���|�[�l���g��j��
	if (bIsEnableReadBufferProcess) mTick.Reset();
	//�V���A���|�[�g�����
	CloseHandle(mComPort);

	bIsOpen = false;
}

int USerialPort::Write(FString Buffer)
{
	if (!bIsOpen) 
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), comNumber);
		return -1;
	}

	//���M����f�[�^
	DWORD lengthOfSent = Buffer.Len();
	//���ۂɑ��M�����f�[�^��
	DWORD numberOfPut;

	char* data = TCHAR_TO_ANSI(*Buffer);
	//�|�[�g�֑��M
	WriteFile(mComPort, data, lengthOfSent, &numberOfPut, NULL);

	return numberOfPut;
}

int64 USerialPort::GetAmoutOfDataReceived()
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), comNumber);
		return -1;
	}

	//��M�f�[�^���𒲂ׂ�
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(mComPort, &errors, &comStat);
	//��M�������b�Z�[�W�����擾����
	int lengthOfRecieved = comStat.cbInQue;

	return static_cast<int64>(lengthOfRecieved);
}

bool USerialPort::Read(FString& Data)
{
	if (!bIsOpen)
	{
		UE_LOG(Serial4Unreal, Error, TEXT("The port is not open : COM%d"), comNumber);
		return false;
	}

	//��M�f�[�^���Ȃ��ꍇ�͓ǂݍ��܂Ȃ�
	if (GetAmoutOfDataReceived() < 1) return false;

	//��M�f�[�^�i�[�p
	char buf[1];
	// ���ۂɎ�M�����o�C�g��
	DWORD numberOfRead;

	//�f�[�^��M
	bool bResult = ReadFile(mComPort, buf, 1, &numberOfRead, NULL);

	Data = FString(UTF8_TO_TCHAR(buf));
	return bResult;
}

void USerialPort::ReadBufferProcess()
{
	FString temp;
	//��������̃f�[�^����M������
	if (Read(temp)) 
	{
		temp = temp.Left(1);
		OnDataRecived.Broadcast(temp);
		mReceivedDataBuffer.Append(temp);
	}
	//���ׂẴf�[�^�̎�M������������
	else if (!mReceivedDataBuffer.IsEmpty()) 
	{
		OnReceptionCompleted.Broadcast(mReceivedDataBuffer);
		mReceivedDataBuffer.Empty();
	}
}
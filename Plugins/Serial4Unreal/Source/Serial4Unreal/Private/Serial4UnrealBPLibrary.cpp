// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Serial4UnrealBPLibrary.h"
#include "Serial4Unreal.h"

// Avoid macro overloading
#include "AllowWindowsPlatformTypes.h"	
#include <Windows.h>
#pragma warning(push)				//
#pragma warning( disable : 4668 )	// Disable warnig (error C4668)
#include <SetupAPI.h>				// '<macro name>' is not defined as a preprocessor macro,
#include <winioctl.h>				// replacing with '0' for '#if/#elif'
#pragma warning(pop)				//
#include "HideWindowsPlatformTypes.h"	

USerial4UnrealBPLibrary::USerial4UnrealBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USerial4UnrealBPLibrary::CreateSerialPort(int ComNumber, FPortConfig PortConfig, bool DisableReadBufferProcess, USerialPort* &SerialPort)
{
	SerialPort = NewObject<USerialPort>();
	SerialPort->ComNumber = ComNumber;
	SerialPort->PortConfig = PortConfig;
	SerialPort->bIsEnableReadBufferProcess = !DisableReadBufferProcess;
}

bool USerial4UnrealBPLibrary::GetComPortList(TMap<int, FString>& List)
{
	HDEVINFO HDevInfo;
	SP_DEVINFO_DATA DevData = { sizeof(SP_DEVINFO_DATA) };

	//�f�o�C�X���Z�b�g���擾
	HDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); 
	if (HDevInfo == 0)
	{
		return false;
	}
	DevData.cbSize = sizeof(DevData);

	//�f�o�C�X�C���^�[�t�F�C�X���擾
	int Counter = 0;
	while (SetupDiEnumDeviceInfo(HDevInfo, Counter, &DevData)) 
	{  
		DWORD DataType;
		DWORD BufferSize;
		LPTSTR PropertyBuffer;
		int ListKey;
		FString ListValue;

		//COM�|�[�g�̖��O���擾
		HKEY Key = SetupDiOpenDevRegKey(HDevInfo, &DevData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
		if (Key) 
		{
			TCHAR KeyName[256];
			DWORD KeyType = 0;
			BufferSize = sizeof(KeyName);
			RegQueryValueEx(Key, _T("PortName"), NULL, &KeyType, (LPBYTE)KeyName, &BufferSize);
			//COM[�ԍ�]�̕����񂩂�ԍ����������o��
			sscanf_s(TCHAR_TO_ANSI(KeyName), "COM%d", &ListKey);
		}

		//�f�o�C�X�̐������擾
		BufferSize = 0;
		PropertyBuffer = NULL;
		while (!SetupDiGetDeviceRegistryProperty(HDevInfo, &DevData, SPDRP_DEVICEDESC, &DataType, (PBYTE)PropertyBuffer, BufferSize, &BufferSize)) 
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
			{
				if (PropertyBuffer)
				{
					LocalFree(PropertyBuffer);
				}
				PropertyBuffer = (LPTSTR)LocalAlloc(LPTR, BufferSize * 2);
			}
			else break;
		}
		//LPTSTR�^��FString�^�ɕϊ�
		ListValue = (TEXT("%s"), PropertyBuffer);

		//���X�g�ɒǉ�
		List.Add(ListKey, ListValue);

		if (PropertyBuffer)
		{
			LocalFree(PropertyBuffer);
		}

		Counter++;
	}
	//�f�o�C�X���Z�b�g�����
	SetupDiDestroyDeviceInfoList(HDevInfo); 
	return true;
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

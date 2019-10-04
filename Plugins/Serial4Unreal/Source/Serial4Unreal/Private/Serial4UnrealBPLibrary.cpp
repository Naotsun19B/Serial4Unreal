// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Serial4UnrealBPLibrary.h"
#include "Serial4Unreal/Public/Serial4Unreal.h"

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

bool USerial4UnrealBPLibrary::GetComPortList(TMap<int, FString>& List)
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA devData = { sizeof(SP_DEVINFO_DATA) };

	//デバイス情報セットを取得
	hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE); 
	if (hDevInfo == 0) return false;
	devData.cbSize = sizeof(devData);

	//デバイスインターフェイスを取得
	int counter = 0;
	while (SetupDiEnumDeviceInfo(hDevInfo, counter, &devData)) 
	{  
		DWORD dataType;
		DWORD bufferSize;
		LPTSTR propertyBuffer;
		int listkey;
		FString listValue;

		//COMポートの名前を取得
		HKEY key = SetupDiOpenDevRegKey(hDevInfo, &devData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
		if (key) 
		{
			TCHAR name[256];
			DWORD type = 0;
			bufferSize = sizeof(name);
			RegQueryValueEx(key, _T("PortName"), NULL, &type, (LPBYTE)name, &bufferSize);
			//COM[番号]の文字列から番号だけを取り出す
			sscanf_s(TCHAR_TO_ANSI(name), "COM%d", &listkey);
		}

		//デバイスの説明を取得
		bufferSize = 0;
		propertyBuffer = NULL;
		while (!SetupDiGetDeviceRegistryProperty(hDevInfo, &devData, SPDRP_DEVICEDESC, &dataType, (PBYTE)propertyBuffer, bufferSize, &bufferSize)) 
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
			{
				if (propertyBuffer) LocalFree(propertyBuffer);
				propertyBuffer = (LPTSTR)LocalAlloc(LPTR, bufferSize * 2);
			}
			else break;
		}
		//LPTSTR型をFString型に変換
		listValue = (TEXT("%s"), propertyBuffer);

		//リストに追加
		List.Add(listkey, listValue);

		if (propertyBuffer) LocalFree(propertyBuffer);
		counter++;
	}
	//デバイス情報セットを解放
	SetupDiDestroyDeviceInfoList(hDevInfo); 
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
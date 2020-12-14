#pragma once

#include <mferror.h>

FString ResultToString(HRESULT Result)
{
	void* DllHandle = nullptr;

	// load error resource library
	if (HRESULT_FACILITY(Result) == FACILITY_MF)
	{
		const LONG Code = HRESULT_CODE(Result);

		if (((Code >= 0) && (Code <= 1199)) || ((Code >= 3000) && (Code <= 13999)))
		{
			static void* WmErrorDll = nullptr;

			if (WmErrorDll == nullptr)
			{
				WmErrorDll = FPlatformProcess::GetDllHandle(TEXT("wmerror.dll"));
			}

			DllHandle = WmErrorDll;
		}
		else if ((Code >= 2000) && (Code <= 2999))
		{
			static void* AsfErrorDll = nullptr;

			if (AsfErrorDll == nullptr)
			{
				AsfErrorDll = FPlatformProcess::GetDllHandle(TEXT("asferror.dll"));
			}

			DllHandle = AsfErrorDll;
		}
		else if ((Code >= 14000) & (Code <= 44999))
		{
			static void* MfErrorDll = nullptr;

			if (MfErrorDll == nullptr)
			{
				MfErrorDll = FPlatformProcess::GetDllHandle(TEXT("mferror.dll"));
			}

			DllHandle = MfErrorDll;
		}
	}

	TCHAR Buffer[1024];
	Buffer[0] = TEXT('\0');
	DWORD BufferLength = 0;

	// resolve error code
	if (DllHandle != nullptr)
	{
		BufferLength = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS, DllHandle, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Buffer, 1024, NULL);
	}
	else
	{
		BufferLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, Result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Buffer, 1024, NULL);
	}

	if (BufferLength == 0)
	{
		return FString::Printf(TEXT("0x%08x"), Result);
	}

	// remove line break
	TCHAR* NewLine = FCString::Strchr(Buffer, TEXT('\r'));

	if (NewLine != nullptr)
	{
		*NewLine = TEXT('\0');
	}

	return Buffer;
}
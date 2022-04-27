// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "pch.h"
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <iostream>
#include <string>

HANDLE GetProcessByName(const std::wstring& processName)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			// Compare process.szExeFile based on format of name, i.e., trim file path
			// trim .exe if necessary, etc.
			if (!processName.compare(process.szExeFile))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	return NULL;
}

HMODULE GetModule(const std::wstring& processName, HANDLE& pHandle)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	unsigned int i;

	if (EnumProcessModules(pHandle, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			TCHAR szModName[MAX_PATH];
			if (GetModuleFileNameEx(pHandle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
			{
				std::wstring wstrModName = szModName;
				//you will need to change this to the name of the exe of the foreign process
				if (wstrModName.find(processName) != std::string::npos)
				{
					return hMods[i];
				}
			}
		}
	}
	return nullptr;
}

void debugMessage(std::string message)
{
	std::wstring wMessage = std::wstring(message.begin(), message.end());
	MessageBox(NULL, wMessage.c_str(), L"Debug", MB_OK | MB_SETFOREGROUND);
}

void hackThread()
{
	HANDLE pHandle = GetProcessByName(L"ac_client.exe");
	if (!pHandle)
	{
		return;
	}

	DWORD applicationBaseAddress = (DWORD)GetModule(L"ac_client.exe", pHandle);
	CloseHandle(pHandle);

	//Get local player & their health
	DWORD* localPlayer = (DWORD*)(applicationBaseAddress + 0x0018AC00);
	DWORD localPlayerAddress = *localPlayer;
	uint32_t* health = (uint32_t*)(localPlayerAddress + 0xEC);
	uint32_t healthValue = *health;

	debugMessage(std::to_string(healthValue));
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);		//disables attach and detach notifications
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hackThread, NULL, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "pch.h"
#include <iostream>
#include <string>

void hackThread();

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
		//DisableThreadLibraryCalls(hModule);		//disables attach and detach notifications
		//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hackThread, NULL, NULL, NULL);
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void hackThread()
{
	//Set console use for dll
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	//freopen("/my/newstdin", "r", stdin);

	//std::cout << "Debug log test" << std::endl;
	//int x;
	//std::cin >> x;

	MessageBox(NULL, L"Com Object Function Called", L"COMServer", MB_OK | MB_SETFOREGROUND);

	//Get local player & their health
	DWORD* localPlayer = (DWORD*)0x0017E254;
	MessageBox(NULL, L"Com Object Function Called 2", L"COMServer", MB_OK | MB_SETFOREGROUND);

	int localPlayerAddress = *localPlayer;

	MessageBox(NULL, L"Com Object Function Called 3", L"COMServer", MB_OK | MB_SETFOREGROUND);

	int* health = (int*)(localPlayerAddress + 0xEC);

	MessageBox(NULL, L"Com Object Function Called 4", L"COMServer", MB_OK | MB_SETFOREGROUND);

	std::string s = std::to_string(*health);
	std::wstring stemp = std::wstring(s.begin(), s.end());
	MessageBox(NULL, stemp.c_str(), L"COMServer", MB_OK | MB_SETFOREGROUND);

	std::cout << *health;
}


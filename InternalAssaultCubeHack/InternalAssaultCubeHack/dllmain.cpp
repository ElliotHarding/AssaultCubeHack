// dllmain.cpp : Defines the entry point for the DLL application.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "pch.h"
#include <iostream>

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
		DisableThreadLibraryCalls(hModule);		//disables attach and detach notifications
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hackThread, NULL, NULL, NULL);
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void hackThread()
{
	//Set console use for dll
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("/my/newstdin", "r", stdin);

	std::cout << "Debug log test" << std::endl;
	int x;
	std::cin >> x;

	//Get local player & their health
	DWORD* localPlayer = (DWORD*)0x0017E254;
	int* health = (int*)(*localPlayer + 0xEC);

	std::cout << *health;
}


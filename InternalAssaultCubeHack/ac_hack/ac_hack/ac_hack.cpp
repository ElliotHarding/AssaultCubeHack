// ac_hack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

static int cStaticPlayerPointerAddress = 0x0017E254; //0x0018AC00 alt 254 seems not loaded at start...
static int cStaticHealthOffset = 0xEC;

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

template<class T>
bool readMemory(HANDLE pHandle, LPCVOID address, T& value)
{
	SIZE_T r = 0;
	if (!ReadProcessMemory(pHandle, address, &value, sizeof(T), &r))
	{
		std::cout << "ReadProcessMemory failed" << std::endl;
		std::cout << GetLastError();
		return false;
	}
	return true;
}

template<class T>
bool writeMemory(HANDLE pHandle, LPVOID address, T& value)
{
	DWORD oldprotect;
	if (!VirtualProtectEx(pHandle, address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldprotect))
	{
		std::cout << "VirtualProtectEx failed" << std::endl;
		std::cout << GetLastError();
		return false;
	}
	SIZE_T r = 0;
	if (!WriteProcessMemory(pHandle, address, &value, sizeof(T), &r))
	{
		std::cout << "WriteProcessMemory failed" << std::endl;
		std::cout << GetLastError();
		return false;
	}
	if (!VirtualProtectEx(pHandle, address, sizeof(T), oldprotect, &oldprotect))
	{
		std::cout << "VirtualProtectEx failed" << std::endl;
		std::cout << GetLastError();
		return false;
	}
	return true;
}

int main()
{
	HANDLE pHandle = GetProcessByName(L"ac_client.exe");
	if (!pHandle)
	{
		std::cout << "GetProcessByName returned null" << std::endl;
		std::cout << GetLastError();
		return 0;
	}

	DWORD applicationBaseAddress = (DWORD)GetModule(L"ac_client.exe", pHandle);

	UINT_PTR localPlayerLocation = 0;
	if(!readMemory<UINT_PTR>(pHandle, (LPCVOID)(cStaticPlayerPointerAddress + applicationBaseAddress), localPlayerLocation))
	{
		CloseHandle(pHandle);
		return 0;
	}

	unsigned int healthLocation = localPlayerLocation + cStaticHealthOffset;
	unsigned int newValue = 300;

	if (!writeMemory<unsigned int>(pHandle, (LPVOID)healthLocation, newValue))
	{
		CloseHandle(pHandle);
		return 0;
	}

	CloseHandle(pHandle);
	return 0;
}

#include <iostream>
#include <Windows.h>

int main()
{
	DWORD pid;
	HWND program = FindWindowA(NULL, "AssaultCube");

	if (program != NULL)
	{
		GetWindowThreadProcessId(program, &pid);
		HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		if (process != NULL)
		{
			char dllName[MAX_PATH];
			GetFullPathNameA("InternalAssaultCubeHack.dll", MAX_PATH, dllName, NULL);
			LPVOID loadLib = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
			LPVOID remoteString = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllName), /*MEM_RESERVE |*/ MEM_COMMIT, PAGE_READWRITE);
			if (WriteProcessMemory(process, remoteString, dllName, strlen(dllName), NULL))
			{
				HANDLE h = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)loadLib, (LPVOID)remoteString, NULL, NULL);
				std::cout << "Succeeded";
				CloseHandle(process);
				return 0;
			}
			CloseHandle(process);
		}
	}

	std::cout << "Failed";
	return 0;
}

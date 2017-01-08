#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define BHOP_KEY 'c'	//press this to bhop

struct module_t {
	DWORD dwBase, dwSize;
};

//debugger claass from csgo api

class CDebugger {
private:
	DWORD dwPid;
	HANDLE hProcess;
public:
	bool attach(char * szProcess);
	module_t GetModule(char *szModule);

	//reads and write to and from game memory
	template <typename T> T ReadMemory(DWORD Address) {
		T read;
		ReadProcessMemory(hProcess, Address, &read, sizeof(T), 0);
	}

	template <typename T> T WriteMemory(DWORD value) {
		T read;
		WriteProcessMemory(hProcess, Address, &value, sizeof(T), 0);
	}
};

bool CDebugger::attach(char*szProcess) {
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 entry;
	do {
		if (!strcmp(entry.szExeFile, szProcess)) {
			dwPid = entry.th32ProcessID;
			CloseHandle(handle);
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
			return true;
		}
	} while (Process32Next(handle, &entry));
	return false;
}

module_t CDebugger::GetModule(char *szModule) {
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, NULL);
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(entry);
	do {
		if (!strcmp(entry.szModule, szModule)) {
			dwPid = entry.th32ModuleID;
			CloseHandle(handle);
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwPid);
			return{ (DWORD)entry.hModule, entry.modBaseSize };
		}
	}while (Module32Next(handle, &entry));
	return{ (DWORD)false, false };
};

//offsets:

DWORD dwLocalPlayer = 0xA9948C;
DWORD dwJump = 0x4AD0374;
DWORD dwFlags = 0x100;

CDebugger debugger;
module_t moduleClient;

module_t* GetModuleClient()
{
	if (!moduleClient.dwBase && !moduleClient.dwSize) {
		moduleClient = debugger.GetModule("client.dll");
	}
	return &moduleClient;
}
class CBunnyhop
{
public:
    static unsigned long __stdcall ThreadRountime(void*)
    {
        while (true)
        {
            DWORD dwMe = debugger.ReadMemory<DWORD>(GetModuleClient()->dwBase + dwLocalPlayer);
            int CanJump = debugger.ReadMemory<DWORD>(dwMe + dwFlags);
            if ((GetAsyncKeyState(BHOP_KEY) & 0x8000) && (CanJump & 0x1 == true)) {
                debugger.WriteMemory<int>(GetModuleClient()->dwBase + dwJump); //jump in game
                Sleep(50);
                debugger.WriteMemory<int>(GetModuleClient()->dwBase + dwJump);
            }
        }
    }
};
 
int main()
{
    while (!debugger.attach("csgo.exe")) { 
        Sleep(100);
    }
    CreateThread(0, 0, &CBunnyhop::ThreadRountime, 0, 0, 0); 
    while (1) {
        Sleep(100); 
    }
 
}
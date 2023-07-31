#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <psapi.h>
#include <shlwapi.h>
#include <string.h>
#include <wchar.h>
#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")
#include "tlhelp32.h"
#include "CdigitalSig.h"
_ZwQueryVirtualMemory fnZwQueryVirtualMemory = NULL;

BOOL Is64BitPorcess(HANDLE hProcess)
{
	BOOL bIsWow64 = false;
	IsWow64Process(hProcess, &bIsWow64);
	return bIsWow64 == false;
}
BOOL EnableDebugPrivilege(BOOL bEnable)
{
	//Enabling the debug privilege allows the application to see
	//information about service application
	BOOL fOK = FALSE;     //Assume function fails
	HANDLE hToken;
	//Try to open this process's acess token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		//Attempt to modify the "Debug" privilege
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOK = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOK;
}

void WCharToChar(const WCHAR* tchar, char* _char)
{
	int iLength;
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}
void CharToWchar(const char* _char, WCHAR* tchar)
{
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}
BOOL DosPathToNtPath(WCHAR* pszDosPath, LPTSTR pszNtPath)
{
	TCHAR            szDriveStr[500];
	TCHAR            szDrive[3];
	TCHAR            szDevName[100];
	INT                cchDevName;
	INT                i;

	//������
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//��ȡ���ش����ַ���
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), L"A:\\") || !lstrcmpi(&(szDriveStr[i]), L"B:\\"))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//��ѯ Dos �豸��
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_wcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//����
			{
				lstrcpy(pszNtPath, szDrive);//����������
				lstrcat(pszNtPath, pszDosPath + cchDevName);//����·��

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}
DWORD64 GetProcessMoudleHandle(DWORD pid) {
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(0x00000008, pid);
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(handle, &moduleEntry)) {
		CloseHandle(handle);
		return NULL;
	}
	do {
		CloseHandle(handle);
		return (DWORD64)moduleEntry.hModule;
	} while (Module32Next(handle, &moduleEntry));
	return 0;
}
bool CheckThreadIsInCodeSection(DWORD64 pAddress, HANDLE pHandle, DWORD64 pID) {
	bool result = false;
	MODULEENTRY32 moduleEntry;
	HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	char* AllocBuff = (char*)VirtualAlloc(NULL, PE_BUFF_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (AllocBuff) {
		if (!Module32First(handle, &moduleEntry)) {
			CloseHandle(handle);
			return false;
		}
		do {
			DWORD64 ReadNum = 0;
			if (ReadProcessMemory(pHandle, moduleEntry.modBaseAddr, AllocBuff, PE_BUFF_SIZE, &ReadNum)) {
				if (AllocBuff[0] == 'M' && AllocBuff[1] == 'Z') {
					PIMAGE_DOS_HEADER CopyDosHead = (PIMAGE_DOS_HEADER)AllocBuff;
					PIMAGE_NT_HEADERS CopyNthead = (PIMAGE_NT_HEADERS)((LPBYTE)AllocBuff + CopyDosHead->e_lfanew);
					DWORD64 BaseOfCode = 0;
					DWORD64 SizeOfCode = 0;
					if (CopyNthead->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
						PIMAGE_NT_HEADERS64 CopyNthead64 = (PIMAGE_NT_HEADERS64)CopyNthead;
						BaseOfCode = CopyNthead64->OptionalHeader.BaseOfCode;
						SizeOfCode = CopyNthead64->OptionalHeader.SizeOfCode;
					}
					else {
						PIMAGE_NT_HEADERS32 CopyNthead32 = (PIMAGE_NT_HEADERS32)CopyNthead;
						BaseOfCode = CopyNthead32->OptionalHeader.BaseOfCode;
						SizeOfCode = CopyNthead32->OptionalHeader.SizeOfCode;
					}
					if (pAddress >= (DWORD64)moduleEntry.modBaseAddr + BaseOfCode && pAddress <= (DWORD64)moduleEntry.modBaseAddr + BaseOfCode + SizeOfCode) {
						result = true;
						break;
					}
				}
			}

		} while (Module32Next(handle, &moduleEntry));
		VirtualFree(AllocBuff, 0, MEM_RELEASE);
	}
	CloseHandle(handle);
	return result;
}
void CheckThreadAddressIsExcute(DWORD64 pAddress,HANDLE pHandle, HANDLE pID, HANDLE Tid, BOOL isRipBackTrack) {

	DWORD64 ReadNum = 0;
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	if (fnZwQueryVirtualMemory(pHandle, (PVOID)pAddress, MemoryBasicInformation, &mbi, sizeof(mbi), &ReadNum) >= 0) {
		if (mbi.AllocationBase) {
			if (mbi.Type != MEM_IMAGE) {
				BOOL CheckExcuteFlag = mbi.AllocationProtect & PAGE_EXECUTE || mbi.AllocationProtect & PAGE_EXECUTE_READ || mbi.AllocationProtect & PAGE_EXECUTE_READWRITE || mbi.AllocationProtect & PAGE_EXECUTE_WRITECOPY;
				if (CheckExcuteFlag)
				{
					printf("\t => [�̶߳�ջ����] ��⵽δ֪�ڴ�����[VirtualAlloc��ɱ?] ��ַ %p PID %d TID %d \n", pAddress, pID, Tid);
					char PEStack[0x2];
					if (ReadProcessMemory(pHandle, mbi.BaseAddress, PEStack, sizeof(PEStack), &ReadNum)) {
						if (PEStack[0] == 'M' && PEStack[1] == 'Z') {
							printf("\t => [!!!�̶߳�ջ����!!!] ��⵽�ڴ���س��� �̵߳�ַ %p PID %d TID %d �ڴ����ģ���ַ: %p\n", pAddress, pID, Tid, mbi.BaseAddress);
						}
					}
				}
				else if (isRipBackTrack && mbi.AllocationProtect & PAGE_READONLY && mbi.AllocationProtect & PAGE_NOACCESS) {
					printf("\t => [�̶߳�ջ����] ��⵽�߳����ڲ���ִ�еĴ�������ִ�й�[�����Ƿ���Rootkit���ڻ����Ƿ�Hook] ��ַ %p PID %d TID %d \n", pAddress, pID, Tid);
				}
			}
		}
	}
	return;
}
void ThreadStackWalk() {
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	DWORD ExitCode = 0;
	hThreadSnap = CreateToolhelp32Snapshot(0x00000004, GetCurrentProcessId());
	if (hThreadSnap)
	{
		te32.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(hThreadSnap, &te32))
		{
			CloseHandle(hThreadSnap);
			return;
		}
		do
		{
			if (te32.th32OwnerProcessID != GetCurrentProcessId() && te32.th32ThreadID != GetCurrentThreadId())
			{
				HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, 0, te32.th32ThreadID);
				if (hThread && hThread != (HANDLE)-1)
				{
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, te32.th32OwnerProcessID);
					if (hProcess) {
						STACKFRAME_EX StackFarmeEx;
						memset(&StackFarmeEx, 0, sizeof(STACKFRAME_EX));
						if (Is64BitPorcess(hProcess)) {
							CONTEXT context = { 0 };
							context.ContextFlags = CONTEXT_ALL;
							if (GetThreadContext(hThread, &context))
							{
								if (context.Dr0 != 0 || context.Dr1 != 0 || context.Dr2 != 0 || context.Dr3 != 0)
								{
									//hwbp hook
									printf("\t => [�̶߳�ջ����] ��⵽HWBP Hook PID %d TID %d \n", te32.th32OwnerProcessID, te32.th32ThreadID);
								}
								CheckThreadAddressIsExcute(context.Rip, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, TRUE);
								/*
								if (CheckThreadIsInCodeSection(context.Rip, hProcess, (DWORD64)te32.th32OwnerProcessID) == false){
									__debugbreak();
								}
								*/
								StackFarmeEx.AddrPC.Offset = context.Rip;
								StackFarmeEx.AddrPC.Mode = AddrModeFlat;
								StackFarmeEx.AddrStack.Offset = context.Rsp;
								StackFarmeEx.AddrStack.Mode = AddrModeFlat;
								StackFarmeEx.AddrFrame.Offset = context.Rsp;
								StackFarmeEx.AddrFrame.Mode = AddrModeFlat;
								DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
								while (true)
								{
									if (!StackWalkEx(machineType, hProcess, hThread, &StackFarmeEx, &context, NULL, NULL, NULL, NULL, NULL))
										break;
									if (StackFarmeEx.AddrFrame.Offset == 0)
										break;
									CheckThreadAddressIsExcute(StackFarmeEx.AddrPC.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, TRUE);
									CheckThreadAddressIsExcute(StackFarmeEx.AddrReturn.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, FALSE);
									CheckThreadAddressIsExcute(StackFarmeEx.AddrStack.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, FALSE);
								}
							}
						} else {
							WOW64_CONTEXT context = { 0 };
							context.ContextFlags = CONTEXT_ALL;
							if (Wow64GetThreadContext(hThread, &context))
							{
								if (context.Dr0 != 0 || context.Dr1 != 0 || context.Dr2 != 0 || context.Dr3 != 0)
								{
									//hwbp hook
									printf("\t => [�̶߳�ջ����] ��⵽HWBP Hook PID %d TID %d \n", te32.th32OwnerProcessID, te32.th32ThreadID);
								}
								CheckThreadAddressIsExcute(context.Eip, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, TRUE);
								/*
								if (CheckThreadIsInCodeSection(context.Eip, hProcess, (DWORD64)te32.th32OwnerProcessID) == false) {
									__debugbreak();
								}
								*/

								StackFarmeEx.AddrPC.Offset = context.Eip;
								StackFarmeEx.AddrPC.Mode = AddrModeFlat;
								StackFarmeEx.AddrStack.Offset = context.Esp;
								StackFarmeEx.AddrStack.Mode = AddrModeFlat;
								StackFarmeEx.AddrFrame.Offset = context.Ebp;
								StackFarmeEx.AddrFrame.Mode = AddrModeFlat;
								DWORD machineType = IMAGE_FILE_MACHINE_I386;//IMAGE_FILE_MACHINE_I386; IMAGE_FILE_MACHINE_AMD64;
								while (true)
								{
									if (!StackWalkEx(machineType, hProcess, hThread, &StackFarmeEx, NULL, NULL, NULL, NULL, NULL, NULL))
										break;
									if (StackFarmeEx.AddrFrame.Offset == 0)
										break;
									CheckThreadAddressIsExcute(StackFarmeEx.AddrPC.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, TRUE);
									CheckThreadAddressIsExcute(StackFarmeEx.AddrReturn.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, FALSE);
									CheckThreadAddressIsExcute(StackFarmeEx.AddrStack.Offset, hProcess, (HANDLE)te32.th32OwnerProcessID, (HANDLE)te32.th32ThreadID, FALSE);
								}
							}
						}
						CloseHandle(hProcess);
					}
					CloseHandle(hThread);
				}
			}

		} while (Thread32Next(hThreadSnap, &te32));
		CloseHandle(hThreadSnap);
	}
}
void WalkProcessMoudle(DWORD pID,HANDLE pHandle,WCHAR* pMoudleName,BOOL pCheckMoudle) {
	if (pCheckMoudle == false)
		return;
	MODULEENTRY32 moduleEntry;
	HANDLE handle = NULL;
	handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
	ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	char* AllocBuff = (char*)VirtualAlloc(NULL, PE_BUFF_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (AllocBuff) {
		if (!Module32First(handle, &moduleEntry)) {
			CloseHandle(handle);
			return;
		}
		do {
			if (pCheckMoudle) {
				CdigitalSig DigitalSig(moduleEntry.szExePath);
				DWORD dDigitalState = DigitalSig.GetDigitalState();
				if (dDigitalState != DIGITAL_SIGSTATE_VALID) {
					printf("\t => [ģ��ɨ��] ��⵽����ģ��(Ҳ������) ·�� %ws �������� %ws pid %d \n", moduleEntry.szExePath, pMoudleName, pID);
				}
			}
			/*
			DWORD64 ReadNum = 0;
			if (ReadProcessMemory(pHandle, moduleEntry.modBaseAddr, AllocBuff, PE_BUFF_SIZE, &ReadNum)) {
				if (AllocBuff[0] == 'M' && AllocBuff[1] == 'Z') {
					PIMAGE_DOS_HEADER CopyDosHead = (PIMAGE_DOS_HEADER)AllocBuff;
					PIMAGE_NT_HEADERS CopyNthead = (PIMAGE_NT_HEADERS)((LPBYTE)AllocBuff + CopyDosHead->e_lfanew);
					DWORD64 BaseOfCode = 0;
					DWORD64 SizeOfCode = 0;
					DWORD64 SizeSignature = 0;
					DWORD64 SizeFileHeader = 0;
					DWORD64 SizeFileHeaderSizeOfOptionalHeader = 0;
					if (CopyNthead->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64) {
						PIMAGE_NT_HEADERS64 CopyNthead64 = (PIMAGE_NT_HEADERS64)CopyNthead;
						BaseOfCode = CopyNthead64->OptionalHeader.BaseOfCode;
						SizeOfCode = CopyNthead64->OptionalHeader.SizeOfCode;
						SizeSignature = sizeof(CopyNthead64->Signature);
						SizeFileHeader = sizeof(CopyNthead64->FileHeader);
						SizeFileHeaderSizeOfOptionalHeader = CopyNthead64->FileHeader.SizeOfOptionalHeader;
					}
					else {
						PIMAGE_NT_HEADERS32 CopyNthead32 = (PIMAGE_NT_HEADERS32)CopyNthead;
						BaseOfCode = CopyNthead32->OptionalHeader.BaseOfCode;
						SizeOfCode = CopyNthead32->OptionalHeader.SizeOfCode;
						SizeSignature = sizeof(CopyNthead32->Signature);
						SizeFileHeader = sizeof(CopyNthead32->FileHeader);
						SizeFileHeaderSizeOfOptionalHeader = CopyNthead32->FileHeader.SizeOfOptionalHeader;
					}
					PIMAGE_SECTION_HEADER SectionHeader = (PIMAGE_SECTION_HEADER)((PUCHAR)CopyNthead + SizeSignature + SizeFileHeader + SizeFileHeaderSizeOfOptionalHeader);
					int FoundNum = 0;
					for (WORD i = 0; i < CopyNthead->FileHeader.NumberOfSections; i++)
					{
						if (SectionHeader[i].Characteristics & IMAGE_SCN_MEM_EXECUTE) {
							FoundNum++;
						}
						if (FoundNum > 3) {
							printf("\t => [���̼��] ��⵽����Ŀ�ִ������(.rdata��ɱ or �ӿǳ���) ������ %ws ·�� %ws ����id %d\n", pMoudleName, moduleEntry.szExePath, pID);
							break;
						}
						
					}
				}
			}
			*/

		} while (Module32Next(handle, &moduleEntry));
		VirtualFree(AllocBuff, 0, MEM_RELEASE);
	}
	CloseHandle(handle);
}
void ProcessStackWalk() {
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot error.\n");
		return;
	}
	BOOL bProcess = Process32First(hProcessSnap, &pe32);
	while (bProcess)
	{
		//��ӡ�������ͽ���ID
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pe32.th32ProcessID);
		if (hProcess) {
			WCHAR szImagePath[MAX_PATH];
			WCHAR pszFullPath[MAX_PATH];
			if (GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
			{
				if (DosPathToNtPath(szImagePath, pszFullPath))
				{
					CdigitalSig DigitalSig(pszFullPath);
					DWORD dDigitalState = DigitalSig.GetDigitalState();
					if (dDigitalState == DIGITAL_SIGSTATE_REVOKED || dDigitalState == DIGITAL_SIGSTATE_EXPIRE) {
						printf("\t => [����ɨ��] ��⵽����ǩ������ ·�� %ws static %d \n", pszFullPath, dDigitalState);
					}
					WalkProcessMoudle(pe32.th32ProcessID, hProcess, pe32.szExeFile, dDigitalState == DIGITAL_SIGSTATE_VALID);
				}
			}
			CloseHandle(hProcess);
		}
		bProcess = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return;

}
void ScanSystemDrivers() {
	DWORD cbNeeded = 0; // drivers[] ���ص��ֽ���
	LPVOID drivers[MAX_PATH] = { 0 }; // ���������ַ�б�����
	int cDrivers = 0;	// ��������
	Wow64EnableWow64FsRedirection(0);
	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers)) // EnumDeviceDrivers ����ÿ�������ļ��ļ��ص�ַ
	{
		char szDriver[MAX_PATH] = { 0 };	// �����ļ���
		char szPath[MAX_PATH] = { 0 };	// ��������ļ�ȫ·��
		char szNtPath[MAX_PATH] = { 0 };
		char szSystemPath[MAX_PATH] = { 0 }; // ��� system32 �ļ���·��
		cDrivers = cbNeeded / sizeof(LPVOID);	// ��������

		for (int i = 0; i < cDrivers; i++)
		{
			if (drivers[i]) {
				if (GetDeviceDriverBaseNameA(drivers[i], szDriver, sizeof(szDriver) / sizeof(LPVOID)))
				{
					if (GetDeviceDriverFileNameA(drivers[i], szPath, sizeof(szPath))) {
						bool isSystemDriver = true;
						//ֻ�жϷ�ϵͳ����
						if (szPath[1] == '?')
						{
							isSystemDriver = false;
							int len = strlen(szPath);
							szPath[len + 1] = '\0';
							for (int j = 0; j < len; j++)
							{
								szPath[j] = szPath[j + 4];
							}
							WCHAR UnicodeFilePath[MAX_PATH << 1] = { 0 };
							CharToWchar(szPath, UnicodeFilePath);
							CdigitalSig DigitalSig(UnicodeFilePath);
							DWORD dDigitalState = DigitalSig.GetDigitalState();
							if (dDigitalState != DIGITAL_SIGSTATE_VALID) {
								printf("\t => [����ɨ��] ��⵽δ֪���� ·�� %ws \n", UnicodeFilePath);
							}
						}
					}
				}
			}
		}
	}
	Wow64EnableWow64FsRedirection(1);
}
int main()
{
	printf("DuckMemoryScan By huoji 2021-02-23 \n");
	if (EnableDebugPrivilege(true) == false) {
		printf("Ȩ������ʧ��,���Թ���Ա������� \n");
		system("pause");
		return 0;
	}
	if (fnZwQueryVirtualMemory == NULL) {
		fnZwQueryVirtualMemory = (_ZwQueryVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"),"ZwQueryVirtualMemory");
		if (fnZwQueryVirtualMemory == NULL)
		{
			printf("û���ҵ�ZwQueryVirtualMemory����, ���޸�Դ��ZwQueryVirtualMemory => VirtualQueryEx \n");
			system("pause");
			return 0;
		}
	}
	printf("�̶߳�ջ���ݼ�� ... \n");
	ThreadStackWalk();
	printf("�������... \n");
	ScanSystemDrivers();
	printf("���̼��... \n");
	ProcessStackWalk();
	printf("������ ... \n");
	system("pause");
	return 0;
}
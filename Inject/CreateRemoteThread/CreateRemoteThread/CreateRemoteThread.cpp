/*
x86��x64��ע����Ϊx64��ϵͳ�����˽϶��Ȩ�޵�У�飬��Ҫ������Ȩ����
x64��Ȩ��Ҫ�����õ���ntdll.dll�е�δ����������RtlAdjustPrivilege().
*/
#include "stdafx.h"
#include "CreateRemoteThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

typedef enum  _WIN_VERSION
{
	WindowsNT,
	Windows2000,
	WindowsXP,
	Windows2003,
	WindowsVista,
	Windows7,
	Windows8,
	WinUnknown
}WIN_VERSION;

VOID InjectDll(ULONG_PTR ProcessID);
WIN_VERSION  GetWindowsVersion();
BOOL InjectDllByRemoteThread32(const TCHAR* wzDllFile, ULONG_PTR ProcessId);
WIN_VERSION  WinVersion = WinUnknown;

BOOL InjectDllByRemoteThread64(const TCHAR* wzDllFile, ULONG_PTR ProcessId);
typedef long (__fastcall *pfnRtlAdjustPrivilege64)(ULONG,ULONG,ULONG,PVOID);
pfnRtlAdjustPrivilege64 RtlAdjustPrivilege;





int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	cout<<"�鿴Ҫע����̵�ID"<<endl;   
	ULONG_PTR ProcessID = 0;
	WinVersion = GetWindowsVersion();
	printf("Input ProcessID\r\n");
	cin>>ProcessID;
	InjectDll(ProcessID);
	return 0;
}

VOID InjectDll(ULONG_PTR ProcessID)
{
    CString strPath32 = L"Dllx86.dll";   //32λdllע��32λϵͳ
    CString strPath64 = L"Dllx64.dll";
    if (ProcessID == 0)
    {
        return;
    }
    if (PathFileExists(strPath32)&&PathFileExists(strPath64))
    {
        switch(WinVersion)
        {
        case Windows7:   //�����õ���Win7 x64 sp1
            {

                WCHAR wzPath[MAX_PATH] = {0};
                GetCurrentDirectory(260,wzPath);
                wcsncat_s(wzPath, L"\\", 2);
                wcsncat_s(wzPath, strPath64.GetBuffer(), strPath64.GetLength());//dll����·��
                strPath32.ReleaseBuffer();
                if (!InjectDllByRemoteThread64(wzPath,ProcessID))
                    printf("Inject Fail\r\n");
                else printf ("Inject Success\r\n");
                break;
            }

        case WindowsXP:  //WinXp x86 sp3
            {
                WCHAR wzPath[MAX_PATH] = {0};
                GetCurrentDirectory(260,wzPath);
                wcsncat_s(wzPath, L"\\", 2);
                wcsncat_s(wzPath, strPath32.GetBuffer(), strPath32.GetLength());

                strPath32.ReleaseBuffer();
                if (!InjectDllByRemoteThread32(wzPath,ProcessID))
                    printf("Inject Fail\r\n");            
                else printf("Inject Success\r\n");
                break;
            }
        }
    
    }    
}



BOOL InjectDllByRemoteThread64(const TCHAR* wzDllFile, ULONG_PTR ProcessId)
{
    if (NULL == wzDllFile || 0 == ::_tcslen(wzDllFile) || ProcessId == 0 || -1 == _taccess(wzDllFile, 0))
    {
        return FALSE;
    }
    HANDLE                 hProcess = NULL;
    HANDLE                 hThread  = NULL;
    DWORD                  dwRetVal    = 0;
    LPTHREAD_START_ROUTINE FuncAddress = NULL;
    DWORD  dwSize = 0;
    TCHAR* VirtualAddress = NULL;
    //Ԥ���룬֧��Unicode
#ifdef _UNICODE
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
#endif

    if (FuncAddress==NULL)
    {
        return FALSE;
    }

    RtlAdjustPrivilege=(pfnRtlAdjustPrivilege64)GetProcAddress((HMODULE)(FuncAddress(L"ntdll.dll")),"RtlAdjustPrivilege");

    if (RtlAdjustPrivilege==NULL)
    {
        return FALSE;
    }
        /*
        .���� SE_BACKUP_PRIVILEGE, "17", ����
        .���� SE_RESTORE_PRIVILEGE, "18", ����
        .���� SE_SHUTDOWN_PRIVILEGE, "19", ����
        .���� SE_DEBUG_PRIVILEGE, "20", ����
        */
    RtlAdjustPrivilege(20,1,0,&dwRetVal);  //19

    hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, ProcessId);

    if (NULL == hProcess)
    {
        printf("Open Process Fail\r\n");
        return FALSE;
    }

    // ��Ŀ������з����ڴ�ռ�
    dwSize = (DWORD)::_tcslen(wzDllFile) + 1;
    VirtualAddress = (TCHAR*)::VirtualAllocEx(hProcess, NULL, dwSize * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);  
    if (NULL == VirtualAddress)
    {
        printf("Virtual Process Memory Fail\r\n");
        CloseHandle(hProcess);
        return FALSE;
    }

    // ��Ŀ����̵��ڴ�ռ���д���������(ģ����)
    if (FALSE == ::WriteProcessMemory(hProcess, VirtualAddress, (LPVOID)wzDllFile, dwSize * sizeof(TCHAR), NULL))
    {
        printf("Write Data Fail\r\n");
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }

    hThread = ::CreateRemoteThread(hProcess, NULL, 0, FuncAddress, VirtualAddress, 0, NULL);
    if (NULL == hThread)
    {
        printf("CreateRemoteThread Fail\r\n");
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }
    // �ȴ�Զ���߳̽���
    WaitForSingleObject(hThread, INFINITE);
    // ������Դ
    VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return TRUE;

}


BOOL InjectDllByRemoteThread32(const TCHAR* wzDllFile, ULONG_PTR ProcessId)
{
    // ������Ч
    if (NULL == wzDllFile || 0 == ::_tcslen(wzDllFile) || ProcessId == 0 || -1 == _taccess(wzDllFile, 0))
    {    
        return FALSE;
    }
    HANDLE hProcess = NULL;
    HANDLE hThread  = NULL;
    DWORD dwSize = 0;
    TCHAR* VirtualAddress = NULL;
    LPTHREAD_START_ROUTINE FuncAddress = NULL;
    // ��ȡĿ����̾��
    hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, ProcessId);
    if (NULL == hProcess)
    {
        printf("Open Process Fail\r\n");
        return FALSE;
    }
    // ��Ŀ������з����ڴ�ռ�
    dwSize = (DWORD)::_tcslen(wzDllFile) + 1;
    VirtualAddress = (TCHAR*)::VirtualAllocEx(hProcess, NULL, dwSize * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
    if (NULL == VirtualAddress)
    {
        printf("Virtual Process Memory Fail\r\n");
        CloseHandle(hProcess);
        return FALSE;
    }
    // ��Ŀ����̵��ڴ�ռ���д���������(ģ����)
    if (FALSE == ::WriteProcessMemory(hProcess, VirtualAddress, (LPVOID)wzDllFile, dwSize * sizeof(TCHAR), NULL))
    {
        printf("Write Data Fail\r\n");
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }
    // �� Kernel32.dll �л�ȡ LoadLibrary ������ַ
#ifdef _UNICODE
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else
    FuncAddress = (PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(_T("Kernel32")), "LoadLibraryA");
#endif

    if (NULL == FuncAddress)
    {
        printf("Get LoadLibrary Fail\r\n");
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return false;
    }

    // ����Զ���̵߳��� LoadLibrary
    hThread = ::CreateRemoteThread(hProcess, NULL, 0, FuncAddress, VirtualAddress, 0, NULL);
    if (NULL == hThread)
    {
        printf("CreateRemoteThread Fail\r\n");
        VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
        CloseHandle(hProcess);
        return FALSE;
    }

    // �ȴ�Զ���߳̽���
    WaitForSingleObject(hThread, INFINITE);
    // ����
    VirtualFreeEx(hProcess, VirtualAddress, dwSize, MEM_DECOMMIT);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}

WIN_VERSION  GetWindowsVersion()
{
    OSVERSIONINFOEX    OsVerInfoEx;
    OsVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO *)&OsVerInfoEx); // ע��ת������
    switch (OsVerInfoEx.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT:
        {
            if (OsVerInfoEx.dwMajorVersion <= 4 )
            {
                return WindowsNT;
            }
            if (OsVerInfoEx.dwMajorVersion == 5 && OsVerInfoEx.dwMinorVersion == 0)
            {
                return Windows2000;
            }

            if (OsVerInfoEx.dwMajorVersion == 5 && OsVerInfoEx.dwMinorVersion == 1)
            {
                return WindowsXP;
            }
            if (OsVerInfoEx.dwMajorVersion == 5 && OsVerInfoEx.dwMinorVersion == 2)
            {
                return Windows2003;
            }
            if (OsVerInfoEx.dwMajorVersion == 6 && OsVerInfoEx.dwMinorVersion == 0)
            {
                return WindowsVista;
            }

            if (OsVerInfoEx.dwMajorVersion == 6 && OsVerInfoEx.dwMinorVersion == 1)
            {
                return Windows7;
            }
            if (OsVerInfoEx.dwMajorVersion == 6 && OsVerInfoEx.dwMinorVersion == 2 )
            {
                return Windows8;
            }
            break;
        }

    default:
        {
            return WinUnknown;
        }
    }

}

#ifndef CXX_MONITOR_H
#	include "Monitor.h"
#include "HookKiFastCallEntry.h"
#endif



extern BOOLEAN         IsClear;//��Դ�Ƿ����� 

extern PKSERVICE_TABLE_DESCRIPTOR KeServiceDescriptorTable;
WCHAR FirstModulePath[260] = {0};
ULONG_PTR FirstModuleBase = 0;
ULONG_PTR FirstModuleSize = 0;
NTSTATUS
DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING pRegistryString)
{
	NTSTATUS		status = STATUS_SUCCESS;


	// Unload routine
	DriverObject->DriverUnload = DriverUnload;



	
	InitGlobalVariable();

	GetFirstModuleInformation(DriverObject);

	_KisInstallKiFastCallEntryHook();

	return STATUS_SUCCESS;
}


VOID GetFirstModuleInformation(PDRIVER_OBJECT DriverObject)
{
	  ULONG Base=0;//ģ�����ַ
    LDR_DATA_TABLE_ENTRY* SectionBase=NULL;
    LIST_ENTRY* Entry=NULL;
    LIST_ENTRY InLoadOrderLinks;
    Entry=((LIST_ENTRY*)DriverObject->DriverSection)->Flink;
    do
    {
        SectionBase=CONTAINING_RECORD(Entry,LDR_DATA_TABLE_ENTRY,InLoadOrderLinks);//�õ����Entry������Section�ĵ�ַ���˷���������֤����
        if (SectionBase->EntryPoint&&SectionBase->BaseDllName.Buffer&&SectionBase->FullDllName.Buffer&&
            SectionBase->LoadCount
            )
        {
			if(wcscmp(SectionBase->BaseDllName.Buffer,L"ntkrnlpa.exe")!=0)
			{
				wcsncpy(FirstModulePath,SectionBase->FullDllName.Buffer,SectionBase->FullDllName.Length);
				FirstModuleBase= SectionBase->DllBase;
				FirstModuleSize = SectionBase->SizeOfImage;
				DbgPrint("��һģ������:%wZ,��ַ:%x\r\n",&(SectionBase->FullDllName),SectionBase->DllBase);
				break;
			}
			DbgPrint("��һģ������:%wZ,��ַ:%x\n",&(SectionBase->FullDllName),SectionBase->DllBase);
			
  
        }
        Entry=Entry->Flink;
    }while(Entry!=((LIST_ENTRY*)DriverObject->DriverSection)->Flink);//ֱ����������
}




VOID
	DriverUnload(IN PDRIVER_OBJECT DriverObject)
{	

	return;
}

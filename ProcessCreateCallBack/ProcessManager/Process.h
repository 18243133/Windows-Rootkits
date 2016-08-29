#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "Monitor.h"
#include "EnumProcess.h"
// CProcess �Ի���

class CProcess : public CDialogEx
{
	DECLARE_DYNAMIC(CProcess)

public:
	CProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProcess();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROCESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CMonitor Monitor;
	CEnumProcess EnumProcess;
	CTabCtrl m_Tab_Process;
	afx_msg void OnTcnSelchangeTabProcess(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
};

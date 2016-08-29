// Process.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "Process.h"
#include "afxdialogex.h"


// CProcess �Ի���



IMPLEMENT_DYNAMIC(CProcess, CDialogEx)

CProcess::CProcess(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcess::IDD, pParent)
{

}

CProcess::~CProcess()
{
}

void CProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_Process, m_Tab_Process);
}


BEGIN_MESSAGE_MAP(CProcess, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_Process, &CProcess::OnTcnSelchangeTabProcess)
END_MESSAGE_MAP()


// CProcess ��Ϣ�������



void CProcess::OnTcnSelchangeTabProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ULONG        m_SelectTab = 0;
	m_SelectTab = m_Tab_Process.GetCurSel();
	CRect tabRect;   
	m_Tab_Process.GetClientRect(&tabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   
	// ����tabRect��ʹ�串�Ƿ�Χ�ʺϷ��ñ�ǩҳ   
	tabRect.left += 1;                  
	tabRect.right -= 1;   
	tabRect.top += 25;   
	tabRect.bottom -= 1;   
	switch(m_SelectTab)
	{
	case 0:
		{
			EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
			if(::MessageBox(NULL,L"ȷ���뿪�����",L"Noti",1)==MB_OK)
			{
				Monitor.m_bOk = FALSE;
			}
			break;
		}
	case 1:
		{
			EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
			Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
			break;
		}
	}
	*pResult = 0;
}


BOOL CProcess::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	int dpix;
	int dpiy;


	//��ʼ��Tab��
	m_Tab_Process.InsertItem(0, _T("������Ϣ"));           
	m_Tab_Process.InsertItem(1, _T("���̼��"));   



	//�������ԶԻ������
	EnumProcess.Create(IDD_DIALOG_ENUMPROCESS,GetDlgItem(IDC_TAB_Process));
	Monitor.Create(IDD_DIALOG_MONITOR,GetDlgItem(IDC_TAB_Process));

	CRect tabRect;
	GetWindowRect(&tabRect);
	CPaintDC dc(this);
	dpix = GetDeviceCaps(dc.m_hDC,LOGPIXELSX);
	dpiy = GetDeviceCaps(dc.m_hDC,LOGPIXELSY);
	tabRect.bottom += (LONG)(1+21*(dpiy/96.0));
	MoveWindow(&tabRect);
	m_Tab_Process.GetClientRect(&tabRect);    // ��ȡ��ǩ�ؼ��ͻ���Rect   

	// ����tabRect��ʹ�串�Ƿ�Χ�ʺϷ��ñ�ǩҳ   
	tabRect.left += 1;                  
	tabRect.right -= 1;   
	tabRect.top += 25;   
	tabRect.bottom -= 1;   

	// ���ݵ����õ�tabRect����m_jzmDlg�ӶԻ��򣬲�����Ϊ��ʾ   
	EnumProcess.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
	Monitor.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);



	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// ConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ACE400Statistics.h"
#include "ConfigDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog


CConfigDialog::CConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDialog)
	m_bPrtBasic = TRUE;
	m_bPrtLevel1 = FALSE;
	m_bPrtLevel2 = FALSE;
	m_bPrtLevel3 = FALSE;
	m_bPrtDevice = FALSE;
	m_bPrtDevD64 = FALSE;

	//g_wPrintState = 0x01;
	//}}AFX_DATA_INIT
}


void CConfigDialog::InitMember()
{
	/*
	m_bPrtBasic = TRUE;
	m_bPrtLevel1 = FALSE;
	m_bPrtLevel2 = FALSE;
	m_bPrtLevel3 = FALSE;
	m_bPrtDevice = FALSE;
	m_bPrtDevD64 = FALSE;

	g_wPrintState = 0x01;
	*/		
}

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDialog)
	DDX_Check(pDX, IDC_CHECK_PRT_BASIC, m_bPrtBasic);
	DDX_Check(pDX, IDC_CHECK_PRT_LEVEL1, m_bPrtLevel1);
	DDX_Check(pDX, IDC_CHECK_PRT_Level2, m_bPrtLevel2);
	DDX_Check(pDX, IDC_CHECK_PRT_LEVEL3, m_bPrtLevel3);
	DDX_Check(pDX, IDC_CHECK_PRT_DEVICE, m_bPrtDevice);
	DDX_Check(pDX, IDC_CHECK_PRT_DEV_D64, m_bPrtDevD64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDialog, CDialog)
	//{{AFX_MSG_MAP(CConfigDialog)
	ON_BN_CLICKED(IDC_CHECK_PRT_BASIC, OnCheckPrtBasic)
	ON_BN_CLICKED(IDC_CHECK_PRT_LEVEL1, OnCheckPrtLevel1)
	ON_BN_CLICKED(IDC_CHECK_PRT_Level2, OnCHECKPRTLevel2)
	ON_BN_CLICKED(IDC_CHECK_PRT_LEVEL3, OnCheckPrtLevel3)
	ON_BN_CLICKED(IDC_CHECK_PRT_DEVICE, OnCheckPrtDevice)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CHECK_PRT_DEV_D64, OnCheckPrtDevD64)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog message handlers


void CConfigDialog::InitView()
{
	OnCheckPrtBasic();
	OnCheckPrtLevel1();
	OnCHECKPRTLevel2();
	OnCheckPrtLevel3();
	OnCheckPrtDevice();
	OnCheckPrtDevD64();

	CString strTemp;
	strTemp.Format("%d", GetProcessWorkingSetSize());
	GetDlgItem(IDC_STATIC_USED_MEM)->SetWindowText(strTemp);
}



void CConfigDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE){
		InitMember();	
		InitView();		
	}

}




//--------------------------------------------------------
//	bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0
//	                  DEV   PRT3  PRT2  PRT1  BASIC
//--------------------------------------------------------

void CConfigDialog::OnCheckPrtBasic() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtBasic) 
		g_wPrintState |= (0x01 << PRT_BASIC);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_BASIC);	

	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

void CConfigDialog::OnCheckPrtLevel1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtLevel1) 
		g_wPrintState |= (0x01 << PRT_LEVEL1);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_LEVEL1);	
	
	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

void CConfigDialog::OnCHECKPRTLevel2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtLevel2) 
		g_wPrintState |= (0x01 << PRT_LEVEL2);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_LEVEL2);	
	
	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

void CConfigDialog::OnCheckPrtLevel3() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtLevel3) 
		g_wPrintState |= (0x01 << PRT_LEVEL3);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_LEVEL3);	
	
	
	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

// Contec  or  ICPDAS D96
void CConfigDialog::OnCheckPrtDevice() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtDevice) 
		g_wPrintState |= (0x01 << PRT_DEVICE);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_DEVICE);	
	
	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

// ICPDAS D64
void CConfigDialog::OnCheckPrtDevD64() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// bit data가 1인 경우에만 bit 위치를 On으로 설정
	if(m_bPrtDevD64) 
		g_wPrintState |= (0x01 << PRT_DEV_D64);

	// bit data가 1이 아닌 경우, bit 위치를 Off로 설정
	else
		g_wPrintState &= ~(0x01 << PRT_DEV_D64);	
	
	CString strTemp;
	strTemp.Format("0x%08x", g_wPrintState);
	GetDlgItem(IDC_STATIC_PRINT_STATE)->SetWindowText(strTemp);
	
}

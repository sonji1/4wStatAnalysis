// ACE400MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ACE400Statistics.h"
#include "ACE400MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACE400MainDlg dialog


CACE400MainDlg::CACE400MainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CACE400MainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CACE400MainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CACE400MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CACE400MainDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CACE400MainDlg, CDialog)
	//{{AFX_MSG_MAP(CACE400MainDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_STATISTICS, OnCheckStatistics)
	ON_BN_CLICKED(IDC_CHECK_PCHART,     OnCheckPchart)
	ON_BN_CLICKED(IDC_CHECK_FR_RANK,    OnCheckFrRank)
	ON_BN_CLICKED(IDC_CHECK_GAGE,       OnCheckGage)
	ON_BN_CLICKED(IDC_CHECK_CONFIG,     OnCheckConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACE400MainDlg message handlers

BOOL CACE400MainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	// TODO: Add extra initialization here
	m_StatisticsDlg.Create(IDD_STATISTICS_DIALOG, this);// make Modeless Dialog
	m_StatisticsDlg.ShowWindow(SW_SHOW);
	m_ConfigDlg.Create(IDD_CONFIG_DIALOG, this);		// make Modeless Dialog
	m_ConfigDlg.ShowWindow(SW_HIDE);
	m_PChartDlg.Create(IDD_PCHART_DIALOG, this);		// make Modeless Dialog
	m_PChartDlg.ShowWindow(SW_HIDE);
	m_FrRankDlg.Create(IDD_FR_RANK_DIALOG, this);		// make Modeless Dialog
	m_FrRankDlg.ShowWindow(SW_HIDE);
	if (m_GageDlg.Create(IDD_GAGE_DIALOG, this) == 0)			// make Modeless Dialog
		MyTrace(PRT_BASIC, "m_GageDlg Create Fail!\n" );
	m_GageDlg.ShowWindow(SW_HIDE);

	// m_StatisticsDlg에서  m_FrRankDlg와, m_PChartDlg로 메시지를 보낼때 사용할 hwnd 를 StatisticsDlg에 넘겨준다.
	m_StatisticsDlg.DlgHwndSetter(m_FrRankDlg.GetSafeHwnd(), m_PChartDlg.GetSafeHwnd());

	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(1);	// 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(0); 

#if 0
//#ifdef _DEBUG
	// 콘솔창 띄우기
	// 출력은 일반적인 콘솔 프로그램과 같이 cout이나 printf 등을 사용하면 된다.
    if( !AllocConsole() )
    {
        AfxMessageBox(_T("Failed to create the console!"), MB_ICONEXCLAMATION);
    }
#endif


	MyTrace(PRT_BASIC, "\"4W Statistics SW\" Started...\n" );

	CRect m_rectCurHist;
	this->GetWindowRect(m_rectCurHist);
	MyTrace(PRT_BASIC, "4W Statistics SW Main Dialog:   top=%d, bottom=%d, left=%d, right=%d\n\n", 
							m_rectCurHist.top, m_rectCurHist.bottom,
							m_rectCurHist.left, m_rectCurHist.right);

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CACE400MainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CACE400MainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CACE400MainDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



BOOL CACE400MainDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_StatisticsDlg.DestroyWindow();	// vector를 반환
	m_ConfigDlg.DestroyWindow();
	m_FrRankDlg.DestroyWindow();		// vector를 반환
	m_PChartDlg.DestroyWindow();
	m_GageDlg.DestroyWindow();

	MyTrace(PRT_BASIC, "\"4W Statistics SW\" Destroyed...\n\n\n\n\n" );

#if 0
//#ifdef _DEBUG
	// 콘솔창 해제
    if( !FreeConsole() )
    {
        AfxMessageBox(_T("Failed to free the console!"), MB_ICONEXCLAMATION);
    }
#endif

		
	return CDialog::DestroyWindow();
}

void CACE400MainDlg::OnCheckStatistics() 
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(1);	// 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(0); 

	m_StatisticsDlg.ShowWindow(SW_SHOW);
	m_ConfigDlg.ShowWindow(SW_HIDE);
	m_PChartDlg.ShowWindow(SW_HIDE);
	m_FrRankDlg.ShowWindow(SW_HIDE);
	m_GageDlg.ShowWindow(SW_HIDE);

	UpdateData(FALSE);
	
}


void CACE400MainDlg::OnCheckConfig() 
{
	// TODO: Add your control notification handler code here

	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(1);	// 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(0); 

	m_StatisticsDlg.ShowWindow(SW_HIDE);
	m_ConfigDlg.ShowWindow(SW_SHOW);
	m_PChartDlg.ShowWindow(SW_HIDE);
	m_FrRankDlg.ShowWindow(SW_HIDE);
	m_GageDlg.ShowWindow(SW_HIDE);
	UpdateData(FALSE);
	
}

void CACE400MainDlg::OnCheckPchart() 
{
	// TODO: Add your control notification handler code here

	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(1);	// 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(0); 

	m_StatisticsDlg.ShowWindow(SW_HIDE);
	m_ConfigDlg.ShowWindow(SW_HIDE);
	m_PChartDlg.ShowWindow(SW_SHOW);
	m_FrRankDlg.ShowWindow(SW_HIDE);
	m_GageDlg.ShowWindow(SW_HIDE);
	UpdateData(FALSE);
	
}

void CACE400MainDlg::OnCheckFrRank() 
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(0);	
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(1); // 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(0); 

	m_StatisticsDlg.ShowWindow(SW_HIDE);
	m_ConfigDlg.ShowWindow(SW_HIDE);
	m_PChartDlg.ShowWindow(SW_HIDE);
	m_FrRankDlg.ShowWindow(SW_SHOW);
	m_GageDlg.ShowWindow(SW_HIDE);
	UpdateData(FALSE);
	
}

void CACE400MainDlg::OnCheckGage() 
{
	// TODO: Add your control notification handler code here
	((CButton*)GetDlgItem(IDC_CHECK_STATISTICS))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_CONFIG))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK_PCHART))->SetCheck(0);	
	((CButton*)GetDlgItem(IDC_CHECK_FR_RANK))->SetCheck(0); // 해당 check 버튼을 눌린 상태로 유지
	((CButton*)GetDlgItem(IDC_CHECK_GAGE))->SetCheck(1); 

	m_StatisticsDlg.ShowWindow(SW_HIDE);
	m_ConfigDlg.ShowWindow(SW_HIDE);
	m_PChartDlg.ShowWindow(SW_HIDE);
	m_FrRankDlg.ShowWindow(SW_HIDE);
	m_GageDlg.ShowWindow(SW_SHOW);
	UpdateData(FALSE);

	
}

// 엔터키, ESC키 메시지를 후킹해서 엔터키나 ESC 키가 눌려도 다이얼로그가 종료되지 않도록 한다.
// 클래스 위자드에서 CACE400MainDlg의 'Messages -> PreTranslateMessage'를 선택해서 더블 클릭하여 추가.
BOOL CACE400MainDlg::PreTranslateMessage(MSG* pMsg) 
{

	// ESC 키나 리턴키 눌렸을 때 종료 방지
	if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
    }


	// TODO: Add your specialized code here and/or call the base class
	return CDialog::PreTranslateMessage(pMsg);
}

// FrRankDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ace400statistics.h"
#include "FrRankDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrRankDialog dialog


CFrRankDialog::CFrRankDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFrRankDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrRankDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFrRankDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrRankDialog)
	DDX_Control(pDX, IDC_COMBO_FR_DATE, m_comboFrDate);
	DDX_Control(pDX, IDC_COMBO_FR_LOT, 	m_comboFrLot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrRankDialog, CDialog)
	//{{AFX_MSG_MAP(CFrRankDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_FR_LOT, OnSelchangeComboFrLot)
	ON_CBN_SELCHANGE(IDC_COMBO_FR_DATE, OnSelchangeComboFrDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrRankDialog message handlers


BOOL CFrRankDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFrRankDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
	if(bShow == TRUE)
	{
		// 멤버 초기화
		if (InitMember() == FALSE)
			return;


		// View 초기화 
		if (InitView() == FALSE)
			return;	

		DisplayFrRank();

	}	
}


BOOL CFrRankDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	//deleteAllNetData();
	
	return CDialog::DestroyWindow();
}


BOOL CFrRankDialog::InitMember()
{
	m_nCombo_CurrLot = 0;		// 초기화 	
	m_nCombo_CurrDate = 0; 		// 초기화 


	UpdateData(FALSE);		// m_nNetCount, m_nOocNetCount edit box 화면 갱신을 위해 필요
	return TRUE;
}



BOOL CFrRankDialog::InitView()
{

	UpdateData(TRUE);

	//----------------------------
	// Lot,Date Combo Box 초기화
	
	// Statistics 창에서 초기화된 Lot data 기준으로 Lot combo를 채운다. 
	ComboLot_UpdateContents();

	m_comboFrDate.ResetContent();		// 현재 combo의 date data를 모두 지운다.
	ComboDate_UpdateContents(0);		// Lot 0 기준으로 초기화한다.



	//------------------------
	// Fault Grid  초기화 



	UpdateData(FALSE);
					

	return TRUE;
}


// g_sLotNetDate_Info를 활용하여 Statistics 창에서 초기화된 Lot data 기준으로 Lot combo를 채운다. 
void  CFrRankDialog::ComboLot_UpdateContents()
{
	// 현재 combo의 data를 모두 지운다.
	m_comboFrLot.ResetContent();	

	for (int lot = 0 ; lot < g_sLotNetDate_Info.nLotCnt; lot++)
		m_comboFrLot.InsertString(-1, g_sLotNetDate_Info.strLot[lot]);			
	m_nCombo_CurrLot = 0;		// 초기화 	
	m_comboFrLot.SetCurSel(0);

	UpdateData(FALSE);		// SetCurSel 결과를 UI에 반영.
}


// g_sLotNetDate_Info를 활용하여 Date combo를 채운다. 
void  CFrRankDialog::ComboDate_UpdateContents(int nLot)
{
	// Lot 번호가 바뀌었다면, 선택된 Lot에 속하는 date 들을 comboDate에 넣어준다.
	//if (m_nCombo_CurrLot != nLot)
	{
		// 현재 combo의 data를 모두 지운다.
		m_comboFrDate.ResetContent();	

		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboFrDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			
		m_nCombo_CurrDate = 0; 		// 초기화 
		m_comboFrDate.SetCurSel(0);

		UpdateData(FALSE);		// SetCurSel 결과를 UI에 반영.
	}
}

void CFrRankDialog::DisplayFrRank() 
{
	MyTrace(PRT_BASIC, "DisplayFrRank(): nLot=%d, nDate=%d\n", m_nCombo_CurrLot, m_nCombo_CurrDate);

	//CalcFrRank(m_nCombo_CurrLot, m_nCombo_CurrDate);
	//DisplayFrRankGrid(m_nCombo_CurrLot, m_nCombo_CurrDate);

}

void CFrRankDialog::OnSelchangeComboFrLot() 
{
	// TODO: Add your control notification handler code here
	
	CString strTemp;
	
	int 	comboPrevLot = m_nCombo_CurrLot;
	m_nCombo_CurrLot = m_comboFrLot.GetCurSel();		// 현재 combo에서 선택된 Lot# 

	// Range Check
	if ( m_nCombo_CurrLot < 0 || m_nCombo_CurrLot >= MAX_LOT ) 
	{

		strTemp.Format("OnSelchangeComboFrLot(): m_nCombo_CurrLot=%d, Range(0<= Lot <%d) Over\n", 
					m_nCombo_CurrLot, MAX_LOT );
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		m_nCombo_CurrLot = comboPrevLot;
		return;
	}
	
	
	// Statistics 창에서 초기화된 Date data 기준으로 Lot번호에 맞는 날짜 data를 채워준다.
	ComboDate_UpdateContents(m_nCombo_CurrLot);

	DisplayFrRank();	
	
}

void CFrRankDialog::OnSelchangeComboFrDate() 
{
	// TODO: Add your control notification handler code here
	
	CString strTemp;

	int 	comboPrevDate = m_nCombo_CurrDate;
	m_nCombo_CurrDate = m_comboFrDate.GetCurSel();		// 현재 combo에서 선택된 date# (0=ALL이므로 실제 date+1),  미선택이면 -1

	// Range Check
	if ( m_nCombo_CurrDate < 0 || m_nCombo_CurrDate >= (MAX_DATE + 1) )
	{

		strTemp.Format("OnSelchangeComboDate(): m_nCombo_CurrDate=%d, Range(0<= Date <%d) Over\n", 
					m_nCombo_CurrDate, (MAX_DATE + 1) );
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		m_nCombo_CurrDate = comboPrevDate;
		return;
	}
	
	DisplayFrRank();
}

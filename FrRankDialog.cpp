// FrRankDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ace400statistics.h"
#include "FrRankDialog.h"

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrRankDialog dialog


CFrRankDialog::CFrRankDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFrRankDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFrRankDialog)
	m_nNetCount = 0;
	m_nFaultNetCount = 0;
	m_bFaultListFaultOnly = TRUE;
	m_bFaultListFrRank = TRUE;
	//}}AFX_DATA_INIT
}


void CFrRankDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFrRankDialog)
	DDX_Control(pDX, 	IDC_COMBO_FR_LOT, 			m_comboFrLot);
	DDX_Control(pDX, 	IDC_COMBO_FR_DATE, 			m_comboFrDate);
	DDX_Text(pDX, 		IDC_EDIT_FR_NET_CNT, 		m_nNetCount);
	DDX_Text(pDX, 		IDC_EDIT_FR_FAULT_CNT2, 	m_nFaultNetCount);
	DDX_Control(pDX, 	IDC_GRID_FR_LIST, 			m_gridFault);
	DDX_Check(pDX, 		IDC_CHECK_FR_FAULT_ONLY, 	m_bFaultListFaultOnly);
	DDX_Check(pDX, 		IDC_CHECK_SORT_FAULT_RATE, 	m_bFaultListFrRank);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFrRankDialog, CDialog)
	//{{AFX_MSG_MAP(CFrRankDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_FR_LOT, OnSelchangeComboFrLot)
	ON_CBN_SELCHANGE(IDC_COMBO_FR_DATE, OnSelchangeComboFrDate)
	ON_BN_CLICKED(IDC_CHECK_SORT_FAULT_RATE, OnCheckSortFaultRate)
	ON_BN_CLICKED(IDC_CHECK_FR_FAULT_ONLY, OnCheckFrFaultOnly)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_FR_LIST_CSV, OnButtonSaveFrListCsv)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_FR_LIST_CSV, OnButtonViewFrListCsv)
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
	
	// vector를 초기화한다. 
	if (m_vsFrData.empty() == false)	
		m_vsFrData.clear();	
	
	return CDialog::DestroyWindow();
}


BOOL CFrRankDialog::InitMember()
{
	m_nCombo_CurrLot = 0;		// 초기화 	
	m_nCombo_CurrDate = 0; 		// 초기화 

	m_nNetCount = 0;
	m_nFaultNetCount = 0;

	m_bFaultListFaultOnly = TRUE;
	m_bFaultListFrRank = FALSE;

	//----------------------------
	// Grid 관련 멤버 변수 초기화
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_FR_GRID_COL;
	m_nRows = MAX_FR_GRID_ROW;
	m_bEditable = FALSE;

	//----------------------------
	// vector를 초기화한다. 
	if (m_vsFrData.empty() == false)	
		m_vsFrData.clear();	

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


	m_gridFault.SetEditable(m_bEditable);				// FALSE 설정
	//m_gridFault.SetListMode(m_bListMode);
	//m_gridFault.EnableDragAndDrop(TRUE);
	m_gridFault.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 back ground

	TRY {
		m_gridFault.SetRowCount(m_nRows);
		m_gridFault.SetColumnCount(m_nCols);
		m_gridFault.SetFixedRowCount(m_nFixRows);
		m_gridFault.SetFixedColumnCount(m_nFixCols);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH	


	// FAULT Grid Header 설정
	
	//            0     1        2      3       4                       5           6                      7            8       9       10       11        12
	char faultGridHeader[NUM_FR_GRID_COL][30] = 
				{"No", "Lot", "Date", "Net",  "Total\n(Tuple*Data)",  "NgCount", "Count\n(n:Total-NG)",  "Average",  "Sigma",  "Min",  "Max", "Fault", "FR\n(Fault/Count)"  };

	int faultGridColWidth[NUM_FR_GRID_COL] =    
				{ 40,    115,     70,    55,     80,                     70,         80,                   70,          70,      70,    70,     60,        90,       }; 
	int i;
	for (i=0; i < NUM_FR_GRID_COL; i++)
	{
		m_gridFault.SetColumnWidth(i, faultGridColWidth[i]);
		m_gridFault.SetItemText(0, i, faultGridHeader[i]);
	}
	m_gridFault.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조정

    
	ClearGrid_Fault();

	// scroll bar 위치를 초기화
	m_gridFault.SetScrollPos32(SB_VERT, 0);
	m_gridFault.SetScrollPos32(SB_HORZ, 0);



	UpdateData(FALSE);		// m_nNetCount, m_nOocNetCount edit box 화면 갱신을 위해 필요
					

	return TRUE;
}


void CFrRankDialog::ClearGrid_Fault()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridFault.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
		for (int col = 0; col < m_gridFault.GetColumnCount(); col++)
		{
			m_gridFault.SetItemText(row, col, "                 ");

			// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
			m_gridFault.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

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

void CFrRankDialog::DisplayFrRank() 
{
	MyTrace(PRT_BASIC, "DisplayFrRank(): nLot=%d, nDate=%d\n", m_nCombo_CurrLot, m_nCombo_CurrDate);

	CalcFrRank(m_nCombo_CurrLot, m_nCombo_CurrDate);	// DATE_ALL이 combo에 없으므로 comboDate를 그냥 nDate로 사용

	DisplayGridFault(m_nCombo_CurrLot, m_nCombo_CurrDate);
	//DisplayGridFault_Old(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
	UpdateData(FALSE);

}

void CFrRankDialog::CalcFrRank(int nLot, int nDate)
{

	MyTrace(PRT_BASIC, "CalcFrRank(): nLot=%d, nDate=%d\n", nLot, nDate);


	// 이전에 사용한 값이 남아 있을 수 있으므로 이전 vector를 초기화한다. 
	if (m_vsFrData.empty() == false)	
		m_vsFrData.clear();	


	m_nNetCount =  g_sLotNetDate_Info.naLotNetCnt[nLot];	// for 'Net Count' edit box
	m_nFaultNetCount = 0;									// for 'Fault Net count' edit box

	//------------------------------------------------
	// FR 계산하기  
	
	FaultRankData	frData;		// 한번 정의해서 계속 재사용.

	int net;
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;

		int nFault =  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate];
		if (nFault > 0) 
			m_nFaultNetCount++;		// Fault Net 갯수. for 'Fault Net Count' edit box
		

		// Calc Total, Count, Avg, Sigma, Min, Max
		int	   	nCount=0, nTotal=0;
		double 	dAvg=0, dSigma=0, dMax=0, dMin=0; 
		CalcSummary_AvgSigmaMinMax(nLot, net, nDate, 					// argument 
							nCount, nTotal, dAvg, dSigma, dMin, dMax);	// reference 

		int nNgCount   = g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate];

		frData.wNet    = net;
		frData.wCount  = nTotal - nNgCount;	
		frData.dAvg    = dAvg;
		frData.dSigma  = dSigma;
		frData.dMin    = dMin;
		frData.dMax    = dMax;
		frData.dFR     = nFault / (double)frData.wCount;

		m_vsFrData.push_back(frData);		// member 변수 vector에 추가
	}
}


struct FR_FR_COMP
{
	bool operator()(const FaultRankData& fr1, const FaultRankData& fr2)
	{
		return fr1.dFR > fr2.dFR;		// greater(내림차순) sort 
	}
};

struct FR_NET_COMP
{
	bool operator()(const FaultRankData& fr1, const FaultRankData& fr2)
	{
		return fr1.wNet < fr2.wNet;		// less (오름차순) sort
	}
};

void CFrRankDialog::DisplayGridFault(int nLot, int nDate)
{
	MyTrace(PRT_BASIC, _T("DisplayGridFault(): nLot=%d, nDate=%d\n"), nLot, nDate);


	// CheckBox On이면 FR(Fault Rate) 기준으로 vector를 내림차순(greater) Sort 
	if (m_bFaultListFrRank == TRUE)		
		sort( m_vsFrData.begin(), m_vsFrData.end(), FR_FR_COMP() );

	// CheckBox Off이면 net 기준으로 vector를 오름차순(less) Sort
	else
		sort( m_vsFrData.begin(), m_vsFrData.end(), FR_NET_COMP() );


	// 이전의 Grid data를 화면에서 지운다.
	ClearGrid_Fault();

	// scroll bar 위치를 초기화
	m_gridFault.SetScrollPos32(SB_VERT, 0);
	m_gridFault.SetScrollPos32(SB_HORZ, 0);


	int nRow = 0;
	
	// Data  출력 (배열 방식으로 vector 출력)
	for (int i =0; i < m_vsFrData.size(); i++)
	{

		int net = m_vsFrData[i].wNet;

		// FR Only Check가 On이면, FALUT 인 net만 출력하기 (Fault 아닌 net은 출력 안 함)
		if (m_bFaultListFaultOnly == TRUE	
				&& g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;

		nRow++; // row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력

		DisplayGrid_FaultTuple(nRow,										// Grid Row No
				nLot, nDate, net, 											// Lot, Date, Net
				(m_vsFrData[i].wCount + g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate]),
																			// Total (nCount + NG)
				g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate], 	// NG Count
				m_vsFrData[i].wCount, 										// n(Total-NG) Count
				m_vsFrData[i].dAvg, 										// Average 
				m_vsFrData[i].dSigma, 										// Sigma 
				m_vsFrData[i].dMin, 										// Min 
				m_vsFrData[i].dMax, 										// Max 
				g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate],	// Fault Count
				m_vsFrData[i].dFR);											// FR (Fault / Count)

	}

	UpdateData(FALSE);
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

}

#if 0
void CFrRankDialog::CalcFrRank_Old(int nLot, int nDate)
{

	MyTrace(PRT_BASIC, "CalcFrRank(): nLot=%d, nDate=%d\n", nLot, nDate);


	// 이전에 사용한 값이 남아 있을 수 있으므로 반드시 초기화하고 사용한다.
	::ZeroMemory(m_waCount, sizeof(m_waCount));	
	::ZeroMemory(m_daFR, sizeof(m_daFR));	
	


	m_nNetCount =  g_sLotNetDate_Info.naLotNetCnt[nLot];	// for 'Net Count' edit box
	m_nFaultNetCount = 0;									// for 'Fault Net count' edit box

	//------------------------------------------------
	// FR 계산하기  
	

	int net;
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;


		// FR(불량율): 측정 대상
		int tupleSize  = g_pvsaNetData[nLot][net][nDate]->size();
		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
		int nTotal  = tupleSize * sampleSize; 
		m_waCount[net]  = nTotal - g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate];	// Total - NG

		int nFault =  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate];
		m_daFR[net]  = nFault / (double)m_waCount[net];		

		if (nFault > 0)
			m_nFaultNetCount++;		// Fault Net 갯수. for 'Fault Net Count' edit box
	}
}
void CFrRankDialog::DisplayGridFault_Old(int nLot, int nDate)
{
	MyTrace(PRT_BASIC, _T("DisplayGridFault(): nLot=%d, nDate=%d\n"), nLot, nDate);

	ClearGrid_Fault();

	// scroll bar 위치를 초기화
	m_gridFault.SetScrollPos32(SB_VERT, 0);
	m_gridFault.SetScrollPos32(SB_HORZ, 0);


	int nRow = 0;
	// Data  출력
	for (int net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;


		// FALUT 인 net만 출력하기
		if (g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;


		nRow++; // row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력

		DisplayGrid_FaultTuple(nRow,										// Grid Row No
				nLot, nDate, net, 											// Lot, Date, Net
				(m_waCount[net] + g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate]),
																			// Total (nCount + NG)
				g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate], 	// NG Count
				m_waCount[net], 											// n(Total-NG) Count
				g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate],	// Fault Count
				m_daFR[net]);												// FR (Fault / Count)

	}

	UpdateData(FALSE);
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

}
#endif

void CFrRankDialog::DisplayGrid_FaultTuple(int nRow, int nLot, int nDate, int nNet, 
					int nTotal, int nNgCount, int nCount, 
					double dAvg, double dSigma, double dMin, double dMax,
					int nFault, double dFR )
{

	CString strTemp;

	//    0     1        2      3       4         5         6       7            8       9     10      11      12
	//	{"No", "Lot", "Date", "Net", "Total", "NgCount", "Count",  "Average", "Sigma", "Min", "Max", "Fault", "FR"  };
	

	// row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력
	strTemp.Format("%d", nRow);
	m_gridFault.SetItemText(nRow, FR_COL_NO, strTemp);					// col 0: No
	
	strTemp.Format("%s", g_sLotNetDate_Info.strLot[nLot]); 		
	m_gridFault.SetItemText(nRow, FR_COL_LOT, strTemp);					// col 1: Lot Str

	strTemp.Format("%s", g_sLotNetDate_Info.strLotDate[nLot][nDate]); 		
	m_gridFault.SetItemText(nRow, FR_COL_DATE, strTemp);				// col 2: Date Str

	strTemp.Format("%d", nNet);
	m_gridFault.SetItemText(nRow, FR_COL_NET, strTemp);					// col 3: Net No
	
	strTemp.Format("%d", nTotal);
	m_gridFault.SetItemText(nRow, FR_COL_TOTAL, strTemp);				// col 4: Total Count

	strTemp.Format("%d", nNgCount);
	m_gridFault.SetItemText(nRow, FR_COL_NGCOUNT, strTemp);				// col 5: NG Count

	strTemp.Format("%d", nCount);
	m_gridFault.SetItemText(nRow, FR_COL_COUNT, strTemp);				// col 6: n(Total-NG) Count


	strTemp.Format("%.2f", dAvg);
	m_gridFault.SetItemText(nRow, FR_COL_AVG, strTemp);					// col 7: Average

	strTemp.Format("%.2f", dSigma);
	m_gridFault.SetItemText(nRow, FR_COL_SIGMA, strTemp);				// col 8: Sigma

	strTemp.Format("%.2f", dMin);
	m_gridFault.SetItemText(nRow, FR_COL_MIN, strTemp);					// col 9: Min

	strTemp.Format("%.2f", dMax);
	m_gridFault.SetItemText(nRow, FR_COL_MAX, strTemp);					// col 10: Max

	strTemp.Format("%d", nFault);
	m_gridFault.SetItemText(nRow, FR_COL_FAULT, strTemp);				// col 11: Fault Count
	
	strTemp.Format("%.4f", dFR);
	m_gridFault.SetItemText(nRow, FR_COL_FR, strTemp);					// col 12: FR


	if (nFault > 0)
	{
    	m_gridFault.SetItemBkColour(nRow, FR_COL_FAULT, RGB(0xff, 0x63, 0x47));	// tomato : 진한 주황
    	m_gridFault.SetItemBkColour(nRow, FR_COL_FR, RGB(0xff, 0x63, 0x47));	// tomato : 진한 주황
	}



}


void CFrRankDialog::OnCheckSortFaultRate() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	MyTrace(PRT_BASIC, "m_bFaultListFrRank Value Changed to %d\n", m_bFaultListFrRank);


	// 바뀐모드로 Fault Grid를 다시 그린다.
	DisplayGridFault(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
	UpdateData(FALSE);
}

void CFrRankDialog::OnCheckFrFaultOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
 	MyTrace(PRT_BASIC, "m_bFaultListFaultOnly Value Changed to %d\n", m_bFaultListFaultOnly);

 	// 바뀐모드로 Fault Grid를 다시 그린다.
	DisplayGridFault(m_nCombo_CurrLot, m_nCombo_CurrDate);
 	UpdateData(FALSE);
	
	
}

void CFrRankDialog::OnButtonSaveFrListCsv() 
{
	// TODO: Add your control notification handler code here
	
	if (m_vsFrData.empty())	
		return;
	
	int nLot = m_nCombo_CurrLot;
	int nDate = m_nCombo_CurrDate;
	MyTrace(PRT_BASIC, "SaveFrListCsv(): nLot=%d, nDate=%d\n", nLot, nDate);
	


	// CSV 파일 출력.
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;

	::ZeroMemory(&fName, sizeof(fName));
	strTemp.Format(".\\Data\\%s_%s_FaultRankOut.csv", 
			g_sLotNetDate_Info.strLot[nLot], g_sLotNetDate_Info.strLotDate[nLot][nDate]);
	strcat(fName, strTemp);

	fp=fopen(fName,"wt");
	if(fp == NULL)
	{ 	
		strTemp.Format("fName=%s, err=%s", fName, strerror( errno ));
		ERR.Set(FLAG_FILE_CANNOT_OPEN, strTemp); 
		return; 
	}


	//----------------------------
	// Fault Rank CSV 출력
	
	
	// 헤더 출력
	fprintf(fp, "No, Lot=%d, Date=%d, Net, Total, NgCount, Count, Average, Sigma, Min, Max, Fault, FR\n", 
				nLot, nDate);

	int nRow = 0;

	// Data 출력
	for (int i =0; i < m_vsFrData.size(); i++)
	{

		int net = m_vsFrData[i].wNet;

		// FR Only Check가 On이면, FALUT 인 net만 출력하기 (Fault 아닌 net은 출력 안 함)
		if (m_bFaultListFaultOnly == TRUE	
				&& g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;

		nRow++;

		fprintf(fp, "%d, %s, %s, %d, %d, %d, %d, %.2f, %.2f, %.2f, %.2f, %d, %.4f\n", 
				nRow,														// 0: No
				g_sLotNetDate_Info.strLot[nLot], 							// 1: Lot Str
				g_sLotNetDate_Info.strLotDate[nLot][nDate], 				// 2: Date Str
				net, 														// 3: Net No
				(m_vsFrData[i].wCount + g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate]), 	
																			// 4: Total Count
				g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate], 	// 5: NG Count
				m_vsFrData[i].wCount, 										// 6: (Total-NG) Count
				m_vsFrData[i].dAvg, 										// 7: Average 
				m_vsFrData[i].dSigma, 										// 8: Sigma 
				m_vsFrData[i].dMin, 										// 9: Min 
				m_vsFrData[i].dMax, 										// 10:Max 


				g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate], // 11: Fault Count
				m_vsFrData[i].dFR ); 										// 12: FR(Fault Rate)
	}

	fclose(fp);

	strTemp.Format("\'Save to CSV\' completed. \n(%s)", fName);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
}

void CFrRankDialog::OnButtonViewFrListCsv() 
{
	// TODO: Add your control notification handler code here
	

	CString strTemp;
	
	strTemp.Format(".\\Data\\%s_%s_FaultRankOut.csv", 
			g_sLotNetDate_Info.strLot[m_nCombo_CurrLot], 
			g_sLotNetDate_Info.strLotDate[m_nCombo_CurrLot][m_nCombo_CurrDate]);

	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

}

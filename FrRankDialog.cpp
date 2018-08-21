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
	m_nNetCount = 0;
	m_nFaultNetCount = 0;
	m_bFaultListFaultOnly = TRUE;
	m_bFaultListFrRank = FALSE;
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
	ON_BN_CLICKED(IDC_CHECK_FR_FAULT_ONLY, OnCheckFrFaultOnly)
	ON_BN_CLICKED(IDC_CHECK_SORT_FAULT_RATE, OnCheckSortFaultRate)
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
	
	//            0     1        2      3       4                       5           6                      7            8            
	char faultGridHeader[NUM_FR_GRID_COL][30] = 
				{"No", "Lot", "Date", "Net",  "Total\n(Tuple*Data)",  "NgCount", "Count\n(n:Total-NG)",  "Fault", "FR\n(Fault/Count)"  };

	int faultGridColWidth[NUM_FR_GRID_COL] =    
				{ 40,    80,     70,    50,     80,                     70,         80,                   70,           100,       }; 
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
	
	UpdateData(FALSE);

}

void CFrRankDialog::CalcFrRank(int nLot, int nDate)
{

	MyTrace(PRT_BASIC, "CalcFrRank(): nLot=%d, nDate=%d\n", nLot, nDate);


	// 이전에 사용한 값이 남아 있을 수 있으므로 반드시 초기화하고 사용한다.
	::ZeroMemory(m_waCount, sizeof(m_waCount));	
	::ZeroMemory(m_daFR, sizeof(m_daFR));	
	


	m_nNetCount =  g_sLotNetDate_Info.naLotNetCnt[nLot];	// for 'Net Count' edit box
	m_nFaultNetCount = 0;									// for 'Fault Net count' edit box

	//------------------------------------------------
	// FR 계산하기  
	
	faultRankData	frData;		// 한번 정의해서 계속 재사용.

	int net;
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;

		//frData.wNet = net;

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

void CFrRankDialog::DisplayGridFault(int nLot, int nDate)
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
		if (m_bFaultListFaultOnly == TRUE 
				&&  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;


		nRow++; // row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력

		DisplayGrid_FaultTuple(nRow,											// Grid Row No
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

void CFrRankDialog::DisplayGrid_FaultTuple(int nRow, int nLot, int nDate, int nNet, 
					int nTotal, int nNgCount, int nCount, int nFault, double dYR )
{

	CString strTemp;

	//    0     1        2      3       4         5         6       7        8      
	//	{"No", "Lot", "Date", "Net", "Total", "NgCount", "Count",  "Fault", "FR"  };
	

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

	strTemp.Format("%d", nFault);
	m_gridFault.SetItemText(nRow, FR_COL_FAULT, strTemp);				// col 7: Fault Count
	
	strTemp.Format("%.4f", dYR);
	m_gridFault.SetItemText(nRow, FR_COL_FR, strTemp);					// col 8: YR


	if (nFault > 0)
	{
    	m_gridFault.SetItemBkColour(nRow, FR_COL_FAULT, RGB(0xff, 0x63, 0x47));	// tomato : 진한 주황
    	m_gridFault.SetItemBkColour(nRow, FR_COL_FR, RGB(0xff, 0x63, 0x47));	// tomato : 진한 주황
	}



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

void CFrRankDialog::OnCheckSortFaultRate() 
{
	// TODO: Add your control notification handler code here
	MyTrace(PRT_BASIC, "m_bFaultListFrRank Value Changed to %d\n", m_bFaultListFrRank);


	// 바뀐모드로 Fault Grid를 다시 그린다.
	DisplayGridFault(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
	UpdateData(FALSE);
}

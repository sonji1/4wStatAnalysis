// PChartDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ace400statistics.h"
#include "PChartDialog.h"

#include "math.h"
#include "chartdir.h" 			// p-chart를 그리기 위해 추가
#include <vector>
#include <sstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CPChartDialog dialog


CPChartDialog::CPChartDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPChartDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPChartDialog)
	m_nNetCount = 0;
	m_nOocNetCount = 0;
	m_nFaultNetCount = 0;

	m_bOocListOocOnly = FALSE;		// for check box
	m_bOocListFaultOnly = TRUE;	// for check box
	//}}AFX_DATA_INIT
}


void CPChartDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPChartDialog)
	DDX_Control(pDX, IDC_COMBO_LOT, 		m_comboLot);
	DDX_Control(pDX, IDC_COMBO_DATE2, 		m_comboDate);
	DDX_Control(pDX, IDC_CHART, 			m_ChartViewer);
	DDX_Control(pDX, IDC_GRID_PCHART_OOC, 	m_gridOOC);
	DDX_Text(pDX, IDC_EDIT_PCAHRT_NET_CNT, 	m_nNetCount);

	DDX_Check(pDX, IDC_CHECK_OOCNET_ONLY, 	m_bOocListOocOnly);
	DDX_Text(pDX, IDC_EDIT_PCAHRT_OOC_CNT, 	m_nOocNetCount);
	DDX_Check(pDX, IDC_CHECK_FAULT_ONLY, m_bOocListFaultOnly);
	DDX_Text(pDX, IDC_EDIT_PCAHRT_FAULT_CNT, m_nFaultNetCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPChartDialog, CDialog)
	//{{AFX_MSG_MAP(CPChartDialog)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(UWM_LOAD_LOG4W_DATA, 				OnStatLoad4wData)		// 사용자 지정 메시지

	ON_CBN_SELCHANGE(IDC_COMBO_LOT, 				OnSelchangeComboLot)
	ON_CBN_SELCHANGE(IDC_COMBO_DATE2, 				OnSelchangeComboDate)
	ON_BN_CLICKED(IDC_BUTTON_DIS_PCHART, 			OnButtonDisPchart)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PCHART, 			OnButtonSavePchart)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_PCHART_CSV, 		OnButtonViewPchartCsv)
	ON_BN_CLICKED(IDC_CHECK_OOCNET_ONLY, 			OnCheckOocnetOnly)
	ON_BN_CLICKED(IDC_CHECK_FAULT_ONLY, 			OnCheckFaultOnly)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_PCHART_OOC, 	OnGridSelChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPChartDialog message handlers





BOOL CPChartDialog::InitMember()
{
	m_nCombo_CurrLot = 0;		// 초기화 	
	m_nCombo_CurrDate = 0; 		// 초기화 

	::ZeroMemory(m_waNormal, sizeof(m_waNormal));	
	::ZeroMemory(m_waCount, sizeof(m_waCount));	
	::ZeroMemory(m_daLCL, sizeof(m_daLCL));	
	::ZeroMemory(m_daYR, sizeof(m_daYR));	
	::ZeroMemory(m_baOOC, sizeof(m_baOOC));	
	m_dMaxYR = 0.0;
	m_dMinYR = 0.0;

	m_nNetCount = 0;
	m_nOocNetCount = 0;
	m_nFaultNetCount = 0;

	m_bOocListOocOnly = FALSE;
	m_bOocListFaultOnly = TRUE;

	m_nNetSum_Normal = 0; 
	m_nNetSum_Count = 0; 
	m_dCenter = 0.0;

	//----------------------------
	// Grid 관련 멤버 변수 초기화
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_OOC_GRID_COL;
	//m_nRows = MAX_OOC_GRID_ROW;
	m_nRows = 201;			// default . DisplayGrid 시에 다시 맞춘다.
	m_bEditable = FALSE;

	UpdateData(FALSE);		// m_nNetCount, m_nOocNetCount edit box 화면 갱신을 위해 필요
	return TRUE;
}


BOOL CPChartDialog::InitView()
{

	UpdateData(TRUE);


	//----------------------------
	// Lot Combo Box 초기화
	
	// Statistics 창에서 초기화된 Lot data 기준으로 Lot combo를 채운다. 
	ComboLot_UpdateContents();

	m_comboDate.ResetContent();				// 현재 combo의 date data를 모두 지운다.

#if 0 	// 2018.07.18: Date ALL은 P Chart에서는 사용하지 않기로.
	m_comboDate.InsertString(-1, "ALL");	// -1: add string("ALL") to end of the list
	m_comboDate.SetCurSel(0); 				// combo idx = 0: ALL
#else
	ComboDate_UpdateContents(0);	// Lot 0 기준으로 초기화한다.
	//m_comboDate.SetCurSel(0); 		// 초기화  => 선택을 해야 출력이 되므로 초기화를 삭제

#endif

	//------------------------
	// Chart 초기화 
	
    m_ChartViewer.setChart(0); // 이전에 그려졌던 그래프가 있다면 초기화한다.


	//------------------------
	// OOC Grid  초기화 


	m_gridOOC.SetEditable(m_bEditable);				// FALSE 설정
	//m_gridOOC.SetListMode(m_bListMode);
	//m_gridOOC.EnableDragAndDrop(TRUE);
	m_gridOOC.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 back ground

	TRY {
		m_gridOOC.SetRowCount(m_nRows);
		m_gridOOC.SetColumnCount(m_nCols);
		m_gridOOC.SetFixedRowCount(m_nFixRows);
		m_gridOOC.SetFixedColumnCount(m_nFixCols);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH	

	// OOC Grid Header 설정
	
	//            0     1        2      3        4                    5           6                   7             8                    9                   10        11      12
	char oocHeader[NUM_OOC_GRID_COL][30] = 
				{"No", "Lot", "Date", "Net", "Total\n(Tuple*Data)", "NgCount", "Count\n(Total-NG)", "Fault", "Normal\n(Count-Fault)", "YR\n(Normal/Count)", "Center", "LCL",  "OOC Alarm\n(YR<LCL)" };


	int oocColWidth[NUM_OOC_GRID_COL] =    
				{ 40,    85,     70,    50,     80,                   70,         80,                45,            80,                 100,                 70,       70,     80 };

	int i;
	for (i=0; i < NUM_OOC_GRID_COL; i++)
	{
		m_gridOOC.SetColumnWidth(i, oocColWidth[i]);
		m_gridOOC.SetItemText(0, i, oocHeader[i]);
	}
	m_gridOOC.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조정

    
	ClearGrid_OOC();

	// scroll bar 위치를 초기화
	m_gridOOC.SetScrollPos32(SB_VERT, 0);
	m_gridOOC.SetScrollPos32(SB_HORZ, 0);

	UpdateData(FALSE);
					

	return TRUE;
}


BOOL CPChartDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//
	// 멤버 초기화
	if (InitMember() == FALSE)
		return FALSE;


	// View 초기화 
	if (InitView() == FALSE)
		return FALSE;	

	DisplayPchart_ALL();

	MyTrace(PRT_BASIC, "\"YR PChart Dialog\" Started...\n" );

	CRect m_rectCurHist;
	this->GetWindowRect(m_rectCurHist);
	MyTrace(PRT_BASIC, "YR PChart Dialog:   top=%d, bottom=%d, left=%d, right=%d\n\n", 
							m_rectCurHist.top, m_rectCurHist.bottom,
							m_rectCurHist.left, m_rectCurHist.right);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


// 창이 새로 보일때 때마다 초기화해야 하는 멤버들은 다음의 
// OnShowWindow()의 TODO 밑에서 초기화한다. 
void CPChartDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE)
	{
/*
		// 멤버 초기화
		if (InitMember() == FALSE)
			return;


		// View 초기화 
		if (InitView() == FALSE)
			return;	

		DisplayPchart_ALL();
*/
	}	
	
}


// Stat 창에서  Load를 새로 했을 때 (Load 완료 메시지 수신시) 에만 InitMember(), InitView()를 수행한다. 
// Load를 새로 하지 않았을 때에는 P-chart 같은 다른 창을 갔다 와도 이전에 선택된 Lot, Date가 유지 되도록 
// OnShowWindow()가 아니라 여기서 InitMember(), InitView() 를 수행한다.
LRESULT CPChartDialog::OnStatLoad4wData(WPARAM wParam, LPARAM lParam)
{

/*	// message recv test print
	CString strTemp;
	strTemp.Format("PChartDlg:: OnStatLoad4wData(): Recv \"UWM_LOAD_LOG4W_DATA\".\n");
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
*/
	
	// 멤버 초기화
	if (InitMember() == FALSE)
		return FALSE;


	// View 초기화 
	if (InitView() == FALSE)
		return FALSE;	

	DisplayPchart_ALL();

	return TRUE;
}

// g_sLotNetDate_Info를 활용하여 Statistics 창에서 초기화된 Lot data 기준으로 Lot combo를 채운다. 
void  CPChartDialog::ComboLot_UpdateContents()
{
	// 현재 combo의 data를 모두 지운다.
	m_comboLot.ResetContent();	

	for (int lot = 0 ; lot < g_sLotNetDate_Info.nLotCnt; lot++)
		m_comboLot.InsertString(-1, g_sLotNetDate_Info.strLot[lot]);			
	m_nCombo_CurrLot = 0;		// 초기화 	
	m_comboLot.SetCurSel(0);

	UpdateData(FALSE);		// SetCurSel 결과를 UI에 반영.
}


// g_sLotNetDate_Info를 활용하여 Date combo를 채운다. 
void  CPChartDialog::ComboDate_UpdateContents(int nLot)
{
	// Lot 번호가 바뀌었다면, 선택된 Lot에 속하는 date 들을 comboDate에 넣어준다.
	//if (m_nCombo_CurrLot != nLot)
	{
		// 현재 combo의 data를 모두 지운다.
		m_comboDate.ResetContent();	
		
#if 0 	// 2018.07.18: Date ALL은 P Chart에서는 사용하지 않기로.

		// com idx=0 추가
		m_comboDate.InsertString(-1, "ALL");		// combo idx = 0: ALL

		// date_0이 두번째 (idx=1) 로  combo에 추가된다.  combo idx는 'date+1'로 다룰 것.
		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			

		// net이나 combo 클릭시에 바로 display를 위해 date 를 default로 "ALL"을 설정해 준다.
		m_nCombo_CurrDate = 0; 	// All     0=All,  1~7 : 실제 date#+1
		m_comboDate.SetCurSel(0);
#else

		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			
		m_nCombo_CurrDate = 0; 		// 초기화 
		m_comboDate.SetCurSel(0);

#endif		

		UpdateData(FALSE);		// SetCurSel 결과를 UI에 반영.
	}
}



void CPChartDialog::OnSelchangeComboLot() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	
	int 	comboPrevLot = m_nCombo_CurrLot;
	m_nCombo_CurrLot = m_comboLot.GetCurSel();		// 현재 combo에서 선택된 Lot# 

	// Range Check
	if (m_nCombo_CurrLot < 0 || m_nCombo_CurrLot >= MAX_LOT) 
	{

		strTemp.Format("OnSelchangeComboLot(): m_nCombo_CurrLot=%d, Range(0<= Lot <%d) Over\n", 
					m_nCombo_CurrLot, MAX_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		m_nCombo_CurrLot = comboPrevLot;
		return;
	}
	
	
	// Statistics 창에서 초기화된 Date data 기준으로 Lot번호에 맞는 날짜 data를 채워준다.
	ComboDate_UpdateContents(m_nCombo_CurrLot);

	DisplayPchart_ALL();
}

void CPChartDialog::OnSelchangeComboDate() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;

	int 	comboPrevDate = m_nCombo_CurrDate;
	m_nCombo_CurrDate = m_comboDate.GetCurSel();		// 현재 combo에서 선택된 date# (0=ALL이므로 실제 date+1),  미선택이면 -1

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
	
	DisplayPchart_ALL();
}


void CPChartDialog::OnButtonDisPchart() 
{
	// TODO: Add your control notification handler code here
	DisplayPchart_ALL();
}	
	

void CPChartDialog::DisplayPchart_ALL() 
{
	// 선택된 Lot, Date 기준으로 P-Chart와 관련 data를 출력한다.
	CalcPChart(m_nCombo_CurrLot, m_nCombo_CurrDate);		// DATE_ALL이 combo에 없으므로 comboDate를 그냥 nDate로 사용
	DisplayPChart(m_nCombo_CurrLot, m_nCombo_CurrDate);		// trackLine은 빼고 차트를 그린다.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}


// Normal, Count, YR, Center, LCL 계산하기  
void CPChartDialog::CalcPChart(int nLot, int nDate)
{

	MyTrace(PRT_BASIC, "CalcPChart(): nLot=%d, nDate=%d\n", nLot, nDate);


										 	
	// 이전에 사용한 값이 남아 있을 수 있으므로 반드시 초기화하고 사용한다.
	::ZeroMemory(m_waNormal, sizeof(m_waNormal));	
	::ZeroMemory(m_waCount, sizeof(m_waCount));	
	::ZeroMemory(m_daLCL, sizeof(m_daLCL));	
	::ZeroMemory(m_daYR, sizeof(m_daYR));	
	::ZeroMemory(m_baOOC, sizeof(m_baOOC));	
	m_dMaxYR = 0;
	m_dMinYR = 0;
	m_nNetSum_Normal = 0; 
	m_nNetSum_Count = 0; 
	m_dCenter = 0.0;

	m_nNetCount =  g_sLotNetDate_Info.naLotNetCnt[nLot];	// for 'Net Count' edit box
	m_nOocNetCount = 0;										// for 'Ooc Net Count' edit box
	m_nFaultNetCount = 0;									// for 'Fault Net count' edit box


	//----------------------------------------------------
	// Net 별 p chart data 계산  (문제있는 Net을 찾기)
	int net;
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;


		int tupleSize  = g_pvsaNetData[nLot][net][nDate]->size();
		int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
		int nTotal  = tupleSize * stepSize; 
		int nFault  = g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate];
		m_waCount[net]  = nTotal - g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate];	// Total - NG
		m_waNormal[net] = m_waCount[net] - nFault; 		// Count - Fault

		m_nNetSum_Normal += m_waNormal[net];
		m_nNetSum_Count += m_waCount[net];

		// YR(수율): 그래프의 측정 대상
		if (nFault == 0 && m_waCount[net] == 0) // Fault=0 이면, FR이 0이므로, YR은 1이 되는게 논리상 맞다.
			m_daYR[net] = 1.0;		

		if (m_waCount[net] > 0)							// check devide by zero
			m_daYR[net]  = m_waNormal[net] / (double)m_waCount[net];		


		// m_dMaxYR, m_dMinYR 계산
		if (net == 1)		// net은 1부터 시작.
		{
			m_dMaxYR = m_daYR[net];
			m_dMinYR = m_daYR[net];
		}
		else 
		{
			if (m_dMaxYR <  m_daYR[net])	// 최대값보다 현재값이 크면 최대값 변경
				m_dMaxYR =  m_daYR[net];

			if (m_dMinYR > m_daYR[net])		// 최소값보다 현재값이 작으면 최소값 변경
				m_dMinYR =  m_daYR[net];
		}

	}

	// Center 계산: 그래프 중심축
	if (m_nNetSum_Count > 0)		// check devide by zero
		m_dCenter =  m_nNetSum_Normal / (double)m_nNetSum_Count;

	// LCL, OOC 계산 
	//for (net =0; net < MAX_NET_PER_LOT; net++)
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{

		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;


		// LCL: OOC(Out Of Control) 기준
		//      Center - 3 * SQRT(Center* (1-Center) / Count)
		if ( m_waCount[net] > 0 )									// check devide by zero
			m_daLCL[net] = m_dCenter - (3 * sqrt((m_dCenter * (1 - m_dCenter)) / m_waCount[net])); 	

		// OOC 판단 
		//if (m_daYR[net] <= m_daLCL[net])	 //2018.07.31  Fault가 전혀 없는 경우, 모든 data에 대해
		//										    YR, Center, LCL이 모두 1임.  이 때  OOC가 True이면 안되므로
		//										    OOC기준에서 YR이 LCL과 같은 경우는 빼도록 한다.
		if (m_daYR[net] < m_daLCL[net])	
		{
			m_baOOC[net] = TRUE;	// OOC 여부
			m_nOocNetCount++;		// OOC Net 갯수.  for 'OOC Net Count' edit box
		}

		if (g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] > 0)
			m_nFaultNetCount++;		// Fault Net 갯수. for 'Fault Net Count' edit box
	}

}
	

void CPChartDialog::DisplayPChart(int nLot, int nDate, int nTrackNet /*= -1*/)
{
	
	MyTrace(PRT_BASIC, _T("DisplayPChart(): nLot=%d, nDate=%d, nTrackNet=%d\n"), nLot, nDate, nTrackNet);
	



/*
	// Simple Line Chart
    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(600, 350);		// 바탕 비트맵의 크기 지정

    // Set the plotarea at (80, 80) and of size 200 x 200 pixels
    c->setPlotArea(80, 68, 		// X, Y point 위치  : 시작 point  
    				420, 200);	// X width, Y hight : 실제 그래프의 크기 지정

    // Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9pt Arial Bold
    // font. Set the background and border color to Transparent.
    c->addLegend(50, 20,		// x, y point 
    			false, 			// vertical(true),  
    			"arialbd.ttf", 	// font*
    			9				// font size
    			)->setBackground(Chart::Transparent);	//  

    // Add a title to the y axis
    TextBox *textbox = c->yAxis()->setTitle("proportion");
    // Set the y axis title upright (font angle = 0)
    textbox->setFontAngle(0);
    // Put the y axis title on top of the axis
    textbox->setAlignment(Chart::TopLeft2);


    // Output the chart
    m_ChartViewer.setChart(c);
	
    // Include tool tip for the chart
    m_ChartViewer.setImageMap(
    	c->getHTMLImageMap("", "", "title='{xLabel}: US${value}K'"));

	// In this Fample project, we do not need to chart object any more, so we 
	// delete it now.
    delete c;
*/
	
	//------------------------------------------------
	// YR, Center, LCL 로 그래프 그리기 
	//
	// Draw Chart and set to CChartViewer
	

    // The data for the bar chart
    //double proportion[] = {0.08, 0.02, 0.06, 0.02, 0.04, 0.12, 0.06, 0.04, 0.06, 0, 0.25, 0.16666667, 0.14893617, 0.127659574, 0.130434783 };
    //double center[] = { 0.087, 0.087, 0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087, 0.087 };
    //double UCL[] = { 0.207, 0.207, 0.207, 0.207,0.207, 0.207,0.207, 0.207,0.207, 0.207,0.209, 0.209,0.210, 0.210, 0.210 };
	// The labels for the bar chart
    //const char *labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

   	double center[MAX_NET_PER_LOT];		// center값이 array가 아니므로 출력을 위한 array를 만든다.
    double dataX0[MAX_NET_PER_LOT];		// X 축 label로 사용할 array 
	for (int net =1; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		center[net] = m_dCenter;
		dataX0[net] = net;
	}


	// 차트 생성 ------
    // Create an XYChart object of size 950 x 300 pixels, with a light blue (EEEEFF) background,
    // black border, 1 pxiel 3D border effect and rounded corners
    XYChart *c = new XYChart(850, 270, 0xeeeeff, 0x000000, 1);
    //c->setRoundedFrame();

    // Set the plotarea at (55:x, 58:y) and of size 870 x 195 pixels, with white background. Turn on
    // both horizontal and vertical grid lines with light grey color (0xcccccc)
    c->setPlotArea(55, 58, 770, 175, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);


	// 범례형식 지정  ------- 
    // Add a legend box at (250:x, 30:y) (top of the chart) with horizontal layout. Use 9pt Arial Bold
    // font. Set the background and border color to Transparent.
    c->addLegend(250, 30, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);



	// X축, Y 축 타이틀 지정 ----------------
	
    // Add a title box to the chart using 15pt Times Bold Italic font, on a light blue (CCCCFF)
    // background with glass effect. white (0xffffff) on a dark red (0x800000) background, with a 1
    // pixel 3D border.
	c->addTitle("Net YR p-Chart", "timesbi.ttf", 15)->setBackground(0xccccff,
      0x000000, Chart::glassEffect());

    // data가 실제 있는 범위만 출력되도록 Scale 조정하기. 
    //   net[0]에 0값이 있는 의미없는 data가 있었음. net[0]을 제외하고 출력하자 아래 코드 불필요.
    //c->yAxis()->setAutoScale(0.1, 0.1);			
    //c->yAxis()->setLinearScale(m_dMinYR, m_dMaxYR);
    //c->yAxis()->setMargin(10, 10);
    
    // Add a title to the y axis
/* 
    TextBox *textbox_Y = c->yAxis()->setTitle("YR");
    textbox_Y->setFontAngle(0);					// Set the y axis title upright (font angle = 0)
    textbox_Y->setAlignment(Chart::TopLeft2);	// Put the y axis title on top of the axis
*/

    // Add a title to the x axis
    TextBox *textbox_X = c->xAxis()->setTitle("Net");
    textbox_X->setAlignment(Chart::BottomRight);

	// layer.setXData()를 사용하고 아래코드는 코멘트처리
    // Set the labels on the x axis.
    //c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));
    //c->xAxis()->setLabels(StringArray((const char * const *)&labels[1], g_sLotNetDate_Info.naLotNetCnt[nLot])); 
  	//c->xAxis()->setLabelStep(g_sLotNetDate_Info.naLotNetCnt[nLot] / 10);


	// XY LineChart 생성 & 옵션 설정 ----------------------


    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);


    // Add the three data sets to the line layer. 
    layer->addDataSet( DoubleArray(&m_daYR[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 	// Net 1번부터 처리, Net 갯수
    				0x3333ff, "YR(Yield Rate)")->setDataSymbol(Chart::DiamondSymbol, 4, -1, 0x3333ff);	
    				//					// 남색,  Plot the points with a 4 pixel dismond symbol
    									
    layer->addDataSet( DoubleArray(&center[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 
	   				//0x008800, "Center(p-bar)");	// 초록색
	   				0x008800, "Center(p-bar)")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);	

    layer->addDataSet( DoubleArray(&m_daLCL[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 
    				//0xff0000, "LCL");				// 빨간색 
    				0xff00ff, "LCL")->setDataSymbol(Chart::DiamondSymbol, 4, -1, 0xff00ff);		// 자주색

    // For demo purpose, we use a dash line color for the last line
    //layer->addDataSet(DoubleArray(m_daLCL, (int)(sizeof(m_daLCL) / sizeof(m_daLCL[0]))), 
    //				c->dashLineColor(0x3333ff, Chart::DashLine), "LCL");	// 남색 점선
    
    // The x-coordinates for the line layer	 : X 축 data label 설정.
	layer->setXData(DoubleArray(&dataX0[1], (int)g_sLotNetDate_Info.naLotNetCnt[nLot] ));


	// Track Net이 설정된 경우, Track line도 그려준다.
    if (nTrackNet != -1)
	{
    	//trackLineLegend(c, c->getPlotArea()->getRightX());
    	
    	// Include track line for the latest data values
	    trackLineLabel(c, nTrackNet);
	}
    

    // XY LineChart  출력 ----------------


    // Output the chart
    m_ChartViewer.setChart(c);
	
    // Include tool tip for the chart
    m_ChartViewer.setImageMap( c->getHTMLImageMap("", "", "title='{xLabel}: US${value}K'"));

	// In this Fample project, we do not need to chart object any more, so we 
	// delete it now.
    delete c;

}

//void CPChartDialog::trackLineLabel(XYChart *c, int mouseX)
void CPChartDialog::trackLineLabel(XYChart *c, int xValue)
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();

#if 0
    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);
#else
    int xCoor = c->getXCoor((double)xValue);
#endif

    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, 
		d->dashLineColor(0x000000, 0x0101));

    // Draw a label on the x-axis to show the track line position.
	ostringstream xlabel;
//	xlabel << "<*font,bgColor=000000*> " << c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy") << " <*/font*>";		
	xlabel << "<*font,bgColor=000000*> " << c->formatValue(xValue, "{value|P4}") << " <*/font*>";
	TTFText *t = d->text(xlabel.str().c_str(), "arialbd.ttf", 8);

    // Restrict the x-pixel position of the label to make sure it stays inside the chart image.
    int xLabelPos = max(0, min(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
	t->draw(xLabelPos, plotArea->getBottomY() + 6, 0xffffff);
	t->destroy();	

    // Iterate through all layers to draw the data labels
    for (int i = 0; i < c->getLayerCount(); ++i) {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j) 
		{
            DataSet *dataSet = layer->getDataSetByZ(j);
 			const char *dataSetName = dataSet->getDataName();

            // Get the color, name and position of the data label
            int color = dataSet->getDataColor();
            int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

            // Draw a track dot with a label next to it for visible data points in the plot area
            if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color !=
				Chart::Transparent) && dataSetName && *dataSetName)
			{
                d->circle(xCoor, yCoor, 4, 4, color, color);

				ostringstream label;
				label << "<*font,bgColor=" << hex << color << "*> " 
					<< c->formatValue(dataSet->getValue(xIndex), "{value|P4}") << " <*font*>";
				t = d->text(label.str().c_str(), "arialbd.ttf", 8);
				
				// Draw the label on the right side of the dot if the mouse is on the left side the
                // chart, and vice versa. This ensures the label will not go outside the chart image.
                if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
					t->draw(xCoor + 5, yCoor, 0xffffff, Chart::Left);
                else
					t->draw(xCoor - 5, yCoor, 0xffffff, Chart::Right);
                
				t->destroy();
            }
        }
    }
}

#if 0
//void CPChartDialog::trackLineLegend(XYChart *c, int mouseX)
void CPChartDialog::trackLineLegend(XYChart *c, int xValue) 
{
    // Clear the current dynamic layer and get the DrawArea object to draw on it.
    DrawArea *d = c->initDynamicLayer();

    // The plot area object
    PlotArea *plotArea = c->getPlotArea();
#if 0
    // Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
    double xValue = c->getNearestXValue(mouseX);
    int xCoor = c->getXCoor(xValue);
#else
    int xCoor = c->getXCoor((double)xValue);
#endif


    // Draw a vertical track line at the x-position
    d->vline(plotArea->getTopY(), plotArea->getBottomY(), xCoor, d->dashLineColor(0x000000, 0x0101));

    // Container to hold the legend entries
	vector<string> legendEntries;

    // Iterate through all layers to build the legend array
    int i;
    for (i = 0; i < c->getLayerCount(); ++i) {
        Layer *layer = c->getLayerByZ(i);

        // The data array index of the x-value
        int xIndex = layer->getXIndexOf(xValue);

        // Iterate through all the data sets in the layer
        for (int j = 0; j < layer->getDataSetCount(); ++j) {
            DataSet *dataSet = layer->getDataSetByZ(j);

            // We are only interested in visible data sets with names
            const char *dataName = dataSet->getDataName();
            int color = dataSet->getDataColor();
			if (dataName && *dataName && (color != Chart::Transparent)) {
                // Build the legend entry, consist of the legend icon, name and data value.
                double dataValue = dataSet->getValue(xIndex);
				ostringstream legendEntry;
				legendEntry << "<*block*>" << dataSet->getLegendIcon() << " " << dataName << ": " <<
					((dataValue == Chart::NoValue) ? "N/A" : c->formatValue(dataValue, "{value|P4}"))
					<< "<*/*>";
				legendEntries.push_back(legendEntry.str());

                // Draw a track dot for data points within the plot area
                int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());
                if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY())) {
                    d->circle(xCoor, yCoor, 4, 4, color, color);
                }
            }
        }
    }

    // Create the legend by joining the legend entries
	ostringstream legendText;
	legendText << "<*block,maxWidth=" << plotArea->getWidth() << "*><*block*><*font=arialbd.ttf*>["
		<< c->xAxis()->getFormattedLabel(xValue, "mmm dd, yyyy") << "]<*/*>";
	for (i = ((int)legendEntries.size()) - 1; i >= 0; --i)
		legendText << "        " << legendEntries[i];
	
    // Display the legend on the top of the plot area
    TTFText *t = d->text(legendText.str().c_str(), "arial.ttf", 8);
	t->draw(plotArea->getLeftX() + 5, plotArea->getTopY() - 3, 0x000000, Chart::BottomLeft);
	t->destroy();
}
#endif


void CPChartDialog::DisplayGrid_OOC(int nLot, int nDate, int nTrackNet /*= -1*/, BOOL bScrollBarInit /*= TRUE*/)
{
	
	MyTrace(PRT_BASIC, _T("DisplayGrid_OOC(): nLot=%d, nDate=%d, nTrackNet=%d, bScrollBarInit=%d\n"), 
			nLot, nDate, nTrackNet, bScrollBarInit);
	ClearGrid_OOC();
__PrintMemSize(FUNC(DisplayGrid_OOC), __LINE__);

	int netSize = g_sLotNetDate_Info.naLotNetCnt[nLot];
	if (m_nRows != (netSize +1))
	{
		m_nRows = netSize + 1; 	// netSize + 1 (헤더 라인)
		TRY {
			m_gridOOC.SetRowCount(m_nRows);
			m_gridOOC.SetColumnCount(m_nCols);
			m_gridOOC.SetFixedRowCount(m_nFixRows);
			m_gridOOC.SetFixedColumnCount(m_nFixCols);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
			return;
		}
	    END_CATCH	
	}

	// Scroll Bar 초기화 옵션이 켜져 있으면 초기화 수행.
	if (bScrollBarInit == TRUE)
	{
		m_gridOOC.SetScrollPos32(SB_VERT, 0);
		m_gridOOC.SetScrollPos32(SB_HORZ, 0);
	}


	int nRow = 0;

	// Data  출력
	for (int net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;

		// OOC = TRUE 인 net만 출력하기
		if (m_bOocListOocOnly == TRUE 
				&& m_baOOC[net] == FALSE)		
			continue;

		// FALUT 인 net만 출력하기
		if (m_bOocListFaultOnly == TRUE 
				&&  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;

		nRow++; // row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력

		DisplayGrid_OOCTuple(nRow,											// Grid Row No
				nLot, nDate, net, 											// Lot, Date, Net
				(m_waCount[net] + g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate]),
																			// Total (nCount + NG)
				g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate], 	// NG Count
				m_waCount[net], 											// Total-NG => Count
				g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate],	// Fault Count
				m_waNormal[net], 											// Normal Count (Count-Fault)
				m_daYR[net],												// YR (Normal / Count)
				m_dCenter,													// Center
				m_daLCL[net],												// LCL
				m_baOOC[net]);												// OOC

		// track net이 설정된 경우, 해당 Net번호 Cell을 하늘색 표시
		if (nTrackNet != -1 && nTrackNet == net)
		{
	    	//m_gridOOC.SetItemBkColour(nRow, OOC_COL_NET, RGB(0x87, 0xce, 0xeb)); 	// 하늘색		(skyblue)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_NET, RGB(0xa9, 0xa9, 0xa9)); 	// 회색		(darkgray)

	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_YR, RGB(0x64, 0x95, 0xed)); 	// 연한 남색    (cornflowerblue)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_CENTER, RGB(0x90, 0xee, 0x90)); // 연한 초록색	(palegreen)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_LCL, RGB(0xdd, 0xa0, 0xdd)); 	// 연한 자주색  (plum)
		}
	}

__PrintMemSize(FUNC(DisplayGrid_OOC), __LINE__);

	UpdateData(FALSE);
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.
}
	
void CPChartDialog::DisplayGrid_OOCTuple(int nRow, int nLot, int nDate, int nNet, 
								int nTotal, int nNgCount, int nCount, int nFault, int nNormal, 
								double dYR, double dCenter, double dLCL, bool bOOC)
{
	CString strTemp;

	//    0     1        2      3       4         5         6       7          8        9       10      11      12
	//	{"No", "Lot", "Date", "Net", "Total", "NgCount", "Count",  "Fault", "Normal",  "YR", "Center", "LCL",  "OOC" };
	
		
	// row 0 헤더는 제외하고 data영역인 row 1 (net 1)부터 출력
	strTemp.Format("%d", nRow);
	m_gridOOC.SetItemText(nRow, OOC_COL_NO, strTemp);					// col 0: No
	
	strTemp.Format("%s", g_sLotNetDate_Info.strLot[nLot]); 		
	m_gridOOC.SetItemText(nRow, OOC_COL_LOT, strTemp);					// col 1: Lot Str

	strTemp.Format("%s", g_sLotNetDate_Info.strLotDate[nLot][nDate]); 		
	m_gridOOC.SetItemText(nRow, OOC_COL_DATE, strTemp);					// col 2: Date Str

	strTemp.Format("%d", nNet);
	m_gridOOC.SetItemText(nRow, OOC_COL_NET, strTemp);					// col 3: Net No
	
	strTemp.Format("%d", nTotal);
	m_gridOOC.SetItemText(nRow, OOC_COL_TOTAL, strTemp);				// col 4: Total Count

	strTemp.Format("%d", nNgCount);
	m_gridOOC.SetItemText(nRow, OOC_COL_NGCOUNT, strTemp);				// col 5: NG Count

	strTemp.Format("%d", nCount);
	m_gridOOC.SetItemText(nRow, OOC_COL_COUNT, strTemp);				// col 6: Total-NG Count

	strTemp.Format("%d", nFault);
	m_gridOOC.SetItemText(nRow, OOC_COL_FAULT, strTemp);				// col 7: Fault Count

	strTemp.Format("%d", nNormal);
	m_gridOOC.SetItemText(nRow, OOC_COL_NORMAL, strTemp);				// col 8: Normal Count
	
	strTemp.Format("%.4f", dYR);
	m_gridOOC.SetItemText(nRow, OOC_COL_YR, strTemp);					// col 9: YR

	strTemp.Format("%.4f", dCenter);
	m_gridOOC.SetItemText(nRow, OOC_COL_CENTER, strTemp);				// col 10: Center

	strTemp.Format("%.4f", dLCL);
	m_gridOOC.SetItemText(nRow, OOC_COL_LCL, strTemp);					// col 11: LCL

	strTemp.Format("%s", (bOOC == TRUE) ? "On": "Off");		
	m_gridOOC.SetItemText(nRow, OOC_COL_OOC, strTemp);					// col 12: OOC


	// OOC가 TRUE라면 OOC_COL_YR(9)과  OOC_COL_OOC(12)를 빨간색 표시
	if (bOOC == TRUE) 
	{
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_YR, RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)보다 약간 연한 빨강
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_OOC, RGB(0xdc, 0x24, 0x4c));		
	}

	// Fault가 있으면 OOC_COL_FAULT를 주황색 표시
	if (nFault > 0)
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_FAULT, RGB(0xff, 0x63, 0x47));	// tomato : 진한 주황
			
	// LCL은 OOC Alarm의 기준이므로 default로 분홍색 배경 표시
	m_gridOOC.SetItemBkColour(nRow, OOC_COL_LCL, RGB(0xFF, 0xC0, 0xCB));	// pink 

}

void CPChartDialog::ClearGrid_OOC()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridOOC.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
		for (int col = 0; col < m_gridOOC.GetColumnCount(); col++)
		{
			m_gridOOC.SetItemText(row, col, "                 ");

			// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
			m_gridOOC.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

}



void CPChartDialog::OnButtonSavePchart() 
{
	// TODO: Add your control notification handler code here
	
/*	
	m_nNetSum_Normal 은 모든 data가 NG 라면 data 로딩을 했어도 0일 수 있음.
	if (m_nNetSum_Normal == 0)	// P-Chart Calc가 되지 않은 경우
	{
		CString strTemp;
		strTemp.Format("First, run the 'Display Data' button please.\n");
		AfxMessageBox(strTemp, MB_ICONINFORMATION);
		MyTrace(PRT_BASIC, strTemp);
		return;
	}
*/
	
	SavePChart_CsvFile(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}


void CPChartDialog::OnButtonViewPchartCsv() 
{
	// TODO: Add your control notification handler code here
	
	CString strTemp;
	
	strTemp.Format(".\\Data\\%s_%s_PChartOut.csv", 
			g_sLotNetDate_Info.strLot[m_nCombo_CurrLot], 
			g_sLotNetDate_Info.strLotDate[m_nCombo_CurrLot][m_nCombo_CurrDate]);

	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

	
}

void CPChartDialog::SavePChart_CsvFile(int nLot, int nDate)
{
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;


	MyTrace(PRT_BASIC, "SavePCahrt_CsvFile(): nLot=%d, nDate=%d\n", nLot, nDate);

	// CSV 파일 출력.
	::ZeroMemory(&fName, sizeof(fName));
	strTemp.Format(".\\Data\\%s_%s_PChartOut.csv", 
			g_sLotNetDate_Info.strLot[nLot], g_sLotNetDate_Info.strLotDate[nLot][nDate]);
	strcat(fName, strTemp);

	fp=fopen(fName,"wt");
	if(fp == NULL)
	{ 	
		strTemp.Format("fName=%s, err=%s", fName, strerror( errno ));
		ERR.Set(FLAG_FILE_CANNOT_OPEN, strTemp); 
		return; 
	}


	//----------------------------------------------------
	// p Chart CSV 출력

	// 헤더 출력
	fprintf(fp, "NetCount =, %d, ,OocNetCnt=, %d, ,FaultNetCnt=, %d\n\n",  m_nNetCount, m_nOocNetCount, m_nFaultNetCount );

	fprintf(fp, "No, Lot=%d, Date=%d, Net, Total, NgCount, Count, Fault, Normal, YR, Center, LCL, OOC Alarm\n", 
				nLot, nDate);

	int nRow = 0;

	// Data  출력
	for (int net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{

		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// 없는 Net이면 skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// 없는 date면 skip
			continue;

		// OOC = TRUE 인 net만 출력하기
		if (m_bOocListOocOnly == TRUE 
				&& m_baOOC[net] == FALSE)		
			continue;

		// FALUT 인 net만 출력하기
		if (m_bOocListFaultOnly == TRUE 
				&&  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;

		nRow++;

		fprintf(fp, "%d, %s, %s, %d, %d, %d, %d, %d, %d, %.4f, %.4f, %.4f, %s\n", 
						nRow,														// 0: No
						g_sLotNetDate_Info.strLot[nLot], 							// 1: Lot Str
						g_sLotNetDate_Info.strLotDate[nLot][nDate], 				// 2: Date Str
						net, 														// 3: Net No
						(m_waCount[net] + g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate]), 	
																					// 4: Total Count
						g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate], 	// 5: NG Count
						m_waCount[net], 											// 6: Total-NG => Count
						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate], // 7: Fault Count
						m_waNormal[net], 											// 8: Normal Count
						m_daYR[net],												// 9: YR
						m_dCenter,													//10: Center
						m_daLCL[net],												//11: LCL
						(m_baOOC[net] == TRUE) ? "On": "Off");						//12: OOC Alarm 여부

	}	


	fclose(fp);

	strTemp.Format("\'Save to CSV\' completed. \n(%s)", fName);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
}


// 체크박스 상태가 변경되었을 때 변경된 state에 맞추어서 grid만 다시 그려준다. 
void CPChartDialog::OnCheckOocnetOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	MyTrace(PRT_BASIC, "m_bOocListOocOnly Value Changed to %d\n", m_bOocListOocOnly);
	m_bOocListFaultOnly = FALSE;			// Fault Only와 OOC Only가 동시에 체크되지 않도록
	UpdateData(FALSE);
	
	// 바뀐모드로 OOC Grid를 다시 그린다.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}


void CPChartDialog::OnCheckFaultOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	MyTrace(PRT_BASIC, "m_bOocListFaultOnly Value Changed to %d\n", m_bOocListFaultOnly);
	m_bOocListOocOnly = FALSE;			// Fault Only와 OOC Only가 동시에 체크되지 않도록
	UpdateData(FALSE);

	// 바뀐모드로 OOC Grid를 다시 그린다.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}

// GVN_SELCHANGED  OOC Grid Notification 처리
//  : grid에서 최종적으로 클릭한 cell의 좌표를 가지고 noti가 넘어온다.
//    클릭한 row의 3번째 col에서 net 번호를 획득하고 해당 net에 trackLine을
//    표시하여 차트를 다시 그린다.
void CPChartDialog::OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	// 해당 row, col=3 (Net 위치)의 값을 읽어 온다.
	CString strTemp = m_gridOOC.GetItemText(pItem->iRow, OOC_COL_NET);		
	int net = atoi(strTemp);

	MyTrace(PRT_BASIC, "OnGridSelChanged(), row=%d, col=%d, net=%d\n", pItem->iRow, pItem->iColumn, net);

	// 해당 net에 trackLine을 표시하도록 차트를 다시 그린다.
	DisplayPChart(m_nCombo_CurrLot, m_nCombo_CurrDate, net);
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate, net, 
							FALSE);		// scroll bar init을 하지 않는 옵션
										// cell을 클릭했을 때는 다시 그리더라도 scroll 위치를 유지해야 
	
}



BOOL CPChartDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_gridOOC.DeleteAllItems();	

	MyTrace(PRT_BASIC, "YR PChart Dialog Destroyed...\n" );
	
	return CDialog::DestroyWindow();
}

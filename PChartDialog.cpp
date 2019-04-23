// PChartDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ace400statistics.h"
#include "PChartDialog.h"

#include "math.h"
#include "chartdir.h" 			// p-chart�� �׸��� ���� �߰�
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
	ON_MESSAGE(UWM_LOAD_LOG4W_DATA, 				OnStatLoad4wData)		// ����� ���� �޽���

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
	m_nCombo_CurrLot = 0;		// �ʱ�ȭ 	
	m_nCombo_CurrDate = 0; 		// �ʱ�ȭ 

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
	// Grid ���� ��� ���� �ʱ�ȭ
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_OOC_GRID_COL;
	//m_nRows = MAX_OOC_GRID_ROW;
	m_nRows = 201;			// default . DisplayGrid �ÿ� �ٽ� �����.
	m_bEditable = FALSE;

	UpdateData(FALSE);		// m_nNetCount, m_nOocNetCount edit box ȭ�� ������ ���� �ʿ�
	return TRUE;
}


BOOL CPChartDialog::InitView()
{

	UpdateData(TRUE);


	//----------------------------
	// Lot Combo Box �ʱ�ȭ
	
	// Statistics â���� �ʱ�ȭ�� Lot data �������� Lot combo�� ä���. 
	ComboLot_UpdateContents();

	m_comboDate.ResetContent();				// ���� combo�� date data�� ��� �����.

#if 0 	// 2018.07.18: Date ALL�� P Chart������ ������� �ʱ��.
	m_comboDate.InsertString(-1, "ALL");	// -1: add string("ALL") to end of the list
	m_comboDate.SetCurSel(0); 				// combo idx = 0: ALL
#else
	ComboDate_UpdateContents(0);	// Lot 0 �������� �ʱ�ȭ�Ѵ�.
	//m_comboDate.SetCurSel(0); 		// �ʱ�ȭ  => ������ �ؾ� ����� �ǹǷ� �ʱ�ȭ�� ����

#endif

	//------------------------
	// Chart �ʱ�ȭ 
	
    m_ChartViewer.setChart(0); // ������ �׷����� �׷����� �ִٸ� �ʱ�ȭ�Ѵ�.


	//------------------------
	// OOC Grid  �ʱ�ȭ 


	m_gridOOC.SetEditable(m_bEditable);				// FALSE ����
	//m_gridOOC.SetListMode(m_bListMode);
	//m_gridOOC.EnableDragAndDrop(TRUE);
	m_gridOOC.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// ������� back ground

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

	// OOC Grid Header ����
	
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
	m_gridOOC.SetRowHeight(0, 40);	// ����� ���� ���� ǥ�ø� ���� ���� ����

    
	ClearGrid_OOC();

	// scroll bar ��ġ�� �ʱ�ȭ
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
	// ��� �ʱ�ȭ
	if (InitMember() == FALSE)
		return FALSE;


	// View �ʱ�ȭ 
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


// â�� ���� ���϶� ������ �ʱ�ȭ�ؾ� �ϴ� ������� ������ 
// OnShowWindow()�� TODO �ؿ��� �ʱ�ȭ�Ѵ�. 
void CPChartDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE)
	{
/*
		// ��� �ʱ�ȭ
		if (InitMember() == FALSE)
			return;


		// View �ʱ�ȭ 
		if (InitView() == FALSE)
			return;	

		DisplayPchart_ALL();
*/
	}	
	
}


// Stat â����  Load�� ���� ���� �� (Load �Ϸ� �޽��� ���Ž�) ���� InitMember(), InitView()�� �����Ѵ�. 
// Load�� ���� ���� �ʾ��� ������ P-chart ���� �ٸ� â�� ���� �͵� ������ ���õ� Lot, Date�� ���� �ǵ��� 
// OnShowWindow()�� �ƴ϶� ���⼭ InitMember(), InitView() �� �����Ѵ�.
LRESULT CPChartDialog::OnStatLoad4wData(WPARAM wParam, LPARAM lParam)
{

/*	// message recv test print
	CString strTemp;
	strTemp.Format("PChartDlg:: OnStatLoad4wData(): Recv \"UWM_LOAD_LOG4W_DATA\".\n");
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
*/
	
	// ��� �ʱ�ȭ
	if (InitMember() == FALSE)
		return FALSE;


	// View �ʱ�ȭ 
	if (InitView() == FALSE)
		return FALSE;	

	DisplayPchart_ALL();

	return TRUE;
}

// g_sLotNetDate_Info�� Ȱ���Ͽ� Statistics â���� �ʱ�ȭ�� Lot data �������� Lot combo�� ä���. 
void  CPChartDialog::ComboLot_UpdateContents()
{
	// ���� combo�� data�� ��� �����.
	m_comboLot.ResetContent();	

	for (int lot = 0 ; lot < g_sLotNetDate_Info.nLotCnt; lot++)
		m_comboLot.InsertString(-1, g_sLotNetDate_Info.strLot[lot]);			
	m_nCombo_CurrLot = 0;		// �ʱ�ȭ 	
	m_comboLot.SetCurSel(0);

	UpdateData(FALSE);		// SetCurSel ����� UI�� �ݿ�.
}


// g_sLotNetDate_Info�� Ȱ���Ͽ� Date combo�� ä���. 
void  CPChartDialog::ComboDate_UpdateContents(int nLot)
{
	// Lot ��ȣ�� �ٲ���ٸ�, ���õ� Lot�� ���ϴ� date ���� comboDate�� �־��ش�.
	//if (m_nCombo_CurrLot != nLot)
	{
		// ���� combo�� data�� ��� �����.
		m_comboDate.ResetContent();	
		
#if 0 	// 2018.07.18: Date ALL�� P Chart������ ������� �ʱ��.

		// com idx=0 �߰�
		m_comboDate.InsertString(-1, "ALL");		// combo idx = 0: ALL

		// date_0�� �ι�° (idx=1) ��  combo�� �߰��ȴ�.  combo idx�� 'date+1'�� �ٷ� ��.
		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			

		// net�̳� combo Ŭ���ÿ� �ٷ� display�� ���� date �� default�� "ALL"�� ������ �ش�.
		m_nCombo_CurrDate = 0; 	// All     0=All,  1~7 : ���� date#+1
		m_comboDate.SetCurSel(0);
#else

		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			
		m_nCombo_CurrDate = 0; 		// �ʱ�ȭ 
		m_comboDate.SetCurSel(0);

#endif		

		UpdateData(FALSE);		// SetCurSel ����� UI�� �ݿ�.
	}
}



void CPChartDialog::OnSelchangeComboLot() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	
	int 	comboPrevLot = m_nCombo_CurrLot;
	m_nCombo_CurrLot = m_comboLot.GetCurSel();		// ���� combo���� ���õ� Lot# 

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
	
	
	// Statistics â���� �ʱ�ȭ�� Date data �������� Lot��ȣ�� �´� ��¥ data�� ä���ش�.
	ComboDate_UpdateContents(m_nCombo_CurrLot);

	DisplayPchart_ALL();
}

void CPChartDialog::OnSelchangeComboDate() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;

	int 	comboPrevDate = m_nCombo_CurrDate;
	m_nCombo_CurrDate = m_comboDate.GetCurSel();		// ���� combo���� ���õ� date# (0=ALL�̹Ƿ� ���� date+1),  �̼����̸� -1

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
	// ���õ� Lot, Date �������� P-Chart�� ���� data�� ����Ѵ�.
	CalcPChart(m_nCombo_CurrLot, m_nCombo_CurrDate);		// DATE_ALL�� combo�� �����Ƿ� comboDate�� �׳� nDate�� ���
	DisplayPChart(m_nCombo_CurrLot, m_nCombo_CurrDate);		// trackLine�� ���� ��Ʈ�� �׸���.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}


// Normal, Count, YR, Center, LCL ����ϱ�  
void CPChartDialog::CalcPChart(int nLot, int nDate)
{

	MyTrace(PRT_BASIC, "CalcPChart(): nLot=%d, nDate=%d\n", nLot, nDate);


										 	
	// ������ ����� ���� ���� ���� �� �����Ƿ� �ݵ�� �ʱ�ȭ�ϰ� ����Ѵ�.
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
	// Net �� p chart data ���  (�����ִ� Net�� ã��)
	int net;
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// ���� Net�̸� skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// ���� date�� skip
			continue;


		int tupleSize  = g_pvsaNetData[nLot][net][nDate]->size();
		int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
		int nTotal  = tupleSize * stepSize; 
		int nFault  = g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate];
		m_waCount[net]  = nTotal - g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][net][nDate];	// Total - NG
		m_waNormal[net] = m_waCount[net] - nFault; 		// Count - Fault

		m_nNetSum_Normal += m_waNormal[net];
		m_nNetSum_Count += m_waCount[net];

		// YR(����): �׷����� ���� ���
		if (nFault == 0 && m_waCount[net] == 0) // Fault=0 �̸�, FR�� 0�̹Ƿ�, YR�� 1�� �Ǵ°� ���� �´�.
			m_daYR[net] = 1.0;		

		if (m_waCount[net] > 0)							// check devide by zero
			m_daYR[net]  = m_waNormal[net] / (double)m_waCount[net];		


		// m_dMaxYR, m_dMinYR ���
		if (net == 1)		// net�� 1���� ����.
		{
			m_dMaxYR = m_daYR[net];
			m_dMinYR = m_daYR[net];
		}
		else 
		{
			if (m_dMaxYR <  m_daYR[net])	// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
				m_dMaxYR =  m_daYR[net];

			if (m_dMinYR > m_daYR[net])		// �ּҰ����� ���簪�� ������ �ּҰ� ����
				m_dMinYR =  m_daYR[net];
		}

	}

	// Center ���: �׷��� �߽���
	if (m_nNetSum_Count > 0)		// check devide by zero
		m_dCenter =  m_nNetSum_Normal / (double)m_nNetSum_Count;

	// LCL, OOC ��� 
	//for (net =0; net < MAX_NET_PER_LOT; net++)
	for (net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{

		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// ���� Net�̸� skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// ���� date�� skip
			continue;


		// LCL: OOC(Out Of Control) ����
		//      Center - 3 * SQRT(Center* (1-Center) / Count)
		if ( m_waCount[net] > 0 )									// check devide by zero
			m_daLCL[net] = m_dCenter - (3 * sqrt((m_dCenter * (1 - m_dCenter)) / m_waCount[net])); 	

		// OOC �Ǵ� 
		//if (m_daYR[net] <= m_daLCL[net])	 //2018.07.31  Fault�� ���� ���� ���, ��� data�� ����
		//										    YR, Center, LCL�� ��� 1��.  �� ��  OOC�� True�̸� �ȵǹǷ�
		//										    OOC���ؿ��� YR�� LCL�� ���� ���� ������ �Ѵ�.
		if (m_daYR[net] < m_daLCL[net])	
		{
			m_baOOC[net] = TRUE;	// OOC ����
			m_nOocNetCount++;		// OOC Net ����.  for 'OOC Net Count' edit box
		}

		if (g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] > 0)
			m_nFaultNetCount++;		// Fault Net ����. for 'Fault Net Count' edit box
	}

}
	

void CPChartDialog::DisplayPChart(int nLot, int nDate, int nTrackNet /*= -1*/)
{
	
	MyTrace(PRT_BASIC, _T("DisplayPChart(): nLot=%d, nDate=%d, nTrackNet=%d\n"), nLot, nDate, nTrackNet);
	



/*
	// Simple Line Chart
    // Create a XYChart object of size 250 x 250 pixels
    XYChart *c = new XYChart(600, 350);		// ���� ��Ʈ���� ũ�� ����

    // Set the plotarea at (80, 80) and of size 200 x 200 pixels
    c->setPlotArea(80, 68, 		// X, Y point ��ġ  : ���� point  
    				420, 200);	// X width, Y hight : ���� �׷����� ũ�� ����

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
	// YR, Center, LCL �� �׷��� �׸��� 
	//
	// Draw Chart and set to CChartViewer
	

    // The data for the bar chart
    //double proportion[] = {0.08, 0.02, 0.06, 0.02, 0.04, 0.12, 0.06, 0.04, 0.06, 0, 0.25, 0.16666667, 0.14893617, 0.127659574, 0.130434783 };
    //double center[] = { 0.087, 0.087, 0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087,0.087, 0.087, 0.087 };
    //double UCL[] = { 0.207, 0.207, 0.207, 0.207,0.207, 0.207,0.207, 0.207,0.207, 0.207,0.209, 0.209,0.210, 0.210, 0.210 };
	// The labels for the bar chart
    //const char *labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};

   	double center[MAX_NET_PER_LOT];		// center���� array�� �ƴϹǷ� ����� ���� array�� �����.
    double dataX0[MAX_NET_PER_LOT];		// X �� label�� ����� array 
	for (int net =1; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		center[net] = m_dCenter;
		dataX0[net] = net;
	}


	// ��Ʈ ���� ------
    // Create an XYChart object of size 950 x 300 pixels, with a light blue (EEEEFF) background,
    // black border, 1 pxiel 3D border effect and rounded corners
    XYChart *c = new XYChart(850, 270, 0xeeeeff, 0x000000, 1);
    //c->setRoundedFrame();

    // Set the plotarea at (55:x, 58:y) and of size 870 x 195 pixels, with white background. Turn on
    // both horizontal and vertical grid lines with light grey color (0xcccccc)
    c->setPlotArea(55, 58, 770, 175, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);


	// �������� ����  ------- 
    // Add a legend box at (250:x, 30:y) (top of the chart) with horizontal layout. Use 9pt Arial Bold
    // font. Set the background and border color to Transparent.
    c->addLegend(250, 30, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);



	// X��, Y �� Ÿ��Ʋ ���� ----------------
	
    // Add a title box to the chart using 15pt Times Bold Italic font, on a light blue (CCCCFF)
    // background with glass effect. white (0xffffff) on a dark red (0x800000) background, with a 1
    // pixel 3D border.
	c->addTitle("Net YR p-Chart", "timesbi.ttf", 15)->setBackground(0xccccff,
      0x000000, Chart::glassEffect());

    // data�� ���� �ִ� ������ ��µǵ��� Scale �����ϱ�. 
    //   net[0]�� 0���� �ִ� �ǹ̾��� data�� �־���. net[0]�� �����ϰ� ������� �Ʒ� �ڵ� ���ʿ�.
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

	// layer.setXData()�� ����ϰ� �Ʒ��ڵ�� �ڸ�Ʈó��
    // Set the labels on the x axis.
    //c->xAxis()->setLabels(StringArray(labels, sizeof(labels)/sizeof(labels[0])));
    //c->xAxis()->setLabels(StringArray((const char * const *)&labels[1], g_sLotNetDate_Info.naLotNetCnt[nLot])); 
  	//c->xAxis()->setLabelStep(g_sLotNetDate_Info.naLotNetCnt[nLot] / 10);


	// XY LineChart ���� & �ɼ� ���� ----------------------


    // Add a line layer to the chart
    LineLayer *layer = c->addLineLayer();

    // Set the default line width to 2 pixels
    layer->setLineWidth(2);


    // Add the three data sets to the line layer. 
    layer->addDataSet( DoubleArray(&m_daYR[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 	// Net 1������ ó��, Net ����
    				0x3333ff, "YR(Yield Rate)")->setDataSymbol(Chart::DiamondSymbol, 4, -1, 0x3333ff);	
    				//					// ����,  Plot the points with a 4 pixel dismond symbol
    									
    layer->addDataSet( DoubleArray(&center[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 
	   				//0x008800, "Center(p-bar)");	// �ʷϻ�
	   				0x008800, "Center(p-bar)")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);	

    layer->addDataSet( DoubleArray(&m_daLCL[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 
    				//0xff0000, "LCL");				// ������ 
    				0xff00ff, "LCL")->setDataSymbol(Chart::DiamondSymbol, 4, -1, 0xff00ff);		// ���ֻ�

    // For demo purpose, we use a dash line color for the last line
    //layer->addDataSet(DoubleArray(m_daLCL, (int)(sizeof(m_daLCL) / sizeof(m_daLCL[0]))), 
    //				c->dashLineColor(0x3333ff, Chart::DashLine), "LCL");	// ���� ����
    
    // The x-coordinates for the line layer	 : X �� data label ����.
	layer->setXData(DoubleArray(&dataX0[1], (int)g_sLotNetDate_Info.naLotNetCnt[nLot] ));


	// Track Net�� ������ ���, Track line�� �׷��ش�.
    if (nTrackNet != -1)
	{
    	//trackLineLegend(c, c->getPlotArea()->getRightX());
    	
    	// Include track line for the latest data values
	    trackLineLabel(c, nTrackNet);
	}
    

    // XY LineChart  ��� ----------------


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
		m_nRows = netSize + 1; 	// netSize + 1 (��� ����)
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

	// Scroll Bar �ʱ�ȭ �ɼ��� ���� ������ �ʱ�ȭ ����.
	if (bScrollBarInit == TRUE)
	{
		m_gridOOC.SetScrollPos32(SB_VERT, 0);
		m_gridOOC.SetScrollPos32(SB_HORZ, 0);
	}


	int nRow = 0;

	// Data  ���
	for (int net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{
		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// ���� Net�̸� skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// ���� date�� skip
			continue;

		// OOC = TRUE �� net�� ����ϱ�
		if (m_bOocListOocOnly == TRUE 
				&& m_baOOC[net] == FALSE)		
			continue;

		// FALUT �� net�� ����ϱ�
		if (m_bOocListFaultOnly == TRUE 
				&&  g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][net][nDate] == 0)
			continue;

		nRow++; // row 0 ����� �����ϰ� data������ row 1 (net 1)���� ���

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

		// track net�� ������ ���, �ش� Net��ȣ Cell�� �ϴû� ǥ��
		if (nTrackNet != -1 && nTrackNet == net)
		{
	    	//m_gridOOC.SetItemBkColour(nRow, OOC_COL_NET, RGB(0x87, 0xce, 0xeb)); 	// �ϴû�		(skyblue)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_NET, RGB(0xa9, 0xa9, 0xa9)); 	// ȸ��		(darkgray)

	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_YR, RGB(0x64, 0x95, 0xed)); 	// ���� ����    (cornflowerblue)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_CENTER, RGB(0x90, 0xee, 0x90)); // ���� �ʷϻ�	(palegreen)
	    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_LCL, RGB(0xdd, 0xa0, 0xdd)); 	// ���� ���ֻ�  (plum)
		}
	}

__PrintMemSize(FUNC(DisplayGrid_OOC), __LINE__);

	UpdateData(FALSE);
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.
}
	
void CPChartDialog::DisplayGrid_OOCTuple(int nRow, int nLot, int nDate, int nNet, 
								int nTotal, int nNgCount, int nCount, int nFault, int nNormal, 
								double dYR, double dCenter, double dLCL, bool bOOC)
{
	CString strTemp;

	//    0     1        2      3       4         5         6       7          8        9       10      11      12
	//	{"No", "Lot", "Date", "Net", "Total", "NgCount", "Count",  "Fault", "Normal",  "YR", "Center", "LCL",  "OOC" };
	
		
	// row 0 ����� �����ϰ� data������ row 1 (net 1)���� ���
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


	// OOC�� TRUE��� OOC_COL_YR(9)��  OOC_COL_OOC(12)�� ������ ǥ��
	if (bOOC == TRUE) 
	{
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_YR, RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)���� �ణ ���� ����
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_OOC, RGB(0xdc, 0x24, 0x4c));		
	}

	// Fault�� ������ OOC_COL_FAULT�� ��Ȳ�� ǥ��
	if (nFault > 0)
    	m_gridOOC.SetItemBkColour(nRow, OOC_COL_FAULT, RGB(0xff, 0x63, 0x47));	// tomato : ���� ��Ȳ
			
	// LCL�� OOC Alarm�� �����̹Ƿ� default�� ��ȫ�� ��� ǥ��
	m_gridOOC.SetItemBkColour(nRow, OOC_COL_LCL, RGB(0xFF, 0xC0, 0xCB));	// pink 

}

void CPChartDialog::ClearGrid_OOC()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridOOC.GetRowCount(); row++)		// ����� �����ϰ� Ŭ�����Ѵ�.
		for (int col = 0; col < m_gridOOC.GetColumnCount(); col++)
		{
			m_gridOOC.SetItemText(row, col, "                 ");

			// Ȥ�� ����� faultó���� cell�� �ִٸ� ������ display�� ���� ���󺹱� �Ѵ�.
			m_gridOOC.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// ������� 
		}

}



void CPChartDialog::OnButtonSavePchart() 
{
	// TODO: Add your control notification handler code here
	
/*	
	m_nNetSum_Normal �� ��� data�� NG ��� data �ε��� �߾ 0�� �� ����.
	if (m_nNetSum_Normal == 0)	// P-Chart Calc�� ���� ���� ���
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

	// CSV ���� ���.
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
	// p Chart CSV ���

	// ��� ���
	fprintf(fp, "NetCount =, %d, ,OocNetCnt=, %d, ,FaultNetCnt=, %d\n\n",  m_nNetCount, m_nOocNetCount, m_nFaultNetCount );

	fprintf(fp, "No, Lot=%d, Date=%d, Net, Total, NgCount, Count, Fault, Normal, YR, Center, LCL, OOC Alarm\n", 
				nLot, nDate);

	int nRow = 0;

	// Data  ���
	for (int net =0; net <= g_sLotNetDate_Info.naLotNetCnt[nLot]; net++)
	{

		if (g_sLotNetDate_Info.naLotNet[nLot][net] == -1)	// ���� Net�̸� skip
			continue;

		if(g_pvsaNetData[nLot][net][nDate] == NULL)			// ���� date�� skip
			continue;

		// OOC = TRUE �� net�� ����ϱ�
		if (m_bOocListOocOnly == TRUE 
				&& m_baOOC[net] == FALSE)		
			continue;

		// FALUT �� net�� ����ϱ�
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
						(m_baOOC[net] == TRUE) ? "On": "Off");						//12: OOC Alarm ����

	}	


	fclose(fp);

	strTemp.Format("\'Save to CSV\' completed. \n(%s)", fName);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
}


// üũ�ڽ� ���°� ����Ǿ��� �� ����� state�� ���߾ grid�� �ٽ� �׷��ش�. 
void CPChartDialog::OnCheckOocnetOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	MyTrace(PRT_BASIC, "m_bOocListOocOnly Value Changed to %d\n", m_bOocListOocOnly);
	m_bOocListFaultOnly = FALSE;			// Fault Only�� OOC Only�� ���ÿ� üũ���� �ʵ���
	UpdateData(FALSE);
	
	// �ٲ���� OOC Grid�� �ٽ� �׸���.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}


void CPChartDialog::OnCheckFaultOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	MyTrace(PRT_BASIC, "m_bOocListFaultOnly Value Changed to %d\n", m_bOocListFaultOnly);
	m_bOocListOocOnly = FALSE;			// Fault Only�� OOC Only�� ���ÿ� üũ���� �ʵ���
	UpdateData(FALSE);

	// �ٲ���� OOC Grid�� �ٽ� �׸���.
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate);
	
}

// GVN_SELCHANGED  OOC Grid Notification ó��
//  : grid���� ���������� Ŭ���� cell�� ��ǥ�� ������ noti�� �Ѿ�´�.
//    Ŭ���� row�� 3��° col���� net ��ȣ�� ȹ���ϰ� �ش� net�� trackLine��
//    ǥ���Ͽ� ��Ʈ�� �ٽ� �׸���.
void CPChartDialog::OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;

	// �ش� row, col=3 (Net ��ġ)�� ���� �о� �´�.
	CString strTemp = m_gridOOC.GetItemText(pItem->iRow, OOC_COL_NET);		
	int net = atoi(strTemp);

	MyTrace(PRT_BASIC, "OnGridSelChanged(), row=%d, col=%d, net=%d\n", pItem->iRow, pItem->iColumn, net);

	// �ش� net�� trackLine�� ǥ���ϵ��� ��Ʈ�� �ٽ� �׸���.
	DisplayPChart(m_nCombo_CurrLot, m_nCombo_CurrDate, net);
	DisplayGrid_OOC(m_nCombo_CurrLot, m_nCombo_CurrDate, net, 
							FALSE);		// scroll bar init�� ���� �ʴ� �ɼ�
										// cell�� Ŭ������ ���� �ٽ� �׸����� scroll ��ġ�� �����ؾ� 
	
}



BOOL CPChartDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_gridOOC.DeleteAllItems();	

	MyTrace(PRT_BASIC, "YR PChart Dialog Destroyed...\n" );
	
	return CDialog::DestroyWindow();
}

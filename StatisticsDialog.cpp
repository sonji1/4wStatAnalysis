// StatisticsDialog.cpp : implementation file
//

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "ACE400Statistics.h"
#include "StatisticsDialog.h"
#include "math.h"
#include "cderr.h"



//---------------------
// 전역변수 선언
//---------------------
LotNetDate_Info		g_sLotNetDate_Info;	
vector <statNetData>	*g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// 동적할당 vector



/////////////////////////////////////////////////////////////////////////////
// CStatisticsDialog dialog


CStatisticsDialog::CStatisticsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CStatisticsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatisticsDialog)
	m_editStrSelect = _T("");
	m_editTupleNum = 0;
	m_editStepNum = 0;
	m_bDataGridFaultOnly = FALSE;
	m_bSimulateULSL = FALSE;
	m_bApplyULSL = FALSE;
	m_editStrUSL = _T("");
	m_editStrLSL = _T("");
	//}}AFX_DATA_INIT
	
	m_nTree_CurrLot = -1;	
	m_nTree_CurrNet = -1;	
	m_nCombo_CurrDate = DATE_ALL;	

	m_dSimulUsl = -1;		// 미사용의 의미로 -1. m_bApplyULSL = TRUE 일 때에만 이 값이 설정된다.
	m_dSimulLsl = -1;		// 미사용의 의미로 -1. m_bApplyULSL = TRUE 일 때에만 이 값이 설정된다.
	m_nPrevSimulLoT = -1;
	m_nPrevSimulNet = -1;
	m_nPrevSimulDate = DATE_ALL;
	m_hPrevSimulSelNode = NULL;

}


void CStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticsDialog)
	DDX_Control(pDX, IDC_COMBO_DATE, 			m_comboDate);
	DDX_Control(pDX, IDC_TREE, 					m_treeCtrl);
	DDX_Control(pDX, IDC_GRID_SUMMARY, 			m_gridSummary);
	DDX_Control(pDX, IDC_GRID_DATA, 			m_gridData);
	DDX_Text(pDX, IDC_EDIT_SELECT, 				m_editStrSelect);
	DDX_Text(pDX, IDC_EDIT_TUPLE_NUM, 			m_editTupleNum);
	DDX_Text(pDX, IDC_EDIT_STEP_NUM, 			m_editStepNum);
	DDX_Check(pDX, IDC_CHECK_DATA_FAULT_ONLY, 	m_bDataGridFaultOnly);
	DDX_Check(pDX, IDC_CHECK_SIMUL_ULSL, 		m_bSimulateULSL);
	DDX_Check(pDX, IDC_CHECK_APPLY_ULSL, 		m_bApplyULSL);
	DDX_Text(pDX, IDC_EDIT_USL, 				m_editStrUSL);
	DDX_Text(pDX, IDC_EDIT_LSL, 				m_editStrLSL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticsDialog, CDialog)
	//{{AFX_MSG_MAP(CStatisticsDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, 			OnSelchangedTree)
	ON_CBN_SELCHANGE(IDC_COMBO_DATE, 				OnSelchangeComboDate)
	ON_BN_CLICKED(IDC_BUTTON_LOAD4W_DATA, 			OnButtonLoad4wData)
	ON_BN_CLICKED(IDC_BUTTON_LOAD4W_DATA_SINGLE, 	OnButtonLoad4wData_SingleLot)
	ON_BN_CLICKED(IDC_CHECK_DATA_FAULT_ONLY, 		OnCheckDataFaultOnly)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_STAT_CSV, 		OnButtonSaveStatCsv)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_STAT_CSV, 		OnButtonViewStatCsv)
	ON_BN_CLICKED(IDC_CHECK_SIMUL_ULSL, 			OnCheckSimulUlsl)
	ON_BN_CLICKED(IDC_CHECK_APPLY_ULSL, 			OnCheckApplyUlsl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDialog message handlers

BOOL CStatisticsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	


	
//__PrintMemSize(FUNC(OnInitDialog), __LINE__);
	if (InitMember() == FALSE)
		return FALSE;

	if (InitView() == FALSE)
		return FALSE;
//__PrintMemSize(FUNC(OnInitDialog), __LINE__);

	g_sFile.Init();
//__PrintMemSize(FUNC(OnInitDialog), __LINE__);

#if 0
	MyTrace(PRT_BASIC, "       sizeof(sSTAT_TIME) = %d\n", sizeof(sSTAT_TIME));
	MyTrace(PRT_BASIC, "           sizeof(double) = %d\n", sizeof(double));
	MyTrace(PRT_BASIC, "   sizeof(statNetData) = %d\n", sizeof(statNetData));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info)= %d\n", sizeof(g_sLotNetDate_Info));

	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.strLot)= %d\n",      sizeof(g_sLotNetDate_Info.strLot));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.nLotCnt)= %d\n",     sizeof(g_sLotNetDate_Info.nLotCnt));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.strLotDate)= %d\n",  sizeof(g_sLotNetDate_Info.strLotDate));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.naLotDateCnt)= %d\n", sizeof(g_sLotNetDate_Info.naLotDateCnt));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.naLotNet)= %d\n",     sizeof(g_sLotNetDate_Info.naLotNet));
	MyTrace(PRT_BASIC, "sizeof(g_sLotNetDate_Info.waLotNetPin)= %d\n", sizeof(g_sLotNetDate_Info.waLotNetPin));
#endif
	
	// 이미 측정완료된  raw data 파일을 read한다.
	//Load_Log4wDir();   <= 여기서 자동 로드하면, 로드하는 동안 메시지 박스 외에 아무창도 안 보인다.
	
#if 0
//2018.07.03 : 파일 다이얼로그 로딩 기능 추가후에 아래 timer는 사용하지 않도록 함.
//#ifndef _DEBUG
	// 200ms 후에 Load_Log4wDir()를 수행하도록 Timer 설정.   (Release 모드일때만)
	SetTimer (0, 		// OnTimer 수행시 받을 Timer Id
			200, 		// 200ms 주기
			NULL);		// Timer 수신시 자동수행할 CallBack Function은 NULL로 설정.
#endif

	
	MyTrace(PRT_BASIC, "\"Statistics Dialog\" Started...\n" );


	CRect m_rectCurHist;
	this->GetWindowRect(m_rectCurHist);
	MyTrace(PRT_BASIC, "Statistics Dialog:   top=%d, bottom=%d, left=%d, right=%d\n\n", 
							m_rectCurHist.top, m_rectCurHist.bottom,
							m_rectCurHist.left, m_rectCurHist.right);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CStatisticsDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 0)
	{
		KillTimer(0);		// 1회성으로 쓰기 위해  Timer 받자 마자 해당 Timer Id를 반환한다.

		//CString strTemp;
		//strTemp.Format("The 'Load 4W Data' button will launch automatically to load \"%s\" files.", g_sFile.ACE400_4WDataDir);
		//AfxMessageBox(strTemp, MB_ICONINFORMATION);

		// 이미 측정완료된 raw data 파일을 read한다.
		Load_Log4wDir();
	}
	
	CDialog::OnTimer(nIDEvent);
}

// 창이 새로 보일때 때마다 초기화해야 하는 멤버들은 다음의 
// OnShowWindow()의 TODO 밑에 초기화한다. 
void CStatisticsDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE)
	{
		// 멤버 초기화
		


		// View 초기화 

	}
	
}

BOOL CStatisticsDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	Stat_deleteAllNetData();
	m_gridSummary.DeleteAllItems();

	m_gridData.DeleteAllItems();		// 이거 시간이 너무 오래 걸릴 수 있으나 하는게 맞다.
	
	MyTrace(PRT_BASIC, "StatDialog Destroyed...\n" );
	return CDialog::DestroyWindow();
}


BOOL CStatisticsDialog::InitMember()
{


	//----------------------------
	// Tree Current data 초기화 
	m_nTree_CurrLot = -1;	
	m_nTree_CurrNet = -1;	
	m_nCombo_CurrDate = DATE_ALL;	
	m_bDataGridFaultOnly = FALSE;


	m_dSimulUsl = -1;		// 미사용의 의미로 -1. m_bApplyULSL = TRUE 일 때에만 이 값이 설정된다.
	m_dSimulLsl = -1;		// 미사용의 의미로 -1. m_bApplyULSL = TRUE 일 때에만 이 값이 설정된다.
	m_nPrevSimulLoT = -1;
	m_nPrevSimulNet = -1;
	m_nPrevSimulDate = DATE_ALL;
	m_hPrevSimulSelNode = NULL;


	m_bSimulateULSL = FALSE;
	m_bApplyULSL = FALSE;
	m_editStrUSL = _T("");
	m_editStrLSL = _T("");

	//----------------------------
	// 4WData Load 관련 초기화
	//m_nLoad_CurrLot = 0;

	//----------------------------
	// Grid 관련 멤버 변수 초기화
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_DATA_GRID_COL;
	//m_nRows = MAX_DATA_GRID_ROW;
	m_nRows = 201;			// Default. 실제 Display시에 제대로 맞추자.

	m_bEditable = FALSE;
/*
	m_bListMode = TRUE;
	m_bHorzLines = TRUE;
	m_bVertLines = TRUE;
	m_bSelectable = TRUE;
	m_bAllowColumnResize = TRUE;
	m_bAllowRowResize = TRUE;
	m_bHeaderSort = TRUE;
	m_bReadOnly = TRUE;
	m_bItalics = TRUE;
	m_btitleTips = TRUE;
	m_bSingleSelMode = FALSE;
*/
	for (int lot=0; lot < MAX_LOT; lot++)
		for (int net=0; net < MAX_NET_PER_LOT; net++)
			for (int date=0; date < MAX_DATE; date++)
				g_pvsaNetData[lot][net][date] = NULL;


	g_sLotNetDate_Info.InitMember();
	m_nNetDataCount = 0;

	m_GridSnap.InitMember();

	MyTrace(PRT_BASIC, "        m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "sizeof(LotNetDate_Info) = %d\n", sizeof(LotNetDate_Info));
	MyTrace(PRT_BASIC, "    sizeof(statNetData) = %d\n", sizeof(statNetData));
	MyTrace(PRT_BASIC, "\n");

	return TRUE;
}


BOOL CStatisticsDialog::InitView()
{

//__PrintMemSize(FUNC(InitView), __LINE__);
	MyTrace(PRT_BASIC, "\n");

	UpdateData(TRUE);

	//----------------------------
	// Date Combo Box 초기화
	m_comboDate.InsertString(-1, "ALL");		// -1: add string("ALL") to end of the list
	m_comboDate.SetCurSel(0); 					// combo idx = 0: ALL

	//----------------------------
	// Summary Grid 초기화 

	m_gridSummary.SetEditable(m_bEditable);					// FALSE 설정
	//m_gridSummary.SetListMode(m_bListMode);
	//m_gridSummary.EnableDragAndDrop(TRUE);
	m_gridSummary.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 back ground


	TRY {
		m_gridSummary.SetRowCount(2);
		m_gridSummary.SetColumnCount(NUM_SUM_GRID_COL);
		m_gridSummary.SetFixedRowCount(1);
		m_gridSummary.SetFixedColumnCount(0);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH	

	Display_SummaryGridHeader(); 	// Summary Grid header 설정


	//----------------------------
	// Data Grid 초기화 

	m_gridData.SetEditable(m_bEditable);				// FALSE 설정
	//m_gridData.SetListMode(m_bListMode);
	//m_gridData.EnableDragAndDrop(TRUE);
	m_gridData.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 back ground


	TRY {
		m_gridData.SetRowCount(m_nRows);
		m_gridData.SetColumnCount(m_nCols);
		m_gridData.SetFixedRowCount(m_nFixRows);
		m_gridData.SetFixedColumnCount(m_nFixCols);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH	


	Display_DataGridHeader(); 	// Data Grid Header 설정

	ClearGrid_Data();	
	m_GridSnap.InitMember();


	//----------------------------
	// tree 초기화 
	
	InitTree();

	// Tree 이미지리스트 설정
	m_ImageList.Create(IDB_BITMAP1, 16, 	// 이미지의 가로, 세로 길이
									2,		// 이미지 관리 메모리 부족시 늘려야 할 메모리크기 
					RGB(255,255,255));		// 화면 출력 컬러 (투명)
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);


	// Simulate 관련 컨트롤 초기화
	if (m_bSimulateULSL)
	{
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(FALSE);
	}


	UpdateData(FALSE);

//__PrintMemSize(FUNC(InitView), __LINE__);
	MyTrace(PRT_BASIC, "\n");

	return TRUE;
}

// Summary Grid header 설정
void CStatisticsDialog::Display_SummaryGridHeader()
{
    m_gridSummary.SetItemText(0, SUM_COL_NET, "Net");
	m_gridSummary.SetColumnWidth(SUM_COL_NET, 60);

    m_gridSummary.SetItemText(0, SUM_COL_DATE, "Date");
	m_gridSummary.SetColumnWidth(SUM_COL_DATE, 80);
	
	m_gridSummary.SetItemText(0, SUM_COL_TOTAL, "Total\n(Tuple*Data)");
	m_gridSummary.SetColumnWidth(SUM_COL_TOTAL, 80);

    m_gridSummary.SetItemText(0, SUM_COL_NG, "NG Count");
	m_gridSummary.SetColumnWidth(SUM_COL_NG, 70);
	
    m_gridSummary.SetItemText(0, SUM_COL_COUNT, "Count\n(Total-NG)");
	m_gridSummary.SetColumnWidth(SUM_COL_COUNT, 80);

    m_gridSummary.SetItemText(0, SUM_COL_AVG, "Average");
	m_gridSummary.SetColumnWidth(SUM_COL_AVG, 80);

    m_gridSummary.SetItemText(0, SUM_COL_SIGMA, "Sigma\n(StDev)");
	m_gridSummary.SetColumnWidth(SUM_COL_SIGMA, 80);

    m_gridSummary.SetItemText(0, SUM_COL_DATAMIN, "Min");
	m_gridSummary.SetColumnWidth(SUM_COL_DATAMIN, 80);

    m_gridSummary.SetItemText(0, SUM_COL_DATAMAX, "Max");		
	m_gridSummary.SetColumnWidth(SUM_COL_DATAMAX, 80);

    m_gridSummary.SetItemText(0, SUM_COL_FAULT, "Fault\nCount");		
	m_gridSummary.SetColumnWidth(SUM_COL_FAULT, 70);
	
	m_gridSummary.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조정
}

// Data Grid Header 설정
void CStatisticsDialog::Display_DataGridHeader()
{
	
	// 0     1      2     3     4     5     6     7     8     9     10    11    12    13     14      ....   25
	// No,  Date, Time,  Pin1, Pin2, Pin3, Pin4,  R,   LSL,  USL,  Avg, Sigma, Min, Max,  Data1, .... Data12
	char dataHeader[NUM_DATA_GRID_COL][20] = {"No", "Date", "Time", "Pin1", "Pin2", "Pin3", "Pin4",  "R",  "LSL", "USL",
													"Average", "Sigma",   "DataMin", "DataMax"    , 
													"Data1", "Data2", "Data3", "Data4", "Data5", "Data6",
													"Data7", "Data8", "Data9", "Data10", "Data11", "Data12" };

	int dataColWidth[NUM_DATA_GRID_COL] =    {40,    68,     60,   45,     45,     45,     45,     65,    65,    65,
													 70,         70,         70,        70,
													 70,         70,         70,        70,         70,       70,
		                                             70,         70,         70,        70,         70,       70 };

	for (int i=0; i < NUM_DATA_GRID_COL; i++)
	{
		m_gridData.SetItemText(0, i, dataHeader[i]);
		m_gridData.SetColumnWidth(i, dataColWidth[i]);

		if (m_bApplyULSL == TRUE && i == DATA_COL_LSL)
			m_gridData.SetItemText(0, i, "LSL (simul)");

		if (m_bApplyULSL == TRUE && i == DATA_COL_USL)
			m_gridData.SetItemText(0, i, "USL (simul)");
	}
}

void CStatisticsDialog::InitTree()
{
	//이전에 작성한 Tree Contents가 있다면  지운다.
	m_treeCtrl.DeleteAllItems(); // <=이렇게하면 root까지 다 지워짐.

	// 'Total'을 root 항목으로 먼저 초기화
	TV_INSERTSTRUCT tvStruct;

	tvStruct.hParent = 0;					// 부모항목:  hParent가 0이면 root 항목
	tvStruct.hInsertAfter = TVI_LAST;		// 삽입방법 
	tvStruct.item.mask = TVIF_TEXT;			// item 멤버들의 유효화 설정: pszText 멤버 유효화
	tvStruct.item.pszText = "Total";		// 트리에 설정할 문자열
	m_hRoot = m_treeCtrl.InsertItem(&tvStruct);	

	m_hSelectedNode = NULL;		// SelectedNode 를 NULL 로 초기화해서 Node 선택없이 Display시에 대처한다.

}

// 'Load 4w Data' 버튼 클릭시에 호출.
void CStatisticsDialog::OnButtonLoad4wData() 
{
	BeginWaitCursor();		// 커서를 waiting 모드로 바꾼다.

	Load_Log4wDir();


	// default Net 조회: 별도로 클릭이 없어도 첫번째 Lot, 첫번째 Net을 조회해 준다.
	DisplayGrid_DefaultLotNet();
//__PrintMemSize(FUNC(OnButtonLoad4wData), __LINE__);
//
	EndWaitCursor();		// 커서를 원상복구한다.

	// LOAD LOG4W가 완료되었음을 FR Rank Dlg에 알린다.
	//::SendMessage(m_hwnd_FrRankDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
//__PrintMemSize(FUNC(OnButtonLoad4wData), __LINE__);
}

// 별도로 클릭이 없어도 첫번째 Lot, 첫번째 Net을 조회해 준다.
void CStatisticsDialog::DisplayGrid_DefaultLotNet() 
{ 
	// Lot의 갯수가 1개 이상인지 확인
	if (g_sLotNetDate_Info.nLotCnt <= 0)			
		return; 

   	// Lot_0의 Net 갯수가 1개 이상인지 확인
    if (g_sLotNetDate_Info.naLotNetCnt[0] <= 0)		
    	return; 

	// Root에서 Lot_0의 item을 먼저 찾고, Net_1을 그 밑에서 찾아서 클릭한 것으로 처리해 준다.
	HTREEITEM 	hLotItem = NULL, hNetItem = NULL;
	hLotItem = Tree_FindLotItem(&m_treeCtrl, m_hRoot, g_sLotNetDate_Info.strLot[0]);	// Lot_0
	if (hLotItem == NULL) 		
		return; 

	// Found: 존재하는 Lot이면 그 밑에서 Net_1을 찾는다.
	hNetItem = Tree_FindStrItem(&m_treeCtrl, hLotItem, "Net_1"); 
	if (hNetItem != NULL)
	{
		_SelChangedTree(hNetItem); 
		m_treeCtrl.SelectItem(hNetItem);			// Tree에서 해당 item을 선택된 것으로 표시
		m_treeCtrl.Expand(hLotItem, TVE_EXPAND);	// 해당 parent Lot을 Expand해서 선택 NetItem 을 보여준다.
	}
}

void CStatisticsDialog::Load_Log4wDir() 
{
	// TODO: Add your control notification handler code here
	MyTrace(PRT_BASIC, "Load 4wData...\n" );

	CString strTemp;
	strTemp.Format("Load \"%s\" files.\n\nIt may take a few minutes. Please wait.\n\n\nYou can load up to maximum 850MB files.\n", g_sFile.ACE400_4WDataDir);
	int ret = AfxMessageBox(strTemp, MB_OKCANCEL|MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);

	if (ret == IDCANCEL)
		return;

//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);
	MyTrace(PRT_BASIC, "          m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "\n");

	ClearGrid(); // data Grid, summary Grid, edit박스 등 UI를 초기화한다.

//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	Stat_deleteAllNetData();

//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);
	InitMember();

	Display_DataGridHeader(); 	// Data Grid Header 다시 설정 (simul이 on이었다면 off로 바꾸고 헤더를 다시 그려줘야함)

	InitTree();					// root부터 Tree를 다시 생성.	
//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	// 측정 raw data 파일을 read한다.
	Load_Log4wDir(g_sFile.ACE400_4WDataDir);
//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	
	// Load한 모든 Tree >Lot > Net에 대해 Fault 체크를 수행한다.
	Tree_CheckAll_LotNetFaults();
	
	strTemp.Format("Load_Log4wDir() for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", g_sFile.ACE400_4WDataDir);
	MyTrace(PRT_BASIC, "Load_Log4wDir() for\"%s\" Completed. \n", g_sFile.ACE400_4WDataDir);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "          m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "  sizeof(LotNetDate_Info) = %d\n", sizeof(LotNetDate_Info));
	MyTrace(PRT_BASIC, "      sizeof(statNetData) = %d\n", sizeof(statNetData));
	MyTrace(PRT_BASIC, "\n");

__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	// 2018.04.27 모든 Lot이 read 완료 되었으면  현재  lot정보를 파일로 저장한다. 
	//Save_StatLotDataFile(m_nLoad_CurrLot);		// lot별 data를 file 저장하는 기능은 봉인함. 리스크 크고 시간 많이 잡아먹을 수.
	
}

// "D:Log4w" 디렉토리를 조사하여 4w Raw Data를 Load한다.
#define		MAX_LOT_ERR_COUNT		3
#define		MAX_DATE_ERR_COUNT		3
void CStatisticsDialog::Load_Log4wDir(LPCTSTR pstr)
{

	CFileFind 	finder;
	CString		strTemp;
	int nLotErrCnt = 0, nDateErrCnt = 0;

	// build a string with wildcards
	CString 	strWildcard(pstr);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);


	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;


		// if it's a directory, recursively search it
		if (finder.IsDirectory())
		{
			CString lotName;
			CString	dirName = finder.GetFileName();
			CString dirPath = finder.GetFilePath();
			int	length = strlen(dirName);


			// "20170925" 처럼 날짜로 시작하지 않는다면 다음 디렉토리로
			CString dateName  = dirName.Left(8);		// 날짜 str 보관.
			CString dateExclu = dateName.SpanExcluding("0123456789"); 
			if (!dateExclu.IsEmpty())	// "0123456789"에 해당하지 않는 문자만 선택. Empty가 아니면 글자포함이란 얘기
				continue;


			// "20170925_A30-19-C-4W-NEW-0920" 라면 맨앞의 날짜는 떼고 tree item으로 삽입한다.
#if 0
			
			// 맨 뒤의 -0920은 떼고 트리 item으로 삽입한다.
			// 맨 뒤의 -0920을 떼기 위해 strRight가 '-', '_'으로 시작하는지 체크.
			CString strRight = dirName.Right(5);	
			if (strRight.GetAt(0) == '-' || strRight.GetAt(0) == '_')	
			{
				strRight = strRight.Right(4); 	// '-' 또는 '_'를 떼어낸다.
				CString strExclu = strRight.SpanExcluding("0123456789");	

				// 맨 뒤 4글자가 모두 0920같은 숫자라면 맨앞의 날짜 9글자와 맨뒤의 5글자(-0920, 또는 _0920)는 빼고 lot item str으로 사용
				if (strExclu.IsEmpty())		
					lotName = dirName.Mid(9, (length -9 -5));
				else
					lotName = dirName.Mid(9, (length -9));
			}

			// strRight가 -로 시작이 아니면 strRight까지 포함해서 lot item str으로 사용.
			else 
				lotName = dirName.Mid(9, (length -9));
#else
			// 2018.06.22 :  ACE400_2222,  ACE400_3333 이런 것들을 같은 lot으로 처리하다보니 
			//               step count가 달라서 ACE400_3333은 처리를 하지 않고 건너뛰고 그러다 로직이 꼬여서 죽음.
			//               맨뒤의 숫자 네개 떼어내는 기능을 위와 같이 막아서  다른 lot으로 처리하기로 함.
			lotName = dirName.Mid(9, (length -9));
#endif
	
			// 빈 lot이 추가되는 것을 막기 위해 Insert할 디렉토리가 빈 디렉토리인지 검사한다.  (시간, 자원절약)
			if(CheckEmptyDirectory(dirPath) == TRUE)		
			{
				strTemp.Format("%s is Empty Directory. Can't do InsertItem. \n : Load_Log4wDir()\n\n", dirPath);
				ERR.Set(USER_ERR, strTemp); 
				ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
				continue;
			}

			// Lot 정보, 날짜 정보를 저장.
			//CString lotName = dirName;
			int lotId, dateId;
			lotId  = g_sLotNetDate_Info.getLotId(lotName);
			if (lotId == -1)	// lot range를 넘어선다면 load하지 않는다.
			{
				strTemp.Format("Load_Log4wDir(): %s getLotId(%s) Failed! nLotCnt=%d Range(<%d) Over!\n\n Can't do InsertItem.", 
							dirName, lotName, g_sLotNetDate_Info.nLotCnt, MAX_LOT);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

		// 2018.10.18  Lot을 5개 이상 로드하면 에러 출력은 좋은데, 기존 lot을 더 채우려고 아래와 같이 시도하는게 
		//      뭔가 에러가 있는 인상을 줄 수 있다. 큰 이익이 있는 것인지 잘 판단이 안됨. 일단 max 3회만 하도록 수정함.
				nLotErrCnt++;
				if (nLotErrCnt < MAX_LOT_ERR_COUNT) 
					continue;	// 다음번 lot이 기존 lot과 이름이 같을 수도 있으므로 break하지 않고 continue 처리
				break;			// Lot Error가 3회 이상 반복되면 그냥 break. 
			}

			dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
			if (dateId == -1)	// date range를 넘어선다면 load하지 않는다. 
			{
				strTemp.Format("Load_Log4wDir(): getLotDateId(%s_%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
						dateName, lotName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

				nDateErrCnt++;
				if (nDateErrCnt < MAX_DATE_ERR_COUNT) 
					continue;	// 다음번 lot이 있을 수 있으므로 break는 하지 않고 continue 처리
				break;			// Date Error가 3회 이상 반복되면 그냥 break
			}

			// Tree에서 해당 'Lot' item을  찾거나, 새로 insert 한다.
			HTREEITEM	hLotItem;
			BOOL		bLotInserted = FALSE;
			hLotItem = Tree_GetLotItem(lotName, dirPath, lotId, dateId, bLotInserted);			
												// bInserted: 호출후에 Lot이 insert 됐는지 여부 확인용도
												
			if (hLotItem == NULL)	// logic 오류 케이스로 이미 에러메시지 박스  출력했으므로 contiune처리만 한다.
				continue;

			// Lot이 아니면 pass
			int nLevel = Tree_GetCurrentLevel(hLotItem);
			if (nLevel != 2)	
				continue;

			// Lot 디렉토리라면 하위의 Net Data File을 Open해서 Load 
			// 신규 Node, 기존재하는 Node 모두 하위 디렉토리의 Net Data를 Load를 수행한다.
			// 만약 time file이 하나도 로딩이 안 되었다면 다음을 수행하고 continue
			if (Tree_LoadLot_4WFiles(dirPath, hLotItem, lotId, dateId) < 0) 
			{
				strTemp.Format("%s is Empty Directory(Tree_LoadLot_4WFiles()< 0). Delete Inserted Lot & Date.\n : OnCheckSimulUlsl()\n", dirPath);
				ERR.Set(USER_ERR, strTemp); 
				ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
				
				// LOT이 tree에 추가 됐는데,  data없이 헤더만 있는 파일이라던지해서 로딩에 실패했다면
				if (bLotInserted == TRUE)
				{
					m_treeCtrl.DeleteItem(hLotItem); 		// Lot tree item 삭제, 
					g_sLotNetDate_Info.putBackLotId(lotId); // 추가된 lot str, 추가된 date str 모두 삭제
				}

				// 기존 Lot이라면  
				else
				{	
					// 추가된 date str만 삭제 
					g_sLotNetDate_Info.strLotDate[lotId][dateId] = "";
					g_sLotNetDate_Info.naLotDateCnt[lotId]--;
				}
				
				continue;
			}
//__PrintMemSize(FUNC(Load_Log4wDir_D:log4w), __LINE__);



			// 현재 디렉토리 밑에 sub 디렉토리가 있다면 Load_Log4wDir()를 재귀적으로 또 수행한다. 
			// 이 코드는 수행해서는 안됨. 3 Level 이상 더 내려갈 일이 없음.
			//Load_Log4wDir(dirPath, hItem);		// Recursive call
		}

		// 2018.06.18 Tree_LoadLot_4WFiles()에서 nNetDataCount >= MAX_NET_DATA 여서 빠져나온 경우 ,
		//            더 이상 다른 Lot도 찾지 않도록 아래 코드 추가함.
		if (m_nNetDataCount >= MAX_NET_DATA)
			break;
	}

	finder.Close();

	
}



// 빈 디렉토리인지 검사한다.   (TRUE: Empty)
BOOL CStatisticsDialog::CheckEmptyDirectory(CString dirPath)
{
	int 		nFile = 0;
	CFileFind 	finder_sub;

	CString 	strWildcard(dirPath);
	strWildcard += _T("\\*.*"); 					// build a string with wildcard
	BOOL bWorking = finder_sub.FindFile(strWildcard); // start working for files
	while (bWorking)
	{
		bWorking = finder_sub.FindNextFile();
		if (finder_sub.IsDots())
			continue;

		if (finder_sub.IsDirectory())
			continue;

		// 2018.05.25: file name에 'Sample'이 포함되어 있다면.  
		// Auto sampling을 위해 일시적으로 생성된 파일이므로 분석 data에서 제외한다.  
		// 2018.06.22: 빈 디렉토리를 lot 처리 하지 않도록 Load_4wDataFile()뿐 아니라 
		// 			   CheckEmptyDirectory()에서도 체크하도록 수정. 
		CString	dataFileName = finder_sub.GetFileName();
		CString dataFilePath = finder_sub.GetFilePath();
		if (dataFileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		nFile++;

		// Sample이 아닌 정상 4W Data file이 하나라도 있으면 FALSE 리턴
		return FALSE;
	}
	if (nFile == 0)
		return TRUE;

	return FALSE;
}


// Tree에서 해당 'Lot' item을  찾거나, 새로 insert 한다.
HTREEITEM CStatisticsDialog::Tree_GetLotItem(LPCTSTR pStrInsert, LPCTSTR pStrPath, int nLot, int nDate, int& rbInserted)
{
	// TODO: Add your control notification handler code here
	HTREEITEM 	hItemFind= NULL, hLotItem = NULL;
	CString		strTemp;

	HTREEITEM   hItemGet = NULL;	// return 값

	// Insert할  텍스트가 비어있나 검사한다.
	if(pStrInsert == "")
	{
		strTemp.Format("Tree_GetLotItem(): pStrInsert is NULL, Can't do InsertItem.");
		ERR.Set(SW_LOGIC_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return NULL;

	}



	// 추가해야할 strInsert(Lot Name)가 현재 branch에 존재하는지 검사한다.
	// 새로운 Lot Name이라면 지정한 정보의 Lot 노드를 추가한다.
	//if ((hItemFind = Tree_FindStrItem(&m_treeCtrl, m_hRoot, pStrInsert)) == NULL)	// 새로운 Node(Lot) 
	
	// 기존 Lot이면 기존의 Lot의 tree Item을 획득
	hItemFind = Tree_FindLotItem(&m_treeCtrl, m_hRoot, pStrInsert);
	if (hItemFind != NULL) 				// Found: 기존에 존재하는 노드(Lot)
	{
		// 같은 이름의 노드(Lot)가 여러번 추가되지 않도록 return 값을 NULL로 설정한다.
		hItemGet = NULL;		
		hLotItem = hItemFind;	// Found이므로 찾아낸 hItemFind을 hLotItem으로 리턴하기 위해 할당.

		MyTrace(PRT_BASIC, "%s Lot already exist! use existing one.\n\n", pStrInsert);

		rbInserted = FALSE;
	}

	// 새로운 Lot이면 tree에 Lot item을 추가 
	else	// Not Found:
	{
		// Tree InsertItem을 그대로 이용하여 root에 새로운 Lot Node를 추가
		hItemGet = m_treeCtrl.InsertItem(pStrInsert, m_hRoot); 
		m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
		hLotItem = hItemGet;	// 새로 생성한 hItemGet을 hLotItem으로 리턴하기위해 할당.

		MyTrace(PRT_BASIC, _T("%s Lot: Inserted to Tree.\n"), (LPCTSTR)pStrInsert);
		// 이 지점에서 Insert된 dir의 dirName, dirPath, hLotItem handle 값을 저장해야 할 수 있다.
		// 추후 진짜 필요하다면 저장하자. 

		rbInserted = TRUE;
	}

	
	return hLotItem;
}

// 입력받은 Tree Item의 Child 노드들 중에서 해당 str에 맡는 item을 찾아 리턴한다.
HTREEITEM CStatisticsDialog::Tree_FindLotItem(CTreeCtrl* pTree, HTREEITEM hParent, LPCTSTR pStr)
{
	HTREEITEM hItem;
	hItem = pTree->GetChildItem( hParent );	

	return  Tree_FindLotItem2(pTree, hItem, pStr);		// recursive 함수 호출
}

// Recursive 함수: 입력받은 Tree Item의 같은 레벨 노드들 중에서 해당 str에 맡는 item을 찾아 리턴한다.
// Child의 Child를 찾도록 recursive 호출하지 않는다. sibling은 recursive 호출함.
HTREEITEM CStatisticsDialog::Tree_FindLotItem2(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr)
{
	HTREEITEM hItemFind, hItemChild, hItemSibling;
	hItemFind = hItemChild = hItemSibling = NULL;
	CString		hItemName = "";


	hItemName = pTree->GetItemText(hItem);

	// 입력받은 str이 hItem 자신의 str이면 Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		//MyTrace(PRT_BASIC, "Tree_FindLotItem2(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// 못 찾았으면 
	else 
	{
		// Lot 레벨에서만  찾아야 하므로, Child까지 내려갈 필요 없다.

		// 다음 형제노드를 찾는다. (형제노드 recursive 호출)
		hItemSibling = pTree->GetNextSiblingItem( hItem );
		if ( hItemFind==NULL && hItemSibling )
		{
			//CString		nodeName = "";
			//nodeName = m_treeCtrl.GetItemText(hItemSibling);
			//MyTrace(PRT_BASIC, "Tree_FindLotItem2(hItem=%s, pStr=%s) => recursive Tree_FindLotItem2(%s,%s) for Sibling \n",  
			//		hItemName, pStr, nodeName, pStr);	// test prt
			hItemFind = Tree_FindLotItem2( pTree, hItemSibling, pStr );	// Recursive call
		}
	}

	return hItemFind;
}


// hItem이하 모든 노드에서 pStr를 찾는다.
// Net의 경우는 갯수가 너무 많아서 이 함수를 사용하면 시간이 너무 많이 걸리는 문제 발생.
// Net은 별도의 함수 g_sLotNetDate_Info.findLotNetId() 로 구현.
//
// 2018.05.28 이 함수를 여러번 호출시에 stack over flow로 죽는 현상 있었음.  
// 불필요하게  Net 하위에서 Lot의 str를 검색하는 부분을 삭제하기 위해 
// 이 함수는 사용하지 않고  Tree_FindLotStrItem()을 사용하기로 한다.
//
// 2018.09.19 Lot_0, Net_1의 hItem 위치를 찾기위해 다시 부활시킴. Lot을 찾는데 말고 
//            Net을 찾는데에만 사용한다.
HTREEITEM CStatisticsDialog::Tree_FindStrItem(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr)
{
	HTREEITEM hItemFind, hItemChild, hItemSibling;
	hItemFind = hItemChild = hItemSibling = NULL;
	CString		hItemName = "";
	CString		nodeName = "";


	hItemName = pTree->GetItemText(hItem);

	// 입력받은 str이 hItem 자신의 str이면 Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		//MyTrace(PRT_BASIC, "Tree_FindStrItem(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// 못 찾았으면 
	else 
	{
		// 자식 노드를 찾는다.
		hItemChild = pTree->GetChildItem( hItem );
		if ( hItemChild )
		{
			nodeName = m_treeCtrl.GetItemText(hItemChild);
			//MyTrace(PRT_BASIC, "Tree_FindStrItem(hItem=%s, pStr=%s) => recursive Tree_FindStrItem(%s,%s) for Child\n",  hItemName, pStr, nodeName, pStr);	// test prt
			hItemFind = Tree_FindStrItem( pTree, hItemChild, pStr );	// Recursive call

		}

		// 형제노드를 찾는다.
		hItemSibling = pTree->GetNextSiblingItem( hItem );
		if ( hItemFind==NULL && hItemSibling )
		{
			nodeName = m_treeCtrl.GetItemText(hItemSibling);
			//MyTrace(PRT_BASIC, "Tree_FindStrItem(hItem=%s, pStr=%s) => recursive Tree_FindStrItem(%s,%s) for Sibling\n",  hItemName, pStr, nodeName, pStr);	// test prt
			hItemFind = Tree_FindStrItem( pTree, hItemSibling, pStr );	// Recursive call
		}
	}

	return hItemFind;
}



/*
// hItem이하 모든 모든 노드에서 dwData를 찾는다.
HTREEITEM CStatisticsDialog::Tree_FindData(CTreeCtrl* pTree, HTREEITEM hItem, DWORD dwData)
{
	HTREEITEM hitemFind, hItemChild, hItemSibling;
	hitemFind = hItemChild = hItemSibling = NULL;


	if ( pTree->GetItemData( hItem ) == dwData )
	{
		hitemFind = hItem;
	}
	else 
	{
		// 자식 노드를 찾는다.
		hItemChild = pTree->GetChildItem( hItem );
		if ( hItemChild )
		{
			hitemFind = Tree_FindData( pTree, hItemChild, dwData );

		}

		// 형제노드를 찾는다.
		hItemSibling = pTree->GetNextSiblingItem( hItem );
		if ( hitemFind==NULL && hItemSibling )
		{
			hitemFind = Tree_FindData( pTree, hItemSibling, dwData );
		}
	}

	return hitemFind;
}
*/

// 주어진 hItem 의 tree depth를 리턴한다.
int CStatisticsDialog::Tree_GetCurrentLevel(HTREEITEM hItem)
{
	int nLevel = 1; 	// Root를 1 level로 한다.

	HTREEITEM	hNode = NULL;
	hNode = m_treeCtrl.GetParentItem(hItem);

	// hItem이 존재하지 않으면 root이므로 (nLevel == 1) 이 리턴된다.
	if (hNode)
	{
		while (hNode != NULL)
		{
			nLevel++;
			hNode = m_treeCtrl.GetParentItem(hNode);
		}
	}

	return nLevel;
}

// Lot 디렉토리 하위의 모든 Net Data File을 Open해서 memory에 Load한다.
int CStatisticsDialog::Tree_LoadLot_4WFiles(LPCTSTR pParentPath, HTREEITEM hParentLot, int nLot, int nDate)
{
	CFileFind 	finder;

	MyTrace(PRT_BASIC, "%s nLot=%d, nDate=%d: Tree_LoadLot_4WFiles... \n\n", pParentPath, nLot, nDate);

	// build a string with wildcard
	CString 	strWildcard(pParentPath);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	int nFile = 0;
	while (bWorking)
	{
		if (nFile >= MAX_TIME_FILE)
		{
			if (nFile == MAX_TIME_FILE)	
			{
				CString strTemp;
				strTemp.Format("nFile=%d(maximum), Can not load the file anymore. nLot=%d(%s), nDate=%d(%s)\n",
						nFile, nLot, g_sLotNetDate_Info.strLot[nLot], nDate, g_sLotNetDate_Info.strLotDate[nLot][nDate]);
				AfxMessageBox(strTemp, MB_ICONINFORMATION);
				MyTrace(PRT_BASIC, strTemp);
			}
			break;		
		}

		// 2018.06.18 Tree_LoadLot_4WFiles()에서 nNetDataCount >= MAX_NET_DATA 여서 빠져나온 경우 ,
		//            더 이상 다른 Time File도 찾지 않도록 아래 코드 추가함.
		if (m_nNetDataCount >= MAX_NET_DATA)
		{
			if (m_nNetDataCount == MAX_NET_DATA)
				MyTrace(PRT_BASIC, "m_nNetDataCount=%d, Tree_LoadLot_4WFiles() Stopped!! nFile=%d\n\n", m_nNetDataCount , nFile);
			break;
		}

		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
			continue;
		
		CString	dataFileName = finder.GetFileName();
		CString dataFilePath = finder.GetFilePath();
		if (Load_4wDataFile(dataFilePath, dataFileName, hParentLot, nLot, nDate) < 0) 
			continue;

		nFile++;
	
	}

	finder.Close();

	if (nFile == 0)
		return -1;

	return 0;

}

#define 	MAX_NETFILE_COL		20
// 1. 4W Data File을 Open하고 Net별 Data를 read하여 메모리 구조체에 Load
// 2. 새로운 Net 번호인지 확인하고 새로운 Net 번호라면, hParentLot Tree에 Net Item을 Insert 
int CStatisticsDialog::Load_4wDataFile(CString dataFilePath, CString dataFileName, HTREEITEM hParentLot, int nLot, int nDate)
{

	FILE 	*fp; 
	char  	fName[200]; 		
	char	str[1024];			// fgets()로 라인째 읽어 오기 위한 buffer

	// 2018.05.25: file name에 'Sample'이 포함되어 있다면.  
	// Auto sampling을 위해 일시적으로 생성된 파일이므로 분석 data에서 제외한다.  
	if (dataFileName.Find("Sample") != -1)
	{
		MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
		return -1;
	}


	::ZeroMemory(&fName, sizeof(fName));
	strcat(fName, dataFilePath);

	if(!FileExists(fName)) 
	{ 	ERR.Set(FLAG_FILE_NOT_FOUND, fName); 
		ErrMsg();  ERR.Reset(); return -1; }

	fp=fopen(fName,"rt");
	if(fp == NULL)
	{ 	ERR.Set(FLAG_FILE_CANNOT_OPEN, fName); 
		ErrMsg();  ERR.Reset(); return -1; }



	CString strTemp, strTime;
	int 	nNet, i, nNet2;
	char	strWord[300];
	char 	strWord2[3][200];
	char 	strStep[NUM_STAT_STEP*2][200];
	int		nFileStep = 0;

	//------------------------------
	//  4W Raw Data 파일 헤더 Read 
	::ZeroMemory(str, sizeof(str));
	::ZeroMemory(&strStep, sizeof(strStep));
	fgets(str, sizeof(str), fp);		// 파일 헤더 read.  파일 헤더는 라인째 통째로 읽는다.

	// 헤더에서 data 의 갯수를 파악하기 위해 "S=" 의 갯수를 카운트한다.
	sscanf(str, "    ,  Pin1 ,Pin2 ,Pin3 , Pin4 ,  Min., Ref., Max.,%s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   ", 
			strStep[0], strStep[1], strStep[2], strStep[3], strStep[4], strStep[5],
			strStep[6], strStep[7], strStep[8], strStep[9], strStep[10], strStep[11]);
	for (i = 0; i < NUM_STAT_STEP; i++)
	{
		strTemp = strStep[i];
		if (strTemp.Left(2) == "S=")		
			nFileStep++;
	}
	if (nFileStep != g_sLotNetDate_Info.naLotStepCnt[nLot])
	{
		if (g_sLotNetDate_Info.naLotStepCnt[nLot] == 0)	// 초기값이면 nFileStep 값으로  바꾼다.
			g_sLotNetDate_Info.naLotStepCnt[nLot] = nFileStep;			

		// 같은 nLot 안에서는 nFileStep의 값이 모두 같아야 한다.
		else
		{
			strTemp.Format("Load_4wDataFile(): nFileStep=%d is not same with g_sLotNetDate_Info.naLotStepCnt[%d]=%d!", 
					nFileStep, nLot, g_sLotNetDate_Info.naLotStepCnt[nLot]);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 
			return -1;
		}

	}


	statNetData netData;		// 한번 정의해서 최대 5000번까지 재활용.
								// Stat_insertNetData()에서 실제 data로 copy가 일어난다.

	// data file name에서 time 획득.
	strTime = dataFileName.Mid(15, 6);
	netData.statTime.hour = atoi(strTime.Left(2));		// 맨앞의 2글자
	netData.statTime.min  = atoi(strTime.Mid(2, 2));	// 3번째부터 2글자 
	netData.statTime.sec  = atoi(strTime.Mid(4, 2));	// 5번째부터 2글자. 


	// Net 갯수 만큼 Line을 읽는 동작을 반복
	int 	row=0;
	while(!feof(fp))
	{
		if (row > MAX_NET_PER_LOT)
		{
			strTemp.Format("Load_4wDataFile(): %s row=%d Range(row<%d) Over Error!\n", 
					dataFilePath, row, MAX_NET_PER_LOT);
			ERR.Set(RANGE_OVER, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 
			break;
		}
		// 2018.10.17 Tree_LoadLot_4WFiles()에서 nNetDataCount >= MAX_NET_DATA 여서 빠져나온 경우 ,
		//            더 이상 다음 Net도 찾지 않도록 아래 코드 추가함.
		if (m_nNetDataCount >= MAX_NET_DATA)
			break;

		::ZeroMemory(str, sizeof(str));
		fgets(str, sizeof(str), fp);	// 한 라인을 통째로 읽어온 다음 세부처리는 sscanf에 맡긴다. 
										// 대량의 데이터를 fscanf로 읽으니 메모리 overflow문제 생김.
										// sscanf() buffer도 CString을 사용하면 대량으로 read 반복하는 과정에서 
										// CString 버퍼가 overflow가 나서  char[]로 대체함.
		
		//if (row >= 454)
		//	MyTrace(PRT_BASIC, "aaa%saaa\n", str);		// str 입력값 확인용 테스트

		// fgets로 read한 str이 빈 str 또는 '\n' 만 있는 경우 sscanf 수행하지 않고 skip.
		//if (strcmp(str,"") == 0 || strcmp(str," ") == 0 || strcmp(str,"   \n") == 0)		
		//{
		//	row++;
		//	continue;
		//}		==> 이 체크코드 추가후 시간이 너무 오래 걸림. 
		//          하기 코드에 nNet, strWord를 초기화하는 방식으로 해결하기로 바꿈.
										
		// "Net"으로 시작하는 라인이 아니면 처리하지 않고 line skip.
		nNet = -1;
		::ZeroMemory(strWord, sizeof(strWord));
		sscanf(str, "%s %d", strWord, &nNet); 	// Read "Net" Str, nNet #
		if (strcmp(strWord,"Net") != 0)		
		{
			row++;
			continue;
		}

		// nNet 이 range over인 경우 line skip
		if (nNet < 0 || nNet >= MAX_NET_PER_LOT)	 
		{
			row++;
			continue;
		}


		// loop를 돌 때 마다 아래 버퍼를 초기화한다. 
		::ZeroMemory(netData.daStep, sizeof(netData.daStep));
		::ZeroMemory(&strStep, sizeof(strStep));
		sscanf(str, "%s %d, %d, %d, %d, %d, %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
				strWord, &nNet2, 
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][0],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][1],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][2],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][3],
				&strWord2[0],  
				&strWord2[1],  
				&strWord2[2], 
				&strStep[0], &strStep[1], &strStep[2],   &strStep[3], 
				&strStep[4], &strStep[5], &strStep[6],   &strStep[7], 
				&strStep[8], &strStep[9], &strStep[10],  &strStep[11],
				&strStep[12], &strStep[13], &strStep[14],   &strStep[15], 
				&strStep[16], &strStep[17], &strStep[18],   &strStep[19], 
				&strStep[20], &strStep[21], &strStep[22],  &strStep[23] 
				);

		//double min, ref, max;
		netData.dLsl  = (double)atof(strWord2[0]);	// min-> LSL(Lower Spec Limit)으로 사용.
		netData.dRefR = (double)atof(strWord2[1]);		
		netData.dUsl  = (double)atof(strWord2[2]);	// max-> USL(Upper Spec Limit)으로 사용.

		// 2018.05.31 Lsl, Usl이 ACE400에서 설정한 값인 만큼, Data Read시의 time tuple 마다 다른 Lsl, Usl 값으로 
		// 			  fault를 체크하는게 적정한 것으로 판단됨.  (ACE400에서 설정을 바꿨다면 중간에 달라질수 있으므로)
		//            추후 Net의 tuple data는 사용하지 않고 summary data만 listup하는 기능이 추가된다면
		//            Net의 tuple 공통 Usl, Lsl도 추후 필요할 수 있음. 
		//            Net의 tuple 공통으로 Usl, Lsl 사용하는 것은  날리지 않고 않고 다음과 같이 막음.
		//g_sLotNetDate_Info.daLotNetLsl[nLot][nNet] = (double)atof(strWord2[0]);	// min-> LSL(Lower Spec Limit)으로 사용.
		//g_sLotNetDate_Info.daLotNetUsl[nLot][nNet] = (double)atof(strWord2[2]);	// max-> USL(Upper Spec Limit)으로 사용.

		// Read Measure Value --------
		int	   nStep = 0;
		for (i = 0; i < nFileStep; i++)
		{
			// NG 값 체크
			if (strcmp("NG", strStep[nStep]) == 0)
			{
				nStep++;			// NG는 skip 한다.

				// NG 999999.00 인 경우에는 진짜 NG 처리
				if (strcmp("999999.00,", strStep[nStep]) == 0)
				{

					netData.daStep[i] = (double)(-1);
					nStep++;		// 999999.00 은 -1로 처리
				}

				// NG 64.28 과 같이  숫자가 뒤에 오는 경우
				// NG가 이전 값에 대한 에러 표시 이므로 숫자를  정상처리한다.
				else
				{
					netData.daStep[i] = (double)atof(strStep[nStep]);
					nStep++;		
				}
			}

			// NG 없이 999999.00 만 있는 경우  NG 처리
			else if (strcmp("999999.00,", strStep[nStep]) == 0)
			{
				netData.daStep[i] = (double)(-1);		
				nStep++;			// 999999.00 은 -1로 처리
			}

			else
			{
				// daStep 값 
				netData.daStep[i] = (double)atof(strStep[nStep]);
				nStep++;

			}
		}


		//----------------------
		// Tree에 Net item 추가
		
		// 획득한 Net 번호를 Tree의 해당 Lot 위치에 추가한다. 
		// 1. 추가해야할 Net의 string이 현재 branch에 존재하는지 검사한다.
		// 2. 새로운 노드라면 지정한 정보로 노드를 추가한다.
		CString strInsert;
		strInsert.Format("Net_%d", nNet);
		if ((g_sLotNetDate_Info.findLotNetId(nLot, nNet)) == -1)
		{
			// Tree InsertItem을 그대로 이용하여 새로운 Node를 추가
			m_treeCtrl.InsertItem(strInsert, hParentLot); 
			//m_treeCtrl.Expand(hParentLot, TVE_EXPAND);
			g_sLotNetDate_Info.setLotNetId(nLot, nNet);		// 해당 Lot, Net이 존재함을 표시

		}
		strInsert.Empty();

		//---------------------------------
		// Read한 netData를 메모리에 추가
		Stat_insertNetData(nLot, nNet, nDate, netData);		

		row++;

	}
	fclose(fp);

	//TRACE ("g_pvsaNetData[nLot=%d][nNet=%d][nDate=%d]->size() = %d\n", 
	//					nLot, nNet, nDate, g_pvsaNetData[nLot][nNet][nDate]->size());



	return 0;
}



// Stat Data를 메모리에 추가 한다. 
int CStatisticsDialog::Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData)
{
	// 2018.06.18: m_nNetDataCount가  MAX_NET_DATA 갯수를 넘어서면 4W Data 로딩을 중지하는 기능추가
	if (m_nNetDataCount >= MAX_NET_DATA)
		return -1;

	// g_pvsaNetData[nLot][nNet][nDate]가 없다면 새로 new해서 생성한다. 이미 존재한다면 그냥 0리턴, 문제있다면 -1리턴
	int ret = Stat_insertNetVector(nLot, nNet, nDate);
	if (ret < 0)
		return -1;

	// 2K를 넘는다면 vector 공간을 더 확보한다.  
	// 2018.06.19: 2048이 되는 시점에 1회성으로 수행하면,  
	//             vector resize 도중에 "메모리 부족" 메시지 나오는 현상을 막을 수 있다.
	if ( g_pvsaNetData[nLot][nNet][nDate]->size() == 2048)		
	{
		/*  Test Print
		MyTrace(PRT_BASIC, "g_pvsaNetData[%d][%d][%d]->size()=%d, capacity()=%d, m_nNetDataCount=%d\n",
					nLot, nNet, nDate,
					g_pvsaNetData[nLot][nNet][nDate]->size(),
					g_pvsaNetData[nLot][nNet][nDate]->capacity(), m_nNetDataCount);

		MyTrace(PRT_BASIC, "g_pvsaNetData[%d][%d][%d]->size() = 2048! reserve other space for %d files..  \n", 
				nLot, nNet, nDate, MAX_TIME_FILE);
		*/
		g_pvsaNetData[nLot][nNet][nDate]->reserve(MAX_TIME_FILE);
	}

	// 해당 vector에 실제 netData를 insert한다.
	//   :  time 갯수를 가질 vector에 이번 time의 netData를 push_back 한다. 
	//   netData를 value형 인자로 넘기는 과정에서 copy해야 하므로 statNetData 클래스의 
	//   copy constructor(복사생성자) 호출이 일어난다.  포인터를 넘기는 경우라면, 복사 생성자안에서
	//   포인터에 대해 새로 new를 해서 공간을 만들어 줘야 나중에 delete[]할때에 문제가 안 생김. (deep copy..)
	g_pvsaNetData[nLot][nNet][nDate]->push_back(netData);


	// 2018.06.18: m_nNetDataCount가  MAX_NET_DATA 갯수를 넘어서면 4W Data 로딩을 중지하는 기능추가
	m_nNetDataCount++;
	if (m_nNetDataCount == MAX_NET_DATA)
	{
		CString strTemp;
		strTemp.Format("m_nNetDataCount=%d, Stop Stat_insertNetData()!!\nCurrent nLot=%d(%s), nNet=%d, nDate=%d(%s)\n", 
				m_nNetDataCount , 
				nLot, g_sLotNetDate_Info.strLot[nLot], nNet, nDate,  g_sLotNetDate_Info.strLotDate[nLot][nDate]);
		AfxMessageBox(strTemp, MB_ICONINFORMATION);
		MyTrace(PRT_BASIC, strTemp);
	}

	return 0;
}

// g_pvsaNetData[nLot][nNet][nDate]가 없다면 새로 new해서 생성한다. 이미 존재한다면 그냥 0리턴, 문제있다면 -1리턴
int CStatisticsDialog::Stat_insertNetVector(int nLot, int nNet, int nDate)
{
	CString strTemp;
	if ((nLot < 0 || nLot >= MAX_LOT) 
			|| (nNet < 0 || nNet >= MAX_NET_PER_LOT) 
			|| (nDate < 0 || nDate >= MAX_DATE))
	{ 	
		strTemp.Format("Stat_insertNetVector(): nLot=%d, nNet=%d, nDate=%d  Range(0<=Lot<%d, 0<=Net<%d, 0<=Date<%d) Over Error!\n",
				nLot, nNet, nDate, MAX_LOT, MAX_NET_PER_LOT, MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return -1; 
	}
	
	// 해당 Net에 data가 insert된 적이 없다면  vector를 새로 생성해서 assign한다.
	//  : Lot * Net * Date 갯수의 vector 포인터에 실제 vector를 assign
	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		vector <statNetData>*  pvNetData = new vector <statNetData>;
		if (pvNetData == NULL)		// 메모리할당 실패!
		{
			strTemp.Format("Stat_insertNetVector() Fail!: nLot=%d, nNet=%d, nDate=%d\n", nLot, nNet, nDate);
			ERR.Set(MEM_ALLOC_FAIL, strTemp); 
			//ErrMsg(-1, TRUE);  ERR.Reset(); 
			ErrMsg();  ERR.Reset(); 
			return -1;
		}
#if 0
		// test print
		else
			MyTrace(PRT_BASIC, "Stat_insertNetVector(): nLot=%d, nNet=%d, nDate=%d\n", nLot, nNet, nDate);
#endif

		g_pvsaNetData[nLot][nNet][nDate] = pvNetData;
	}

	return 0;
}

// 모든 Stat data를 삭제하고 할당된 memory를 delete한다.
void CStatisticsDialog::Stat_deleteAllNetData()
{
	int lot, net, date;

	
	for (lot =0; lot < MAX_LOT; lot++)
	{
		for (net =0; net < MAX_NET_PER_LOT; net++)
		{
			for (date =0; date < MAX_DATE; date++)
			{
				if(g_pvsaNetData[lot][net][date] != NULL)
				{
					//int prevTupleSize = g_pvsaNetData[lot][net][date]->size();	// date마다 tupleSize가 다름!

					// vector의 모든 data를비우고 (모든 tuple을 삭제하고)
					if (g_pvsaNetData[lot][net][date]->empty() == false)
						g_pvsaNetData[lot][net][date]->clear();		


					delete g_pvsaNetData[lot][net][date];		// 동적할당된 vcetor를 반환한다.
					g_pvsaNetData[lot][net][date] = NULL;		// 반환한 vector 포인터도 NULL 처리
				}
			}
		}
	}

}

// Tree를 scan하면서 모든 Lot, 모든 Net의 Fault 표시를 지운다.
void CStatisticsDialog::Tree_CheckAll_LotNetFaults()
{
	HTREEITEM 	hItemChild, hItemNextChild;
	CString		lotName = "";
	int			nLot = 0;

	// 루트의  자식 Lot 노드를  찾는다.
	hItemChild = m_treeCtrl.GetChildItem(m_hRoot);
	if ( hItemChild )
	{
		lotName = m_treeCtrl.GetItemText(hItemChild);
		nLot = g_sLotNetDate_Info.findLotId(lotName);
		if (nLot >= 0)
		{
			Tree_Check_LotNetFaults(hItemChild, nLot);
			MyTrace(PRT_BASIC, "Tree_Check_LotNetFaults(): lotName=%s, nLot=%d\n", lotName, nLot);
		}
	}

	while (hItemChild)
	{
		// 여기서는 Lot의 Child 체크하지 않고  Tree_Check_LotNetFaults(hItemChild, nLot) 에 맡긴다.
		// 자식의 형제, 즉 다음번 Lot 노드를 찾는다.
		hItemNextChild = m_treeCtrl.GetNextSiblingItem(hItemChild);
		if (hItemNextChild)
		{
			lotName = m_treeCtrl.GetItemText(hItemNextChild);
			nLot = g_sLotNetDate_Info.findLotId(lotName);
			if (nLot >= 0)
			{
				Tree_Check_LotNetFaults(hItemNextChild, nLot);
				MyTrace(PRT_BASIC, "Tree_Check_LotNetFaults(): lotName=%s, nLot=%d\n", lotName, nLot);
			}
		}
		hItemChild = hItemNextChild;
	}
}
/*
// 해당 Lot에 속한  모든 Net의 Fault 표시를 지운다.
void CStatisticsDialog::Tree_Clear_LotNetFaults(HTREEITEM hItem, int nLot)
{
	HTREEITEM hItemChild, hItemNextChild;
	CString netName = "";

	// 자식 노드를 찾는다.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		netName = m_treeCtrl.GetItemText(hItemChild);
		MyTrace(PRT_BASIC, "Tree_Clear_LotNetFaults(nLot=%d, %s): Clear Icon...\n", nLot, netName);
		m_treeCtrl.SetItemImage(hItemChild, 0, 0); 	// hNetItem 위치를 Normal 처리한다. 
	}

	while (hItemChild)
	{
		// 자식의 형제노드를 찾는다.
		hItemNextChild = m_treeCtrl.GetNextSiblingItem(hItemChild);
		if (hItemNextChild)
		{
			netName = m_treeCtrl.GetItemText(hItemNextChild);
			MyTrace(PRT_BASIC, "Tree_Clear_LotNetFaults(nLot=%d, %s): Clear Icon...\n", nLot, netName);
			m_treeCtrl.SetItemImage(hItemNextChild, 0, 0); 	// hNetItem 위치를 Normal 처리한다. 
		}

		hItemChild = hItemNextChild;
	}

}
*/

// 해당 Lot에 속한  모든 Net의 Fault 여부를 판단한다.  
// Tree에서의 Net Fault는 해당 net에 속하는 모든 date의 fault카운트를 확인해서 
// 결정해야 하므로  data는 ALL로 체크해야 한다.
void CStatisticsDialog::Tree_Check_LotNetFaults(HTREEITEM hItem, int nLot)
{
	HTREEITEM hItemChild, hItemNextChild;
	CString netName = "";

	// Lot의 자식 노드, 즉 net을  찾는다.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		Tree_CheckNetFault(hItemChild, nLot);		// Date = ALL로 체크
		//netName = m_treeCtrl.GetItemText(hItemChild);
		//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
	}

	while (hItemChild)
	{
		// net 레벨에서만 찾아야 하며 Child까지 내려가지 않는다.
		// 자식의 형제노드 (형제 net)를 찾는다.
		hItemNextChild = m_treeCtrl.GetNextSiblingItem(hItemChild);
		if (hItemNextChild)
		{
			Tree_CheckNetFault(hItemNextChild, nLot);	// Date = ALL로 체크
			//netName = m_treeCtrl.GetItemText(hItemNextChild);
			//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
		}
		hItemChild = hItemNextChild;
	}

}

// 해당 Net의 Fault 여부를 판단한다.  (Net의 fault 여부)
// Net Tree의 아이콘은 date 중에 하나라도 fault가 있으면 fault유지해야 함.
// date 하나에 fault가 없다고 fault를 클리어하면 안되고, 다른 모든 date를 체크한다.
void CStatisticsDialog::Tree_CheckNetFault(HTREEITEM hNetItem, int nLot)
{
	CString	itemName, strTemp; 
	int 	nNet;
	
	
	itemName = m_treeCtrl.GetItemText(hNetItem);
	nNet = atoi(itemName.Mid(4));


	//-----------------------
	// Fault 처리 수행
	
	int 	tupleSize, tuple, step, stepSize;
	double 	dLsl, dUsl;

	// 2018.05.31  Net 내부 공통 Lsl, Usl은 막고 tuple별 Lsl, Usl을 사용하기로 함.
	//dLsl   = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];	
	//dUsl   = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];	
	
	// USL, LSL값을 time tuple마다 다른 값으로 처리하려면 Display 단계에서의 fault 체크. 
	// LSL, USL 값을 모든 time tuple에 공통으로 최종 결과만으로 체크하려면 여기에서 미리 체크한다.  
	// 상기 두가지 방법에 따라 fault의 최종 결과가 다를 수 있다.
	//int		nNetFaultCount = 0;
	int date;
	g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetNgCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetTotalCount[nLot][nNet] = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)	
	{
		g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date] = 0;
		g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date] = 0;
	}

	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)	
	{
		if (g_pvsaNetData[nLot][nNet][date] == NULL)
		{
			strTemp.Format("Tree_CheckNetFault(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, date );
			ERR.Set(SW_NO_INIT_VAR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
			continue;
		}



		tupleSize = g_pvsaNetData[nLot][nNet][date]->size();
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			if ( m_bApplyULSL && (m_dSimulLsl != -1 && m_dSimulLsl != -1) ) 
			{
				dLsl = m_dSimulLsl;
				dUsl = m_dSimulUsl;
			}
			else
			{
				// 2018.05.31  tuple별 Lsl, Usl 사용
				dLsl = (*g_pvsaNetData[nLot][nNet][date])[tuple].dLsl;
				dUsl = (*g_pvsaNetData[nLot][nNet][date])[tuple].dUsl;
			}

			stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
			for (step= 0; step < stepSize; step++)
			{
				double dStepVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step];
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step] == -1)	//NG
					g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date]++;
				else
				{
					if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]++;
				}
			}

		}
		//nNetFaultCount += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetNgCount[nLot][nNet] += g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetTotalCount[nLot][nNet] += (tupleSize * stepSize);
	}


	//--------------------------------------
	// Check 결과를 Net  Icon에 반영한다.
	// Net Icon 반영기준이 Date = ALL 이므로, 해당 date의 fault count를 모두 더한 값으로 판단한다.
	
	m_treeCtrl.SetItemImage(hNetItem, 0, 0); 	// hNetItem 위치를 일단 Normal 처리한다. 

	//if (nNetFaultCount > 0)
	if (g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] > 0)
	{
		// Fault가 있는 경우만 Tree의 hNetItem 위치를 Fault(빨강색 장애 아이콘) 처리한다.
		m_treeCtrl.SetItemImage(hNetItem, 
								3, 			// nImage:         tree 선택되지 않았을때의 이미지
								3);			// nSelectedImage: tree 선택되었을 때의 이미지
		// Test Print
		//MyTrace(PRT_LEVEL2, _T("Tree_CheckNetFault(nLot=%d, nNet=%d): dateCount=%d, nNetFaultCount=%d\n"), 
		//			nLot, nNet, g_sLotNetDate_Info.naLotDateCnt[nLot], nNetFaultCount);
		MyTrace(PRT_LEVEL2, _T("Tree_CheckNetFault(nLot=%d, nNet=%d): dateCount=%d, waNetFaultCount=%d, waNetNgCount=%d, TotCount=%d\n"), 
					nLot, nNet, g_sLotNetDate_Info.naLotDateCnt[nLot], 
					g_sLotNetDate_Info.waNetFaultCount[nLot][nNet],
					g_sLotNetDate_Info.waNetNgCount[nLot][nNet],
					g_sLotNetDate_Info.waNetTotalCount[nLot][nNet]);

		// Fault 리스트 박스에 추가  (TBD)

	}

}



// File Open Dialog를 띄워서 특정 Lot_Date 디렉토리를 선택하고 tree에 로딩할 수 있도록 한다.
void CStatisticsDialog::OnButtonLoad4wData_SingleLot() 
{
	// TODO: Add your control notification handler code here
	//
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	

	//-----------------------------
	// File Open Dialog 생성
	
	CString strTemp;
	char szFilter[] = "CSV Files(*.CSV)|*.CSV|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, 		// bOpenFileDialg = TRUE: 타이틀바에 '열기' 출력
					NULL, 		// lpszDefExt           : 기본 확장자
					NULL, 		// lpszFileName         : "파일 이름" Edit 컨트롤에 처음 출력될 파일명.
					OFN_HIDEREADONLY |OFN_NOCHANGEDIR |OFN_ALLOWMULTISELECT, 
								// dwFlags :  속성   (HIDEREADONLY : read only 파일은 출력하지 않음.
					szFilter);	// 대화상자에 출력될 파일을 확장자로 거르기 위한 필터. 


	// 오픈할때 초기 경로 지정. 맨앞에 소문자 d: 여야 제대로 동작함..
	dlg.m_ofn.lpstrInitialDir = _T("d:\\log4w"); 
	
	// 여기서 버퍼 크기 늘려줘야 함.  안 늘려 주면 6개이상 file 지정이 안 됨.
	// default 256byte => file 6개 처리 가능.  각 fileName이 약 43문자 정도라는 뜻임.
	// fileName 사이즈를 넉넉하게 60문자로 주고 4000개를 처리해야 하므로  60*4000 = 240000 
	// 2018.07.17 buffer의 사이즈가 240000 으로 매우 크므로 stack overflow를 유발할 수 있어 static으로 변경함.
	static char buffer[60 * MAX_TIME_FILE] = {0}; 	// 버퍼
	dlg.m_ofn.lpstrFile = buffer; 						// 버퍼 포인터
	dlg.m_ofn.nMaxFile = (60 * MAX_TIME_FILE); 	// 버퍼 크기,  file 3000개 처리하려면 이정도 buffer필요함.


	if (IDOK != dlg.DoModal())
	{
		int err = CommDlgExtendedError();
		MyTrace(PRT_BASIC,"FileDialog Open Fail! err=0x%x(%s)\n\n", err,
				(err == CDERR_DIALOGFAILURE) ? "Invalid Window Handle" :
				(err == CDERR_FINDRESFAILURE) ? "Find Resource Failure" :
				(err == CDERR_INITIALIZATION) ? "Memory not Avalible" :
				(err == CDERR_LOADRESFAILURE) ? "Load Resource Failure" :
				(err == CDERR_LOADSTRFAILURE) ? "Load String Failure" :
				(err == CDERR_LOCKRESFAILURE) ? "Lock Resource Failure" :
				(err == CDERR_MEMALLOCFAILURE) ? "MemAlloc Failure" :
				(err == CDERR_MEMLOCKFAILURE) ? "MemLock Failure" :
				(err == CDERR_NOHINSTANCE) ? "Fail to provide hinstance" :
				(err == CDERR_NOHOOK) ? "Fail to provide hook procedure pointer" :

				(err == CDERR_NOTEMPLATE) ? "Fail to provide template" :
				(err == CDERR_REGISTERMSGFAIL) ? "RegisterWindowMessage err" :
				(err == CDERR_STRUCTSIZE) ? "lStructSize member invalid" : "Undefined Err");
		return;
	}


//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
//

	//-----------------------------
	// LotName, DateName 처리

	// 선택된 CSV파일들의 폴더명(dirPath)을 획득한다. --------------
	

	// '\'위치(nSlashLoc) 뒷부분(fileName)을  떼어내면 폴더명(dirPath)만 남는다.
	//   ex) "D:\log4w\20180503_036782A2S\Ng_Log4WSample0_20180503_010135_NG_Net_-2558.CSV"
	//        nSlashLoc는 27이 리턴됨. 27부터 Left()를 이용해서 왼쪽을 선택하면 폴더Path (dirPath).
	//        dirPath는 "D:\log4w\20180503_036782A2S"   . 결과적으로 오른쪽 fileName만 삭제됨.
	CString dataFilePath, dirPath;
	POSITION pos= dlg.GetStartPosition();
	dataFilePath = dlg.GetNextPathName(pos);
	
	//  ReverseFind()로 뒤에서부터 '\' 위치를 찾는다.  
	int nSlashLoc = dataFilePath.ReverseFind('\\');	 
	dirPath = dataFilePath.Left(nSlashLoc); 
	MyTrace(PRT_BASIC, "Load 4wData(Single)...: Lot=%s\n\n\n", dirPath);


#if 0
 	// 2019.01.29 :  D:\\log\\ 이외의 디렉토리도 로딩이 필요하여 아래 코드를 코멘트 처리함
 	
	// d:\log4w 디렉토리 아래의 디렉토리인지 확인한다. ---------------
 	if (dirPath.Find("D:\\log4w\\") == -1)	
	{
		strTemp.Format("%s is not 4W Data Directory. Can't do InsertItem. :OnButtonLoad4wData_SingleLot()\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);


	// 먼저 "D:\log4w\"에 해당하는 첫번째 level을 lotName에서 제외한다.
	CString lotName = dirPath;
	int	length = strlen(lotName);
	lotName = lotName.Mid(9, (length -9));		
							// 2018.06.28: 잘라내는 위치를 9로 고정하면 D:\log4w이외에 다른 경우를 커버하지 못함.
							// 			   사용자 실수 이므로 이정도는 용인한다. d:\log4w_2 를 선택한다면 처리는 정상적으로
							// 			   되는데 date name은 이상하게 출력된다.

#else
	// ReverseFind()로 뒤에서부터 두번째의 '\' 위치 (D:\log4w\의 마지막 위치)를 찾는다.
	// log4w가 아니라 log4w_2같은 이름이어도 사용이 가능하게 된다.
	int nSlashLoc2 = dirPath.ReverseFind('\\');	 
 
	// 2019.01.31: 잘라내는 시작지점을 nSlashLoc2+1로 한다. 원래의 D:\log4w\라면 nSlashLo2+1은 9가 된다.
	CString lotName = dirPath;
	int	length = strlen(lotName);
	lotName = lotName.Mid((nSlashLoc2+1), (length - (nSlashLoc2 + 1)));	

#endif

	// "20170925" 처럼 날짜로 시작하지 않는다면 리턴.  (Lot 디렉토리가 아님) -------------
	CString dateName  = lotName.Left(8);		// 날짜 str 보관.
	CString dateExclu = dateName.SpanExcluding("0123456789"); 
	if (!dateExclu.IsEmpty())	// "0123456789"에 해당하지 않는 문자만 선택. Empty가 아니면 글자포함이란 얘기
	{
		strTemp.Format("\"%s\" \n '4w Data' Directory has not proper DateName. Can't do InsertItem.: OnButtonLoad4wData_SingleLot(): \n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}



	// "20170925_A30-19-C-4W-NEW-0920" 라면 맨앞의 날짜는 떼고 tree item으로 삽입한다. 
	lotName = lotName.Mid(9, (length -9));		// "20170925_" 날짜 부분을 떼어낸다.


	// 여기까지.. Lot Name, Date Name 확보.
	MyTrace(PRT_BASIC, "Load 4wData(Single): Lot=%s, Date=%s \n\n\n", lotName, dateName);
	

	//-------------------------
	// Check Empty Directory     
	
	// 빈 lot이 추가되는 것을 막기 위해 Insert할 디렉토리가 빈 디렉토리인지 검사한다.  (자원절약)
	// 선택한 파일의 name이 처리가 가능한 파일들인지 확인한다.  'Sample_xxxx' 파일만 선택했을 경우를 대비
	CString fileName; 
	BOOL	bFileFound = FALSE;


	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath 부분을 떼어내서 fileName만 남긴다. (dirPath+'\' 사이즈만큼 앞부분을 자름)
		int pathLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (pathLength - (nSlashLoc +1)));		 
		if (fileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		// 파일이름이 'Sample_xxxx'가 아닌 정상 4W data file이 하나라도 있다면 break; 
		bFileFound = TRUE;
		break;

	}
	if(bFileFound == FALSE)		
	{
		strTemp.Format("Any proper '4w Data File' was not Selected. Can't do InsertItem. : OnButtonLoad4wData_SingleLot()\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}


	//----------------------------------------------
	// UI (Tree, Date, Grid )  및 메모리 초기화 
	
	BeginWaitCursor();			// 커서를 waiting 모드로 바꾼다.
	
	ClearGrid(); // data Grid, summary Grid, edit박스 등 UI를 초기화한다.
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	Stat_deleteAllNetData();	// 기존 netData를 모두 메모리에서 지운다.
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	InitMember();				// CStatisticsDialog 멤버 변수를 모두 초기화 한다.
	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	Display_DataGridHeader(); 	// Data Grid Header 다시 설정 (simul이 on이었다면 off로 바꾸고 헤더를 다시 그려줘야함)

	InitTree();					// root부터 Tree를 다시 생성.	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);


	//-------------------------
	// Lot, Date 항목 추가
	
	int lotId, dateId;
	lotId  = g_sLotNetDate_Info.getLotId(lotName);
	if (lotId == -1)	// lot range를 넘어선다면 load하지 않는다.
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): %s getLotId(%s) Failed! nLotCnt=%d Range(<%d) Over!\n\n Can't do InsertItem.", 
					dirPath, lotName, g_sLotNetDate_Info.nLotCnt, MAX_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		return;
	}
	dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
	if (dateId == -1)	// date range를 넘어선다면 load하지 않는다. 
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): getLotDateId(%s/%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
				lotName, dateName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		return;
	}

	// Tree InsertItem을 그대로 이용하여 root에 해당 Lot Node를 추가
	HTREEITEM hLotItem = m_treeCtrl.InsertItem(lotName, m_hRoot); 
	m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);


	//------------------------------------------
	// Lot 밑에 선택된 CSV 파일들을 하나씩 Load 
	
	int nFile = 0;
	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		if (nFile >= MAX_TIME_FILE)
		{
			if (nFile == MAX_TIME_FILE)	
			{
				CString strTemp;
				strTemp.Format("nFile=%d(maximum), Can not load the file anymore. nLot=%d(%s), nDate=%d(%s)\n",
						nFile, lotId, g_sLotNetDate_Info.strLot[lotId], dateId, g_sLotNetDate_Info.strLotDate[lotId][dateId]);
				AfxMessageBox(strTemp, MB_ICONINFORMATION);
				MyTrace(PRT_BASIC, strTemp);
			}
			break;		
		}

		// 2018.06.18 Tree_LoadLot_4WFiles()에서 nNetDataCount >= MAX_NET_DATA 여서 빠져나온 경우 ,
		//            더 이상 다른 Time File도 찾지 않도록 아래 코드 추가함.
		if (m_nNetDataCount >= MAX_NET_DATA)
		{
			if (m_nNetDataCount == MAX_NET_DATA)
				MyTrace(PRT_BASIC, "m_nNetDataCount=%d, Load4wData(Single) CsvFile Loading Stopped!! nFile=%d\n\n", m_nNetDataCount , nFile);
			break;
		}

		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath 부분을 떼어내서 fileName만 남긴다. (dirPath+'\' 사이즈만큼 앞부분을 자름)
		int pathLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (pathLength - (nSlashLoc +1)));		 

		//MyTrace(PRT_BASIC, "Load4wData(Single): Path=%s, file=%s \n", dataFilePath, fileName );	 // test print

		// 해당 CSV file을 load한다. 
		if (Load_4wDataFile(dataFilePath, fileName, hLotItem, lotId, dateId) < 0)
			continue;

		nFile++;
	}

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	//------------------------------------------
	// 해당 Lot의 모든 Net의 Fault Check
	Tree_Check_LotNetFaults(hLotItem, lotId);
	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	strTemp.Format("\'Load 4w Data\' for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", dirPath);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "OnButtonLoad4wData_SingleLot() for\"%s\" Completed. \n", dirPath);
	

	
	// default Net 조회: 별도로 클릭이 없어도 첫번째 Lot, 첫번째 Net을 조회해 준다.
	DisplayGrid_DefaultLotNet();

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	MyTrace(PRT_BASIC, "          m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "\n");


	//--------------
	// 완료 
	
	EndWaitCursor();		// 커서를 원상 복구한다.

	// LOAD LOG4W가 완료되었음을 FR Rank Dlg에 알린다.
	//::SendMessage(m_hwnd_FrRankDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
}



// Tree Control의 선택 item이 변경될 때 호출
void CStatisticsDialog::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	_SelChangedTree(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

// 2018.09.19 실제 click 없이도 default로 Lot_0, Net_1을 클릭시켜 주기 위해 이렇게 인터페이스를 분리함.
//            실제 click인 OnSelchangedTree()도 이 함수를 사용한다. (동일 효과)
void CStatisticsDialog::_SelChangedTree(HTREEITEM  hSelItem)
{

	HTREEITEM	hParent;
	CString		strTemp;
	CString		parentName = ""; 
	CString		selectedName = "";

	// 현재 선택한 아이템의 핸들 값을 멤버변수에 저장한다.
	m_hSelectedNode = hSelItem;
	hParent			= m_treeCtrl.GetParentItem(m_hSelectedNode);

	// test
	//m_treeCtrl.SetItemImage(m_hSelectedNode, 2, 3);		// item에 빨강색 장애 아이콘 설정  테스트

	// 선택한 아이템의 이름을 대화상자의 에디트 상자에 설정한다.  
	if (hParent != NULL)
		parentName   = m_treeCtrl.GetItemText(hParent);
	selectedName = m_treeCtrl.GetItemText(m_hSelectedNode);

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);

	if (nLevel == 3)			// 선택한 아이템이 Net라면 'lot# - net#'를 설정
		m_editStrSelect = parentName + " / " + selectedName;
	else
		m_editStrSelect = selectedName;


	// Net인 경우
	if (nLevel == 3)
	{
		// find NetId,  LotId
		int nLot, nNet;
		CString strTemp;
		nNet = atoi(selectedName.Mid(4));
		nLot = g_sLotNetDate_Info.findLotId(parentName);
		if (nLot < 0 || nLot >= MAX_LOT)
		{
			strTemp.Format("OnSelchangedTree(): Can't find Lot#(%d) for item=%s, or Range(Lot<%d) Over\n", 
					nLot, m_editStrSelect, MAX_LOT);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); 
			return;
		}
		if (nNet < 0 || nNet >= MAX_NET_PER_LOT)
		{
			strTemp.Format("OnSelchangedTree(): %s Net# Range(0 <= Net < %d) Over\n", m_editStrSelect, MAX_NET_PER_LOT);
			ERR.Set(RANGE_OVER, strTemp); 
			ErrMsg();  ERR.Reset(); 
			return;
		}

		// 현재 combo의 date data를 Lot에 맞게 바꾸어 준다.  m_nCombo_CurrDate도 0(DATE_ALL)로 설정. 
		Combo_UpdateDateContents(nLot);
		m_nTree_CurrLot = nLot;
		m_nTree_CurrNet = nNet;

	}

	// Lot 인 경우
	else if (nLevel == 2)
	{
		// find LotId
		int nLot = g_sLotNetDate_Info.findLotId(selectedName);
		if (nLot < 0 || nLot >= MAX_LOT)
		{
			strTemp.Format("OnSelchangedTree(): Can't find Lot#(%d) for item=%s, or Range(<Lot<%d) Over\n", 
					nLot, selectedName, MAX_LOT);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); 
			return;
		}
		// 현재 combo의 date data를 Lot에 맞게 바꾸어 준다.  m_nCombo_CurrDate도 0(ALL)로 설정. 
		Combo_UpdateDateContents(nLot);
		m_nTree_CurrLot = nLot;
		m_nTree_CurrNet = -1;

	}

	// Total인 경우
	else
	{

		// 현재 combo의 date data를 모두 지운다.
		m_comboDate.ResetContent();	

		// Total의 data를 summaryGrid에 출력해 준다.
		m_nTree_CurrLot = -1;			// 미선택
		m_nTree_CurrNet = -1;			// 미선택
		m_nCombo_CurrDate = DATE_ALL; 	// All     0=All,  1~7 : 실제 date#+1

	}

	// 혹시 이전에 Simulation 설정상태였다면 해제한다.
	CheckOff_SimulUlsl();	

	// 선택된 Net이나, Lot의 Summary를 summaryGrid에 출력해 주고,  
	// Net data와 date까지 상세하게 선택이 되었다면 dataGrid도 출력해 준다.
	// 2018.05 Display 버튼을 눌렀을 때 출력해 주는 것으로 수정함.
	// 2018.06.15 이상윤 부장 요청으로 Date나 Tree 클릭만으로 조회가 가능하게 다시 수정함.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		



	// 대화상자의 Edit컨트롤에 m_editStrSelect 를 출력한다.
	UpdateData(FALSE);
	
}

// 현재 combo의 date data를 Lot에 맞게 바꾸어 준다.  m_nCombo_CurrDate도 0(ALL)로 설정. 
// m_nTree_CurrLot 의 값을 바꿀때에는 반드시 먼저 Combo_UpdateDateContents(nLot)을 호출한다.
void  CStatisticsDialog::Combo_UpdateDateContents(int nLot)
{
	// Lot 번호가 바뀌었다면, 선택된 Lot에 속하는 date 들을 comboDate에 넣어준다.
	if (m_nTree_CurrLot != nLot)
	{
		// 현재 combo의 data를 모두 지운다.
		m_comboDate.ResetContent();	
		
		// com idx=0 추가
		m_comboDate.InsertString(-1, "ALL");		// combo idx = 0: DATE_ALL

		// date_0이 두번째 (idx=1) 로  combo에 추가된다.  combo idx는 'date+1'로 다룰 것.
		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			

		// net이나 combo 클릭시에 바로 display를 위해 date 를 default로 "ALL"을 설정해 준다.
		m_nCombo_CurrDate = DATE_ALL; 	//  0=DATE_ALL,  1~7 : 실제 date#+1
		m_comboDate.SetCurSel(0);

		UpdateData(FALSE);		// SetCurSel 결과를 UI에 반영.
	}
}

void CStatisticsDialog::OnSelchangeComboDate() 
{

	// TODO: Add your control notification handler code here
	CString strTemp;

	int 	comboPrevDate = m_nCombo_CurrDate;
	m_nCombo_CurrDate = m_comboDate.GetCurSel();		// 현재 combo에서 선택된 date# (0=ALL이므로 실제 date+1),  미선택이면 -1

	// Range Check
	if ( m_nCombo_CurrDate < 0 || m_nCombo_CurrDate > (MAX_DATE + 1) )
	{

		strTemp.Format("OnSelchangeComboDate(): m_nCombo_CurrDate=%d, Range(0<= Date <%d) Over\n", 
					m_nCombo_CurrDate, (MAX_DATE + 1) );
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		m_nCombo_CurrDate = comboPrevDate;
		return;
	}

	// 혹시 이전에 Simulation 설정상태였다면 해제한다.
	CheckOff_SimulUlsl();	
	
	// 선택된 Net이나, Lot의 Summary를 summaryGrid에 출력해 준다. 
	// 2018.05 Display 버튼을 눌렀을 때 출력해 주는 것으로 수정함.
	// 2018.06.15 이상윤 부장 요청으로 Date나 Tree 클릭만으로 조회가 가능하게 다시 수정함.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

}


// 2018.05 Display 버튼을 눌렀을 때 출력해 주는 것으로 수정함.
// 2018.06.15 이상윤 부장 요청으로 Date나 Tree 클릭만으로 조회가 가능하게 다시 수정함.
//            Display Data 버튼을  UI에서  '보이지 않게' property invisible 처리함.
//            아래 함수 자체는 남겨 두었으나 호출방법이 삭제되었으므로 현재 사용되지는 않고 있음.
/*
void CStatisticsDialog::OnButtonDisplay() 
{
	// TODO: Add your control notification handler code here
	
	// 선택된 Net이나, Lot의 Summary를 summaryGrid에 출력해 준다. 
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		
	
}
*/


void CStatisticsDialog::DisplayGrid(int nLot, int nNet, int nComboDate) 
{
	CString strTemp;

	// Tree에 Root만 있다면 Load 버튼을 눌러야 한다는 메시지 출력.
	if (m_treeCtrl.GetCount() == 1)
	{
		strTemp.Format("Execute \"Load 4W Data\" button, First.\n And select any Tree item and Date item please. \n :DisplayGrid()");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// 클릭한 item이 없다면  아무 item이나 클릭하라는 메시지 출력.
	if (m_hSelectedNode == NULL)
	{
		strTemp.Format("Select any Tree item and Date item please.\n: DisplayGrid()");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);
	
	//--------------
	// Net인 경우
	
	// 현재 선택된 Lot, Net, Date의 data를 dataGrid, summaryGrid에 출력해 준다.
	if (nLevel == 3)
	{
		// Lot, Net, Date가 모두 값이 존재한다면, DataGrid와 SummaryGrid 출력
		if (nComboDate != DATE_ALL )	
		{
			// DataGrid, SummaryGrid 모두 출력.
			// 1. Lot, net, date를 key로 해서  모든 time tuple을 sum해서 summary grid  출력
			// 2. Lot, net, date가 모두 존재하므로 해당되는 모든 time tuple Raw data를  data grid 에 출력. 
			
			GetDlgItem(IDC_GRID_DATA)->ShowWindow(TRUE);		// DataGrid 보이기 

			Display_SumupLotNetDate(nLot, nNet, nComboDate -1);			
 											// m_nCombo_CurrDate는 실제 Date +1 값이므로 -1해서 넘긴다.

		}
		// Lot, Net 존재하고, date=All 이라면 SummaryGrid 출력.
		else
		{
			// SummaryGrid 출력.
			// 1. lot, net을 key로 해서 모든 date, 모든 time tuple을 sum해서 summary grid 출력.
			// 2. data grid는 관련된 date의 모든 data를 출력한다.   (숨기지 않는다.)
			
			GetDlgItem(IDC_GRID_DATA)->ShowWindow(TRUE);		// DataGrid 보이기 
			//GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid 숨기기
			
			Display_SumupLotNet(nLot, nNet);
	
		}
		
	}

	//--------------
	// Lot인 경우
	else if(nLevel == 2)
	{
		GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid 숨기기
		//ClearGrid_Data();		// DataGrid 지우기

		// Lot 존재, Net= -1, Date값이 존재한다면
		// 1. lot, date를 key로 하고,  모든 net, 모든 time tuple을 sum 해서 summary grid  출력 
		if (nComboDate != DATE_ALL )	
			Display_SumupLotDate(nLot, nComboDate -1 ); 	// Summary Data 출력.
 													// m_nCombo_CurrDate는 실제 Date +1 값이므로 -1해서 넘긴다.
 											
		// Lot 존재, Net= -1, Date=All
		// 1. Lot만 key로 해서 모든 net, 모든 date, 모든 time tuple을 sum해서 summary grid 출력
		else	
			Display_SumupLot(nLot);

	}

	//--------------
	// Total인 경우
	else if(nLevel == 1)
	{
		GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid 숨기기
		//ClearGrid_Data();		// DataGrid 지우기

		// Lot 미존재, Net 미존재, Date = All
		// 1. 모든 lot,  모든 net, 모든 date, 모든 time tuple을 sum해서 summary grid 출력.
		if (nComboDate == DATE_ALL)
			Display_SumupTotal();	// Summary Data 생성 

	}

}


// Lot, Net, Date가 모두 값이 존재한다. Date Grid와 Summary Grid 모두 출력.
// 1. Lot, net, date를 key로 해서  모든 time tuple을 sum해서 summary grid  출력
// 2. Lot, net, date가 모두 존재하므로 해당되는 모든 time tuple Raw data를  data grid 에 출력. 
void CStatisticsDialog::Display_SumupLotNetDate(int nLot, int nNet, int nDate)
{
	CString strTemp;

	// Lot 존재해야 하고, Net 존재해야 하고, date 존재해야 한다. date=All도 아니어야 한다.
	if ( (nLot < 0 || nLot >= MAX_LOT) 
			|| (nNet < 0 || nNet >= MAX_NET_PER_LOT )
			|| (nDate < 0 || nDate >= MAX_DATE) ) 
	{
		strTemp.Format("Display_SumupLotNetDate(): nLot=%d(0<= Lot <%d), nNet=%d(0<= Lot <%d), nDate=%d(0<= Date <%d) Range Over!\n", 
					nLot, MAX_LOT, nNet, MAX_NET_PER_LOT, nDate, MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	MyTrace(PRT_BASIC, _T("Display_SumupLotNetDate(): nLot=%d, nNet=%d, nDate=%d\n"), nLot, nNet, nDate);

	if(g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		MyTrace(PRT_BASIC, _T("Display_SumupLotNetDate(): g_pvsaNetData[%d][%d][%d] is NULL!\n"), nLot, nNet, nDate);
		return;
	}

	
	//---------------------------
	// 해당 날짜의 DataGrid 출력
	
	// data를 업데이트하기 전에 지우기. 지워야 하므로 UpdateDate(True)는 필요 없음.
	ClearGrid_Data();	
	ClearGrid_Summary();	
	m_GridSnap.InitMember();


	// tupleSize로  m_gridData  row를 맞춘다.
	// 2019.02.25: grid를 불필요하게 max로 row를 잡아서 메모리를 과도하게 사용하는 문제 해결.
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();		
	if (m_nRows != (tupleSize + 1))
	{
		m_nRows = (tupleSize + 1);		// 헤더라인 포함해서 tupleSize + 1
		TRY {
			m_gridData.SetRowCount(m_nRows);
			m_gridData.SetColumnCount(m_nCols);
			m_gridData.SetFixedRowCount(m_nFixRows);
			m_gridData.SetFixedColumnCount(m_nFixCols);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
			return;
		}
    	END_CATCH	
	}


	// nLot, nNet, NDate에 맞는 모든 data Grid Tuple을 출력한다.
	m_GridSnap.dataCount = DisplayGrid_4wData(nLot, nNet, nDate, 0);


	//---------------------------
	// Net Info Edit 박스값 설정
	//int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();
	m_editTupleNum  = tupleSize;

	m_editStepNum = g_sLotNetDate_Info.naLotStepCnt[nLot];


	// m_editLSL과 m_editUSL 부분은  DisplayGrid_Summary()에서 수행.

	MyTrace(PRT_BASIC, "Lot=%d(%s), Net=%d, Date=%d(%s), tupleSize=%d : \n", 
			nLot, g_sLotNetDate_Info.strLot[nLot], nNet, 
			nDate, g_sLotNetDate_Info.strLotDate[nLot][nDate], tupleSize );


	//-------------------
	// Summary Grid 출력
	

	// Calc Total, Count, Avg, Sigma, Min, Max	 ----------------
	int	   	nTotal=0, nCount=0;
	double 	dAvg=0, dSigma=0, dMax=0, dMin=0; 
	CalcSummary_AvgSigmaMinMax(nLot, nNet, nDate, 						// argument 
							nCount, nTotal, dAvg, dSigma, dMin, dMax);	// reference 

	// Summary Display   ------------------
	DisplayGrid_Summary(nLot, nNet, nDate, nCount, nTotal, dAvg, dSigma, dMin, dMax, 
							//nFaultCount);
							g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);


	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.
}




// 전역  함수
// Stat Dlg 외에 FR Dlg, YR Dlg에서도 쓸 수 있도록  
// CStatisticsDialog 클래스 멤버함수가 아닌 전역함수로 define
void	CalcSummary_AvgSigmaMinMax(int nLot, int nNet, int nDate,  		// argument
			int& rnCount, int& rnTotal, double& rdAvg, double& rdSigma, double& rdMin, double& rdMax )	// reference
{
	CString strTemp;

	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		strTemp.Format("CalcSummary_AvgSigmaMinMax(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, nDate );
		ERR.Set(SW_NO_INIT_VAR, strTemp); 
		ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
		return;
	}

	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, step, stepSize;
	double  dSum=0;
	int		nMinMaxInitStep = 0;	// Min, Max initial 위치
	for (tuple=0; tuple < tupleSize; tuple++)
	{
		nMinMaxInitStep = 0;

		stepSize =  g_sLotNetDate_Info.naLotStepCnt[nLot];
		for (step= 0; step < stepSize; step++)
		{
			rnTotal++;
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)		// NG가 아니라면
			{
				rnCount++;
				dSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

				// Min, Max 초기화
				if (tuple==0 && step==nMinMaxInitStep)
				{
					rdMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
					rdMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
				else
				{
					// 최대값보다 현재값이 크면 최대값 변경
					if (rdMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						rdMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

					// 최소값보다 현재값이 작으면 최소값 변경
					if (rdMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						rdMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}

			}
			else
			{
				// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
				if (step == nMinMaxInitStep)		
					nMinMaxInitStep++;						
			}
		}
	}

	// 2. Calc Sigma   --------------------
	double 	dDiff = 0, dDiffPowerSum = 0, dVariance=0;
	if (rnCount)	// check devide by zero!  : Average
		rdAvg = dSum / (double)rnCount;

	if (rnCount -1)	// check devide by zero!  : Variance
	{
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
			for (step= 0; step < g_sLotNetDate_Info.naLotStepCnt[nLot]; step++)
			{
				if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)
				{
					dDiff =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] - rdAvg;
					dDiffPowerSum += (dDiff * dDiff);		
				}
			}
		}

		dVariance = dDiffPowerSum / (double)(rnCount -1);	// 분산     : (val-avg)의 제곱의 총합을 (n - 1)으로 나눈다.
	}
	rdSigma    = sqrt(dVariance);						// 표준편차 : 분산의 제곱근

	//MyTrace(PRT_BASIC, "rnTotal=%d, rnCount=%d, dSum=%.2f, rdAvg=%.2f, dVar=%.2f, rdSigma=%.2f, rdMin=%.2f, rdMax=%.2f, nFaultCount=%d\n", 
	//						rnTotal, rnCount, dSum, rdAvg, dVariance, rdSigma, rdMin, rdMax, 
	//						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);
							//nFaultCount);


}



// nLot, nNet, NDate에 맞는 모든 data Grid Tuple을 출력한다.
// return : 마지막으로 출력한  tupleOffset 을 리턴한다.
int CStatisticsDialog::DisplayGrid_4wData(int nLot, int nNet, int nDate, int tuplePrtStart)
{
	CString strTemp;

	//-------------------
	// data Grid 출력

	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		strTemp.Format("DisplayGrid_4wData(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, nDate );
		ERR.Set(SW_NO_INIT_VAR, strTemp); 
		ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
		return tuplePrtStart;
	}

	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	int 	tuple, step, prtTuple=0;
	int    	nTupleCount=0/*, nFaultCount=0*/; 
	double  dTupleSum=0;
	double 	dTupleAvg=0, dTupleMin=0, dTupleMax=0;
	double	dTupleDiff = 0, dTupleDiffPowerSum = 0, dTupleVar=0, dTupleSigma=0;


	for (tuple=0; tuple < tupleSize; tuple++)
	{

		// 1. Calc Tuple Avg, Min, Max   ---------------------
		nTupleCount=0; 
		dTupleSum=0;
		dTupleAvg=0; 
		dTupleMin=0; 
		dTupleMax=0;
		int nMinMaxInitStep = 0;		// Min, Max의 초기화 위치 표시

		int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
		for (step= 0; step < stepSize; step++)
		{
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)	// NG가 아니라면
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

				// Min, Max 초기화
				if (step == nMinMaxInitStep)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
				else
				{
					// 최대값보다 현재값이 크면 최대값 변경
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

					// 최소값보다 현재값이 작으면 최소값 변경
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
			}
			else
			{
				// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
				if (step == nMinMaxInitStep)		
					nMinMaxInitStep++;						
			}
		}	




		// 2. Calc Tuple Sigma  ------------------------
		dTupleDiff = 0; 
		dTupleDiffPowerSum = 0; 
		dTupleVar=0; 
		dTupleSigma=0;
		if (nTupleCount)	// check devide by zero!	: Average
			dTupleAvg = dTupleSum / (double)nTupleCount;

		if (nTupleCount -1)	// check devide by zero!	: Variance
		{
			int stepSize =  g_sLotNetDate_Info.naLotStepCnt[nLot];
			for (step= 0; step < stepSize; step++)
			{
				if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)
				{
					dTupleDiff =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] - dTupleAvg;
					dTupleDiffPowerSum += (dTupleDiff * dTupleDiff);		
				}
			}

			dTupleVar   = dTupleDiffPowerSum / (double)(nTupleCount -1);// 분산     : (val-avg)의 제곱의 총합을 (n - 1)으로 나눈다.
		}
		dTupleSigma = sqrt(dTupleVar);								// 표준편차 : 분산의 제곱근


		// 3. Display 4w Data Tuple  ------------------
		int ret = DisplayGrid_4wData_Tuple(nLot, nNet, nDate, tuple, (tuplePrtStart+prtTuple),// ALL인 경우 이전에 출력한 위치 이후로 출력.
							dTupleAvg, dTupleSigma, dTupleMin, dTupleMax);

		// m_bDataGridFaultOnly는 TRUE일때 prt 건너뛰고 리턴한 경우는 prtTuple을 증가시키지 않는다.
		// 이렇게 하지 않으면 Fault Only일때 몇개의 Fault가 빈줄 사이에 드문드문 보이게 됨. 
		if (ret != -1)
			prtTuple++;
	}

	//return (tuplePrtStart + tupleSize);		// 현재까지 출력한 tuple위치(Offset)를 리턴한다.
	return (tuplePrtStart + prtTuple);		// 현재까지 출력한 tuple위치(Offset)를 리턴한다.
}


// Data Grid에 1개 tuple을 Display 한다.
// nPrtTuple: ALL 인 경우 이전에 출력한 위치 이후로 tuple을 출력한다.
int CStatisticsDialog::DisplayGrid_4wData_Tuple( int nLot, int nNet, int nDate, int nTuple, int nPrtTuple,
						double dTupleAvg, double dTupleSigma, double dTupleMin, double dTupleMax)
{
	CString strTemp;
	int 	step;

	// 2018.05.31  Net tuple 공통 Lsl, Usl은 코멘트처리하고 tuple별 Lsl, Usl을 사용하기로 함.
	//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
	//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];

	// 2018.10.02 추가 : ULSL Simulattion On일 때에는 dLsl, dUsl을 Simul값으로 보여준다.
	//           m_GridSnap에도 여기에서 반영이 되므로 csv 파일에 대해서는 따로 simul관련 처리할 필요 없음
	double dLsl, dUsl;		
	if ( m_bApplyULSL && 				// if USL/LSL Simulation on
		(m_dSimulLsl != -1 && m_dSimulLsl != -1) ) 
	{
		dLsl = m_dSimulLsl;
		dUsl = m_dSimulUsl;
	}
	else
	{
		dLsl = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dLsl;
		dUsl = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dUsl;
	}


	// 0     1      2     3     4     5     6     7     8     9     10    11    12    13     14      ....   25
	// No,  Date, Time,  Pin1, Pin2, Pin3, Pin4,  R,   LSL,  USL,  Avg, Sigma, Min, Max,  Data1, .... Data12

	// 1. Check 4W Data Fault  in  steps ---------------------
	int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
	int nFaultCount = 0;
	for (step= 0; step < stepSize; step++) // col 14~25:  Step1~ Step12
	{
															
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step] != -1)		// NG가 아니면
		{
			double dStepVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step];

			if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
				nFaultCount++;
		}
	}

	// m_bDataGridFaultOnly가 TRUE 일때  현재 step 중에 Fault가 한개도 없다면 출력하지 않고 리턴.
	if (m_bDataGridFaultOnly == TRUE &&  nFaultCount == 0)
		return -1;


	// 2. Display  PinInfo, R, LSL, USL  ----------------------
	
	// row 0 헤더는 제외하고 data영역인 row 1 (nTuple + 1)부터 출력.
	strTemp.Format("%d", (nPrtTuple+1));
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_NO, strTemp);				// col 0:  No

	m_GridSnap.saData[nPrtTuple].nPrtTuple = nPrtTuple; // Save data snap for csv


	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATE, 
		g_sLotNetDate_Info.strLotDate[nLot][nDate]);						// col 1:  Date 
	m_GridSnap.saData[nPrtTuple].strDate = g_sLotNetDate_Info.strLotDate[nLot][nDate];

	CString strTime;
	strTime.Format("%02d:%02d:%02d", 
			(*g_pvsaNetData[nLot][nNet][nDate])[nTuple].statTime.hour,
			(*g_pvsaNetData[nLot][nNet][nDate])[nTuple].statTime.min,
			(*g_pvsaNetData[nLot][nNet][nDate])[nTuple].statTime.sec);
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_TIME, strTime);			// col 2:  Time 
	m_GridSnap.saData[nPrtTuple].strTime = strTime;

	for (int pin= 0; pin < NUM_STAT_PIN; pin++)
	{
		strTemp.Format("%d", g_sLotNetDate_Info.waLotNetPin[nLot][nNet][pin]);
		m_gridData.SetItemText(nPrtTuple+1, DATA_COL_PIN1+pin, strTemp);	// col 3~6: Pin1~ Pin4 
	}
	strTemp.Format("%.2f",  (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dRefR);
	m_gridData.SetItemText(nPrtTuple+1,  DATA_COL_R, strTemp);				// col 7:   RefR
	m_GridSnap.saData[nPrtTuple].strRefR = strTemp;


	strTemp.Format("%.2f",  dLsl);
	m_gridData.SetItemText(nPrtTuple+1,  DATA_COL_LSL, strTemp);			// col 8:   LSL
	m_GridSnap.saData[nPrtTuple].strLSL = strTemp;		

	strTemp.Format("%.2f",  dUsl);
	m_gridData.SetItemText(nPrtTuple+1,  DATA_COL_USL, strTemp);			// col 9:   USL
	m_GridSnap.saData[nPrtTuple].strUSL = strTemp;


	// 3. Display Tuple Avg, Sigma, Min, Max  ------------------------
	strTemp.Format("%.2f",  dTupleAvg);
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_AVG, strTemp);	
	m_GridSnap.saData[nPrtTuple].strTupleAvg = strTemp;

	strTemp.Format("%.2f",  dTupleSigma);
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_SIGMA, strTemp);	
	m_GridSnap.saData[nPrtTuple].strTupleSigma = strTemp;

	strTemp.Format("%.2f",  dTupleMin);
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_MIN, strTemp);	
	m_GridSnap.saData[nPrtTuple].strTupleMin = strTemp;

	strTemp.Format("%.2f",  dTupleMax);
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_MAX, strTemp);	
	m_GridSnap.saData[nPrtTuple].strTupleMax = strTemp;

	// LSL, USL은 Fault의 기준이므로 분홍색 배경으로 눈에 띄게 표시한다.
	m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0xFF, 0xC0, 0xCB));	// pink 
	m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0xFF, 0xC0, 0xCB));	// pink 

	// 만약 Simulation LSL, USL이면 보라색 배경으로 표시한다.
	if ( m_bApplyULSL && 				// if USL/LSL Simulation on
		(m_dSimulLsl != -1 && m_dSimulLsl != -1) ) 
	{
		//m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0xDD, 0xA0, 0xDD));	// plum (연보라) 
		//m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0xDD, 0xA0, 0xDD));	// plum (연보라) 

		m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0x93, 0x70, 0xDB));	// medium purple (보라)
		m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0x93, 0x70, 0xDB));	// medium purple (보라)
	}


	// 4. Display Tuple 4W Data   ------------------------
	for (step= 0; step < stepSize; step++) // col 14~25:  Step1~ Step12
	{
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step] == -1)	// NG이면
		{
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+step, "NG");	
			m_GridSnap.saData[nPrtTuple].strData[step] = "NG";
		}

		else		// NG가 아니면
		{
			double dStepVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step];

			strTemp.Format("%.2f",  dStepVal);
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+step, strTemp);	
			m_GridSnap.saData[nPrtTuple].strData[step] = strTemp;

			if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
			{
				m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_DATA1+step, 	// row, col
									//RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)보다 약간 연한 빨강
									RGB(0xff, 0x63, 0x47));			// tomato : 진한 주황
			}
		}
	}

	return 0;
}

// Summary Grid에 값을 Display 한다.
void CStatisticsDialog::DisplayGrid_Summary(int nLot, int nNet, int nDate, int nCount,
								int nTotal, double dAvg, double dSigma, double dMin, double dMax, 
								int nFaultCount)
{
	CString strTemp;

	
	// double형은 값이 변형되어 출력되므로 CString으로 관리하여 아래와 같이 출력한다.
	// => double형인 경우 8.6이 8.59로 변형되는 현상이 있었음.
	


	//----------------------------
	// Display Summary

	strTemp.Format("Net_%d", nNet);
	m_gridSummary.SetItemText(1, SUM_COL_NET, strTemp);			// col 0:  Net Name
	m_GridSnap.Summary.strNetName = strTemp;

	int nComboDate = nDate +1;
	if (nComboDate == DATE_ALL)
		strTemp = "ALL";
	else
		strTemp = g_sLotNetDate_Info.strLotDate[nLot][nDate];

	m_gridSummary.SetItemText(1, SUM_COL_DATE, strTemp);		// col 1:  Date
	m_GridSnap.Summary.strDate = strTemp;

	strTemp.Format("%d", nTotal);
	m_gridSummary.SetItemText(1, SUM_COL_TOTAL, strTemp);		// col 2:  Total 
	m_GridSnap.Summary.strTotal = strTemp;

	strTemp.Format("%d", (nTotal - nCount));
	m_gridSummary.SetItemText(1, SUM_COL_NG, strTemp);			// col 3:  N/G Count
	m_GridSnap.Summary.strNgCount = strTemp;

	strTemp.Format("%d", nCount);
	m_gridSummary.SetItemText(1, SUM_COL_COUNT, strTemp);		// col 4:  Count
	m_GridSnap.Summary.strCount = strTemp;


	strTemp.Format("%.2f", dAvg);
	m_gridSummary.SetItemText(1, SUM_COL_AVG, strTemp);			// col 5:  Average
	m_GridSnap.Summary.strAvg = strTemp;

	strTemp.Format("%.2f", dSigma);
	m_gridSummary.SetItemText(1, SUM_COL_SIGMA, strTemp);		// col 6:  Sigma (표준편차)
	m_GridSnap.Summary.strSigma = strTemp;

	strTemp.Format("%.2f", dMin);
	m_gridSummary.SetItemText(1, SUM_COL_DATAMIN, strTemp);		// col 7:  Min
	m_GridSnap.Summary.strMin = strTemp;

	strTemp.Format("%.2f", dMax);
	m_gridSummary.SetItemText(1, SUM_COL_DATAMAX, strTemp);		// col 8:  Max
	m_GridSnap.Summary.strMax = strTemp;

	strTemp.Format("%d", nFaultCount); 
	m_gridSummary.SetItemText(1, SUM_COL_FAULT, strTemp);		// col 9:  FaultCount
	m_GridSnap.Summary.strFault = strTemp;

	// Fault가 1개라도 있으면 Summary의 FaultCount를 붉은색으로, net Icon도 붉은색으로 변경한다.
	if (nFaultCount > 0)	
	{
    	// 'Fault 셀' 배경을 '주황색' 표시하기	
    	m_gridSummary.SetItemBkColour(1, SUM_COL_FAULT, 
									//RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)보다 약간 연한 빨강
									RGB(0xff, 0x63, 0x47));			// tomato : 진한 주황
    	
	}

	else // Fault가 없으면 
	{
    	// 'Fault 셀' 배경을 원상복구  -> 이건 이미 ClearGrid_Summary를 수행했으므로 안해도 됨.
    	//m_gridSummary.SetItemBkColour(1, SUM_COL_FAULT, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 

	}

	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.
}

/*
void CStatisticsDialog::OnButtonClearGrid() 
{
	// TODO: Add your control notification handler code here
	MyTrace(PRT_BASIC, "OnButtonClearGrid()...\n" );
	
	ClearGrid();

}
*/


void CStatisticsDialog::ClearGrid() 
{
	// Lot/Net 선택사항도 삭제한다.
	m_editStrSelect = _T("");
	
	// Grid와 함께 Grid Sub Edit 박스도 초기화한다.
	m_editTupleNum = 0;
	m_editStepNum = 0;


	// data를 업데이트하기 전에 지우기. 
	ClearGrid_Summary();	

	ClearGrid_Data();	

	m_GridSnap.InitMember();
//__PrintMemSize(FUNC(ClearGrid), __LINE__);

	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.
	
}


void CStatisticsDialog::ClearGrid_Data()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridData.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
		for (int col = 0; col < m_gridData.GetColumnCount(); col++)
		{
			m_gridData.SetItemText(row, col, "                 ");


			// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
			m_gridData.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

	//m_gridData.DeleteAllItems();  -> 이것도 넣으면 속도 느려지고 애써 초기화한 data 다 날아감
	
	// Scroll Bar 초기화 
	m_gridData.SetScrollPos32(SB_VERT, 0);
	m_gridData.SetScrollPos32(SB_HORZ, 0);
	


//	m_gridData.AutoSize(); -> 이것 넣으면 속도 느려짐.
}

void CStatisticsDialog::ClearGrid_Summary()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridSummary.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
		for (int col = 0; col < m_gridSummary.GetColumnCount(); col++)
		{
			m_gridSummary.SetItemText(row, col, "                 ");

			// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
			m_gridSummary.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

	//m_gridSummary.DeleteAllItems();  -> 이것도 넣으면 속도 느려짐
	
	// Scroll Bar 초기화 
	m_gridSummary.SetScrollPos32(SB_VERT, 0);
	m_gridSummary.SetScrollPos32(SB_HORZ, 0);
//	m_gridSummary.AutoSize();	-> 이것 넣으면 속도 느려짐
}


// Lot, Net 존재하고, date=All 이라면 SummaryGrid 출력.
// 1. lot, net을 key로 해서 모든 date, 모든 time tuple을 sum해서 summary grid 출력.
// 2. data grid는 관련된 date의 모든 data를 출력한다.
void CStatisticsDialog::Display_SumupLotNet(int nLot, int nNet)
{
	CString strTemp;

	// Lot 존재해야 하고, Net 존재해야 하고, data=All이어야 한다.
	if ( (nLot < 0 || nLot >= MAX_LOT) 
			|| (nNet < 0 || nNet >= MAX_NET_PER_LOT) ) 
	{
		strTemp.Format("Display_SumupLotNet(): nLot=%d(0<= Lot <%d), nNet=%d(0<= Net <%d) Range Over\n", 
					nLot, MAX_LOT, nNet, MAX_NET_PER_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	MyTrace(PRT_BASIC, _T("Display_SumupLotNet(): nLot=%d, nNet=%d\n"), nLot, nNet);

	
	//---------------------------
	// 해당 날짜의 DataGrid 출력
	
	int date;
	
	// data를 업데이트하기 전에 지우기. 지워야 하므로 UpdateDate(True)는 필요 없음.
	ClearGrid_Summary();	
	ClearGrid_Data();	
	m_GridSnap.InitMember();

	int rowSize = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		rowSize += g_pvsaNetData[nLot][nNet][date]->size();  			// date의 tupleSize 
	if (m_nRows != (rowSize + 1))
	{
		m_nRows = (rowSize + 1);		// 헤더 포함 rowSize +1한다.

		TRY {
			m_gridData.SetRowCount(m_nRows);
			m_gridData.SetColumnCount(m_nCols);
			m_gridData.SetFixedRowCount(m_nFixRows);
			m_gridData.SetFixedColumnCount(m_nFixCols);
		}
		CATCH (CMemoryException, e)
		{
			e->ReportError();
			e->Delete();
			return;
		}
    	END_CATCH	
	}


	// nLot, nNet, nDate에 맞는 모든 data Grid Tuple을 출력한다.
	int tupleOffset = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		tupleOffset= DisplayGrid_4wData(nLot, nNet, date, tupleOffset);
	m_GridSnap.dataCount = tupleOffset;




	//---------------------------
	// Net Info Edit 박스값 설정

	m_editTupleNum = rowSize;
	m_editStepNum = g_sLotNetDate_Info.naLotStepCnt[nLot];
	// m_editLSL과 m_editUSL 부분은  DisplayGrid_Summary()에서 수행.




	//-------------------------
	// Summary Grid 출력
	


	
	
	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, step, tupleSize, stepSize;
	int	   	nTotal=0, nCount=0;
//	int		nFaultCount=0; 
	double  dSum=0;
	double 	dAvg=0, dMax=0, dMin=0; 
	int		nMinMaxInitStepLoc = 0;	// Min, Max initial 위치
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
	{
		if (g_pvsaNetData[nLot][nNet][date] == NULL)
		{
			strTemp.Format("Display_SumupLotNet(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, date );
			ERR.Set(SW_NO_INIT_VAR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 		// 메시지박스 출력은 안하기.
			continue;
		}

		tupleSize = g_pvsaNetData[nLot][nNet][date]->size();
		//m_editTupleNum += tupleSize;
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			nMinMaxInitStepLoc = 0;
			stepSize =  g_sLotNetDate_Info.naLotStepCnt[nLot];
			for (step= 0; step < stepSize; step++)
			{
				nTotal++;
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step] != -1)	// NG가 아니라면
				{
					double dStepVal/*, dLsl, dUsl*/;
					dStepVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step];

					// Min, Max, Sum, Count 구하기 -----------
					nCount++;
					dSum += dStepVal; 
	
					// Min, Max 초기화
					if (date== 0 && tuple==0 && step==nMinMaxInitStepLoc)
					{
						dMax = dStepVal; 
						dMin = dStepVal; 
					}
					else
					{
						// 최대값보다 현재값이 크면 최대값 변경
						if (dMax <  dStepVal)
							dMax =  dStepVal; 
	
						// 최소값보다 현재값이 작으면 최소값 변경
						if (dMin >  dStepVal)
							dMin =  dStepVal;
					}


					// 2018.05.21 직접 체크하지 말고 이미 체크한 
					//     g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]값을 이용하도록 
					//     하기 코드를 코멘트 처리함.
					// Fault Count 구하기 -----------------------
					//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
					//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];
					//if (dStepVal < dLsl || dStepVal > dUsl)
					//	nFaultCount++;
	
				}
				else
				{
					// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
					if (step == nMinMaxInitStepLoc)		
						nMinMaxInitStepLoc++;						
				}
			}
		}

		// 2018.07.09 여기서 아예 계산안하고 g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]을 그냥 사용하기로하고 코멘트처리
		//nFaultCount += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
	}

	double 	dDiff = 0, dDiffPowerSum = 0, dVariance=0, dSigma=0;
	if (nCount)		// check device by zero  : Average
		dAvg = dSum / (double)nCount;

	if (nCount -1)	// check device by zero  : Variance
	{
		// 2. Calc Sigma   --------------------
		for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		{
			if (g_pvsaNetData[nLot][nNet][date] == NULL)
				continue;

			tupleSize = g_pvsaNetData[nLot][nNet][date]->size();	// date마다 tupleSize가 다름!
			for (tuple=0; tuple < tupleSize; tuple++)
			{
				stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
				for (step= 0; step < stepSize; step++)
				{
					if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step] != -1)
					{
						dDiff =  (*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step] - dAvg;
						dDiffPowerSum += (dDiff * dDiff);		
					}
				}
			}
		}
		dVariance = dDiffPowerSum / (double)(nCount -1);// 분산     : (val-avg)의 제곱의 총합을 (n - 1)으로 나눈다.
	}
	dSigma    = sqrt(dVariance);					// 표준편차 : 분산의 제곱근

	MyTrace(PRT_BASIC, "nTotal=%d, nCount=%d, dSum=%.2f, dAvg=%.2f, dVar=%.2f, dSigma=%.2f, dMin=%.2f, dMax=%.2f, waNetFaultCount[%d][%d]=%d\n", 
						nTotal, nCount, dSum, dAvg, dVariance, dSigma, dMin, dMax, 
						nLot, nNet, g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]);


	// 3. Summary Display   ------------------
	DisplayGrid_Summary(nLot, nNet, (-1), nCount, nTotal, dAvg, dSigma, dMin, dMax, g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]);


	UpdateData(FALSE);		// 변경 data 화면 반영

	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

}


// Lot 존재, Net= -1(미존재) , Date값이 존재한다면
// 1. lot, date를 key로 하고,  모든 net, 모든 time tuple을 sum 해서 summary grid  출력 
void CStatisticsDialog::Display_SumupLotDate(int nLot, int nDate)
{
	CString strTemp;

	// Lot 존재해야 하고, Date값이 존재하면서 0이 아니어야 한다.
	if ( (nLot < 0 || nLot >= MAX_LOT)
			|| (nDate < 0 || nDate >= MAX_DATE) ) 
	{
		strTemp.Format("Display_SumupLotDate(): nLot=%d(0<= Lot <%d), nDate=%d(0<= Date <%d) Range Over!\n", 
				nLot, MAX_LOT, nDate, MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// data를 업데이트하기 전에 지우기. 지워야 하므로 UpdateDate(True)는 필요 없음.
	m_editTupleNum = 0;
	m_editStepNum = 0;

	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.
	ClearGrid_Data();	
	m_GridSnap.InitMember();

	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.


	MyTrace(PRT_BASIC, _T("Display_SumupLotDate(): nLot=%d, nDate=%d\n"), nLot, nDate);


}

// Lot 존재, Net= -1, Date=All
// 1. Lot만 key로 해서 모든 net, 모든 date, 모든 time tuple을 sum해서 summary grid 출력
void CStatisticsDialog::Display_SumupLot(int nLot)
{
	CString strTemp;

	// Lot 존재해야 하고, Date값이 All이어야 한다.
	if (nLot < 0 || nLot >= MAX_LOT)
	{
		strTemp.Format("Display_SumupLot(): nLot=%d(0<= Lot <%d) Range Over\n", nLot, MAX_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// data를 업데이트하기 전에 지우기. 지워야 하므로 UpdateData(True)는 필요 없음.
	m_editTupleNum = 0;
	m_editStepNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.
	m_GridSnap.InitMember();

	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

		
	MyTrace(PRT_BASIC, _T("Display_SumupLot(): nLot=%d\n"), nLot);


}

// Lot 미존재, Net 미존재, Date = All
// 1. 모든 lot,  모든 net, 모든 date, 모든 time tuple을 sum해서 summary grid 출력.
void CStatisticsDialog::Display_SumupTotal()
{

	// data를 업데이트하기 전에 지우기. 지워야 하므로 UpdateDate(True)는 필요 없음.
	m_editTupleNum = 0;
	m_editStepNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.
	m_GridSnap.InitMember();


	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.


	MyTrace(PRT_BASIC, _T("Display_SumupTotal()\n"));

}


void CStatisticsDialog::OnCheckDataFaultOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);		// 화면 data를 가져온다.
	
	// 바뀐 m_bDataGridFaultOnly 값으로 Grid를 다시 그린다.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		
}




void CStatisticsDialog::OnButtonSaveStatCsv() 
{
	// TODO: Add your control notification handler code here
	//Save_StatLotDataFile();
	SaveStat_CsvFile(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate);
}


void CStatisticsDialog::OnButtonViewStatCsv() 
{
	// TODO: Add your control notification handler code here
	
	int nLot 		= m_nTree_CurrLot;
	int nNet 		= m_nTree_CurrNet;
	int nDate 		= m_nCombo_CurrDate -1; //  m_nCombo_CurrDate는 실제 Date +1 값

	CString strTemp;
	
	strTemp.Format(".\\Data\\%s_%s_Net%d_StatDataOut.csv", 
			g_sLotNetDate_Info.strLot[nLot], 
			(m_nCombo_CurrDate == DATE_ALL) ? "_" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
			nNet); 					
/*
 *  빠져 있어서 넣었는데 풀어서 테스트는 아직 못해 봄
	if(!FileExists(strTemp)) // 파일이 존재하지 않으면 에러 출력
	{ 	
		ERR.Set(FLAG_FILE_NOT_FOUND, strTemp); 
		ErrMsg();  ERR.Reset(); return; 
	}
*/


	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

	
}


// 1. 지금까지 insert 한 data를  write. 
// 2. data를 delete하고 file을 close  
void CStatisticsDialog::SaveStat_CsvFile(int nLot, int nNet, int nComboDate)
{
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;
	int 	nDate = nComboDate -1; //  m_nCombo_CurrDate는 실제 Date +1 값


	MyTrace(PRT_BASIC, "SaveStat_CsvFile(nLot=%d, nNet=%d, nDate=%d)\n", nLot, nNet, nDate);

	// 디버깅을 위한 파일 프린트 출력.
	::ZeroMemory(&fName, sizeof(fName));
	strTemp.Format(".\\Data\\%s_%s_Net%d_StatDataOut.csv", 
			g_sLotNetDate_Info.strLot[nLot], 
			(nComboDate == DATE_ALL) ? "_" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
			nNet);
	strcat(fName, strTemp);

	fp=fopen(fName,"wt");
	if(fp == NULL)
	{ 	
		strTemp.Format("fName=%s, err=%s", fName, strerror( errno ));
		ERR.Set(FLAG_FILE_CANNOT_OPEN, strTemp); 
		return; 
	}


	//-----------------------------------
	//  Summary Grid Data 출력
	
	// 4w Data의 Net, Data별 Avg, Sigma는 특정 Net, Date 클릭시에 생성되므로 따로 저장되어 있지 않음. 
	// 화면 출력을 위해서는 아래와 같이 별도로 저장해둔 snap data를 이용해서 출력한다. 
	// snap이 아니라면 화면출력을 위해서 다시 계산을 해야 함.  

	// 헤더 출력
	fprintf(fp, "Net, Date, Total, NgCount, Count, Average, Sigma, DataMin, DataMax, FaultCount, , ,TimeTuple#, Data#\n" );

	fprintf(fp, "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, , ,%d, %d\n",
				m_GridSnap.Summary.strNetName, 		// 0: Net Name
				m_GridSnap.Summary.strDate, 		// 1: Date
				m_GridSnap.Summary.strTotal, 		// 2: Total
				m_GridSnap.Summary.strNgCount, 		// 3: N/G Count
				m_GridSnap.Summary.strCount, 		// 4: Count

				m_GridSnap.Summary.strAvg, 			// 5: Average
				m_GridSnap.Summary.strSigma, 		// 6: Sigma
				m_GridSnap.Summary.strMin, 			// 7: Min
				m_GridSnap.Summary.strMax, 			// 8: Max
				m_GridSnap.Summary.strFault, 		// 9: Fault Count

				m_editTupleNum,				// edit box
				m_editStepNum);			// edit box

	//-----------------------------------
	//  Data Grid Data 출력


	fprintf(fp, "\n\n");
	if (m_bApplyULSL)
		fprintf(fp, "No, Date, Time, Pin1, Pin2, Pin3, Pin4, R, LSL(simul), USL(simul), Avg, Sigma(StDev), Min, Max, "); 
	else
		fprintf(fp, "No, Date, Time, Pin1, Pin2, Pin3, Pin4, R, LSL, USL, Avg, Sigma(StDev), Min, Max, "); 

	fprintf(fp, "Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11, Data12\n");

	for (int row=0; row < m_GridSnap.dataCount; row++)
	{
		fprintf(fp, "%d, %s, %s, %d, %d, %d, %d, ",
				(m_GridSnap.saData[row].nPrtTuple + 1),
				m_GridSnap.saData[row].strDate,
				m_GridSnap.saData[row].strTime,
				g_sLotNetDate_Info.waLotNetPin[nLot][nNet][0],
				g_sLotNetDate_Info.waLotNetPin[nLot][nNet][1],
				g_sLotNetDate_Info.waLotNetPin[nLot][nNet][2],
				g_sLotNetDate_Info.waLotNetPin[nLot][nNet][3]);


		fprintf(fp, "%s, %s, %s, %s, %s, %s, %s, ",
				m_GridSnap.saData[row].strRefR,
				m_GridSnap.saData[row].strLSL,
				m_GridSnap.saData[row].strUSL,
				m_GridSnap.saData[row].strTupleAvg,
				m_GridSnap.saData[row].strTupleSigma,
				m_GridSnap.saData[row].strTupleMin,
				m_GridSnap.saData[row].strTupleMax);


		int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
		for (int step=0; step < stepSize; step++)
			fprintf(fp, "%s, ", m_GridSnap.saData[row].strData[step]);

		fprintf(fp, "\n");

	}

	fclose(fp);

	strTemp.Format("\'Save to CSV\' completed.\n(%s)", fName);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
}

// 1. 지금까지 insert 한 data를  binary 파일을 open하여  write. 
// 2. data를 delete하고 file을 close  
void CStatisticsDialog::Save_StatLotDataFile()
{
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;


	MyTrace(PRT_BASIC, "Save_StatLotDataFile()\n");

	// 디버깅을 위한 파일 프린트 출력.
	::ZeroMemory(&fName, sizeof(fName));
	//strTemp.Format(".\\Data\\Lot%02d_DebugOut.csv", nLot);
	strTemp.Format(".\\Data\\Stat_DebugOut.csv");
	strcat(fName, strTemp);

	fp=fopen(fName,"wt");
	if(fp == NULL)
	{ 	
		strTemp.Format("fName=%s, err=%s", fName, strerror( errno ));
		ERR.Set(FLAG_FILE_CANNOT_OPEN, strTemp); 
		return; 
	}

#if 0 
	//-----------------------------------
	// p Chart를 위한 step data 출력 

	// Time 별 p chart data 출력하기  (문제있는 time을 찾기)
	//    1개 lot, date에 한해서(time 갯수가 같아야 하므로)  같은 time의 모든 net의 fault갯수를 세어본다.
	//    time별 fault 갯수를 평균내고 USL, LSL을 판단하여 넘어서는 OOC 값을 찾아내기 위함.
	
	// 같은 lot, 같은 date이므로 모든 net은 tupleSize가 같아야 한다. 이를 하나로 timeSize라고 하자.
	int netCount = 0;
	static short	waTimeFaultCount[MAX_TIME_FILE];
	static short	waTimeNgCount[MAX_TIME_FILE];
	static short	waTimeTotalCount[MAX_TIME_FILE];
	::ZeroMemory(waTimeFaultCount, sizeof(waTimeFaultCount));
	::ZeroMemory(waTimeNgCount, sizeof(waTimeNgCount));
	::ZeroMemory(waTimeTotalCount, sizeof(waTimeTotalCount));

	int lot, time, tupleSize;
	for (lot =0; lot < 1; lot++)		// Lot은 1개로 고정
	{
		//for (int date =0; date < g_sLotNetDate_Info.naLotDateCnt[lot]; date++)
		for (int date =0; date < 1; date++)	// Date 1개로 고정
		{

			for (int net =0; net < MAX_NET_PER_LOT; net++)
			{
				if(g_pvsaNetData[lot][net][date] == NULL)	// 없는 date면 skip
					continue;

				if (g_sLotNetDate_Info.naLotNet[lot][net] == -1)	// 없는 Net이면 skip
					continue;

				netCount++;

				tupleSize = g_pvsaNetData[lot][net][date]->size();		// time 갯수

				for (time=0; time < tupleSize; time++)
				{
					double dLsl = (*g_pvsaNetData[lot][net][date])[time].dLsl;
					double dUsl = (*g_pvsaNetData[lot][net][date])[time].dUsl;

					int stepSize = g_sLotNetDate_Info.naLotStepCnt[lot];
					for (int step= 0; step < stepSize; step++)
					{
						double dStepVal = (*g_pvsaNetData[lot][net][date])[time].daStep[step];
						if ((*g_pvsaNetData[lot][net][date])[time].daStep[step] == -1)	//NG
							waTimeNgCount[time]++;
						else
						{
							if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
								waTimeFaultCount[time]++;
						}
					}

				}
				//fprintf(fp, "Lot=%d(%s), Net=%d, Date=%d(%s), tupleSize=%d,  \n", 
				//		lot, g_sLotNetDate_Info.strLot[lot], net, 
				//		date, g_sLotNetDate_Info.strLotDate[lot][date],
				//		tupleSize );
			}	
		}
	}

	for (lot =0; lot < 1; lot++)		// Lot은 1개로 고정
	{
		int stepSize = g_sLotNetDate_Info.naLotStepCnt[lot];
		for (time =0; time < tupleSize; time++)
		{
			CString strTime;
			strTime.Format("%02d:%02d:%02d", 
					(*g_pvsaNetData[lot][1][0])[time].statTime.hour,
					(*g_pvsaNetData[lot][1][0])[time].statTime.min,
					(*g_pvsaNetData[lot][1][0])[time].statTime.sec);

			waTimeTotalCount[time] = (netCount * stepSize);		// net 갯수 * step 갯수
			fprintf(fp, "Time(%s)=,%d, FaultCount=,%d, Count=,%d, Total=%d, NgCount=%d\n", 
							strTime, time, 
							waTimeFaultCount[time], (waTimeTotalCount[time] - waTimeNgCount[time]),
							waTimeTotalCount[time], waTimeNgCount[time]);
		}	
	}
#endif



#if 0   
	// Net 별 p chart data 출력하기  (문제있는 Net을 찾기)
	//for (int lot =0; lot < g_sLotNetDate_Info.nLotCnt; lot++)
	for (int lot =0; lot < 1; lot++)		// Lot은 1개로 고정

	{
		for (int net =0; net < MAX_NET_PER_LOT; net++)
		{
			if (g_sLotNetDate_Info.naLotNet[lot][net] == -1)
				continue;

			// Total = StepCount* tupleSize,   Count = Total - Ng
			fprintf(fp, "Lot=%d,(%s), Net=,%d, FaultCount=,%d, Count=,%d, NgCount=,%d, TotCount=,%d\n", 
							lot, g_sLotNetDate_Info.strLot[lot], net, 
							g_sLotNetDate_Info.waNetFaultCount[lot][net],
							(g_sLotNetDate_Info.waNetTotalCount[lot][net] - g_sLotNetDate_Info.waNetNgCount[lot][net]),
							g_sLotNetDate_Info.waNetNgCount[lot][net],
							g_sLotNetDate_Info.waNetTotalCount[lot][net]);
		}	
	}
#endif


#if 0
	// Net별로 출력하기 1
	for (int lot =0; lot < g_sLotNetDate_Info.nLotCnt; lot++)
	{
		for (int net =0; net < MAX_NET_PER_LOT; net++)
		{
			if (g_sLotNetDate_Info.naLotNet[lot][net] == -1)
				continue;

			//for (int date =0; date < MAX_DATE; date++)
			for (int date =0; date < g_sLotNetDate_Info.naLotDateCnt[lot]; date++)
			{
				if(g_pvsaNetData[lot][net][date] != NULL)
				{
					int tupleSize = g_pvsaNetData[lot][net][date]->size();
					fprintf(fp, "Lot=%d, (%s), Net=%d, Date=%d, (d_%s), tupleSize=%d : \n", 
							lot, g_sLotNetDate_Info.strLot[lot], net, 
							date, g_sLotNetDate_Info.strLotDate[lot][date],
							tupleSize );
	
					for (int tuple=0; tuple < tupleSize; tuple++)
					{
						fprintf(fp, "    waPin, %d, %d, %d, %d,", 
							g_sLotNetDate_Info.waLotNetPin[lot][net][0],
							g_sLotNetDate_Info.waLotNetPin[lot][net][1],
							g_sLotNetDate_Info.waLotNetPin[lot][net][2],
							g_sLotNetDate_Info.waLotNetPin[lot][net][3] );
						fprintf(fp, "    time, %02d:%02d:%02d: ,", 
								(*g_pvsaNetData[lot][net][date])[tuple].statTime.hour,
								(*g_pvsaNetData[lot][net][date])[tuple].statTime.min,
								(*g_pvsaNetData[lot][net][date])[tuple].statTime.sec);

						fprintf(fp, "    waStep");
						int stepSize = g_sLotNetDate_Info.naLotStepCnt[lot];
						for (int step=0; step < stepSize; step++)
							fprintf(fp, ", %.2f", (*g_pvsaNetData[lot][net][date])[tuple].daStep[step] );

					}
	
				}
			}
		}	
	}
#endif

	fclose(fp);


	// 2018.04.24 모든 Lot을 메모리에 올리기로 하였으므로 하기 date는 Stat Dialog 종료이전엔 Clear하면 안 된다.
	//            코드 수행을 코멘트로 막음.
	//Stat_deleteAllNetData();


}


#if 0
// 1. binary 파일을 open하여  Memory에 load
void CStatisticsDialog::Load_StatLotDataFile(int nLot)
{

}
#endif

// 강제로 SimulUsl을 Off 시킨다.  
// Simulation 도중에 다른 Lot, Net, Date를 선택했을 때에 사용한다.
void CStatisticsDialog::CheckOff_SimulUlsl() 
{
	// 이미 False라면 또 Off 할 필요 없다.
	if (m_bSimulateULSL == FALSE)
		return;

	// 같은 net을 또 클릭한 경우라면 Simulation Off 할 필요 없다.
	if (m_nPrevSimulLoT == m_nTree_CurrLot && 
		m_nPrevSimulNet == m_nTree_CurrNet && 
		m_nPrevSimulDate == m_nCombo_CurrDate)
		return;
	

	// Lot, Net, Date 중의 하나가 변경되었다.
	CString strTemp;
	strTemp.Format("previous Lot=%d, Net=%d, Date=%d was in Simulation Process. Simulation Check Off now.\n", 
				m_nPrevSimulLoT, m_nPrevSimulNet, m_nPrevSimulDate);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);

	// Current Lot, Net, Date를 keep하고 Current를 일시적으로 prevSimul로 바꾼다음
	// SimulUsl Off 작업을 진행한다.  
	int keepedLot = m_nTree_CurrLot;
	int keepedNet = m_nTree_CurrNet;
	int keepedDate = m_nCombo_CurrDate;
	HTREEITEM	keepedNode = m_hSelectedNode;

	m_nTree_CurrLot = m_nPrevSimulLoT;
	m_nTree_CurrNet = m_nPrevSimulNet;
	m_nCombo_CurrDate = m_nPrevSimulDate;
	m_hSelectedNode = m_hPrevSimulSelNode;


	m_bSimulateULSL = FALSE;		// Simul Check box 상태를 강제로 off한다.
	UpdateData(FALSE);				// UI에도 반영
	OnCheckSimulUlsl();				// simulate ULSL Check box off 기능을 처리한다.

	// Simul Off가 완료되었으므로  원래의 Current Lot, Net, Date로 원상복구한다.
	m_nTree_CurrLot = keepedLot;
	m_nTree_CurrNet = keepedNet;
	m_nCombo_CurrDate = keepedDate;
	m_hSelectedNode = keepedNode;


}

void CStatisticsDialog::OnCheckSimulUlsl() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// 선택된 Net이 없으면 무효처리한다.  
	CString strTemp;
	if (  m_hSelectedNode == NULL			// 선택한 Tree Node가 있는지 체크
			|| (m_nTree_CurrLot < 0 || m_nTree_CurrLot >= MAX_LOT)  			// Lot 존재하는지 체크
			|| (m_nTree_CurrNet < 0 || m_nTree_CurrNet >= MAX_NET_PER_LOT ) ) 	// Net 존재하는지 체크
	{
		strTemp.Format("Please select any Net# in the tree.\n : OnCheckSimulUlsl()", 
					m_nTree_CurrNet, MAX_LOT, m_nTree_CurrNet, MAX_NET_PER_LOT );
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 

		// m_bSimulateULSL should be FALSE
		m_bSimulateULSL = FALSE;
		UpdateData(FALSE);
		return;
	}


	// Simulate 관련 컨트롤 상태 변경 
	if (m_bSimulateULSL)
	{
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(TRUE);

		UpdateData(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(FALSE);
		m_bApplyULSL = FALSE;
		m_editStrLSL = _T("");
		m_editStrUSL = _T("");
		m_dSimulUsl = -1;
		m_dSimulLsl = -1;
		m_nPrevSimulLoT = -1;
		m_nPrevSimulNet = -1;
		m_nPrevSimulDate = DATE_ALL;
		m_hPrevSimulSelNode = NULL;

		UpdateData(FALSE);
	
		//------------------------------------------------------------------
		// Simul Off 일 경우에는 Apply Off와 동일한 action을 취해줘야 한다.


		// 현재 Tree에 선택된 Lot, Net에 대해 Fault 체크를 다시 한다.
		Tree_CheckNetFault(m_hSelectedNode, m_nTree_CurrLot);

		// LSL(simul), USL(simul)로 바뀐 data grid 헤더를 다시 LSL, USL로 원상복구한다.
		Display_DataGridHeader();		
	
		// 원상복구된  Fault 기준으로 다시 Display한다.
	 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

		// 다시 체크한 Fault 기준을 FR Rank창, pChart 창에 반영한다.
		::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
		::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	}


}



void CStatisticsDialog::OnCheckApplyUlsl() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	
	// Apply On 상태에서는 더 이상 USL/LSL edit가 안되게 disable한다.
	if (m_bApplyULSL)
	{
		double dTempUSL, dTempLSL;

		//g_sLotNetDate_Info.daLotNetUsl[m_nTree_CurrLot][m_nTree_CurrNet] = (double)atof((char*)m_editStrUSL.GetBuffer(10));
		//g_sLotNetDate_Info.daLotNetLsl[m_nTree_CurrLot][m_nTree_CurrNet] = (double)atof((char*)m_editStrLSL.GetBuffer(10));
		
		//m_dSimulUsl = (double)atof((char*)m_editStrUSL.GetBuffer(10));
		dTempUSL = (double)atof((char*)m_editStrUSL.GetBuffer(10));
		m_editStrUSL.ReleaseBuffer();

		//m_dSimulLsl = (double)atof((char*)m_editStrLSL.GetBuffer(10));
		dTempLSL = (double)atof((char*)m_editStrLSL.GetBuffer(10));
		m_editStrLSL.ReleaseBuffer();

		if (dTempUSL < dTempLSL)		// 에러  처리 : USL은 LSL보다 작을 수 없다.
		{
			CString strTemp;
			strTemp.Format("USL can not be made smaller than LSL.\n\n");
			ERR.Set(USER_ERR, strTemp); 
			ErrMsg();  ERR.Reset();

			// 에러로 입력된 USL, LSL str을 빈칸으로   -> 이건 그냥 놔두자. User가 잘못을 인식하도록
			//m_editStrLSL = _T("");
			//m_editStrUSL = _T("");
			
			// Apply 버튼을  Off로 되돌리고 리턴.
			m_bApplyULSL = FALSE;
			UpdateData(FALSE);
			return;
		}

		m_dSimulUsl = dTempUSL;
		m_dSimulLsl = dTempLSL;

		GetDlgItem(IDC_EDIT_USL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(FALSE);

		m_nPrevSimulLoT = m_nTree_CurrLot;
		m_nPrevSimulNet = m_nTree_CurrNet;
		m_nPrevSimulDate = m_nCombo_CurrDate;
		m_hPrevSimulSelNode = m_hSelectedNode;
	}

	// Apply Off가 되면 edit 박스와 simul 값을 모두 초기화한다.
	else
	{
		m_editStrLSL = _T("");
		m_editStrUSL = _T("");
		m_dSimulUsl = -1;
		m_dSimulLsl = -1;
		m_nPrevSimulLoT = -1;
		m_nPrevSimulNet = -1;
		m_nPrevSimulDate = DATE_ALL;
		m_hPrevSimulSelNode = NULL;


		// Simul Off시와 동일하게 설정 (OnCheckSimulUlsl(): m_bSimulateULSL는 FALSE 일 때)
		m_bSimulateULSL = FALSE;
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);

	// 현재 Tree에 선택된 Lot, Net에 대해 Fault 체크를 다시 한다.
	Tree_CheckNetFault(m_hSelectedNode, m_nTree_CurrLot);

	// LSL(simul), USL(simul)로 data grid 헤더도 바꿔준다.
	Display_DataGridHeader();		

	// 다시 체크한 Fault 기준으로 다시 Display한다.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

	// 다시 체크한 Fault 기준을 FR Rank창, pChart 창에 반영한다.
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
}


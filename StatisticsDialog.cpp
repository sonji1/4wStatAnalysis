// StatisticsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ACE400Statistics.h"
#include "StatisticsDialog.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	m_strSelect = _T("");
	m_editTupleNum = 0;
	m_editSampleNum = 0;
	m_bDataGridFaultOnly = FALSE;
	//}}AFX_DATA_INIT
	

}


void CStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatisticsDialog)
	DDX_Control(pDX, IDC_COMBO_DATE, 			m_comboDate);
	DDX_Control(pDX, IDC_TREE, 					m_treeCtrl);
	DDX_Control(pDX, IDC_GRID_SUMMARY, 			m_gridSummary);
	DDX_Control(pDX, IDC_GRID_DATA, 			m_gridData);
	DDX_Text(pDX, IDC_EDIT_SELECT, 				m_strSelect);
	DDX_Text(pDX, IDC_EDIT_TUPLE_NUM, 			m_editTupleNum);
	DDX_Text(pDX, IDC_EDIT_SAMPLE_NUM, 			m_editSampleNum);
	DDX_Check(pDX, IDC_CHECK_DATA_FAULT_ONLY, 	m_bDataGridFaultOnly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStatisticsDialog, CDialog)
	//{{AFX_MSG_MAP(CStatisticsDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, 		OnSelchangedTree)
	ON_CBN_SELCHANGE(IDC_COMBO_DATE, 			OnSelchangeComboDate)
	ON_BN_CLICKED(IDC_BUTTON_LOAD4W_DATA, 		OnButtonLoad4wData)
	ON_BN_CLICKED(IDC_BUTTON_LOAD4W_DATA_SINGLE, OnButtonLoad4wData_SingleLot)
	ON_EN_CHANGE(IDC_EDIT_USL, 					OnChangeEditUsl)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_GRID, 		OnButtonClearGrid)
	ON_BN_CLICKED(IDC_CHECK_DATA_FAULT_ONLY, 	OnCheckDataFaultOnly)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_STAT_CSV, 	OnButtonSaveStatCsv)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_STAT_CSV, 	OnButtonViewStatCsv)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDialog message handlers

BOOL CStatisticsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	if (InitMember() == FALSE)
		return FALSE;

	if (InitView() == FALSE)
		return FALSE;

	g_sFile.Init();

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

	
	MyTrace(PRT_BASIC, "\"4W Statistics SW\" Started...\n\n" );

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
		//strTemp.Format("The 'Load 4W Data to Tree' button will launch automatically to load \"%s\" files.", g_sFile.ACE400_4WDataDir);
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



BOOL CStatisticsDialog::InitMember()
{

	//----------------------------
	// Tree Current data 초기화 
	m_nTree_CurrLot = -1;	
	m_nTree_CurrNet = -1;	
	m_nCombo_CurrDate = DATE_ALL;	
	m_bDataGridFaultOnly = FALSE;

	//----------------------------
	// 4WData Load 관련 초기화
	//m_nLoad_CurrLot = 0;

	//----------------------------
	// Grid 관련 멤버 변수 초기화
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_DATA_GRID_COL;
	m_nRows = MAX_DATA_GRID_ROW;
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

	return TRUE;
}


BOOL CStatisticsDialog::InitView()
{
	int i;

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

	// Summary Grid header 설정
    m_gridSummary.SetItemText(0, SUM_COL_NET, "Net");
//	m_gridSummary.SetColumnWidth(SUM_COL_NET, 60);

    m_gridSummary.SetItemText(0, SUM_COL_DATE, "Date");
//	m_gridSummary.SetColumnWidth(SUM_COL_DATE, 80);
	
	m_gridSummary.SetItemText(0, SUM_COL_TOTAL, "Total\n(Tuple*Data)");
//	m_gridSummary.SetColumnWidth(SUM_COL_TOTAL, 80);

    m_gridSummary.SetItemText(0, SUM_COL_NG, "NG Count");
//	m_gridSummary.SetColumnWidth(SUM_COL_NG, 70);
	
    m_gridSummary.SetItemText(0, SUM_COL_COUNT, "Count\n(Total-NG)");
//	m_gridSummary.SetColumnWidth(SUM_COL_COUNT, 80);

    m_gridSummary.SetItemText(0, SUM_COL_AVG, "Average");
//	m_gridSummary.SetColumnWidth(SUM_COL_AVG, 80);

    m_gridSummary.SetItemText(0, SUM_COL_SIGMA, "Sigma");
//	m_gridSummary.SetColumnWidth(SUM_COL_SIGMA, 80);

    m_gridSummary.SetItemText(0, SUM_COL_DATAMIN, "Min");
//	m_gridSummary.SetColumnWidth(SUM_COL_DATAMIN, 80);

    m_gridSummary.SetItemText(0, SUM_COL_DATAMAX, "Max");		
//	m_gridSummary.SetColumnWidth(SUM_COL_DATAMAX, 80);

    m_gridSummary.SetItemText(0, SUM_COL_FAULT, "Fault\nCount");		
//	m_gridSummary.SetColumnWidth(SUM_COL_FAULT, 70);
	
	m_gridSummary.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조

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

	// Data Grid Header 설정
	
	// 0     1      2     3     4     5     6     7     8     9     10    11    12    13     14      ....   25
	// No,  Date, Time,  Pin1, Pin2, Pin3, Pin4,  R,   LSL,  USL,  Avg, Sigma, Min, Max,  Data1, .... Data12
	char dataHeader[NUM_DATA_GRID_COL][20] = {"No", "Date", "Time", "Pin1", "Pin2", "Pin3", "Pin4",  "R",  "LSL", "USL",
													"Average", "Sigma",   "DataMin", "DataMax"    , 
													"Data1", "Data2", "Data3", "Data4", "Data5", "Data6",
													"Data7", "Data8", "Data9", "Data10", "Data11", "Data12" };

	int dataColWidth[NUM_DATA_GRID_COL] =    {40,    68,     60,   45,     45,     45,     45,     70,    70,    70,
													 70,         70,         70,        70,
													 70,         70,         70,        70,         70,       70,
		                                             70,         70,         70,        70,         70,       70 };

	for (i=0; i < NUM_DATA_GRID_COL; i++)
	{
		m_gridData.SetColumnWidth(i, dataColWidth[i]);
		m_gridData.SetItemText(0, i, dataHeader[i]);
	}



	//----------------------------
	// tree 초기화 
	
	InitTree();

	// Tree 이미지리스트 설정
	m_ImageList.Create(IDB_BITMAP1, 16, 	// 이미지의 가로, 세로 길이
									2,		// 이미지 관리 메모리 부족시 늘려야 할 메모리크기 
					RGB(255,255,255));		// 화면 출력 컬러 (투명)
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	UpdateData(FALSE);

	return TRUE;
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
	Load_Log4wDir();

}
void CStatisticsDialog::Load_Log4wDir() 
{
	// TODO: Add your control notification handler code here
	
	MyTrace(PRT_BASIC, "Load 4wData...\n" );

	CString strTemp;
	strTemp.Format("Load \"%s\" files.\n\nIt may take a few minutes. Please wait.\n\n\nYou can load up to maximum 850MB files.", g_sFile.ACE400_4WDataDir);
	int ret = AfxMessageBox(strTemp, MB_OKCANCEL|MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);

	if (ret == IDCANCEL)
		return;

	ClearGrid(); // data Grid, summary Grid, edit박스 등 UI를 초기화한다.


	Stat_deleteAllNetData();
	InitMember();
	InitTree();					// root부터 Tree를 다시 생성.	
	

	// 측정 raw data 파일을 read한다.
	Load_Log4wDir(g_sFile.ACE400_4WDataDir);

	
	// Load한 모든 Tree >Lot > Net에 대해 Fault 체크를 수행한다.
	Tree_CheckAll_LotNetFaults();
	
	strTemp.Format("Load_Log4wDir() for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", g_sFile.ACE400_4WDataDir);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "Load_Log4wDir() for\"%s\" Completed. \n", g_sFile.ACE400_4WDataDir);

	int memSize = GetProcessWorkingSetSize(); 
	MyTrace(PRT_BASIC, "                memSize = %d\n", memSize);
	MyTrace(PRT_BASIC, "        m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "sizeof(LotNetDate_Info) = %d\n", sizeof(LotNetDate_Info));
	MyTrace(PRT_BASIC, "    sizeof(statNetData) = %d\n", sizeof(statNetData));
	MyTrace(PRT_BASIC, "\n");


	// 2018.04.27 모든 Lot이 read 완료 되었으면  현재  lot정보를 파일로 저장한다. 
	//Save_StatLotDataFile(m_nLoad_CurrLot);		// lot별로 file을 저장하는 기능은 현재 봉인함.
	
	// 2018.05.08 csv 파일 저장 버튼 클릭시 저장하도록 수정하고 여기서는 막는다.
	//Save_StatLotDataFile();		
}

// "D:Log4w" 디렉토리를 조사하여 4w Raw Data를 Load한다.
#define		MAX_LOT_ERR_COUNT		7
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
			//               sample count가 달라서 ACE400_3333은 처리를 하지 않고 건너뛰고 그러다 로직이 꼬여서 죽음.
			//               맨뒤의 숫자 네개 떼어내는 기능을 위와 같이 막아서  다른 lot으로 처리하기로 함.
			lotName = dirName.Mid(9, (length -9));
#endif

			// 빈 lot이 추가되는 것을 막기 위해 Insert할 디렉토리가 빈 디렉토리인지 검사한다.  (자원절약)
			if(CheckEmptyDirectory(dirPath) == TRUE)		
			{
				strTemp.Format("Load_Log4wDir(): %s is Empty Directory. Can't do InsertItem.\n\n", dirPath);
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

				nLotErrCnt++;
				if (nLotErrCnt < MAX_LOT_ERR_COUNT) 
					continue;	// 다음번 lot이 기존 lot과 이름이 같을 수도 있으므로 break하지 않고 continue 처리
				break;			// Lot Error가 7회 이상 반복되면 그냥 break. 
			}

			dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
			if (dateId == -1)	// date range를 넘어선다면 load하지 않는다. 
			{
				strTemp.Format("Load_Log4wDir(): getLotDateId(%s/%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
						lotName, dateName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

				nDateErrCnt++;
				if (nDateErrCnt < MAX_DATE_ERR_COUNT) 
					continue;	// 다음번 lot이 있을 수 있으므로 break는 하지 않고 continue 처리
				break;			// Date Error가 3회 이상 반복되면 그냥 break
			}


			// Tree에 'Lot' item을 추가한다.  
			// 기존재하는 item이라면 hItem이 NULL일 수 있음.
			HTREEITEM  hItemInserted;
			hItemInserted = Tree_InsertLotItem(lotName, dirPath, lotId, dateId);			
			if (hItemInserted != NULL)		 
			{
				// 추가된 item
				MyTrace(PRT_BASIC, _T("%s: Inserted to Tree.\n"), (LPCTSTR)lotName);
			}

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

		// sample이 아닌 정상 4W Data file이 하나라도 있으면 FALSE 리턴
		return FALSE;
	}
	if (nFile == 0)
		return TRUE;

	return FALSE;
}


// CTreeCtrl에 입력받은 string의 Tree Item을 추가한다.
HTREEITEM CStatisticsDialog::Tree_InsertLotItem(LPCTSTR pStrInsert, LPCTSTR pStrPath, int nLot, int nDate)
{
	// TODO: Add your control notification handler code here
	HTREEITEM 	hItemFind= NULL, hLotItem = NULL;
	int			nLevel;
	CString		strTemp;

	HTREEITEM   hItemGet = NULL;	// return 값

	// Insert할  텍스트가 비어있나 검사한다.
	if(pStrInsert == "")
	{
		strTemp.Format("Tree_InsertLotItem(): pStrInsert is NULL, Can't do InsertItem.");
		ERR.Set(SW_LOGIC_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return NULL;

	}



	// 추가해야할 strInsert(Lot Name)가 현재 branch에 존재하는지 검사한다.
	// 새로운 Lot Name이라면 지정한 정보의 Lot 노드를 추가한다.
	//if ((hItemFind = Tree_FindStrItem(&m_treeCtrl, m_hRoot, pStrInsert)) == NULL)	// 새로운 Node(Lot) 
	
	// 새로운 Lot인지 체크
	hItemFind = Tree_FindLotItem(&m_treeCtrl, m_hRoot, pStrInsert);
	if (hItemFind != NULL) 				// Found: 기존에 존재하는 노드(Lot)
	{
		// 같은 이름의 노드(Lot)가 여러번 추가되지 않도록 return 값을 NULL로 설정한다.
		hItemGet = NULL;		
		hLotItem = hItemFind;	// Found이므로 찾아낸 hItemFind을 hLotItem으로 할당.

		MyTrace(PRT_BASIC, "%s Node already exist! use existing one.\n\n", pStrInsert);

	}
	else	//if (hItemFind == NULL)	// Not Found:
	{
		// Tree InsertItem을 그대로 이용하여 root에 새로운 Lot Node를 추가
		hItemGet = m_treeCtrl.InsertItem(pStrInsert, m_hRoot); 
		m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
		hLotItem = hItemGet;	// 새로 생성한 hItemGet을 hLotItem으로 할당.

		// 이 지점에서 Insert된 dir의 dirName, dirPath, hLotItem handle 값을 저장해야 할 수 있다.
		// 추후 진짜 필요하다면 저장하자. 

	}


#if 0
	// Lot번호가 이전의 Lot과 다르다면, 지금까지 저장한 lot정보를 파일로 이동하여 저장하고  메모리를 clear한다.
	// 새로운 Lot번호의 파일이 이미 존재한다면 메모리에 해당 파일을 Open해서 메모리에 새로운 Lot의 데이터를 로드한다.
	if (nLot != m_nLoad_CurrLot)
	{
		Save_StatLotDataFile(m_nLoad_CurrLot);

		//Load_StatLotDataFile(nLot);	
		m_nLoad_CurrLot = nLot;		

	}

	// Lot 번호가 이전 Lot과 같은데, 새로운 노드인 경우 체크.
	else 
	{ 	
		// 첫번째 Lot이라면 새노드라도 nLot과 m_nLoad_CurrLot이 같을 수 있음.
		// 그 외의 경우는 새노드라면 nLot을 이전 단계에서 새로 받아 왔어야 함.
		if (hItemFind == NULL && nLot != 0)
		{
			strTemp.Format("Tree_InsertLotItem(): pStrInsert=%s is New Node in Tree, But Not New LotId(%d). Check SW Logic!\n",
									pStrInsert, nLot);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); return NULL; 
		}
	}
#endif

	// 신규 Node, 기존재하는 Node 모두 하위 디렉토리의 Net Data를 Load한다.
	nLevel = Tree_GetCurrentLevel(hLotItem);
	if (nLevel == 2)	// Lot 디렉토리라면 하위의 Net Data File을 Open해서 Load
	{
		Tree_LoadLot_4WFiles(pStrPath, hLotItem, nLot, nDate);
		MyTrace(PRT_BASIC, "%s nLot=%d, nDate=%d: Tree_LoadLot_4WFiles... \n\n", pStrPath, nLot, nDate);

	}
	
	return hItemGet;
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
	CString		nodeName = "";


	hItemName = pTree->GetItemText(hItem);

	// 입력받은 str이 hItem 자신의 str이면 Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		MyTrace(PRT_BASIC, "Tree_FindLotItem2(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// 못 찾았으면 
	else 
	{
		// Lot 레벨에서만  찾아야 하므로, Child까지 내려갈 필요 없다.

		// 다음 형제노드를 찾는다. (형제노드 recursive 호출)
		hItemSibling = pTree->GetNextSiblingItem( hItem );
		if ( hItemFind==NULL && hItemSibling )
		{
			nodeName = m_treeCtrl.GetItemText(hItemSibling);
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
/*
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
*/


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
void CStatisticsDialog::Tree_LoadLot_4WFiles(LPCTSTR pParentPath, HTREEITEM hParentLot, int nLot, int nDate)
{
	CFileFind 	finder;

	// build a string with wildcard
	CString 	strWildcard(pParentPath);
	strWildcard += _T("\\*.*");

	// start working for files
	BOOL bWorking = finder.FindFile(strWildcard);

	int nFile = 0;
	while (bWorking)
	{
		if (nFile >= MAX_TIME_FILE)
			break;		

		// 2018.06.18 Tree_LoadLot_4WFiles()에서 nNetDataCount >= MAX_NET_DATA 여서 빠져나온 경우 ,
		//            더 이상 다른 Time File도 찾지 않도록 아래 코드 추가함.
		if (m_nNetDataCount >= MAX_NET_DATA)
			break;

		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
			continue;
		
		CString	dataFileName = finder.GetFileName();
		CString dataFilePath = finder.GetFilePath();
		Load_4wDataFile(dataFilePath, dataFileName, hParentLot, nLot, nDate);

		nFile++;
	
	}

	finder.Close();

	return;

}

#define 	MAX_NETFILE_COL		20
// 1. 4W Data File을 Open하고 Net별 Data를 read하여 메모리 구조체에 Load
// 2. 새로운 Net 번호인지 확인하고 새로운 Net 번호라면, hParentLot Tree에 Net Item을 Insert 
void CStatisticsDialog::Load_4wDataFile(CString dataFilePath, CString dataFileName, HTREEITEM hParentLot, int nLot, int nDate)
{

	FILE 	*fp; 
	char  	fName[200]; 		
	char	str[1024];			// fgets()로 라인째 읽어 오기 위한 buffer

	// 2018.05.25: file name에 'Sample'이 포함되어 있다면.  
	// Auto sampling을 위해 일시적으로 생성된 파일이므로 분석 data에서 제외한다.  
	if (dataFileName.Find("Sample") != -1)
	{
		MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
		return;
	}


	::ZeroMemory(&fName, sizeof(fName));
	strcat(fName, dataFilePath);

	if(!FileExists(fName)) 
	{ 	ERR.Set(FLAG_FILE_NOT_FOUND, fName); 
		ErrMsg();  ERR.Reset(); return; }

	fp=fopen(fName,"rt");
	if(fp == NULL)
	{ 	ERR.Set(FLAG_FILE_CANNOT_OPEN, fName); 
		ErrMsg();  ERR.Reset(); return; }



	CString strTemp, strTime;
	int 	row=0;
	int 	nNet, i, nNet2;
	char	strWord[300];
	char 	strWord2[3][200];
	char 	strSample[NUM_STAT_SAMPLE*2][200];
	int		nFileSample = 0;

	//------------------------------
	//  4W Raw Data 파일 헤더 Read 
	::ZeroMemory(str, sizeof(str));
	::ZeroMemory(&strSample, sizeof(strSample));
	fgets(str, sizeof(str), fp);		// 파일 헤더 read.  파일 헤더는 라인째 통째로 읽는다.

	// 헤더에서 data 의 갯수를 파악하기 위해 "S=" 의 갯수를 카운트한다.
	sscanf(str, "    ,  Pin1 ,Pin2 ,Pin3 , Pin4 ,  Min., Ref., Max.,%s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   %s   ", 
			strSample[0], strSample[1], strSample[2], strSample[3], strSample[4], strSample[5],
			strSample[6], strSample[7], strSample[8], strSample[9], strSample[10], strSample[11]);
	for (i = 0; i < NUM_STAT_SAMPLE; i++)
	{
		strTemp = strSample[i];
		if (strTemp.Left(2) == "S=")		
			nFileSample++;
	}
	if (nFileSample != g_sLotNetDate_Info.naLotSampleCnt[nLot])
	{
		if (g_sLotNetDate_Info.naLotSampleCnt[nLot] == 0)	// 초기값이면 nFileSample 값으로  바꾼다.
			g_sLotNetDate_Info.naLotSampleCnt[nLot] = nFileSample;			

		// 같은 nLot 안에서는 nFileSample의 값이 모두 같아야 한다.
		else
		{
			strTemp.Format("Load_4wDataFile(): nFileSample=%d is not same with g_sLotNetDate_Info.naLotSampleCnt[%d]=%d!", 
					nFileSample, nLot, g_sLotNetDate_Info.naLotSampleCnt[nLot]);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 
			return;
		}

	}


	statNetData netData;		// 한번 정의해서 최대 5000번까지 재활용.
								// Stat_insertNetData()에서 실제 data로 copy가 일어난다.

	// data file name에서 time 획득.
	strTime = dataFileName.Mid(15, 6);
	netData.statTime.hour = atoi(strTime.Left(2));		// 맨앞의 2글자
	netData.statTime.min  = atoi(strTime.Mid(2, 2));	// 3번째부터 2글자 
	netData.statTime.sec  = atoi(strTime.Mid(4, 2));	// 5번째부터 2글자. 



#ifdef DP_SAMPLE
	netData.sampleSize = nFileSample;
	netData.pdaSample  = new double[nFileSample];	// nFileSample개의 double 메모리를 확보한다.
#endif


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


		// loop를 돌 때 마다 아래 버퍼를 
	#ifdef DP_SAMPLE
		::ZeroMemory(netData.pdaSample, (sizeof(double) * nFileSample) );
	#else
		::ZeroMemory(netData.daSample, sizeof(netData.daSample));
	#endif
		::ZeroMemory(&strSample, sizeof(strSample));
		sscanf(str, "%s %d, %d, %d, %d, %d, %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
				strWord, &nNet2, 
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][0],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][1],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][2],
				&g_sLotNetDate_Info.waLotNetPin[nLot][nNet][3],
				&strWord2[0],  
				&strWord2[1],  
				&strWord2[2], 
				&strSample[0], &strSample[1], &strSample[2],   &strSample[3], 
				&strSample[4], &strSample[5], &strSample[6],   &strSample[7], 
				&strSample[8], &strSample[9], &strSample[10],  &strSample[11],
				&strSample[12], &strSample[13], &strSample[14],   &strSample[15], 
				&strSample[16], &strSample[17], &strSample[18],   &strSample[19], 
				&strSample[20], &strSample[21], &strSample[22],  &strSample[23] 
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
		int	   nSample = 0;
		for (i = 0; i < nFileSample; i++)
		{
			// NG 값 체크
			if (strcmp("NG", strSample[nSample]) == 0)
			{
				nSample++;			// NG는 skip 한다.

				// NG 999999.00 인 경우에는 진짜 NG 처리
				if (strcmp("999999.00,", strSample[nSample]) == 0)
				{

				#ifdef DP_SAMPLE
					netData.pdaSample[i] = (double)(-1);
				#else
					netData.daSample[i] = (double)(-1);
				#endif
					nSample++;		// 999999.00 은 -1로 처리
				}

				// NG 64.28 과 같이  숫자가 뒤에 오는 경우
				// NG가 이전 값에 대한 에러 표시 이므로 숫자를  정상처리한다.
				else
				{
				#ifdef DP_SAMPLE
					netData.pdaSample[i] = (double)atof(strSample[nSample]);
				#else
					netData.daSample[i] = (double)atof(strSample[nSample]);
				#endif
					nSample++;		
				}
			}

			// NG 없이 999999.00 만 있는 경우  NG 처리
			else if (strcmp("999999.00,", strSample[nSample]) == 0)
			{
			#ifdef DP_SAMPLE
				netData.pdaSample[i] = (double)(-1);		
			#else
				netData.daSample[i] = (double)(-1);		
			#endif
				nSample++;			// 999999.00 은 -1로 처리
			}

			else
			{
				// daSample 값 
			#ifdef DP_SAMPLE
				netData.pdaSample[i] = (double)atof(strSample[nSample]);
			#else
				netData.daSample[i] = (double)atof(strSample[nSample]);
			#endif
				nSample++;

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
			m_treeCtrl.Expand(hParentLot, TVE_EXPAND);
			g_sLotNetDate_Info.setLotNetId(nLot, nNet);

		}
		strInsert.Empty();

#ifdef DP_SAMPLE
		if (m_nNetDataCount < MAX_NET_DATA)
		{
			// Read한 netData를 메모리에 추가한다.
			// 불필요한 복사생성자의 new, 소멸자의  delete 를  막기 위해 Stat_inserNetData() 호출 대신 직접 코드 수행.
			int ret = Stat_insertNetVector(nLot, nNet, nDate);
			if (ret >= 0)
			{
				// 2K를 넘는다면 vector 공간을 더 확보한다.  
				if ( g_pvsaNetData[nLot][nNet][nDate]->size() == 2048)		
					g_pvsaNetData[nLot][nNet][nDate]->reserve(MAX_TIME_FILE);

				// 해당 vector에 실제 netData를 insert한다.
				//   :  time 갯수를 가질 vector에 이번 time의 netData를 push_back 한다.
				g_pvsaNetData[nLot][nNet][nDate]->push_back(netData);
			}

			// 2018.06.18: m_nNetDataCount가  MAX_NET_DATA 갯수를 넘어서면 4W Data 로딩을 중지하는 기능추가
			m_nNetDataCount++;
			if (m_nNetDataCount == MAX_NET_DATA)
				MyTrace(PRT_BASIC, "m_nNetDataCount=%d, Stat_insertNetData() Stopped!! \n", m_nNetDataCount);
		}

#else
		// Read한 netData를 메모리에 추가한다.
		Stat_insertNetData(nLot, nNet, nDate, netData);		
#endif


		row++;

	}
	fclose(fp);

	//TRACE ("g_pvsaNetData[nLot=%d][nNet=%d][nDate=%d]->size() = %d\n", 
	//					nLot, nNet, nDate, g_pvsaNetData[nLot][nNet][nDate]->size());

#ifdef DP_SAMPLE
	// 여기서 사용한 netData의 메모리는 지우고 나간다. ????
	//delete []netData.pdaSample;  => 할 필요 없다.   
	//이 함수가 끝나면서 netData의 소멸자가 자동호출되고 거기서 delete[]됨
#endif


}



// Stat Data를 메모리에 추가 한다. 
void CStatisticsDialog::Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData)
{

	if (m_nNetDataCount >= MAX_NET_DATA)
		return;

	int ret = Stat_insertNetVector(nLot, nNet, nDate);
	if (ret < 0)
		return;

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
		strTemp.Format("m_nNetDataCount=%d, Stat_insertNetData() Stopped!!\n\n Current nLot=%d(%s), nNet=%d, nDate=%d(%s) \n", 
				m_nNetDataCount , 
				nLot, g_sLotNetDate_Info.strLot[nLot], nNet, nDate,  g_sLotNetDate_Info.strLotDate[nLot][nDate]);
		AfxMessageBox(strTemp, MB_ICONINFORMATION);
		MyTrace(PRT_BASIC, strTemp);
	}
}

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

//#ifdef DP_SAMPLE
#if 0		// NetData 소멸자에서 처리하기로 하고 이 부분은 삭제.
	int tupleSize;
	for (lot =0; lot < MAX_LOT; lot++)
	{
		for (net =0; net < MAX_NET_PER_LOT; net++)
		{
			for (date =0; date < MAX_DATE; date++)
			{
				if(g_pvsaNetData[lot][net][date] != NULL)
				{
					// 먼저 각 tuple의 pdaSample을 delete[]한다.   
					// new[]로 할당했으니 delete[]해야 함.
					tupleSize = g_pvsaNetData[lot][net][date]->size();	// date마다 tupleSize가 다름!
					for (int tuple=0; tuple < tupleSize; tuple++)
					{
						//  이쯤에서 죽는다.... 문제.
						if (((*g_pvsaNetData[lot][net][date])[tuple].pdaSample) != NULL)
						{
							delete []((*g_pvsaNetData[lot][net][date])[tuple].pdaSample);
							(*g_pvsaNetData[lot][net][date])[tuple].pdaSample = NULL;
						}
					}
				}
			}
		}
	}
#endif
	
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

				#ifdef DP_SAMPLE
					//int tupleSize = g_pvsaNetData[lot][net][date]->size();	// date마다 tupleSize가 다름!
					//MyTrace(PRT_BASIC, "Stat_deleteAllNetData(): lot=%d, net=%d, date=%d, tupleSize=(%d => %d)\n", 
					//		lot, net, date, prevTupleSize, tupleSize);
				#endif

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

	// 자식 Lot 노드를  찾는다.
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
		// 자식의 형제 Lot 노드를 찾는다.
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

	// 자식 노드를 찾는다.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		Tree_CheckNetFault(hItemChild, nLot);		// Date = ALL로 체크
		//netName = m_treeCtrl.GetItemText(hItemChild);
		//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
	}

	while (hItemChild)
	{
		// 자식의 형제노드를 찾는다.
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
	CString	itemName; 
	int 	nNet;
	
	
	itemName = m_treeCtrl.GetItemText(hNetItem);
	nNet = atoi(itemName.Mid(4));


	//-----------------------
	// Fault 처리 수행
	
	int 	tupleSize, tuple, sample, sampleSize;
	double 	dLsl, dUsl;

	// 2018.05.31  Net 내부 공통 Lsl, Usl은 막고 tuple별 Lsl, Usl을 사용하기로 함.
	//dLsl   = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];	
	//dUsl   = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];	
	
	
	// USL, LSL값을 time tuple마다 다른 값으로 처리하려면 Display 단계에서의 fault 체크. 
	// LSL, USL 값을 모든 time tuple에 공통으로 최종 결과만으로 체크하려면 여기에서 미리 체크한다.  
	// 상기 두가지 방법에 따라 fault의 최종 결과가 다를 수 있다.
	
	//int		nNetFaultCount = 0;
	g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetNgCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetTotalCount[nLot][nNet] = 0;
	for (int date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)	
	{
		tupleSize = g_pvsaNetData[nLot][nNet][date]->size();
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			// 2018.05.31  tuple별 Lsl, Usl 사용
			dLsl = (*g_pvsaNetData[nLot][nNet][date])[tuple].dLsl;
			dUsl = (*g_pvsaNetData[nLot][nNet][date])[tuple].dUsl;

			sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
		#ifdef DP_SAMPLE
			for (sample= 0; sample < sampleSize; sample++)
			{
				double dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample];
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample] == -1)	//NG
					g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date]++;
				else
				{
					if (dSampleVal < dLsl || dSampleVal > dUsl)		// Fault
						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]++;
				}
			}

		#else
			for (sample= 0; sample < sampleSize; sample++)
			{
				double dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample];
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample] == -1)	//NG
					g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date]++;
				else
				{
					if (dSampleVal < dLsl || dSampleVal > dUsl)		// Fault
						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]++;
				}
			}
		#endif

		}
		//nNetFaultCount += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetNgCount[nLot][nNet] += g_sLotNetDate_Info.waLotNetDate_NgCnt[nLot][nNet][date];
		g_sLotNetDate_Info.waNetTotalCount[nLot][nNet] += (tupleSize * sampleSize);
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
	
	CString strTemp;
	

	//-----------------------------
	// File Open Dialog 생성
	
	char szFilter[] = "CSV Files(*.CSV)|*.CSV|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, 		// bOpenFileDialg = TRUE: 타이틀바에 '열기' 출력
					NULL, 		// lpszDefExt           : 기본 확장자
					NULL, 		// lpszFileName         : "파일 이름" Edit 컨트롤에 처음 출력될 파일명.
					OFN_HIDEREADONLY |OFN_NOCHANGEDIR |OFN_ALLOWMULTISELECT, 
								// dwFlags :  속성   (HIDEREADONLY : read only 파일은 출력하지 않음.
					szFilter);	// 대화상자에 출력될 파일을 확장자로 거르기 위한 필터. 

	dlg.m_ofn.lpstrInitialDir = _T("d:\\log4w"); // 오픈할때 초기 경로 지정. 맨앞에 소문자 d: 여야 제대로 동작함..
	
	// 여기서 버퍼 크기 늘려줘야 함.  안 늘려 주면 6개이상 file 지정이 안 됨.
	// default 256byte => file 6개 처리 가능.  각 fileName이 약 43문자 정도라는 뜻임.
	// fileName 사이즈를 넉넉하게 60문자로 주고 3000개를 처리해야 하므로  60*3000 = 180000 
	// 2018.07.17 buffer의 사이즈가 180000 으로 매우 크므로 stack overflow를 유발할 수 있어 static으로 변경함.
	static char buffer[60 * MAX_TIME_FILE] = {0}; 	// 버퍼
	dlg.m_ofn.lpstrFile = buffer; 				// 버퍼 포인터
	dlg.m_ofn.nMaxFile = (60 * MAX_TIME_FILE); 	// 버퍼 크기,  file 3000개 처리하려면 이정도 buffer필요함.


	if (IDOK != dlg.DoModal())
		return;



	//-----------------------------
	// Lot Name을 먼저 처리
	

	// 선택된 CSV파일들의 폴더명(dirPath)을 획득한다. --------------

	CString dataFilePath, dirPath;
	POSITION pos= dlg.GetStartPosition();
	dataFilePath = dlg.GetNextPathName(pos);
	
	// 1. ReverseFind()로 뒤에서부터 '\' 위치를 찾는다.  
	int nSlashLoc = dataFilePath.ReverseFind('\\');	 

	// 2.  뒷부분(fileName)을  떼어내면 폴더명(dirPath)만 남는다.
	//   ex) "D:\log4w\20180503_036782A2S\Ng_Log4WSample0_20180503_010135_NG_Net_-2558.CSV"
	//        nSlashLoc는 27이 리턴됨. 27부터 Left()를 이용해서 왼쪽을 선택하면 폴더Path (dirPath).
	//        dirPath는 "D:\log4w\20180503_036782A2S"   . 결과적으로 오른쪽 fileName만 삭제됨.
	dirPath = dataFilePath.Left(nSlashLoc); 


	// d:\log4w 디렉토리 아래의 디렉토리인지 확인한다. ---------------
 	if (dirPath.Find("D:\\log4w\\") == -1)	
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): %s is not 4W Data Directory. Can't do InsertItem.\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}


	CString lotName = dirPath;
	MyTrace(PRT_BASIC, "Load 4wData(Single)...: Lot=%s\n\n\n", lotName);
	int	length = strlen(lotName);
	lotName = lotName.Mid(9, (length -9));		// 먼저 "D:\log4w\"에 해당하는 첫번째 level을 떼어낸다.
							// 2018.06.28: 잘라내는 위치를 9로 고정하면 D:\log4w이외에 다른 경우를 커버하지 못함.
							// 			   사용자 실수 이므로 이정도는 용인한다. d:\log4w_2 를 선택한다면 처리는 정상적으로
							// 			   되는데 date name은 이상하게 출력된다.



	// "20170925" 처럼 날짜로 시작하지 않는다면 리턴.  (Lot 디렉토리가 아님) -------------
	CString dateName  = lotName.Left(8);		// 날짜 str 보관.
	CString dateExclu = dateName.SpanExcluding("0123456789"); 
	if (!dateExclu.IsEmpty())	// "0123456789"에 해당하지 않는 문자만 선택. Empty가 아니면 글자포함이란 얘기
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): \"%s\" \n '4w Data' Directory has not proper DateName. Can't do InsertItem.\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}

	// Check Empty Directory     -------------------------
	// 빈 lot이 추가되는 것을 막기 위해 Insert할 디렉토리가 빈 디렉토리인지 검사한다.  (자원절약)
	//  : 직접 파일을 선택한 것이므로 실제로 file이 없는게 아니라 
	//    'sample_xxxx' 파일만 선택했을 경우를 대비해서 필요하다. 
	CString fileName; 
	BOOL	bFileFound = FALSE;
	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath 부분을 떼어내서 fileName만 남긴다. (dirPath+'\' 사이즈만큼 앞부분을 자름)
		int fileNameLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (fileNameLength - (nSlashLoc +1)));		 
		if (fileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		// Sample이 아닌 정상 4W data file이 하나라도 있으면 break; 
		bFileFound = TRUE;
		break;

	}

	if(bFileFound == FALSE)		
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): Any proper '4w Data File' was not Selected. Can't do InsertItem.\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}


	// "20170925_A30-19-C-4W-NEW-0920" 라면 맨앞의 날짜는 떼고 tree item으로 삽입한다. 
	lotName = lotName.Mid(9, (length -9));		// "20170925_" 날짜 부분을 떼어낸다.


	// 여기까지.. Lot Name, Date Name 확보.
	MyTrace(PRT_BASIC, "Load 4wData(Single): Lot=%s, Date=%s \n\n\n", lotName, dateName);
	


	//----------------------------------------------
	// UI (Tree, Date, Grid )  및 메모리 초기화 
	
	ClearGrid(); // data Grid, summary Grid, edit박스 등 UI를 초기화한다.

	Stat_deleteAllNetData();	// 기존 netData를 모두 메모리에서 지운다.
	InitMember();				// CStatisticsDialog 멤버 변수를 모두 초기화 한다.
	InitTree();					// root부터 Tree를 다시 생성.	



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
	
	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath 부분을 떼어내서 fileName만 남긴다. (dirPath+'\' 사이즈만큼 앞부분을 자름)
		int fileNameLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (fileNameLength - (nSlashLoc +1)));		 

		//MyTrace(PRT_BASIC, "Load 4wData(Single): Path=%s, file=%s \n", dataFilePath, fileName );	 // test print

		// 해당 CSV file을 load한다. 
		Load_4wDataFile(dataFilePath, fileName, hLotItem, lotId, dateId);
	}


	//------------------------------------------
	// 해당 Lot의 모든 Net의 Fault Check
	Tree_Check_LotNetFaults(hLotItem, lotId);
	

	strTemp.Format("\'Load 4w Data\' for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", dirPath);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "OnButtonLoad4wData_SingleLot() for\"%s\" Completed. \n", dirPath);
	

}



// Tree Control의 선택 item이 변경될 때 호출
void CStatisticsDialog::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM	hParent;
	CString		strTemp;
	CString		parentName = ""; 
	CString		selectedName = "";

	
	// 현재 선택한 아이템의 핸들 값을 멤버변수에 저장한다.
	m_hSelectedNode = pNMTreeView->itemNew.hItem;
	hParent 		= m_treeCtrl.GetParentItem(m_hSelectedNode);

	// test
	//m_treeCtrl.SetItemImage(m_hSelectedNode, 2, 3);		// item에 빨강색 장애 아이콘 설정  테스트

	// 선택한 아이템의 이름을 대화상자의 에디트 상자에 설정한다.  
	if (hParent != NULL)
		parentName   = m_treeCtrl.GetItemText(hParent);
	selectedName = m_treeCtrl.GetItemText(m_hSelectedNode);

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);

	if (nLevel == 3)			// 선택한 아이템이 Net라면 'lot# - net#'를 설정
		m_strSelect = parentName + " / " + selectedName;
	else
		m_strSelect = selectedName;


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
					nLot, m_strSelect, MAX_LOT);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); 
			return;
		}
		if (nNet < 0 || nNet >= MAX_NET_PER_LOT)
		{
			strTemp.Format("OnSelchangedTree(): %s Net# Range(0 <= Net < %d) Over\n", m_strSelect, MAX_NET_PER_LOT);
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

	// 선택된 Net이나, Lot의 Summary를 summaryGrid에 출력해 주고,  
	// Net data와 date까지 상세하게 선택이 되었다면 dataGrid도 출력해 준다.
	// 2018.05 Display 버튼을 눌렀을 때 출력해 주는 것으로 수정함.
	// 2018.06.15 이상윤 부장 요청으로 Date나 Tree 클릭만으로 조회가 가능하게 다시 수정함.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		



	// 대화상자의 Edit컨트롤에 m_strSelect 를 출력한다.
	UpdateData(FALSE);
	
	*pResult = 0;
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
		strTemp.Format("DisplayGrid(): Execute \"Load 4W Data to Tree\" button, First.\n And select any Tree item and Date item please.");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// 클릭한 item이 없다면  아무 item이나 클릭하라는 메시지 출력.
	if (m_hSelectedNode == NULL)
	{
		strTemp.Format("DisplayGrid(): Select any Tree item and Date item please.");
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



	// nLot, nNet, NDate에 맞는 모든 data Grid Tuple을 출력한다.
	m_GridSnap.dataCount = DisplayGrid_4wData(nLot, nNet, nDate, 0);


	//---------------------------
	// Net Info Edit 박스값 설정
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();
	m_editTupleNum  = tupleSize;

	m_editSampleNum = g_sLotNetDate_Info.naLotSampleCnt[nLot];


	// m_editLSL과 m_editUSL 부분은  DisplayGrid_Summary()에서 수행.

	MyTrace(PRT_BASIC, "Lot=%d(%s), Net=%d, Date=%d(%s), tupleSize=%d : \n", 
			nLot, g_sLotNetDate_Info.strLot[nLot], nNet, 
			nDate, g_sLotNetDate_Info.strLotDate[nLot][nDate], tupleSize );


	//-------------------
	// Summary Grid 출력

	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.
	

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
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, sample, sampleSize;
	double  dSum=0;
	int		nMinMaxInitSample = 0;	// Min, Max initial 위치
	for (tuple=0; tuple < tupleSize; tuple++)
	{
		nMinMaxInitSample = 0;

		sampleSize =  g_sLotNetDate_Info.naLotSampleCnt[nLot];
		for (sample= 0; sample < sampleSize; sample++)
		{
			rnTotal++;
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)		// NG가 아니라면
			{
				rnCount++;
				dSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

				// Min, Max 초기화
				if (tuple==0 && sample==nMinMaxInitSample)
				{
					rdMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
					rdMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
				else
				{
					// 최대값보다 현재값이 크면 최대값 변경
					if (rdMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						rdMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

					// 최소값보다 현재값이 작으면 최소값 변경
					if (rdMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						rdMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}

			}
			else
			{
				// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
				if (sample == nMinMaxInitSample)		
					nMinMaxInitSample++;						
			}
		}
	}

	// 2. Calc Sigma   --------------------
	double 	dDiff = 0, dDiffPowerSum = 0, dVariance=0;
	if (rnCount)	// check devide by zero!
	{
		rdAvg = dSum / (double)rnCount;

		for (tuple=0; tuple < tupleSize; tuple++)
		{
			sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
			for (sample= 0; sample < g_sLotNetDate_Info.naLotSampleCnt[nLot]; sample++)
			{
				if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)
				{
					dDiff =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] - rdAvg;
					dDiffPowerSum += (dDiff * dDiff);		
				}
			}
		}
		dVariance = dDiffPowerSum / (double)rnCount;		// 분산     : (val-avg)의 제곱의 총합을 n으로 나눈다.
		rdSigma    = sqrt(dVariance);					// 표준편차 : 분산의 제곱근
	}

	MyTrace(PRT_BASIC, "rnTotal=%d, rnCount=%d, dSum=%.2f, rdAvg=%.2f, dVar=%.2f, rdSigma=%.2f, rdMin=%.2f, rdMax=%.2f, nFaultCount=%d\n", 
							rnTotal, rnCount, dSum, rdAvg, dVariance, rdSigma, rdMin, rdMax, 
							g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);
							//nFaultCount);


}



// nLot, nNet, NDate에 맞는 모든 data Grid Tuple을 출력한다.
// return : 마지막으로 출력한  tupleOffset 을 리턴한다.
int CStatisticsDialog::DisplayGrid_4wData(int nLot, int nNet, int nDate, int tuplePrtStart)
{

	//-------------------
	// data Grid 출력


	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	int 	tuple, sample, prtTuple=0;
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
		int nMinMaxInitSample = 0;		// Min, Max의 초기화 위치 표시

		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
		for (sample= 0; sample < sampleSize; sample++)
		{
		#ifdef DP_SAMPLE
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample] != -1)	// NG가 아니라면
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];

				// Min, Max 초기화
				if (sample == nMinMaxInitSample)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
				}
				else
				{
					// 최대값보다 현재값이 크면 최대값 변경
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];

					// 최소값보다 현재값이 작으면 최소값 변경
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
				}
			}

		#else
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)	// NG가 아니라면
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

				// Min, Max 초기화
				if (sample == nMinMaxInitSample)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
				else
				{
					// 최대값보다 현재값이 크면 최대값 변경
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

					// 최소값보다 현재값이 작으면 최소값 변경
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
			}
		#endif
			else
			{
				// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
				if (sample == nMinMaxInitSample)		
					nMinMaxInitSample++;						
			}
		}	




		// 2. Calc Tuple Sigma  ------------------------
		dTupleDiff = 0; 
		dTupleDiffPowerSum = 0; 
		dTupleVar=0; 
		dTupleSigma=0;
		if (nTupleCount)	// check devide by zero!
		{
			dTupleAvg = dTupleSum / (double)nTupleCount;


			int sampleSize =  g_sLotNetDate_Info.naLotSampleCnt[nLot];
			for (sample= 0; sample < sampleSize; sample++)
			{
			#ifdef DP_SAMPLE
				if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample] != -1)
				{
					dTupleDiff =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample] - dTupleAvg;
					dTupleDiffPowerSum += (dTupleDiff * dTupleDiff);		
				}
			#else
				if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)
				{
					dTupleDiff =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] - dTupleAvg;
					dTupleDiffPowerSum += (dTupleDiff * dTupleDiff);		
				}
			#endif
			}

			dTupleVar   = dTupleDiffPowerSum / (double)nTupleCount;	// 분산     : (val-avg)의 제곱의 총합을 n으로 나눈다.
			dTupleSigma = sqrt(dTupleVar);							// 표준편차 : 분산의 제곱근

		}


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
	int 	sample;

	// 2018.05.31  Net tuple 공통 Lsl, Usl은 코멘트처리하고 tuple별 Lsl, Usl을 사용하기로 함.
	//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
	//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];
	double 	dLsl = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dLsl;
	double 	dUsl = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dUsl;


	// 0     1      2     3     4     5     6     7     8     9     10    11    12    13     14      ....   25
	// No,  Date, Time,  Pin1, Pin2, Pin3, Pin4,  R,   LSL,  USL,  Avg, Sigma, Min, Max,  Data1, .... Data12

	// 1. Check 4W Data Fault  ------------------------
	int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
	int nFaultCount = 0;
	for (sample= 0; sample < sampleSize; sample++) // col 14~25:  Sample1~ Sample12
	{
															
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample] != -1)		// NG가 아니면
		{
		#ifdef DP_SAMPLE
			double dSampleVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].pdaSample[sample];
		#else
			double dSampleVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample];
		#endif

			if (dSampleVal < dLsl || dSampleVal > dUsl)		// Fault
				nFaultCount++;
		}
	}

	// m_bDataGridFaultOnly가 TRUE 일때  현재 sample 중에 Fault가 한개도 없다면 출력하지 않고 리턴.
	if (m_bDataGridFaultOnly == TRUE &&  nFaultCount == 0)
		return -1;


	// 2. Display  PinInfo, R, LSL, USL  ----------------------
	
	// row 0 헤더는 제외하고 data영역인 row 1 (nTuple + 1)부터 출력.
	strTemp.Format("%d", (nPrtTuple+1));
	m_gridData.SetItemText(nPrtTuple+1, DATA_COL_NO, strTemp);				// col 0:  No

	//m_GridSnap.saData[nTuple].nTuple = nTuple; 	 // Save data snap for csv
	m_GridSnap.saData[nTuple].nPrtTuple = nPrtTuple; // Save data snap for csv


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

	strTemp.Format("%.2f",  (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dLsl);
	m_gridData.SetItemText(nPrtTuple+1,  DATA_COL_LSL, strTemp);			// col 8:   LSL
	m_GridSnap.saData[nPrtTuple].strLSL = strTemp;

	strTemp.Format("%.2f",  (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].dUsl);
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



	// 4. Display Tuple 4W Data   ------------------------
	for (sample= 0; sample < sampleSize; sample++) // col 14~25:  Sample1~ Sample12
	{
															
	#ifdef DP_SAMPLE
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].pdaSample[sample] == -1)
	#else
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample] == -1)	// NG이면
	#endif
		{
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+sample, "NG");	
			m_GridSnap.saData[nPrtTuple].strData[sample] = "NG";
		}

		else		// NG가 아니면
		{
		#ifdef DP_SAMPLE
			double dSampleVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].pdaSample[sample];
		#else
			double dSampleVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample];
		#endif

			strTemp.Format("%.2f",  dSampleVal);
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+sample, strTemp);	
			m_GridSnap.saData[nPrtTuple].strData[sample] = strTemp;

			if (dSampleVal < dLsl || dSampleVal > dUsl)		// Fault
			{
				m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_DATA1+sample, 	// row, col
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

	//----------------------------
	// Net Info Edit 박스값 설정
	
	
	// double형은 값이 변형되어 출력되므로 CString으로 관리하여 아래와 같이 출력한다.
	// => double형인 경우 8.6이 8.59로 변형되는 현상이 있었음.
	
	//sprintf((char*)m_editStrLSL.GetBuffer(10), "%.2f", g_sLotNetDate_Info.daLotNetLsl[nLot][nNet]);
	//m_editStrLSL.ReleaseBuffer();
	//sprintf((char*)m_editStrUSL.GetBuffer(10), "%.2f", g_sLotNetDate_Info.daLotNetUsl[nLot][nNet]);
	//m_editStrUSL.ReleaseBuffer();


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
	m_GridSnap.Summary.strNgCount = strTemp;

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


void CStatisticsDialog::OnButtonClearGrid() 
{
	// TODO: Add your control notification handler code here
	MyTrace(PRT_BASIC, "OnButtonClearGrid()...\n" );
	
	ClearGrid();

}


void CStatisticsDialog::ClearGrid() 
{
	// Lot/Net 선택사항도 삭제한다.
	m_strSelect = _T("");
	
	// Grid와 함께 Grid Sub Edit 박스도 초기화한다.
	m_editTupleNum = 0;
	m_editSampleNum = 0;
	//m_editStrLSL = _T("");
	//m_editStrUSL = _T("");

	// data를 업데이트하기 전에 지우기. 
	ClearGrid_Summary();	
	ClearGrid_Data();	

	m_GridSnap.InitMember();

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

	//m_gridData.DeleteAllItems();
	
	// Scroll Bar 초기화 
	m_gridData.SetScrollPos32(SB_VERT, 0);
	m_gridData.SetScrollPos32(SB_HORZ, 0);

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

	//m_gridSummary.DeleteAllItems();
	
	// Scroll Bar 초기화 
	m_gridSummary.SetScrollPos32(SB_VERT, 0);
	m_gridSummary.SetScrollPos32(SB_HORZ, 0);

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
	ClearGrid_Data();	


	// nLot, nNet, nDate에 맞는 모든 data Grid Tuple을 출력한다.
	int tupleOffset = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		tupleOffset= DisplayGrid_4wData(nLot, nNet, date, tupleOffset);
	m_GridSnap.dataCount = tupleOffset;




	//---------------------------
	// Net Info Edit 박스값 설정

	m_editTupleNum = 0;
	m_editSampleNum = g_sLotNetDate_Info.naLotSampleCnt[nLot];
	// m_editLSL과 m_editUSL 부분은  DisplayGrid_Summary()에서 수행.





	//-------------------------
	// Summary Grid 출력
	

	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.

	
	
	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, sample, tupleSize, sampleSize;
	int	   	nTotal=0, nCount=0;
//	int		nFaultCount=0; 
	double  dSum=0;
	double 	dAvg=0, dMax=0, dMin=0; 
	int		nMinMaxInitSample = 0;	// Min, Max initial 위치
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
	{
		tupleSize = g_pvsaNetData[nLot][nNet][date]->size();
		m_editTupleNum += tupleSize;
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			nMinMaxInitSample = 0;
			sampleSize =  g_sLotNetDate_Info.naLotSampleCnt[nLot];
			for (sample= 0; sample < sampleSize; sample++)
			{
				nTotal++;
			#ifdef DP_SAMPLE
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample] != -1)	// NG가 아니라면
			#else
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample] != -1)	// NG가 아니라면
			#endif
				{
					double dSampleVal/*, dLsl, dUsl*/;
				#ifdef DP_SAMPLE
					dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample];
				#else
					dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample];
				#endif

					// Min, Max, Sum, Count 구하기 -----------
					nCount++;
					dSum += dSampleVal; 
	
					// Min, Max 초기화
					if (date== 0 && tuple==0 && sample==nMinMaxInitSample)
					{
						dMax = dSampleVal; 
						dMin = dSampleVal; 
					}
					else
					{
						// 최대값보다 현재값이 크면 최대값 변경
						if (dMax <  dSampleVal)
							dMax =  dSampleVal; 
	
						// 최소값보다 현재값이 작으면 최소값 변경
						if (dMin >  dSampleVal)
							dMin =  dSampleVal;
					}


					// 2018.05.21 직접 체크하지 말고 이미 체크한 
					//     g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]값을 이용하도록 
					//     하기 코드를 코멘트 처리함.
					// Fault Count 구하기 -----------------------
					//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
					//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];
					//if (dSampleVal < dLsl || dSampleVal > dUsl)
					//	nFaultCount++;
	
				}
				else
				{
					// Min Max 초기화 위치의 값이 NG라면 초기화 위치를 하나 증가 시킨다. 
					if (sample == nMinMaxInitSample)		
						nMinMaxInitSample++;						
				}
			}
		}

		// 2018.07.09 여기서 아예 계산안하고 g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]을 그냥 사용하기로하고 코멘트처리
		//nFaultCount += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
	}

	double 	dDiff = 0, dDiffPowerSum = 0, dVariance=0, dSigma=0;
	if (nCount)
	{
		dAvg = dSum / (double)nCount;

		// 2. Calc Sigma   --------------------
		for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		{
			tupleSize = g_pvsaNetData[nLot][nNet][date]->size();	// date마다 tupleSize가 다름!
			for (tuple=0; tuple < tupleSize; tuple++)
			{
				sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
				for (sample= 0; sample < sampleSize; sample++)
				{
				#ifdef DP_SAMPLE
					if ((*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample] != -1)
					{
						dDiff =  (*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample] - dAvg;
						dDiffPowerSum += (dDiff * dDiff);		
					}
				#else
					if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample] != -1)
					{
						dDiff =  (*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample] - dAvg;
						dDiffPowerSum += (dDiff * dDiff);		
					}
				#endif
				}
			}
		}
		dVariance = dDiffPowerSum / (double)nCount;		// 분산     : (val-avg)의 제곱의 총합을 n으로 나눈다.
		dSigma    = sqrt(dVariance);					// 표준편차 : 분산의 제곱근
	}

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data를 업데이트하기 전에 지우기.


	UpdateData(FALSE);		// 변경 data 화면 반영
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.


	MyTrace(PRT_BASIC, _T("Display_SumupTotal()\n"));

}

BOOL CStatisticsDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	Stat_deleteAllNetData();
	
	MyTrace(PRT_BASIC, "\"4W Statistics SW\" Destroyed...\n\n\n\n\n" );
	return CDialog::DestroyWindow();
}



void CStatisticsDialog::OnChangeEditUsl() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
/*	
	UpdateData(TRUE);		// 화면 data 가져오기 

	CString strTemp;

	// Lot 존재해야 하고, Net 존재해야 한다. 
	if ( (m_nTree_CurrLot < 0 || m_nTree_CurrLot >= MAX_LOT) 
			|| (m_nTree_CurrNet < 0 || m_nTree_CurrNet >= MAX_NET_PER_LOT ) )
	{
		strTemp.Format("OnChangeEditUsl(): Lot=%d(0<= Lot <%d), Net=%d(0<= Lot <%d) Range Over!\nPlease select any Net# in tree.", 
					m_nTree_CurrNet, MAX_LOT, m_nTree_CurrNet, MAX_NET_PER_LOT );
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}
	
	g_sLotNetDate_Info.daLotNetUsl[m_nTree_CurrLot][m_nTree_CurrNet] = (double)atof((char*)m_editStrUSL.GetBuffer(10));
	m_editStrUSL.ReleaseBuffer();

	UpdateData(FALSE);		// 변경 data 화면 반영
*/
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
			(m_nCombo_CurrDate == DATE_ALL) ? "DATE_ALL" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
			nNet); 					

	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

	
}


// 1. 지금까지 insert 한 data를  binary 파일을 open하여  write. 
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
			(nComboDate == DATE_ALL) ? "DATE_ALL" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
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

	// 헤더 출력
	fprintf(fp, "Net, Date, NgCount, Count, Average, Sigma, DataMin, DataMax, FaultCount, , ,TimeTuple#, Data#\n" );

	fprintf(fp, "%s, %s, %s, %s, %s, %s, %s, %s, %s, , ,%d, %d\n",
				m_GridSnap.Summary.strNetName, 		// 0:
				m_GridSnap.Summary.strDate, 		// 1:
				m_GridSnap.Summary.strNgCount, 		// 2:
				m_GridSnap.Summary.strCount, 		// 3:

				m_GridSnap.Summary.strAvg, 			// 4:
				m_GridSnap.Summary.strSigma, 		// 5:
				m_GridSnap.Summary.strMin, 			// 6:
				m_GridSnap.Summary.strMax, 			// 7:
				m_GridSnap.Summary.strFault, 		// 8:

				m_editTupleNum,				// edit box
				m_editSampleNum);			// edit box

	//-----------------------------------
	//  Data Grid Data 출력

	fprintf(fp, "\n\n");
	fprintf(fp, "No, Date, Time, Pin1, Pin2, Pin3, Pin4, R, LSL, USL, Avg, Sigma, Min, Max, "); 
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


		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
		for (int sample=0; sample < sampleSize; sample++)
			fprintf(fp, "%s, ", m_GridSnap.saData[row].strData[sample]);

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
	// p Chart를 위한 sample data 출력 

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

					int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[lot];
					for (int sample= 0; sample < sampleSize; sample++)
					{
						double dSampleVal = (*g_pvsaNetData[lot][net][date])[time].daSample[sample];
						if ((*g_pvsaNetData[lot][net][date])[time].daSample[sample] == -1)	//NG
							waTimeNgCount[time]++;
						else
						{
							if (dSampleVal < dLsl || dSampleVal > dUsl)		// Fault
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
		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[lot];
		for (time =0; time < tupleSize; time++)
		{
			CString strTime;
			strTime.Format("%02d:%02d:%02d", 
					(*g_pvsaNetData[lot][1][0])[time].statTime.hour,
					(*g_pvsaNetData[lot][1][0])[time].statTime.min,
					(*g_pvsaNetData[lot][1][0])[time].statTime.sec);

			waTimeTotalCount[time] = (netCount * sampleSize);		// net 갯수 * sample 갯수
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

			// Total = SampleCount* tupleSize,   Count = Total - Ng
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

						fprintf(fp, "    waSample");
						int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[lot];
						for (int sample=0; sample < sampleSize; sample++)
					#ifdef DP_SAMPLE
							fprintf(fp, ", %.2f", (*g_pvsaNetData[lot][net][date])[tuple].pdaSample[sample] );
					#else
							fprintf(fp, ", %.2f", (*g_pvsaNetData[lot][net][date])[tuple].daSample[sample] );
					#endif

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


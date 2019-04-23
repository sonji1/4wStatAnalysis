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
// �������� ����
//---------------------
LotNetDate_Info		g_sLotNetDate_Info;	
vector <statNetData>	*g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// �����Ҵ� vector



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

	m_dSimulUsl = -1;		// �̻���� �ǹ̷� -1. m_bApplyULSL = TRUE �� ������ �� ���� �����ȴ�.
	m_dSimulLsl = -1;		// �̻���� �ǹ̷� -1. m_bApplyULSL = TRUE �� ������ �� ���� �����ȴ�.
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
	
	// �̹� �����Ϸ��  raw data ������ read�Ѵ�.
	//Load_Log4wDir();   <= ���⼭ �ڵ� �ε��ϸ�, �ε��ϴ� ���� �޽��� �ڽ� �ܿ� �ƹ�â�� �� ���δ�.
	
#if 0
//2018.07.03 : ���� ���̾�α� �ε� ��� �߰��Ŀ� �Ʒ� timer�� ������� �ʵ��� ��.
//#ifndef _DEBUG
	// 200ms �Ŀ� Load_Log4wDir()�� �����ϵ��� Timer ����.   (Release ����϶���)
	SetTimer (0, 		// OnTimer ����� ���� Timer Id
			200, 		// 200ms �ֱ�
			NULL);		// Timer ���Ž� �ڵ������� CallBack Function�� NULL�� ����.
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
		KillTimer(0);		// 1ȸ������ ���� ����  Timer ���� ���� �ش� Timer Id�� ��ȯ�Ѵ�.

		//CString strTemp;
		//strTemp.Format("The 'Load 4W Data' button will launch automatically to load \"%s\" files.", g_sFile.ACE400_4WDataDir);
		//AfxMessageBox(strTemp, MB_ICONINFORMATION);

		// �̹� �����Ϸ�� raw data ������ read�Ѵ�.
		Load_Log4wDir();
	}
	
	CDialog::OnTimer(nIDEvent);
}

// â�� ���� ���϶� ������ �ʱ�ȭ�ؾ� �ϴ� ������� ������ 
// OnShowWindow()�� TODO �ؿ� �ʱ�ȭ�Ѵ�. 
void CStatisticsDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE)
	{
		// ��� �ʱ�ȭ
		


		// View �ʱ�ȭ 

	}
	
}

BOOL CStatisticsDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	Stat_deleteAllNetData();
	m_gridSummary.DeleteAllItems();

	m_gridData.DeleteAllItems();		// �̰� �ð��� �ʹ� ���� �ɸ� �� ������ �ϴ°� �´�.
	
	MyTrace(PRT_BASIC, "StatDialog Destroyed...\n" );
	return CDialog::DestroyWindow();
}


BOOL CStatisticsDialog::InitMember()
{


	//----------------------------
	// Tree Current data �ʱ�ȭ 
	m_nTree_CurrLot = -1;	
	m_nTree_CurrNet = -1;	
	m_nCombo_CurrDate = DATE_ALL;	
	m_bDataGridFaultOnly = FALSE;


	m_dSimulUsl = -1;		// �̻���� �ǹ̷� -1. m_bApplyULSL = TRUE �� ������ �� ���� �����ȴ�.
	m_dSimulLsl = -1;		// �̻���� �ǹ̷� -1. m_bApplyULSL = TRUE �� ������ �� ���� �����ȴ�.
	m_nPrevSimulLoT = -1;
	m_nPrevSimulNet = -1;
	m_nPrevSimulDate = DATE_ALL;
	m_hPrevSimulSelNode = NULL;


	m_bSimulateULSL = FALSE;
	m_bApplyULSL = FALSE;
	m_editStrUSL = _T("");
	m_editStrLSL = _T("");

	//----------------------------
	// 4WData Load ���� �ʱ�ȭ
	//m_nLoad_CurrLot = 0;

	//----------------------------
	// Grid ���� ��� ���� �ʱ�ȭ
	m_nFixRows = 1;
	m_nFixCols = 1;		
	m_nCols = NUM_DATA_GRID_COL;
	//m_nRows = MAX_DATA_GRID_ROW;
	m_nRows = 201;			// Default. ���� Display�ÿ� ����� ������.

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
	// Date Combo Box �ʱ�ȭ
	m_comboDate.InsertString(-1, "ALL");		// -1: add string("ALL") to end of the list
	m_comboDate.SetCurSel(0); 					// combo idx = 0: ALL

	//----------------------------
	// Summary Grid �ʱ�ȭ 

	m_gridSummary.SetEditable(m_bEditable);					// FALSE ����
	//m_gridSummary.SetListMode(m_bListMode);
	//m_gridSummary.EnableDragAndDrop(TRUE);
	m_gridSummary.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// ������� back ground


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

	Display_SummaryGridHeader(); 	// Summary Grid header ����


	//----------------------------
	// Data Grid �ʱ�ȭ 

	m_gridData.SetEditable(m_bEditable);				// FALSE ����
	//m_gridData.SetListMode(m_bListMode);
	//m_gridData.EnableDragAndDrop(TRUE);
	m_gridData.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// ������� back ground


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


	Display_DataGridHeader(); 	// Data Grid Header ����

	ClearGrid_Data();	
	m_GridSnap.InitMember();


	//----------------------------
	// tree �ʱ�ȭ 
	
	InitTree();

	// Tree �̹�������Ʈ ����
	m_ImageList.Create(IDB_BITMAP1, 16, 	// �̹����� ����, ���� ����
									2,		// �̹��� ���� �޸� ������ �÷��� �� �޸�ũ�� 
					RGB(255,255,255));		// ȭ�� ��� �÷� (����)
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);


	// Simulate ���� ��Ʈ�� �ʱ�ȭ
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

// Summary Grid header ����
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
	
	m_gridSummary.SetRowHeight(0, 40);	// ����� ���� ���� ǥ�ø� ���� ���� ����
}

// Data Grid Header ����
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
	//������ �ۼ��� Tree Contents�� �ִٸ�  �����.
	m_treeCtrl.DeleteAllItems(); // <=�̷����ϸ� root���� �� ������.

	// 'Total'�� root �׸����� ���� �ʱ�ȭ
	TV_INSERTSTRUCT tvStruct;

	tvStruct.hParent = 0;					// �θ��׸�:  hParent�� 0�̸� root �׸�
	tvStruct.hInsertAfter = TVI_LAST;		// ���Թ�� 
	tvStruct.item.mask = TVIF_TEXT;			// item ������� ��ȿȭ ����: pszText ��� ��ȿȭ
	tvStruct.item.pszText = "Total";		// Ʈ���� ������ ���ڿ�
	m_hRoot = m_treeCtrl.InsertItem(&tvStruct);	

	m_hSelectedNode = NULL;		// SelectedNode �� NULL �� �ʱ�ȭ�ؼ� Node ���þ��� Display�ÿ� ��ó�Ѵ�.

}

// 'Load 4w Data' ��ư Ŭ���ÿ� ȣ��.
void CStatisticsDialog::OnButtonLoad4wData() 
{
	BeginWaitCursor();		// Ŀ���� waiting ���� �ٲ۴�.

	Load_Log4wDir();


	// default Net ��ȸ: ������ Ŭ���� ��� ù��° Lot, ù��° Net�� ��ȸ�� �ش�.
	DisplayGrid_DefaultLotNet();
//__PrintMemSize(FUNC(OnButtonLoad4wData), __LINE__);
//
	EndWaitCursor();		// Ŀ���� ���󺹱��Ѵ�.

	// LOAD LOG4W�� �Ϸ�Ǿ����� FR Rank Dlg�� �˸���.
	//::SendMessage(m_hwnd_FrRankDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
//__PrintMemSize(FUNC(OnButtonLoad4wData), __LINE__);
}

// ������ Ŭ���� ��� ù��° Lot, ù��° Net�� ��ȸ�� �ش�.
void CStatisticsDialog::DisplayGrid_DefaultLotNet() 
{ 
	// Lot�� ������ 1�� �̻����� Ȯ��
	if (g_sLotNetDate_Info.nLotCnt <= 0)			
		return; 

   	// Lot_0�� Net ������ 1�� �̻����� Ȯ��
    if (g_sLotNetDate_Info.naLotNetCnt[0] <= 0)		
    	return; 

	// Root���� Lot_0�� item�� ���� ã��, Net_1�� �� �ؿ��� ã�Ƽ� Ŭ���� ������ ó���� �ش�.
	HTREEITEM 	hLotItem = NULL, hNetItem = NULL;
	hLotItem = Tree_FindLotItem(&m_treeCtrl, m_hRoot, g_sLotNetDate_Info.strLot[0]);	// Lot_0
	if (hLotItem == NULL) 		
		return; 

	// Found: �����ϴ� Lot�̸� �� �ؿ��� Net_1�� ã�´�.
	hNetItem = Tree_FindStrItem(&m_treeCtrl, hLotItem, "Net_1"); 
	if (hNetItem != NULL)
	{
		_SelChangedTree(hNetItem); 
		m_treeCtrl.SelectItem(hNetItem);			// Tree���� �ش� item�� ���õ� ������ ǥ��
		m_treeCtrl.Expand(hLotItem, TVE_EXPAND);	// �ش� parent Lot�� Expand�ؼ� ���� NetItem �� �����ش�.
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

	ClearGrid(); // data Grid, summary Grid, edit�ڽ� �� UI�� �ʱ�ȭ�Ѵ�.

//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	Stat_deleteAllNetData();

//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);
	InitMember();

	Display_DataGridHeader(); 	// Data Grid Header �ٽ� ���� (simul�� on�̾��ٸ� off�� �ٲٰ� ����� �ٽ� �׷������)

	InitTree();					// root���� Tree�� �ٽ� ����.	
//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	// ���� raw data ������ read�Ѵ�.
	Load_Log4wDir(g_sFile.ACE400_4WDataDir);
//__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	
	// Load�� ��� Tree >Lot > Net�� ���� Fault üũ�� �����Ѵ�.
	Tree_CheckAll_LotNetFaults();
	
	strTemp.Format("Load_Log4wDir() for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", g_sFile.ACE400_4WDataDir);
	MyTrace(PRT_BASIC, "Load_Log4wDir() for\"%s\" Completed. \n", g_sFile.ACE400_4WDataDir);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "          m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "  sizeof(LotNetDate_Info) = %d\n", sizeof(LotNetDate_Info));
	MyTrace(PRT_BASIC, "      sizeof(statNetData) = %d\n", sizeof(statNetData));
	MyTrace(PRT_BASIC, "\n");

__PrintMemSize(FUNC(Load_Log4wDir), __LINE__);

	// 2018.04.27 ��� Lot�� read �Ϸ� �Ǿ�����  ����  lot������ ���Ϸ� �����Ѵ�. 
	//Save_StatLotDataFile(m_nLoad_CurrLot);		// lot�� data�� file �����ϴ� ����� ������. ����ũ ũ�� �ð� ���� ��Ƹ��� ��.
	
}

// "D:Log4w" ���丮�� �����Ͽ� 4w Raw Data�� Load�Ѵ�.
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


			// "20170925" ó�� ��¥�� �������� �ʴ´ٸ� ���� ���丮��
			CString dateName  = dirName.Left(8);		// ��¥ str ����.
			CString dateExclu = dateName.SpanExcluding("0123456789"); 
			if (!dateExclu.IsEmpty())	// "0123456789"�� �ش����� �ʴ� ���ڸ� ����. Empty�� �ƴϸ� ���������̶� ���
				continue;


			// "20170925_A30-19-C-4W-NEW-0920" ��� �Ǿ��� ��¥�� ���� tree item���� �����Ѵ�.
#if 0
			
			// �� ���� -0920�� ���� Ʈ�� item���� �����Ѵ�.
			// �� ���� -0920�� ���� ���� strRight�� '-', '_'���� �����ϴ��� üũ.
			CString strRight = dirName.Right(5);	
			if (strRight.GetAt(0) == '-' || strRight.GetAt(0) == '_')	
			{
				strRight = strRight.Right(4); 	// '-' �Ǵ� '_'�� �����.
				CString strExclu = strRight.SpanExcluding("0123456789");	

				// �� �� 4���ڰ� ��� 0920���� ���ڶ�� �Ǿ��� ��¥ 9���ڿ� �ǵ��� 5����(-0920, �Ǵ� _0920)�� ���� lot item str���� ���
				if (strExclu.IsEmpty())		
					lotName = dirName.Mid(9, (length -9 -5));
				else
					lotName = dirName.Mid(9, (length -9));
			}

			// strRight�� -�� ������ �ƴϸ� strRight���� �����ؼ� lot item str���� ���.
			else 
				lotName = dirName.Mid(9, (length -9));
#else
			// 2018.06.22 :  ACE400_2222,  ACE400_3333 �̷� �͵��� ���� lot���� ó���ϴٺ��� 
			//               step count�� �޶� ACE400_3333�� ó���� ���� �ʰ� �ǳʶٰ� �׷��� ������ ������ ����.
			//               �ǵ��� ���� �װ� ����� ����� ���� ���� ���Ƽ�  �ٸ� lot���� ó���ϱ�� ��.
			lotName = dirName.Mid(9, (length -9));
#endif
	
			// �� lot�� �߰��Ǵ� ���� ���� ���� Insert�� ���丮�� �� ���丮���� �˻��Ѵ�.  (�ð�, �ڿ�����)
			if(CheckEmptyDirectory(dirPath) == TRUE)		
			{
				strTemp.Format("%s is Empty Directory. Can't do InsertItem. \n : Load_Log4wDir()\n\n", dirPath);
				ERR.Set(USER_ERR, strTemp); 
				ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
				continue;
			}

			// Lot ����, ��¥ ������ ����.
			//CString lotName = dirName;
			int lotId, dateId;
			lotId  = g_sLotNetDate_Info.getLotId(lotName);
			if (lotId == -1)	// lot range�� �Ѿ�ٸ� load���� �ʴ´�.
			{
				strTemp.Format("Load_Log4wDir(): %s getLotId(%s) Failed! nLotCnt=%d Range(<%d) Over!\n\n Can't do InsertItem.", 
							dirName, lotName, g_sLotNetDate_Info.nLotCnt, MAX_LOT);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

		// 2018.10.18  Lot�� 5�� �̻� �ε��ϸ� ���� ����� ������, ���� lot�� �� ä����� �Ʒ��� ���� �õ��ϴ°� 
		//      ���� ������ �ִ� �λ��� �� �� �ִ�. ū ������ �ִ� ������ �� �Ǵ��� �ȵ�. �ϴ� max 3ȸ�� �ϵ��� ������.
				nLotErrCnt++;
				if (nLotErrCnt < MAX_LOT_ERR_COUNT) 
					continue;	// ������ lot�� ���� lot�� �̸��� ���� ���� �����Ƿ� break���� �ʰ� continue ó��
				break;			// Lot Error�� 3ȸ �̻� �ݺ��Ǹ� �׳� break. 
			}

			dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
			if (dateId == -1)	// date range�� �Ѿ�ٸ� load���� �ʴ´�. 
			{
				strTemp.Format("Load_Log4wDir(): getLotDateId(%s_%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
						dateName, lotName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

				nDateErrCnt++;
				if (nDateErrCnt < MAX_DATE_ERR_COUNT) 
					continue;	// ������ lot�� ���� �� �����Ƿ� break�� ���� �ʰ� continue ó��
				break;			// Date Error�� 3ȸ �̻� �ݺ��Ǹ� �׳� break
			}

			// Tree���� �ش� 'Lot' item��  ã�ų�, ���� insert �Ѵ�.
			HTREEITEM	hLotItem;
			BOOL		bLotInserted = FALSE;
			hLotItem = Tree_GetLotItem(lotName, dirPath, lotId, dateId, bLotInserted);			
												// bInserted: ȣ���Ŀ� Lot�� insert �ƴ��� ���� Ȯ�ο뵵
												
			if (hLotItem == NULL)	// logic ���� ���̽��� �̹� �����޽��� �ڽ�  ��������Ƿ� contiuneó���� �Ѵ�.
				continue;

			// Lot�� �ƴϸ� pass
			int nLevel = Tree_GetCurrentLevel(hLotItem);
			if (nLevel != 2)	
				continue;

			// Lot ���丮��� ������ Net Data File�� Open�ؼ� Load 
			// �ű� Node, �������ϴ� Node ��� ���� ���丮�� Net Data�� Load�� �����Ѵ�.
			// ���� time file�� �ϳ��� �ε��� �� �Ǿ��ٸ� ������ �����ϰ� continue
			if (Tree_LoadLot_4WFiles(dirPath, hLotItem, lotId, dateId) < 0) 
			{
				strTemp.Format("%s is Empty Directory(Tree_LoadLot_4WFiles()< 0). Delete Inserted Lot & Date.\n : OnCheckSimulUlsl()\n", dirPath);
				ERR.Set(USER_ERR, strTemp); 
				ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
				
				// LOT�� tree�� �߰� �ƴµ�,  data���� ����� �ִ� �����̶�����ؼ� �ε��� �����ߴٸ�
				if (bLotInserted == TRUE)
				{
					m_treeCtrl.DeleteItem(hLotItem); 		// Lot tree item ����, 
					g_sLotNetDate_Info.putBackLotId(lotId); // �߰��� lot str, �߰��� date str ��� ����
				}

				// ���� Lot�̶��  
				else
				{	
					// �߰��� date str�� ���� 
					g_sLotNetDate_Info.strLotDate[lotId][dateId] = "";
					g_sLotNetDate_Info.naLotDateCnt[lotId]--;
				}
				
				continue;
			}
//__PrintMemSize(FUNC(Load_Log4wDir_D:log4w), __LINE__);



			// ���� ���丮 �ؿ� sub ���丮�� �ִٸ� Load_Log4wDir()�� ��������� �� �����Ѵ�. 
			// �� �ڵ�� �����ؼ��� �ȵ�. 3 Level �̻� �� ������ ���� ����.
			//Load_Log4wDir(dirPath, hItem);		// Recursive call
		}

		// 2018.06.18 Tree_LoadLot_4WFiles()���� nNetDataCount >= MAX_NET_DATA ���� �������� ��� ,
		//            �� �̻� �ٸ� Lot�� ã�� �ʵ��� �Ʒ� �ڵ� �߰���.
		if (m_nNetDataCount >= MAX_NET_DATA)
			break;
	}

	finder.Close();

	
}



// �� ���丮���� �˻��Ѵ�.   (TRUE: Empty)
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

		// 2018.05.25: file name�� 'Sample'�� ���ԵǾ� �ִٸ�.  
		// Auto sampling�� ���� �Ͻ������� ������ �����̹Ƿ� �м� data���� �����Ѵ�.  
		// 2018.06.22: �� ���丮�� lot ó�� ���� �ʵ��� Load_4wDataFile()�� �ƴ϶� 
		// 			   CheckEmptyDirectory()������ üũ�ϵ��� ����. 
		CString	dataFileName = finder_sub.GetFileName();
		CString dataFilePath = finder_sub.GetFilePath();
		if (dataFileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		nFile++;

		// Sample�� �ƴ� ���� 4W Data file�� �ϳ��� ������ FALSE ����
		return FALSE;
	}
	if (nFile == 0)
		return TRUE;

	return FALSE;
}


// Tree���� �ش� 'Lot' item��  ã�ų�, ���� insert �Ѵ�.
HTREEITEM CStatisticsDialog::Tree_GetLotItem(LPCTSTR pStrInsert, LPCTSTR pStrPath, int nLot, int nDate, int& rbInserted)
{
	// TODO: Add your control notification handler code here
	HTREEITEM 	hItemFind= NULL, hLotItem = NULL;
	CString		strTemp;

	HTREEITEM   hItemGet = NULL;	// return ��

	// Insert��  �ؽ�Ʈ�� ����ֳ� �˻��Ѵ�.
	if(pStrInsert == "")
	{
		strTemp.Format("Tree_GetLotItem(): pStrInsert is NULL, Can't do InsertItem.");
		ERR.Set(SW_LOGIC_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return NULL;

	}



	// �߰��ؾ��� strInsert(Lot Name)�� ���� branch�� �����ϴ��� �˻��Ѵ�.
	// ���ο� Lot Name�̶�� ������ ������ Lot ��带 �߰��Ѵ�.
	//if ((hItemFind = Tree_FindStrItem(&m_treeCtrl, m_hRoot, pStrInsert)) == NULL)	// ���ο� Node(Lot) 
	
	// ���� Lot�̸� ������ Lot�� tree Item�� ȹ��
	hItemFind = Tree_FindLotItem(&m_treeCtrl, m_hRoot, pStrInsert);
	if (hItemFind != NULL) 				// Found: ������ �����ϴ� ���(Lot)
	{
		// ���� �̸��� ���(Lot)�� ������ �߰����� �ʵ��� return ���� NULL�� �����Ѵ�.
		hItemGet = NULL;		
		hLotItem = hItemFind;	// Found�̹Ƿ� ã�Ƴ� hItemFind�� hLotItem���� �����ϱ� ���� �Ҵ�.

		MyTrace(PRT_BASIC, "%s Lot already exist! use existing one.\n\n", pStrInsert);

		rbInserted = FALSE;
	}

	// ���ο� Lot�̸� tree�� Lot item�� �߰� 
	else	// Not Found:
	{
		// Tree InsertItem�� �״�� �̿��Ͽ� root�� ���ο� Lot Node�� �߰�
		hItemGet = m_treeCtrl.InsertItem(pStrInsert, m_hRoot); 
		m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
		hLotItem = hItemGet;	// ���� ������ hItemGet�� hLotItem���� �����ϱ����� �Ҵ�.

		MyTrace(PRT_BASIC, _T("%s Lot: Inserted to Tree.\n"), (LPCTSTR)pStrInsert);
		// �� �������� Insert�� dir�� dirName, dirPath, hLotItem handle ���� �����ؾ� �� �� �ִ�.
		// ���� ��¥ �ʿ��ϴٸ� ��������. 

		rbInserted = TRUE;
	}

	
	return hLotItem;
}

// �Է¹��� Tree Item�� Child ���� �߿��� �ش� str�� �ô� item�� ã�� �����Ѵ�.
HTREEITEM CStatisticsDialog::Tree_FindLotItem(CTreeCtrl* pTree, HTREEITEM hParent, LPCTSTR pStr)
{
	HTREEITEM hItem;
	hItem = pTree->GetChildItem( hParent );	

	return  Tree_FindLotItem2(pTree, hItem, pStr);		// recursive �Լ� ȣ��
}

// Recursive �Լ�: �Է¹��� Tree Item�� ���� ���� ���� �߿��� �ش� str�� �ô� item�� ã�� �����Ѵ�.
// Child�� Child�� ã���� recursive ȣ������ �ʴ´�. sibling�� recursive ȣ����.
HTREEITEM CStatisticsDialog::Tree_FindLotItem2(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr)
{
	HTREEITEM hItemFind, hItemChild, hItemSibling;
	hItemFind = hItemChild = hItemSibling = NULL;
	CString		hItemName = "";


	hItemName = pTree->GetItemText(hItem);

	// �Է¹��� str�� hItem �ڽ��� str�̸� Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		//MyTrace(PRT_BASIC, "Tree_FindLotItem2(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// �� ã������ 
	else 
	{
		// Lot ����������  ã�ƾ� �ϹǷ�, Child���� ������ �ʿ� ����.

		// ���� ������带 ã�´�. (������� recursive ȣ��)
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


// hItem���� ��� ��忡�� pStr�� ã�´�.
// Net�� ���� ������ �ʹ� ���Ƽ� �� �Լ��� ����ϸ� �ð��� �ʹ� ���� �ɸ��� ���� �߻�.
// Net�� ������ �Լ� g_sLotNetDate_Info.findLotNetId() �� ����.
//
// 2018.05.28 �� �Լ��� ������ ȣ��ÿ� stack over flow�� �״� ���� �־���.  
// ���ʿ��ϰ�  Net �������� Lot�� str�� �˻��ϴ� �κ��� �����ϱ� ���� 
// �� �Լ��� ������� �ʰ�  Tree_FindLotStrItem()�� ����ϱ�� �Ѵ�.
//
// 2018.09.19 Lot_0, Net_1�� hItem ��ġ�� ã������ �ٽ� ��Ȱ��Ŵ. Lot�� ã�µ� ���� 
//            Net�� ã�µ����� ����Ѵ�.
HTREEITEM CStatisticsDialog::Tree_FindStrItem(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr)
{
	HTREEITEM hItemFind, hItemChild, hItemSibling;
	hItemFind = hItemChild = hItemSibling = NULL;
	CString		hItemName = "";
	CString		nodeName = "";


	hItemName = pTree->GetItemText(hItem);

	// �Է¹��� str�� hItem �ڽ��� str�̸� Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		//MyTrace(PRT_BASIC, "Tree_FindStrItem(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// �� ã������ 
	else 
	{
		// �ڽ� ��带 ã�´�.
		hItemChild = pTree->GetChildItem( hItem );
		if ( hItemChild )
		{
			nodeName = m_treeCtrl.GetItemText(hItemChild);
			//MyTrace(PRT_BASIC, "Tree_FindStrItem(hItem=%s, pStr=%s) => recursive Tree_FindStrItem(%s,%s) for Child\n",  hItemName, pStr, nodeName, pStr);	// test prt
			hItemFind = Tree_FindStrItem( pTree, hItemChild, pStr );	// Recursive call

		}

		// ������带 ã�´�.
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
// hItem���� ��� ��� ��忡�� dwData�� ã�´�.
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
		// �ڽ� ��带 ã�´�.
		hItemChild = pTree->GetChildItem( hItem );
		if ( hItemChild )
		{
			hitemFind = Tree_FindData( pTree, hItemChild, dwData );

		}

		// ������带 ã�´�.
		hItemSibling = pTree->GetNextSiblingItem( hItem );
		if ( hitemFind==NULL && hItemSibling )
		{
			hitemFind = Tree_FindData( pTree, hItemSibling, dwData );
		}
	}

	return hitemFind;
}
*/

// �־��� hItem �� tree depth�� �����Ѵ�.
int CStatisticsDialog::Tree_GetCurrentLevel(HTREEITEM hItem)
{
	int nLevel = 1; 	// Root�� 1 level�� �Ѵ�.

	HTREEITEM	hNode = NULL;
	hNode = m_treeCtrl.GetParentItem(hItem);

	// hItem�� �������� ������ root�̹Ƿ� (nLevel == 1) �� ���ϵȴ�.
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

// Lot ���丮 ������ ��� Net Data File�� Open�ؼ� memory�� Load�Ѵ�.
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

		// 2018.06.18 Tree_LoadLot_4WFiles()���� nNetDataCount >= MAX_NET_DATA ���� �������� ��� ,
		//            �� �̻� �ٸ� Time File�� ã�� �ʵ��� �Ʒ� �ڵ� �߰���.
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
// 1. 4W Data File�� Open�ϰ� Net�� Data�� read�Ͽ� �޸� ����ü�� Load
// 2. ���ο� Net ��ȣ���� Ȯ���ϰ� ���ο� Net ��ȣ���, hParentLot Tree�� Net Item�� Insert 
int CStatisticsDialog::Load_4wDataFile(CString dataFilePath, CString dataFileName, HTREEITEM hParentLot, int nLot, int nDate)
{

	FILE 	*fp; 
	char  	fName[200]; 		
	char	str[1024];			// fgets()�� ����° �о� ���� ���� buffer

	// 2018.05.25: file name�� 'Sample'�� ���ԵǾ� �ִٸ�.  
	// Auto sampling�� ���� �Ͻ������� ������ �����̹Ƿ� �м� data���� �����Ѵ�.  
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
	//  4W Raw Data ���� ��� Read 
	::ZeroMemory(str, sizeof(str));
	::ZeroMemory(&strStep, sizeof(strStep));
	fgets(str, sizeof(str), fp);		// ���� ��� read.  ���� ����� ����° ��°�� �д´�.

	// ������� data �� ������ �ľ��ϱ� ���� "S=" �� ������ ī��Ʈ�Ѵ�.
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
		if (g_sLotNetDate_Info.naLotStepCnt[nLot] == 0)	// �ʱⰪ�̸� nFileStep ������  �ٲ۴�.
			g_sLotNetDate_Info.naLotStepCnt[nLot] = nFileStep;			

		// ���� nLot �ȿ����� nFileStep�� ���� ��� ���ƾ� �Ѵ�.
		else
		{
			strTemp.Format("Load_4wDataFile(): nFileStep=%d is not same with g_sLotNetDate_Info.naLotStepCnt[%d]=%d!", 
					nFileStep, nLot, g_sLotNetDate_Info.naLotStepCnt[nLot]);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 
			return -1;
		}

	}


	statNetData netData;		// �ѹ� �����ؼ� �ִ� 5000������ ��Ȱ��.
								// Stat_insertNetData()���� ���� data�� copy�� �Ͼ��.

	// data file name���� time ȹ��.
	strTime = dataFileName.Mid(15, 6);
	netData.statTime.hour = atoi(strTime.Left(2));		// �Ǿ��� 2����
	netData.statTime.min  = atoi(strTime.Mid(2, 2));	// 3��°���� 2���� 
	netData.statTime.sec  = atoi(strTime.Mid(4, 2));	// 5��°���� 2����. 


	// Net ���� ��ŭ Line�� �д� ������ �ݺ�
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
		// 2018.10.17 Tree_LoadLot_4WFiles()���� nNetDataCount >= MAX_NET_DATA ���� �������� ��� ,
		//            �� �̻� ���� Net�� ã�� �ʵ��� �Ʒ� �ڵ� �߰���.
		if (m_nNetDataCount >= MAX_NET_DATA)
			break;

		::ZeroMemory(str, sizeof(str));
		fgets(str, sizeof(str), fp);	// �� ������ ��°�� �о�� ���� ����ó���� sscanf�� �ñ��. 
										// �뷮�� �����͸� fscanf�� ������ �޸� overflow���� ����.
										// sscanf() buffer�� CString�� ����ϸ� �뷮���� read �ݺ��ϴ� �������� 
										// CString ���۰� overflow�� ����  char[]�� ��ü��.
		
		//if (row >= 454)
		//	MyTrace(PRT_BASIC, "aaa%saaa\n", str);		// str �Է°� Ȯ�ο� �׽�Ʈ

		// fgets�� read�� str�� �� str �Ǵ� '\n' �� �ִ� ��� sscanf �������� �ʰ� skip.
		//if (strcmp(str,"") == 0 || strcmp(str," ") == 0 || strcmp(str,"   \n") == 0)		
		//{
		//	row++;
		//	continue;
		//}		==> �� üũ�ڵ� �߰��� �ð��� �ʹ� ���� �ɸ�. 
		//          �ϱ� �ڵ忡 nNet, strWord�� �ʱ�ȭ�ϴ� ������� �ذ��ϱ�� �ٲ�.
										
		// "Net"���� �����ϴ� ������ �ƴϸ� ó������ �ʰ� line skip.
		nNet = -1;
		::ZeroMemory(strWord, sizeof(strWord));
		sscanf(str, "%s %d", strWord, &nNet); 	// Read "Net" Str, nNet #
		if (strcmp(strWord,"Net") != 0)		
		{
			row++;
			continue;
		}

		// nNet �� range over�� ��� line skip
		if (nNet < 0 || nNet >= MAX_NET_PER_LOT)	 
		{
			row++;
			continue;
		}


		// loop�� �� �� ���� �Ʒ� ���۸� �ʱ�ȭ�Ѵ�. 
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
		netData.dLsl  = (double)atof(strWord2[0]);	// min-> LSL(Lower Spec Limit)���� ���.
		netData.dRefR = (double)atof(strWord2[1]);		
		netData.dUsl  = (double)atof(strWord2[2]);	// max-> USL(Upper Spec Limit)���� ���.

		// 2018.05.31 Lsl, Usl�� ACE400���� ������ ���� ��ŭ, Data Read���� time tuple ���� �ٸ� Lsl, Usl ������ 
		// 			  fault�� üũ�ϴ°� ������ ������ �Ǵܵ�.  (ACE400���� ������ �ٲ�ٸ� �߰��� �޶����� �����Ƿ�)
		//            ���� Net�� tuple data�� ������� �ʰ� summary data�� listup�ϴ� ����� �߰��ȴٸ�
		//            Net�� tuple ���� Usl, Lsl�� ���� �ʿ��� �� ����. 
		//            Net�� tuple �������� Usl, Lsl ����ϴ� ����  ������ �ʰ� �ʰ� ������ ���� ����.
		//g_sLotNetDate_Info.daLotNetLsl[nLot][nNet] = (double)atof(strWord2[0]);	// min-> LSL(Lower Spec Limit)���� ���.
		//g_sLotNetDate_Info.daLotNetUsl[nLot][nNet] = (double)atof(strWord2[2]);	// max-> USL(Upper Spec Limit)���� ���.

		// Read Measure Value --------
		int	   nStep = 0;
		for (i = 0; i < nFileStep; i++)
		{
			// NG �� üũ
			if (strcmp("NG", strStep[nStep]) == 0)
			{
				nStep++;			// NG�� skip �Ѵ�.

				// NG 999999.00 �� ��쿡�� ��¥ NG ó��
				if (strcmp("999999.00,", strStep[nStep]) == 0)
				{

					netData.daStep[i] = (double)(-1);
					nStep++;		// 999999.00 �� -1�� ó��
				}

				// NG 64.28 �� ����  ���ڰ� �ڿ� ���� ���
				// NG�� ���� ���� ���� ���� ǥ�� �̹Ƿ� ���ڸ�  ����ó���Ѵ�.
				else
				{
					netData.daStep[i] = (double)atof(strStep[nStep]);
					nStep++;		
				}
			}

			// NG ���� 999999.00 �� �ִ� ���  NG ó��
			else if (strcmp("999999.00,", strStep[nStep]) == 0)
			{
				netData.daStep[i] = (double)(-1);		
				nStep++;			// 999999.00 �� -1�� ó��
			}

			else
			{
				// daStep �� 
				netData.daStep[i] = (double)atof(strStep[nStep]);
				nStep++;

			}
		}


		//----------------------
		// Tree�� Net item �߰�
		
		// ȹ���� Net ��ȣ�� Tree�� �ش� Lot ��ġ�� �߰��Ѵ�. 
		// 1. �߰��ؾ��� Net�� string�� ���� branch�� �����ϴ��� �˻��Ѵ�.
		// 2. ���ο� ����� ������ ������ ��带 �߰��Ѵ�.
		CString strInsert;
		strInsert.Format("Net_%d", nNet);
		if ((g_sLotNetDate_Info.findLotNetId(nLot, nNet)) == -1)
		{
			// Tree InsertItem�� �״�� �̿��Ͽ� ���ο� Node�� �߰�
			m_treeCtrl.InsertItem(strInsert, hParentLot); 
			//m_treeCtrl.Expand(hParentLot, TVE_EXPAND);
			g_sLotNetDate_Info.setLotNetId(nLot, nNet);		// �ش� Lot, Net�� �������� ǥ��

		}
		strInsert.Empty();

		//---------------------------------
		// Read�� netData�� �޸𸮿� �߰�
		Stat_insertNetData(nLot, nNet, nDate, netData);		

		row++;

	}
	fclose(fp);

	//TRACE ("g_pvsaNetData[nLot=%d][nNet=%d][nDate=%d]->size() = %d\n", 
	//					nLot, nNet, nDate, g_pvsaNetData[nLot][nNet][nDate]->size());



	return 0;
}



// Stat Data�� �޸𸮿� �߰� �Ѵ�. 
int CStatisticsDialog::Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData)
{
	// 2018.06.18: m_nNetDataCount��  MAX_NET_DATA ������ �Ѿ�� 4W Data �ε��� �����ϴ� ����߰�
	if (m_nNetDataCount >= MAX_NET_DATA)
		return -1;

	// g_pvsaNetData[nLot][nNet][nDate]�� ���ٸ� ���� new�ؼ� �����Ѵ�. �̹� �����Ѵٸ� �׳� 0����, �����ִٸ� -1����
	int ret = Stat_insertNetVector(nLot, nNet, nDate);
	if (ret < 0)
		return -1;

	// 2K�� �Ѵ´ٸ� vector ������ �� Ȯ���Ѵ�.  
	// 2018.06.19: 2048�� �Ǵ� ������ 1ȸ������ �����ϸ�,  
	//             vector resize ���߿� "�޸� ����" �޽��� ������ ������ ���� �� �ִ�.
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

	// �ش� vector�� ���� netData�� insert�Ѵ�.
	//   :  time ������ ���� vector�� �̹� time�� netData�� push_back �Ѵ�. 
	//   netData�� value�� ���ڷ� �ѱ�� �������� copy�ؾ� �ϹǷ� statNetData Ŭ������ 
	//   copy constructor(���������) ȣ���� �Ͼ��.  �����͸� �ѱ�� �����, ���� �����ھȿ���
	//   �����Ϳ� ���� ���� new�� �ؼ� ������ ����� ��� ���߿� delete[]�Ҷ��� ������ �� ����. (deep copy..)
	g_pvsaNetData[nLot][nNet][nDate]->push_back(netData);


	// 2018.06.18: m_nNetDataCount��  MAX_NET_DATA ������ �Ѿ�� 4W Data �ε��� �����ϴ� ����߰�
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

// g_pvsaNetData[nLot][nNet][nDate]�� ���ٸ� ���� new�ؼ� �����Ѵ�. �̹� �����Ѵٸ� �׳� 0����, �����ִٸ� -1����
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
	
	// �ش� Net�� data�� insert�� ���� ���ٸ�  vector�� ���� �����ؼ� assign�Ѵ�.
	//  : Lot * Net * Date ������ vector �����Ϳ� ���� vector�� assign
	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		vector <statNetData>*  pvNetData = new vector <statNetData>;
		if (pvNetData == NULL)		// �޸��Ҵ� ����!
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

// ��� Stat data�� �����ϰ� �Ҵ�� memory�� delete�Ѵ�.
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
					//int prevTupleSize = g_pvsaNetData[lot][net][date]->size();	// date���� tupleSize�� �ٸ�!

					// vector�� ��� data������ (��� tuple�� �����ϰ�)
					if (g_pvsaNetData[lot][net][date]->empty() == false)
						g_pvsaNetData[lot][net][date]->clear();		


					delete g_pvsaNetData[lot][net][date];		// �����Ҵ�� vcetor�� ��ȯ�Ѵ�.
					g_pvsaNetData[lot][net][date] = NULL;		// ��ȯ�� vector �����͵� NULL ó��
				}
			}
		}
	}

}

// Tree�� scan�ϸ鼭 ��� Lot, ��� Net�� Fault ǥ�ø� �����.
void CStatisticsDialog::Tree_CheckAll_LotNetFaults()
{
	HTREEITEM 	hItemChild, hItemNextChild;
	CString		lotName = "";
	int			nLot = 0;

	// ��Ʈ��  �ڽ� Lot ��带  ã�´�.
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
		// ���⼭�� Lot�� Child üũ���� �ʰ�  Tree_Check_LotNetFaults(hItemChild, nLot) �� �ñ��.
		// �ڽ��� ����, �� ������ Lot ��带 ã�´�.
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
// �ش� Lot�� ����  ��� Net�� Fault ǥ�ø� �����.
void CStatisticsDialog::Tree_Clear_LotNetFaults(HTREEITEM hItem, int nLot)
{
	HTREEITEM hItemChild, hItemNextChild;
	CString netName = "";

	// �ڽ� ��带 ã�´�.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		netName = m_treeCtrl.GetItemText(hItemChild);
		MyTrace(PRT_BASIC, "Tree_Clear_LotNetFaults(nLot=%d, %s): Clear Icon...\n", nLot, netName);
		m_treeCtrl.SetItemImage(hItemChild, 0, 0); 	// hNetItem ��ġ�� Normal ó���Ѵ�. 
	}

	while (hItemChild)
	{
		// �ڽ��� ������带 ã�´�.
		hItemNextChild = m_treeCtrl.GetNextSiblingItem(hItemChild);
		if (hItemNextChild)
		{
			netName = m_treeCtrl.GetItemText(hItemNextChild);
			MyTrace(PRT_BASIC, "Tree_Clear_LotNetFaults(nLot=%d, %s): Clear Icon...\n", nLot, netName);
			m_treeCtrl.SetItemImage(hItemNextChild, 0, 0); 	// hNetItem ��ġ�� Normal ó���Ѵ�. 
		}

		hItemChild = hItemNextChild;
	}

}
*/

// �ش� Lot�� ����  ��� Net�� Fault ���θ� �Ǵ��Ѵ�.  
// Tree������ Net Fault�� �ش� net�� ���ϴ� ��� date�� faultī��Ʈ�� Ȯ���ؼ� 
// �����ؾ� �ϹǷ�  data�� ALL�� üũ�ؾ� �Ѵ�.
void CStatisticsDialog::Tree_Check_LotNetFaults(HTREEITEM hItem, int nLot)
{
	HTREEITEM hItemChild, hItemNextChild;
	CString netName = "";

	// Lot�� �ڽ� ���, �� net��  ã�´�.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		Tree_CheckNetFault(hItemChild, nLot);		// Date = ALL�� üũ
		//netName = m_treeCtrl.GetItemText(hItemChild);
		//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
	}

	while (hItemChild)
	{
		// net ���������� ã�ƾ� �ϸ� Child���� �������� �ʴ´�.
		// �ڽ��� ������� (���� net)�� ã�´�.
		hItemNextChild = m_treeCtrl.GetNextSiblingItem(hItemChild);
		if (hItemNextChild)
		{
			Tree_CheckNetFault(hItemNextChild, nLot);	// Date = ALL�� üũ
			//netName = m_treeCtrl.GetItemText(hItemNextChild);
			//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
		}
		hItemChild = hItemNextChild;
	}

}

// �ش� Net�� Fault ���θ� �Ǵ��Ѵ�.  (Net�� fault ����)
// Net Tree�� �������� date �߿� �ϳ��� fault�� ������ fault�����ؾ� ��.
// date �ϳ��� fault�� ���ٰ� fault�� Ŭ�����ϸ� �ȵǰ�, �ٸ� ��� date�� üũ�Ѵ�.
void CStatisticsDialog::Tree_CheckNetFault(HTREEITEM hNetItem, int nLot)
{
	CString	itemName, strTemp; 
	int 	nNet;
	
	
	itemName = m_treeCtrl.GetItemText(hNetItem);
	nNet = atoi(itemName.Mid(4));


	//-----------------------
	// Fault ó�� ����
	
	int 	tupleSize, tuple, step, stepSize;
	double 	dLsl, dUsl;

	// 2018.05.31  Net ���� ���� Lsl, Usl�� ���� tuple�� Lsl, Usl�� ����ϱ�� ��.
	//dLsl   = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];	
	//dUsl   = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];	
	
	// USL, LSL���� time tuple���� �ٸ� ������ ó���Ϸ��� Display �ܰ迡���� fault üũ. 
	// LSL, USL ���� ��� time tuple�� �������� ���� ��������� üũ�Ϸ��� ���⿡�� �̸� üũ�Ѵ�.  
	// ��� �ΰ��� ����� ���� fault�� ���� ����� �ٸ� �� �ִ�.
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
			ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
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
				// 2018.05.31  tuple�� Lsl, Usl ���
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
	// Check ����� Net  Icon�� �ݿ��Ѵ�.
	// Net Icon �ݿ������� Date = ALL �̹Ƿ�, �ش� date�� fault count�� ��� ���� ������ �Ǵ��Ѵ�.
	
	m_treeCtrl.SetItemImage(hNetItem, 0, 0); 	// hNetItem ��ġ�� �ϴ� Normal ó���Ѵ�. 

	//if (nNetFaultCount > 0)
	if (g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] > 0)
	{
		// Fault�� �ִ� ��츸 Tree�� hNetItem ��ġ�� Fault(������ ��� ������) ó���Ѵ�.
		m_treeCtrl.SetItemImage(hNetItem, 
								3, 			// nImage:         tree ���õ��� �ʾ������� �̹���
								3);			// nSelectedImage: tree ���õǾ��� ���� �̹���
		// Test Print
		//MyTrace(PRT_LEVEL2, _T("Tree_CheckNetFault(nLot=%d, nNet=%d): dateCount=%d, nNetFaultCount=%d\n"), 
		//			nLot, nNet, g_sLotNetDate_Info.naLotDateCnt[nLot], nNetFaultCount);
		MyTrace(PRT_LEVEL2, _T("Tree_CheckNetFault(nLot=%d, nNet=%d): dateCount=%d, waNetFaultCount=%d, waNetNgCount=%d, TotCount=%d\n"), 
					nLot, nNet, g_sLotNetDate_Info.naLotDateCnt[nLot], 
					g_sLotNetDate_Info.waNetFaultCount[nLot][nNet],
					g_sLotNetDate_Info.waNetNgCount[nLot][nNet],
					g_sLotNetDate_Info.waNetTotalCount[nLot][nNet]);

		// Fault ����Ʈ �ڽ��� �߰�  (TBD)

	}

}



// File Open Dialog�� ����� Ư�� Lot_Date ���丮�� �����ϰ� tree�� �ε��� �� �ֵ��� �Ѵ�.
void CStatisticsDialog::OnButtonLoad4wData_SingleLot() 
{
	// TODO: Add your control notification handler code here
	//
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	

	//-----------------------------
	// File Open Dialog ����
	
	CString strTemp;
	char szFilter[] = "CSV Files(*.CSV)|*.CSV|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, 		// bOpenFileDialg = TRUE: Ÿ��Ʋ�ٿ� '����' ���
					NULL, 		// lpszDefExt           : �⺻ Ȯ����
					NULL, 		// lpszFileName         : "���� �̸�" Edit ��Ʈ�ѿ� ó�� ��µ� ���ϸ�.
					OFN_HIDEREADONLY |OFN_NOCHANGEDIR |OFN_ALLOWMULTISELECT, 
								// dwFlags :  �Ӽ�   (HIDEREADONLY : read only ������ ������� ����.
					szFilter);	// ��ȭ���ڿ� ��µ� ������ Ȯ���ڷ� �Ÿ��� ���� ����. 


	// �����Ҷ� �ʱ� ��� ����. �Ǿտ� �ҹ��� d: ���� ����� ������..
	dlg.m_ofn.lpstrInitialDir = _T("d:\\log4w"); 
	
	// ���⼭ ���� ũ�� �÷���� ��.  �� �÷� �ָ� 6���̻� file ������ �� ��.
	// default 256byte => file 6�� ó�� ����.  �� fileName�� �� 43���� ������� ����.
	// fileName ����� �˳��ϰ� 60���ڷ� �ְ� 4000���� ó���ؾ� �ϹǷ�  60*4000 = 240000 
	// 2018.07.17 buffer�� ����� 240000 ���� �ſ� ũ�Ƿ� stack overflow�� ������ �� �־� static���� ������.
	static char buffer[60 * MAX_TIME_FILE] = {0}; 	// ����
	dlg.m_ofn.lpstrFile = buffer; 						// ���� ������
	dlg.m_ofn.nMaxFile = (60 * MAX_TIME_FILE); 	// ���� ũ��,  file 3000�� ó���Ϸ��� ������ buffer�ʿ���.


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
	// LotName, DateName ó��

	// ���õ� CSV���ϵ��� ������(dirPath)�� ȹ���Ѵ�. --------------
	

	// '\'��ġ(nSlashLoc) �޺κ�(fileName)��  ����� ������(dirPath)�� ���´�.
	//   ex) "D:\log4w\20180503_036782A2S\Ng_Log4WSample0_20180503_010135_NG_Net_-2558.CSV"
	//        nSlashLoc�� 27�� ���ϵ�. 27���� Left()�� �̿��ؼ� ������ �����ϸ� ����Path (dirPath).
	//        dirPath�� "D:\log4w\20180503_036782A2S"   . ��������� ������ fileName�� ������.
	CString dataFilePath, dirPath;
	POSITION pos= dlg.GetStartPosition();
	dataFilePath = dlg.GetNextPathName(pos);
	
	//  ReverseFind()�� �ڿ������� '\' ��ġ�� ã�´�.  
	int nSlashLoc = dataFilePath.ReverseFind('\\');	 
	dirPath = dataFilePath.Left(nSlashLoc); 
	MyTrace(PRT_BASIC, "Load 4wData(Single)...: Lot=%s\n\n\n", dirPath);


#if 0
 	// 2019.01.29 :  D:\\log\\ �̿��� ���丮�� �ε��� �ʿ��Ͽ� �Ʒ� �ڵ带 �ڸ�Ʈ ó����
 	
	// d:\log4w ���丮 �Ʒ��� ���丮���� Ȯ���Ѵ�. ---------------
 	if (dirPath.Find("D:\\log4w\\") == -1)	
	{
		strTemp.Format("%s is not 4W Data Directory. Can't do InsertItem. :OnButtonLoad4wData_SingleLot()\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);


	// ���� "D:\log4w\"�� �ش��ϴ� ù��° level�� lotName���� �����Ѵ�.
	CString lotName = dirPath;
	int	length = strlen(lotName);
	lotName = lotName.Mid(9, (length -9));		
							// 2018.06.28: �߶󳻴� ��ġ�� 9�� �����ϸ� D:\log4w�̿ܿ� �ٸ� ��츦 Ŀ������ ����.
							// 			   ����� �Ǽ� �̹Ƿ� �������� �����Ѵ�. d:\log4w_2 �� �����Ѵٸ� ó���� ����������
							// 			   �Ǵµ� date name�� �̻��ϰ� ��µȴ�.

#else
	// ReverseFind()�� �ڿ������� �ι�°�� '\' ��ġ (D:\log4w\�� ������ ��ġ)�� ã�´�.
	// log4w�� �ƴ϶� log4w_2���� �̸��̾ ����� �����ϰ� �ȴ�.
	int nSlashLoc2 = dirPath.ReverseFind('\\');	 
 
	// 2019.01.31: �߶󳻴� ���������� nSlashLoc2+1�� �Ѵ�. ������ D:\log4w\��� nSlashLo2+1�� 9�� �ȴ�.
	CString lotName = dirPath;
	int	length = strlen(lotName);
	lotName = lotName.Mid((nSlashLoc2+1), (length - (nSlashLoc2 + 1)));	

#endif

	// "20170925" ó�� ��¥�� �������� �ʴ´ٸ� ����.  (Lot ���丮�� �ƴ�) -------------
	CString dateName  = lotName.Left(8);		// ��¥ str ����.
	CString dateExclu = dateName.SpanExcluding("0123456789"); 
	if (!dateExclu.IsEmpty())	// "0123456789"�� �ش����� �ʴ� ���ڸ� ����. Empty�� �ƴϸ� ���������̶� ���
	{
		strTemp.Format("\"%s\" \n '4w Data' Directory has not proper DateName. Can't do InsertItem.: OnButtonLoad4wData_SingleLot(): \n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}



	// "20170925_A30-19-C-4W-NEW-0920" ��� �Ǿ��� ��¥�� ���� tree item���� �����Ѵ�. 
	lotName = lotName.Mid(9, (length -9));		// "20170925_" ��¥ �κ��� �����.


	// �������.. Lot Name, Date Name Ȯ��.
	MyTrace(PRT_BASIC, "Load 4wData(Single): Lot=%s, Date=%s \n\n\n", lotName, dateName);
	

	//-------------------------
	// Check Empty Directory     
	
	// �� lot�� �߰��Ǵ� ���� ���� ���� Insert�� ���丮�� �� ���丮���� �˻��Ѵ�.  (�ڿ�����)
	// ������ ������ name�� ó���� ������ ���ϵ����� Ȯ���Ѵ�.  'Sample_xxxx' ���ϸ� �������� ��츦 ���
	CString fileName; 
	BOOL	bFileFound = FALSE;


	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath �κ��� ����� fileName�� �����. (dirPath+'\' �����ŭ �պκ��� �ڸ�)
		int pathLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (pathLength - (nSlashLoc +1)));		 
		if (fileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		// �����̸��� 'Sample_xxxx'�� �ƴ� ���� 4W data file�� �ϳ��� �ִٸ� break; 
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
	// UI (Tree, Date, Grid )  �� �޸� �ʱ�ȭ 
	
	BeginWaitCursor();			// Ŀ���� waiting ���� �ٲ۴�.
	
	ClearGrid(); // data Grid, summary Grid, edit�ڽ� �� UI�� �ʱ�ȭ�Ѵ�.
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	Stat_deleteAllNetData();	// ���� netData�� ��� �޸𸮿��� �����.
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	InitMember();				// CStatisticsDialog ��� ������ ��� �ʱ�ȭ �Ѵ�.
	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	Display_DataGridHeader(); 	// Data Grid Header �ٽ� ���� (simul�� on�̾��ٸ� off�� �ٲٰ� ����� �ٽ� �׷������)

	InitTree();					// root���� Tree�� �ٽ� ����.	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);


	//-------------------------
	// Lot, Date �׸� �߰�
	
	int lotId, dateId;
	lotId  = g_sLotNetDate_Info.getLotId(lotName);
	if (lotId == -1)	// lot range�� �Ѿ�ٸ� load���� �ʴ´�.
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): %s getLotId(%s) Failed! nLotCnt=%d Range(<%d) Over!\n\n Can't do InsertItem.", 
					dirPath, lotName, g_sLotNetDate_Info.nLotCnt, MAX_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		return;
	}
	dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
	if (dateId == -1)	// date range�� �Ѿ�ٸ� load���� �ʴ´�. 
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): getLotDateId(%s/%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
				lotName, dateName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		return;
	}

	// Tree InsertItem�� �״�� �̿��Ͽ� root�� �ش� Lot Node�� �߰�
	HTREEITEM hLotItem = m_treeCtrl.InsertItem(lotName, m_hRoot); 
	m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);


	//------------------------------------------
	// Lot �ؿ� ���õ� CSV ���ϵ��� �ϳ��� Load 
	
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

		// 2018.06.18 Tree_LoadLot_4WFiles()���� nNetDataCount >= MAX_NET_DATA ���� �������� ��� ,
		//            �� �̻� �ٸ� Time File�� ã�� �ʵ��� �Ʒ� �ڵ� �߰���.
		if (m_nNetDataCount >= MAX_NET_DATA)
		{
			if (m_nNetDataCount == MAX_NET_DATA)
				MyTrace(PRT_BASIC, "m_nNetDataCount=%d, Load4wData(Single) CsvFile Loading Stopped!! nFile=%d\n\n", m_nNetDataCount , nFile);
			break;
		}

		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath �κ��� ����� fileName�� �����. (dirPath+'\' �����ŭ �պκ��� �ڸ�)
		int pathLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (pathLength - (nSlashLoc +1)));		 

		//MyTrace(PRT_BASIC, "Load4wData(Single): Path=%s, file=%s \n", dataFilePath, fileName );	 // test print

		// �ش� CSV file�� load�Ѵ�. 
		if (Load_4wDataFile(dataFilePath, fileName, hLotItem, lotId, dateId) < 0)
			continue;

		nFile++;
	}

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	//------------------------------------------
	// �ش� Lot�� ��� Net�� Fault Check
	Tree_Check_LotNetFaults(hLotItem, lotId);
	
//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);

	strTemp.Format("\'Load 4w Data\' for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", dirPath);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "OnButtonLoad4wData_SingleLot() for\"%s\" Completed. \n", dirPath);
	

	
	// default Net ��ȸ: ������ Ŭ���� ��� ù��° Lot, ù��° Net�� ��ȸ�� �ش�.
	DisplayGrid_DefaultLotNet();

//__PrintMemSize(FUNC(Load4wData_SingleLot), __LINE__);
	MyTrace(PRT_BASIC, "          m_nNetDataCount = %d\n", m_nNetDataCount);
	MyTrace(PRT_BASIC, "\n");


	//--------------
	// �Ϸ� 
	
	EndWaitCursor();		// Ŀ���� ���� �����Ѵ�.

	// LOAD LOG4W�� �Ϸ�Ǿ����� FR Rank Dlg�� �˸���.
	//::SendMessage(m_hwnd_FrRankDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
}



// Tree Control�� ���� item�� ����� �� ȣ��
void CStatisticsDialog::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	_SelChangedTree(pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

// 2018.09.19 ���� click ���̵� default�� Lot_0, Net_1�� Ŭ������ �ֱ� ���� �̷��� �������̽��� �и���.
//            ���� click�� OnSelchangedTree()�� �� �Լ��� ����Ѵ�. (���� ȿ��)
void CStatisticsDialog::_SelChangedTree(HTREEITEM  hSelItem)
{

	HTREEITEM	hParent;
	CString		strTemp;
	CString		parentName = ""; 
	CString		selectedName = "";

	// ���� ������ �������� �ڵ� ���� ��������� �����Ѵ�.
	m_hSelectedNode = hSelItem;
	hParent			= m_treeCtrl.GetParentItem(m_hSelectedNode);

	// test
	//m_treeCtrl.SetItemImage(m_hSelectedNode, 2, 3);		// item�� ������ ��� ������ ����  �׽�Ʈ

	// ������ �������� �̸��� ��ȭ������ ����Ʈ ���ڿ� �����Ѵ�.  
	if (hParent != NULL)
		parentName   = m_treeCtrl.GetItemText(hParent);
	selectedName = m_treeCtrl.GetItemText(m_hSelectedNode);

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);

	if (nLevel == 3)			// ������ �������� Net��� 'lot# - net#'�� ����
		m_editStrSelect = parentName + " / " + selectedName;
	else
		m_editStrSelect = selectedName;


	// Net�� ���
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

		// ���� combo�� date data�� Lot�� �°� �ٲپ� �ش�.  m_nCombo_CurrDate�� 0(DATE_ALL)�� ����. 
		Combo_UpdateDateContents(nLot);
		m_nTree_CurrLot = nLot;
		m_nTree_CurrNet = nNet;

	}

	// Lot �� ���
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
		// ���� combo�� date data�� Lot�� �°� �ٲپ� �ش�.  m_nCombo_CurrDate�� 0(ALL)�� ����. 
		Combo_UpdateDateContents(nLot);
		m_nTree_CurrLot = nLot;
		m_nTree_CurrNet = -1;

	}

	// Total�� ���
	else
	{

		// ���� combo�� date data�� ��� �����.
		m_comboDate.ResetContent();	

		// Total�� data�� summaryGrid�� ����� �ش�.
		m_nTree_CurrLot = -1;			// �̼���
		m_nTree_CurrNet = -1;			// �̼���
		m_nCombo_CurrDate = DATE_ALL; 	// All     0=All,  1~7 : ���� date#+1

	}

	// Ȥ�� ������ Simulation �������¿��ٸ� �����Ѵ�.
	CheckOff_SimulUlsl();	

	// ���õ� Net�̳�, Lot�� Summary�� summaryGrid�� ����� �ְ�,  
	// Net data�� date���� ���ϰ� ������ �Ǿ��ٸ� dataGrid�� ����� �ش�.
	// 2018.05 Display ��ư�� ������ �� ����� �ִ� ������ ������.
	// 2018.06.15 �̻��� ���� ��û���� Date�� Tree Ŭ�������� ��ȸ�� �����ϰ� �ٽ� ������.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		



	// ��ȭ������ Edit��Ʈ�ѿ� m_editStrSelect �� ����Ѵ�.
	UpdateData(FALSE);
	
}

// ���� combo�� date data�� Lot�� �°� �ٲپ� �ش�.  m_nCombo_CurrDate�� 0(ALL)�� ����. 
// m_nTree_CurrLot �� ���� �ٲܶ����� �ݵ�� ���� Combo_UpdateDateContents(nLot)�� ȣ���Ѵ�.
void  CStatisticsDialog::Combo_UpdateDateContents(int nLot)
{
	// Lot ��ȣ�� �ٲ���ٸ�, ���õ� Lot�� ���ϴ� date ���� comboDate�� �־��ش�.
	if (m_nTree_CurrLot != nLot)
	{
		// ���� combo�� data�� ��� �����.
		m_comboDate.ResetContent();	
		
		// com idx=0 �߰�
		m_comboDate.InsertString(-1, "ALL");		// combo idx = 0: DATE_ALL

		// date_0�� �ι�° (idx=1) ��  combo�� �߰��ȴ�.  combo idx�� 'date+1'�� �ٷ� ��.
		for (int date = 0 ; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
			m_comboDate.InsertString(-1, g_sLotNetDate_Info.strLotDate[nLot][date]);			

		// net�̳� combo Ŭ���ÿ� �ٷ� display�� ���� date �� default�� "ALL"�� ������ �ش�.
		m_nCombo_CurrDate = DATE_ALL; 	//  0=DATE_ALL,  1~7 : ���� date#+1
		m_comboDate.SetCurSel(0);

		UpdateData(FALSE);		// SetCurSel ����� UI�� �ݿ�.
	}
}

void CStatisticsDialog::OnSelchangeComboDate() 
{

	// TODO: Add your control notification handler code here
	CString strTemp;

	int 	comboPrevDate = m_nCombo_CurrDate;
	m_nCombo_CurrDate = m_comboDate.GetCurSel();		// ���� combo���� ���õ� date# (0=ALL�̹Ƿ� ���� date+1),  �̼����̸� -1

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

	// Ȥ�� ������ Simulation �������¿��ٸ� �����Ѵ�.
	CheckOff_SimulUlsl();	
	
	// ���õ� Net�̳�, Lot�� Summary�� summaryGrid�� ����� �ش�. 
	// 2018.05 Display ��ư�� ������ �� ����� �ִ� ������ ������.
	// 2018.06.15 �̻��� ���� ��û���� Date�� Tree Ŭ�������� ��ȸ�� �����ϰ� �ٽ� ������.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

}


// 2018.05 Display ��ư�� ������ �� ����� �ִ� ������ ������.
// 2018.06.15 �̻��� ���� ��û���� Date�� Tree Ŭ�������� ��ȸ�� �����ϰ� �ٽ� ������.
//            Display Data ��ư��  UI����  '������ �ʰ�' property invisible ó����.
//            �Ʒ� �Լ� ��ü�� ���� �ξ����� ȣ������ �����Ǿ����Ƿ� ���� �������� �ʰ� ����.
/*
void CStatisticsDialog::OnButtonDisplay() 
{
	// TODO: Add your control notification handler code here
	
	// ���õ� Net�̳�, Lot�� Summary�� summaryGrid�� ����� �ش�. 
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		
	
}
*/


void CStatisticsDialog::DisplayGrid(int nLot, int nNet, int nComboDate) 
{
	CString strTemp;

	// Tree�� Root�� �ִٸ� Load ��ư�� ������ �Ѵٴ� �޽��� ���.
	if (m_treeCtrl.GetCount() == 1)
	{
		strTemp.Format("Execute \"Load 4W Data\" button, First.\n And select any Tree item and Date item please. \n :DisplayGrid()");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// Ŭ���� item�� ���ٸ�  �ƹ� item�̳� Ŭ���϶�� �޽��� ���.
	if (m_hSelectedNode == NULL)
	{
		strTemp.Format("Select any Tree item and Date item please.\n: DisplayGrid()");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);
	
	//--------------
	// Net�� ���
	
	// ���� ���õ� Lot, Net, Date�� data�� dataGrid, summaryGrid�� ����� �ش�.
	if (nLevel == 3)
	{
		// Lot, Net, Date�� ��� ���� �����Ѵٸ�, DataGrid�� SummaryGrid ���
		if (nComboDate != DATE_ALL )	
		{
			// DataGrid, SummaryGrid ��� ���.
			// 1. Lot, net, date�� key�� �ؼ�  ��� time tuple�� sum�ؼ� summary grid  ���
			// 2. Lot, net, date�� ��� �����ϹǷ� �ش�Ǵ� ��� time tuple Raw data��  data grid �� ���. 
			
			GetDlgItem(IDC_GRID_DATA)->ShowWindow(TRUE);		// DataGrid ���̱� 

			Display_SumupLotNetDate(nLot, nNet, nComboDate -1);			
 											// m_nCombo_CurrDate�� ���� Date +1 ���̹Ƿ� -1�ؼ� �ѱ��.

		}
		// Lot, Net �����ϰ�, date=All �̶�� SummaryGrid ���.
		else
		{
			// SummaryGrid ���.
			// 1. lot, net�� key�� �ؼ� ��� date, ��� time tuple�� sum�ؼ� summary grid ���.
			// 2. data grid�� ���õ� date�� ��� data�� ����Ѵ�.   (������ �ʴ´�.)
			
			GetDlgItem(IDC_GRID_DATA)->ShowWindow(TRUE);		// DataGrid ���̱� 
			//GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid �����
			
			Display_SumupLotNet(nLot, nNet);
	
		}
		
	}

	//--------------
	// Lot�� ���
	else if(nLevel == 2)
	{
		GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid �����
		//ClearGrid_Data();		// DataGrid �����

		// Lot ����, Net= -1, Date���� �����Ѵٸ�
		// 1. lot, date�� key�� �ϰ�,  ��� net, ��� time tuple�� sum �ؼ� summary grid  ��� 
		if (nComboDate != DATE_ALL )	
			Display_SumupLotDate(nLot, nComboDate -1 ); 	// Summary Data ���.
 													// m_nCombo_CurrDate�� ���� Date +1 ���̹Ƿ� -1�ؼ� �ѱ��.
 											
		// Lot ����, Net= -1, Date=All
		// 1. Lot�� key�� �ؼ� ��� net, ��� date, ��� time tuple�� sum�ؼ� summary grid ���
		else	
			Display_SumupLot(nLot);

	}

	//--------------
	// Total�� ���
	else if(nLevel == 1)
	{
		GetDlgItem(IDC_GRID_DATA)->ShowWindow(FALSE);		// DataGrid �����
		//ClearGrid_Data();		// DataGrid �����

		// Lot ������, Net ������, Date = All
		// 1. ��� lot,  ��� net, ��� date, ��� time tuple�� sum�ؼ� summary grid ���.
		if (nComboDate == DATE_ALL)
			Display_SumupTotal();	// Summary Data ���� 

	}

}


// Lot, Net, Date�� ��� ���� �����Ѵ�. Date Grid�� Summary Grid ��� ���.
// 1. Lot, net, date�� key�� �ؼ�  ��� time tuple�� sum�ؼ� summary grid  ���
// 2. Lot, net, date�� ��� �����ϹǷ� �ش�Ǵ� ��� time tuple Raw data��  data grid �� ���. 
void CStatisticsDialog::Display_SumupLotNetDate(int nLot, int nNet, int nDate)
{
	CString strTemp;

	// Lot �����ؾ� �ϰ�, Net �����ؾ� �ϰ�, date �����ؾ� �Ѵ�. date=All�� �ƴϾ�� �Ѵ�.
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
	// �ش� ��¥�� DataGrid ���
	
	// data�� ������Ʈ�ϱ� ���� �����. ������ �ϹǷ� UpdateDate(True)�� �ʿ� ����.
	ClearGrid_Data();	
	ClearGrid_Summary();	
	m_GridSnap.InitMember();


	// tupleSize��  m_gridData  row�� �����.
	// 2019.02.25: grid�� ���ʿ��ϰ� max�� row�� ��Ƽ� �޸𸮸� �����ϰ� ����ϴ� ���� �ذ�.
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();		
	if (m_nRows != (tupleSize + 1))
	{
		m_nRows = (tupleSize + 1);		// ������� �����ؼ� tupleSize + 1
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


	// nLot, nNet, NDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
	m_GridSnap.dataCount = DisplayGrid_4wData(nLot, nNet, nDate, 0);


	//---------------------------
	// Net Info Edit �ڽ��� ����
	//int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();
	m_editTupleNum  = tupleSize;

	m_editStepNum = g_sLotNetDate_Info.naLotStepCnt[nLot];


	// m_editLSL�� m_editUSL �κ���  DisplayGrid_Summary()���� ����.

	MyTrace(PRT_BASIC, "Lot=%d(%s), Net=%d, Date=%d(%s), tupleSize=%d : \n", 
			nLot, g_sLotNetDate_Info.strLot[nLot], nNet, 
			nDate, g_sLotNetDate_Info.strLotDate[nLot][nDate], tupleSize );


	//-------------------
	// Summary Grid ���
	

	// Calc Total, Count, Avg, Sigma, Min, Max	 ----------------
	int	   	nTotal=0, nCount=0;
	double 	dAvg=0, dSigma=0, dMax=0, dMin=0; 
	CalcSummary_AvgSigmaMinMax(nLot, nNet, nDate, 						// argument 
							nCount, nTotal, dAvg, dSigma, dMin, dMax);	// reference 

	// Summary Display   ------------------
	DisplayGrid_Summary(nLot, nNet, nDate, nCount, nTotal, dAvg, dSigma, dMin, dMax, 
							//nFaultCount);
							g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);


	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.
}




// ����  �Լ�
// Stat Dlg �ܿ� FR Dlg, YR Dlg������ �� �� �ֵ���  
// CStatisticsDialog Ŭ���� ����Լ��� �ƴ� �����Լ��� define
void	CalcSummary_AvgSigmaMinMax(int nLot, int nNet, int nDate,  		// argument
			int& rnCount, int& rnTotal, double& rdAvg, double& rdSigma, double& rdMin, double& rdMax )	// reference
{
	CString strTemp;

	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		strTemp.Format("CalcSummary_AvgSigmaMinMax(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, nDate );
		ERR.Set(SW_NO_INIT_VAR, strTemp); 
		ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
		return;
	}

	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, step, stepSize;
	double  dSum=0;
	int		nMinMaxInitStep = 0;	// Min, Max initial ��ġ
	for (tuple=0; tuple < tupleSize; tuple++)
	{
		nMinMaxInitStep = 0;

		stepSize =  g_sLotNetDate_Info.naLotStepCnt[nLot];
		for (step= 0; step < stepSize; step++)
		{
			rnTotal++;
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)		// NG�� �ƴ϶��
			{
				rnCount++;
				dSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

				// Min, Max �ʱ�ȭ
				if (tuple==0 && step==nMinMaxInitStep)
				{
					rdMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
					rdMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
				else
				{
					// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
					if (rdMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						rdMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

					// �ּҰ����� ���簪�� ������ �ּҰ� ����
					if (rdMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						rdMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}

			}
			else
			{
				// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
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

		dVariance = dDiffPowerSum / (double)(rnCount -1);	// �л�     : (val-avg)�� ������ ������ (n - 1)���� ������.
	}
	rdSigma    = sqrt(dVariance);						// ǥ������ : �л��� ������

	//MyTrace(PRT_BASIC, "rnTotal=%d, rnCount=%d, dSum=%.2f, rdAvg=%.2f, dVar=%.2f, rdSigma=%.2f, rdMin=%.2f, rdMax=%.2f, nFaultCount=%d\n", 
	//						rnTotal, rnCount, dSum, rdAvg, dVariance, rdSigma, rdMin, rdMax, 
	//						g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);
							//nFaultCount);


}



// nLot, nNet, NDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
// return : ���������� �����  tupleOffset �� �����Ѵ�.
int CStatisticsDialog::DisplayGrid_4wData(int nLot, int nNet, int nDate, int tuplePrtStart)
{
	CString strTemp;

	//-------------------
	// data Grid ���

	if (g_pvsaNetData[nLot][nNet][nDate] == NULL)
	{
		strTemp.Format("DisplayGrid_4wData(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, nDate );
		ERR.Set(SW_NO_INIT_VAR, strTemp); 
		ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
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
		int nMinMaxInitStep = 0;		// Min, Max�� �ʱ�ȭ ��ġ ǥ��

		int stepSize = g_sLotNetDate_Info.naLotStepCnt[nLot];
		for (step= 0; step < stepSize; step++)
		{
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step] != -1)	// NG�� �ƴ϶��
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

				// Min, Max �ʱ�ȭ
				if (step == nMinMaxInitStep)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
				else
				{
					// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];

					// �ּҰ����� ���簪�� ������ �ּҰ� ����
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daStep[step];
				}
			}
			else
			{
				// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
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

			dTupleVar   = dTupleDiffPowerSum / (double)(nTupleCount -1);// �л�     : (val-avg)�� ������ ������ (n - 1)���� ������.
		}
		dTupleSigma = sqrt(dTupleVar);								// ǥ������ : �л��� ������


		// 3. Display 4w Data Tuple  ------------------
		int ret = DisplayGrid_4wData_Tuple(nLot, nNet, nDate, tuple, (tuplePrtStart+prtTuple),// ALL�� ��� ������ ����� ��ġ ���ķ� ���.
							dTupleAvg, dTupleSigma, dTupleMin, dTupleMax);

		// m_bDataGridFaultOnly�� TRUE�϶� prt �ǳʶٰ� ������ ���� prtTuple�� ������Ű�� �ʴ´�.
		// �̷��� ���� ������ Fault Only�϶� ��� Fault�� ���� ���̿� �幮�幮 ���̰� ��. 
		if (ret != -1)
			prtTuple++;
	}

	//return (tuplePrtStart + tupleSize);		// ������� ����� tuple��ġ(Offset)�� �����Ѵ�.
	return (tuplePrtStart + prtTuple);		// ������� ����� tuple��ġ(Offset)�� �����Ѵ�.
}


// Data Grid�� 1�� tuple�� Display �Ѵ�.
// nPrtTuple: ALL �� ��� ������ ����� ��ġ ���ķ� tuple�� ����Ѵ�.
int CStatisticsDialog::DisplayGrid_4wData_Tuple( int nLot, int nNet, int nDate, int nTuple, int nPrtTuple,
						double dTupleAvg, double dTupleSigma, double dTupleMin, double dTupleMax)
{
	CString strTemp;
	int 	step;

	// 2018.05.31  Net tuple ���� Lsl, Usl�� �ڸ�Ʈó���ϰ� tuple�� Lsl, Usl�� ����ϱ�� ��.
	//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
	//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];

	// 2018.10.02 �߰� : ULSL Simulattion On�� ������ dLsl, dUsl�� Simul������ �����ش�.
	//           m_GridSnap���� ���⿡�� �ݿ��� �ǹǷ� csv ���Ͽ� ���ؼ��� ���� simul���� ó���� �ʿ� ����
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
															
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step] != -1)		// NG�� �ƴϸ�
		{
			double dStepVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step];

			if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
				nFaultCount++;
		}
	}

	// m_bDataGridFaultOnly�� TRUE �϶�  ���� step �߿� Fault�� �Ѱ��� ���ٸ� ������� �ʰ� ����.
	if (m_bDataGridFaultOnly == TRUE &&  nFaultCount == 0)
		return -1;


	// 2. Display  PinInfo, R, LSL, USL  ----------------------
	
	// row 0 ����� �����ϰ� data������ row 1 (nTuple + 1)���� ���.
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

	// LSL, USL�� Fault�� �����̹Ƿ� ��ȫ�� ������� ���� ��� ǥ���Ѵ�.
	m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0xFF, 0xC0, 0xCB));	// pink 
	m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0xFF, 0xC0, 0xCB));	// pink 

	// ���� Simulation LSL, USL�̸� ����� ������� ǥ���Ѵ�.
	if ( m_bApplyULSL && 				// if USL/LSL Simulation on
		(m_dSimulLsl != -1 && m_dSimulLsl != -1) ) 
	{
		//m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0xDD, 0xA0, 0xDD));	// plum (������) 
		//m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0xDD, 0xA0, 0xDD));	// plum (������) 

		m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_LSL, RGB(0x93, 0x70, 0xDB));	// medium purple (����)
		m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_USL, RGB(0x93, 0x70, 0xDB));	// medium purple (����)
	}


	// 4. Display Tuple 4W Data   ------------------------
	for (step= 0; step < stepSize; step++) // col 14~25:  Step1~ Step12
	{
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step] == -1)	// NG�̸�
		{
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+step, "NG");	
			m_GridSnap.saData[nPrtTuple].strData[step] = "NG";
		}

		else		// NG�� �ƴϸ�
		{
			double dStepVal = (*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daStep[step];

			strTemp.Format("%.2f",  dStepVal);
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+step, strTemp);	
			m_GridSnap.saData[nPrtTuple].strData[step] = strTemp;

			if (dStepVal < dLsl || dStepVal > dUsl)		// Fault
			{
				m_gridData.SetItemBkColour(nPrtTuple+1, DATA_COL_DATA1+step, 	// row, col
									//RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)���� �ణ ���� ����
									RGB(0xff, 0x63, 0x47));			// tomato : ���� ��Ȳ
			}
		}
	}

	return 0;
}

// Summary Grid�� ���� Display �Ѵ�.
void CStatisticsDialog::DisplayGrid_Summary(int nLot, int nNet, int nDate, int nCount,
								int nTotal, double dAvg, double dSigma, double dMin, double dMax, 
								int nFaultCount)
{
	CString strTemp;

	
	// double���� ���� �����Ǿ� ��µǹǷ� CString���� �����Ͽ� �Ʒ��� ���� ����Ѵ�.
	// => double���� ��� 8.6�� 8.59�� �����Ǵ� ������ �־���.
	


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
	m_gridSummary.SetItemText(1, SUM_COL_SIGMA, strTemp);		// col 6:  Sigma (ǥ������)
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

	// Fault�� 1���� ������ Summary�� FaultCount�� ����������, net Icon�� ���������� �����Ѵ�.
	if (nFaultCount > 0)	
	{
    	// 'Fault ��' ����� '��Ȳ��' ǥ���ϱ�	
    	m_gridSummary.SetItemBkColour(1, SUM_COL_FAULT, 
									//RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)���� �ణ ���� ����
									RGB(0xff, 0x63, 0x47));			// tomato : ���� ��Ȳ
    	
	}

	else // Fault�� ������ 
	{
    	// 'Fault ��' ����� ���󺹱�  -> �̰� �̹� ClearGrid_Summary�� ���������Ƿ� ���ص� ��.
    	//m_gridSummary.SetItemBkColour(1, SUM_COL_FAULT, RGB(0xFF, 0xFF, 0xE0));	// ������� 

	}

	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.
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
	// Lot/Net ���û��׵� �����Ѵ�.
	m_editStrSelect = _T("");
	
	// Grid�� �Բ� Grid Sub Edit �ڽ��� �ʱ�ȭ�Ѵ�.
	m_editTupleNum = 0;
	m_editStepNum = 0;


	// data�� ������Ʈ�ϱ� ���� �����. 
	ClearGrid_Summary();	

	ClearGrid_Data();	

	m_GridSnap.InitMember();
//__PrintMemSize(FUNC(ClearGrid), __LINE__);

	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.
	
}


void CStatisticsDialog::ClearGrid_Data()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridData.GetRowCount(); row++)		// ����� �����ϰ� Ŭ�����Ѵ�.
		for (int col = 0; col < m_gridData.GetColumnCount(); col++)
		{
			m_gridData.SetItemText(row, col, "                 ");


			// Ȥ�� ����� faultó���� cell�� �ִٸ� ������ display�� ���� ���󺹱� �Ѵ�.
			m_gridData.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// ������� 
		}

	//m_gridData.DeleteAllItems();  -> �̰͵� ������ �ӵ� �������� �ֽ� �ʱ�ȭ�� data �� ���ư�
	
	// Scroll Bar �ʱ�ȭ 
	m_gridData.SetScrollPos32(SB_VERT, 0);
	m_gridData.SetScrollPos32(SB_HORZ, 0);
	


//	m_gridData.AutoSize(); -> �̰� ������ �ӵ� ������.
}

void CStatisticsDialog::ClearGrid_Summary()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridSummary.GetRowCount(); row++)		// ����� �����ϰ� Ŭ�����Ѵ�.
		for (int col = 0; col < m_gridSummary.GetColumnCount(); col++)
		{
			m_gridSummary.SetItemText(row, col, "                 ");

			// Ȥ�� ����� faultó���� cell�� �ִٸ� ������ display�� ���� ���󺹱� �Ѵ�.
			m_gridSummary.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// ������� 
		}

	//m_gridSummary.DeleteAllItems();  -> �̰͵� ������ �ӵ� ������
	
	// Scroll Bar �ʱ�ȭ 
	m_gridSummary.SetScrollPos32(SB_VERT, 0);
	m_gridSummary.SetScrollPos32(SB_HORZ, 0);
//	m_gridSummary.AutoSize();	-> �̰� ������ �ӵ� ������
}


// Lot, Net �����ϰ�, date=All �̶�� SummaryGrid ���.
// 1. lot, net�� key�� �ؼ� ��� date, ��� time tuple�� sum�ؼ� summary grid ���.
// 2. data grid�� ���õ� date�� ��� data�� ����Ѵ�.
void CStatisticsDialog::Display_SumupLotNet(int nLot, int nNet)
{
	CString strTemp;

	// Lot �����ؾ� �ϰ�, Net �����ؾ� �ϰ�, data=All�̾�� �Ѵ�.
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
	// �ش� ��¥�� DataGrid ���
	
	int date;
	
	// data�� ������Ʈ�ϱ� ���� �����. ������ �ϹǷ� UpdateDate(True)�� �ʿ� ����.
	ClearGrid_Summary();	
	ClearGrid_Data();	
	m_GridSnap.InitMember();

	int rowSize = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		rowSize += g_pvsaNetData[nLot][nNet][date]->size();  			// date�� tupleSize 
	if (m_nRows != (rowSize + 1))
	{
		m_nRows = (rowSize + 1);		// ��� ���� rowSize +1�Ѵ�.

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


	// nLot, nNet, nDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
	int tupleOffset = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		tupleOffset= DisplayGrid_4wData(nLot, nNet, date, tupleOffset);
	m_GridSnap.dataCount = tupleOffset;




	//---------------------------
	// Net Info Edit �ڽ��� ����

	m_editTupleNum = rowSize;
	m_editStepNum = g_sLotNetDate_Info.naLotStepCnt[nLot];
	// m_editLSL�� m_editUSL �κ���  DisplayGrid_Summary()���� ����.




	//-------------------------
	// Summary Grid ���
	


	
	
	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, step, tupleSize, stepSize;
	int	   	nTotal=0, nCount=0;
//	int		nFaultCount=0; 
	double  dSum=0;
	double 	dAvg=0, dMax=0, dMin=0; 
	int		nMinMaxInitStepLoc = 0;	// Min, Max initial ��ġ
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
	{
		if (g_pvsaNetData[nLot][nNet][date] == NULL)
		{
			strTemp.Format("Display_SumupLotNet(): g_pvsaNetData[%d][%d][%d] is NULL! \n", nLot, nNet, date );
			ERR.Set(SW_NO_INIT_VAR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 		// �޽����ڽ� ����� ���ϱ�.
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
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step] != -1)	// NG�� �ƴ϶��
				{
					double dStepVal/*, dLsl, dUsl*/;
					dStepVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daStep[step];

					// Min, Max, Sum, Count ���ϱ� -----------
					nCount++;
					dSum += dStepVal; 
	
					// Min, Max �ʱ�ȭ
					if (date== 0 && tuple==0 && step==nMinMaxInitStepLoc)
					{
						dMax = dStepVal; 
						dMin = dStepVal; 
					}
					else
					{
						// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
						if (dMax <  dStepVal)
							dMax =  dStepVal; 
	
						// �ּҰ����� ���簪�� ������ �ּҰ� ����
						if (dMin >  dStepVal)
							dMin =  dStepVal;
					}


					// 2018.05.21 ���� üũ���� ���� �̹� üũ�� 
					//     g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]���� �̿��ϵ��� 
					//     �ϱ� �ڵ带 �ڸ�Ʈ ó����.
					// Fault Count ���ϱ� -----------------------
					//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
					//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];
					//if (dStepVal < dLsl || dStepVal > dUsl)
					//	nFaultCount++;
	
				}
				else
				{
					// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
					if (step == nMinMaxInitStepLoc)		
						nMinMaxInitStepLoc++;						
				}
			}
		}

		// 2018.07.09 ���⼭ �ƿ� �����ϰ� g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]�� �׳� ����ϱ���ϰ� �ڸ�Ʈó��
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

			tupleSize = g_pvsaNetData[nLot][nNet][date]->size();	// date���� tupleSize�� �ٸ�!
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
		dVariance = dDiffPowerSum / (double)(nCount -1);// �л�     : (val-avg)�� ������ ������ (n - 1)���� ������.
	}
	dSigma    = sqrt(dVariance);					// ǥ������ : �л��� ������

	MyTrace(PRT_BASIC, "nTotal=%d, nCount=%d, dSum=%.2f, dAvg=%.2f, dVar=%.2f, dSigma=%.2f, dMin=%.2f, dMax=%.2f, waNetFaultCount[%d][%d]=%d\n", 
						nTotal, nCount, dSum, dAvg, dVariance, dSigma, dMin, dMax, 
						nLot, nNet, g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]);


	// 3. Summary Display   ------------------
	DisplayGrid_Summary(nLot, nNet, (-1), nCount, nTotal, dAvg, dSigma, dMin, dMax, g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]);


	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�

	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.

}


// Lot ����, Net= -1(������) , Date���� �����Ѵٸ�
// 1. lot, date�� key�� �ϰ�,  ��� net, ��� time tuple�� sum �ؼ� summary grid  ��� 
void CStatisticsDialog::Display_SumupLotDate(int nLot, int nDate)
{
	CString strTemp;

	// Lot �����ؾ� �ϰ�, Date���� �����ϸ鼭 0�� �ƴϾ�� �Ѵ�.
	if ( (nLot < 0 || nLot >= MAX_LOT)
			|| (nDate < 0 || nDate >= MAX_DATE) ) 
	{
		strTemp.Format("Display_SumupLotDate(): nLot=%d(0<= Lot <%d), nDate=%d(0<= Date <%d) Range Over!\n", 
				nLot, MAX_LOT, nDate, MAX_DATE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// data�� ������Ʈ�ϱ� ���� �����. ������ �ϹǷ� UpdateDate(True)�� �ʿ� ����.
	m_editTupleNum = 0;
	m_editStepNum = 0;

	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.
	ClearGrid_Data();	
	m_GridSnap.InitMember();

	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.


	MyTrace(PRT_BASIC, _T("Display_SumupLotDate(): nLot=%d, nDate=%d\n"), nLot, nDate);


}

// Lot ����, Net= -1, Date=All
// 1. Lot�� key�� �ؼ� ��� net, ��� date, ��� time tuple�� sum�ؼ� summary grid ���
void CStatisticsDialog::Display_SumupLot(int nLot)
{
	CString strTemp;

	// Lot �����ؾ� �ϰ�, Date���� All�̾�� �Ѵ�.
	if (nLot < 0 || nLot >= MAX_LOT)
	{
		strTemp.Format("Display_SumupLot(): nLot=%d(0<= Lot <%d) Range Over\n", nLot, MAX_LOT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// data�� ������Ʈ�ϱ� ���� �����. ������ �ϹǷ� UpdateData(True)�� �ʿ� ����.
	m_editTupleNum = 0;
	m_editStepNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.
	m_GridSnap.InitMember();

	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.

		
	MyTrace(PRT_BASIC, _T("Display_SumupLot(): nLot=%d\n"), nLot);


}

// Lot ������, Net ������, Date = All
// 1. ��� lot,  ��� net, ��� date, ��� time tuple�� sum�ؼ� summary grid ���.
void CStatisticsDialog::Display_SumupTotal()
{

	// data�� ������Ʈ�ϱ� ���� �����. ������ �ϹǷ� UpdateDate(True)�� �ʿ� ����.
	m_editTupleNum = 0;
	m_editStepNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.
	m_GridSnap.InitMember();


	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.


	MyTrace(PRT_BASIC, _T("Display_SumupTotal()\n"));

}


void CStatisticsDialog::OnCheckDataFaultOnly() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);		// ȭ�� data�� �����´�.
	
	// �ٲ� m_bDataGridFaultOnly ������ Grid�� �ٽ� �׸���.
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
	int nDate 		= m_nCombo_CurrDate -1; //  m_nCombo_CurrDate�� ���� Date +1 ��

	CString strTemp;
	
	strTemp.Format(".\\Data\\%s_%s_Net%d_StatDataOut.csv", 
			g_sLotNetDate_Info.strLot[nLot], 
			(m_nCombo_CurrDate == DATE_ALL) ? "_" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
			nNet); 					
/*
 *  ���� �־ �־��µ� Ǯ� �׽�Ʈ�� ���� ���� ��
	if(!FileExists(strTemp)) // ������ �������� ������ ���� ���
	{ 	
		ERR.Set(FLAG_FILE_NOT_FOUND, strTemp); 
		ErrMsg();  ERR.Reset(); return; 
	}
*/


	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

	
}


// 1. ���ݱ��� insert �� data��  write. 
// 2. data�� delete�ϰ� file�� close  
void CStatisticsDialog::SaveStat_CsvFile(int nLot, int nNet, int nComboDate)
{
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;
	int 	nDate = nComboDate -1; //  m_nCombo_CurrDate�� ���� Date +1 ��


	MyTrace(PRT_BASIC, "SaveStat_CsvFile(nLot=%d, nNet=%d, nDate=%d)\n", nLot, nNet, nDate);

	// ������� ���� ���� ����Ʈ ���.
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
	//  Summary Grid Data ���
	
	// 4w Data�� Net, Data�� Avg, Sigma�� Ư�� Net, Date Ŭ���ÿ� �����ǹǷ� ���� ����Ǿ� ���� ����. 
	// ȭ�� ����� ���ؼ��� �Ʒ��� ���� ������ �����ص� snap data�� �̿��ؼ� ����Ѵ�. 
	// snap�� �ƴ϶�� ȭ������� ���ؼ� �ٽ� ����� �ؾ� ��.  

	// ��� ���
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
	//  Data Grid Data ���


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

// 1. ���ݱ��� insert �� data��  binary ������ open�Ͽ�  write. 
// 2. data�� delete�ϰ� file�� close  
void CStatisticsDialog::Save_StatLotDataFile()
{
	FILE 	*fp; 
	char  	fName[200]; 
	CString strTemp;


	MyTrace(PRT_BASIC, "Save_StatLotDataFile()\n");

	// ������� ���� ���� ����Ʈ ���.
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
	// p Chart�� ���� step data ��� 

	// Time �� p chart data ����ϱ�  (�����ִ� time�� ã��)
	//    1�� lot, date�� ���ؼ�(time ������ ���ƾ� �ϹǷ�)  ���� time�� ��� net�� fault������ �����.
	//    time�� fault ������ ��ճ��� USL, LSL�� �Ǵ��Ͽ� �Ѿ�� OOC ���� ã�Ƴ��� ����.
	
	// ���� lot, ���� date�̹Ƿ� ��� net�� tupleSize�� ���ƾ� �Ѵ�. �̸� �ϳ��� timeSize��� ����.
	int netCount = 0;
	static short	waTimeFaultCount[MAX_TIME_FILE];
	static short	waTimeNgCount[MAX_TIME_FILE];
	static short	waTimeTotalCount[MAX_TIME_FILE];
	::ZeroMemory(waTimeFaultCount, sizeof(waTimeFaultCount));
	::ZeroMemory(waTimeNgCount, sizeof(waTimeNgCount));
	::ZeroMemory(waTimeTotalCount, sizeof(waTimeTotalCount));

	int lot, time, tupleSize;
	for (lot =0; lot < 1; lot++)		// Lot�� 1���� ����
	{
		//for (int date =0; date < g_sLotNetDate_Info.naLotDateCnt[lot]; date++)
		for (int date =0; date < 1; date++)	// Date 1���� ����
		{

			for (int net =0; net < MAX_NET_PER_LOT; net++)
			{
				if(g_pvsaNetData[lot][net][date] == NULL)	// ���� date�� skip
					continue;

				if (g_sLotNetDate_Info.naLotNet[lot][net] == -1)	// ���� Net�̸� skip
					continue;

				netCount++;

				tupleSize = g_pvsaNetData[lot][net][date]->size();		// time ����

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

	for (lot =0; lot < 1; lot++)		// Lot�� 1���� ����
	{
		int stepSize = g_sLotNetDate_Info.naLotStepCnt[lot];
		for (time =0; time < tupleSize; time++)
		{
			CString strTime;
			strTime.Format("%02d:%02d:%02d", 
					(*g_pvsaNetData[lot][1][0])[time].statTime.hour,
					(*g_pvsaNetData[lot][1][0])[time].statTime.min,
					(*g_pvsaNetData[lot][1][0])[time].statTime.sec);

			waTimeTotalCount[time] = (netCount * stepSize);		// net ���� * step ����
			fprintf(fp, "Time(%s)=,%d, FaultCount=,%d, Count=,%d, Total=%d, NgCount=%d\n", 
							strTime, time, 
							waTimeFaultCount[time], (waTimeTotalCount[time] - waTimeNgCount[time]),
							waTimeTotalCount[time], waTimeNgCount[time]);
		}	
	}
#endif



#if 0   
	// Net �� p chart data ����ϱ�  (�����ִ� Net�� ã��)
	//for (int lot =0; lot < g_sLotNetDate_Info.nLotCnt; lot++)
	for (int lot =0; lot < 1; lot++)		// Lot�� 1���� ����

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
	// Net���� ����ϱ� 1
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


	// 2018.04.24 ��� Lot�� �޸𸮿� �ø���� �Ͽ����Ƿ� �ϱ� date�� Stat Dialog ���������� Clear�ϸ� �� �ȴ�.
	//            �ڵ� ������ �ڸ�Ʈ�� ����.
	//Stat_deleteAllNetData();


}


#if 0
// 1. binary ������ open�Ͽ�  Memory�� load
void CStatisticsDialog::Load_StatLotDataFile(int nLot)
{

}
#endif

// ������ SimulUsl�� Off ��Ų��.  
// Simulation ���߿� �ٸ� Lot, Net, Date�� �������� ���� ����Ѵ�.
void CStatisticsDialog::CheckOff_SimulUlsl() 
{
	// �̹� False��� �� Off �� �ʿ� ����.
	if (m_bSimulateULSL == FALSE)
		return;

	// ���� net�� �� Ŭ���� ����� Simulation Off �� �ʿ� ����.
	if (m_nPrevSimulLoT == m_nTree_CurrLot && 
		m_nPrevSimulNet == m_nTree_CurrNet && 
		m_nPrevSimulDate == m_nCombo_CurrDate)
		return;
	

	// Lot, Net, Date ���� �ϳ��� ����Ǿ���.
	CString strTemp;
	strTemp.Format("previous Lot=%d, Net=%d, Date=%d was in Simulation Process. Simulation Check Off now.\n", 
				m_nPrevSimulLoT, m_nPrevSimulNet, m_nPrevSimulDate);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);

	// Current Lot, Net, Date�� keep�ϰ� Current�� �Ͻ������� prevSimul�� �ٲ۴���
	// SimulUsl Off �۾��� �����Ѵ�.  
	int keepedLot = m_nTree_CurrLot;
	int keepedNet = m_nTree_CurrNet;
	int keepedDate = m_nCombo_CurrDate;
	HTREEITEM	keepedNode = m_hSelectedNode;

	m_nTree_CurrLot = m_nPrevSimulLoT;
	m_nTree_CurrNet = m_nPrevSimulNet;
	m_nCombo_CurrDate = m_nPrevSimulDate;
	m_hSelectedNode = m_hPrevSimulSelNode;


	m_bSimulateULSL = FALSE;		// Simul Check box ���¸� ������ off�Ѵ�.
	UpdateData(FALSE);				// UI���� �ݿ�
	OnCheckSimulUlsl();				// simulate ULSL Check box off ����� ó���Ѵ�.

	// Simul Off�� �Ϸ�Ǿ����Ƿ�  ������ Current Lot, Net, Date�� ���󺹱��Ѵ�.
	m_nTree_CurrLot = keepedLot;
	m_nTree_CurrNet = keepedNet;
	m_nCombo_CurrDate = keepedDate;
	m_hSelectedNode = keepedNode;


}

void CStatisticsDialog::OnCheckSimulUlsl() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	// ���õ� Net�� ������ ��ȿó���Ѵ�.  
	CString strTemp;
	if (  m_hSelectedNode == NULL			// ������ Tree Node�� �ִ��� üũ
			|| (m_nTree_CurrLot < 0 || m_nTree_CurrLot >= MAX_LOT)  			// Lot �����ϴ��� üũ
			|| (m_nTree_CurrNet < 0 || m_nTree_CurrNet >= MAX_NET_PER_LOT ) ) 	// Net �����ϴ��� üũ
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


	// Simulate ���� ��Ʈ�� ���� ���� 
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
		// Simul Off �� ��쿡�� Apply Off�� ������ action�� ������� �Ѵ�.


		// ���� Tree�� ���õ� Lot, Net�� ���� Fault üũ�� �ٽ� �Ѵ�.
		Tree_CheckNetFault(m_hSelectedNode, m_nTree_CurrLot);

		// LSL(simul), USL(simul)�� �ٲ� data grid ����� �ٽ� LSL, USL�� ���󺹱��Ѵ�.
		Display_DataGridHeader();		
	
		// ���󺹱���  Fault �������� �ٽ� Display�Ѵ�.
	 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

		// �ٽ� üũ�� Fault ������ FR Rankâ, pChart â�� �ݿ��Ѵ�.
		::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
		::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
	}


}



void CStatisticsDialog::OnCheckApplyUlsl() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	
	// Apply On ���¿����� �� �̻� USL/LSL edit�� �ȵǰ� disable�Ѵ�.
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

		if (dTempUSL < dTempLSL)		// ����  ó�� : USL�� LSL���� ���� �� ����.
		{
			CString strTemp;
			strTemp.Format("USL can not be made smaller than LSL.\n\n");
			ERR.Set(USER_ERR, strTemp); 
			ErrMsg();  ERR.Reset();

			// ������ �Էµ� USL, LSL str�� ��ĭ����   -> �̰� �׳� ������. User�� �߸��� �ν��ϵ���
			//m_editStrLSL = _T("");
			//m_editStrUSL = _T("");
			
			// Apply ��ư��  Off�� �ǵ����� ����.
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

	// Apply Off�� �Ǹ� edit �ڽ��� simul ���� ��� �ʱ�ȭ�Ѵ�.
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


		// Simul Off�ÿ� �����ϰ� ���� (OnCheckSimulUlsl(): m_bSimulateULSL�� FALSE �� ��)
		m_bSimulateULSL = FALSE;
		GetDlgItem(IDC_CHECK_APPLY_ULSL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_USL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LSL)->EnableWindow(FALSE);
	}
	UpdateData(FALSE);

	// ���� Tree�� ���õ� Lot, Net�� ���� Fault üũ�� �ٽ� �Ѵ�.
	Tree_CheckNetFault(m_hSelectedNode, m_nTree_CurrLot);

	// LSL(simul), USL(simul)�� data grid ����� �ٲ��ش�.
	Display_DataGridHeader();		

	// �ٽ� üũ�� Fault �������� �ٽ� Display�Ѵ�.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		

	// �ٽ� üũ�� Fault ������ FR Rankâ, pChart â�� �ݿ��Ѵ�.
	::PostMessage(m_hwnd_FrRankDlg,   UWM_LOAD_LOG4W_DATA, 0, 0);
	::PostMessage(m_hwnd_YrPChartDlg, UWM_LOAD_LOG4W_DATA, 0, 0);
}


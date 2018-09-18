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

	
	MyTrace(PRT_BASIC, "\"4W Statistics SW\" Started...\n\n" );

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
		//strTemp.Format("The 'Load 4W Data to Tree' button will launch automatically to load \"%s\" files.", g_sFile.ACE400_4WDataDir);
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



BOOL CStatisticsDialog::InitMember()
{

	//----------------------------
	// Tree Current data �ʱ�ȭ 
	m_nTree_CurrLot = -1;	
	m_nTree_CurrNet = -1;	
	m_nCombo_CurrDate = DATE_ALL;	
	m_bDataGridFaultOnly = FALSE;

	//----------------------------
	// 4WData Load ���� �ʱ�ȭ
	//m_nLoad_CurrLot = 0;

	//----------------------------
	// Grid ���� ��� ���� �ʱ�ȭ
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

	// Summary Grid header ����
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
	
	m_gridSummary.SetRowHeight(0, 40);	// ����� ���� ���� ǥ�ø� ���� ���� ��

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

	// Data Grid Header ����
	
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
	// tree �ʱ�ȭ 
	
	InitTree();

	// Tree �̹�������Ʈ ����
	m_ImageList.Create(IDB_BITMAP1, 16, 	// �̹����� ����, ���� ����
									2,		// �̹��� ���� �޸� ������ �÷��� �� �޸�ũ�� 
					RGB(255,255,255));		// ȭ�� ��� �÷� (����)
	m_treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	UpdateData(FALSE);

	return TRUE;
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

	ClearGrid(); // data Grid, summary Grid, edit�ڽ� �� UI�� �ʱ�ȭ�Ѵ�.


	Stat_deleteAllNetData();
	InitMember();
	InitTree();					// root���� Tree�� �ٽ� ����.	
	

	// ���� raw data ������ read�Ѵ�.
	Load_Log4wDir(g_sFile.ACE400_4WDataDir);

	
	// Load�� ��� Tree >Lot > Net�� ���� Fault üũ�� �����Ѵ�.
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


	// 2018.04.27 ��� Lot�� read �Ϸ� �Ǿ�����  ����  lot������ ���Ϸ� �����Ѵ�. 
	//Save_StatLotDataFile(m_nLoad_CurrLot);		// lot���� file�� �����ϴ� ����� ���� ������.
	
	// 2018.05.08 csv ���� ���� ��ư Ŭ���� �����ϵ��� �����ϰ� ���⼭�� ���´�.
	//Save_StatLotDataFile();		
}

// "D:Log4w" ���丮�� �����Ͽ� 4w Raw Data�� Load�Ѵ�.
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
			//               sample count�� �޶� ACE400_3333�� ó���� ���� �ʰ� �ǳʶٰ� �׷��� ������ ������ ����.
			//               �ǵ��� ���� �װ� ����� ����� ���� ���� ���Ƽ�  �ٸ� lot���� ó���ϱ�� ��.
			lotName = dirName.Mid(9, (length -9));
#endif

			// �� lot�� �߰��Ǵ� ���� ���� ���� Insert�� ���丮�� �� ���丮���� �˻��Ѵ�.  (�ڿ�����)
			if(CheckEmptyDirectory(dirPath) == TRUE)		
			{
				strTemp.Format("Load_Log4wDir(): %s is Empty Directory. Can't do InsertItem.\n\n", dirPath);
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

				nLotErrCnt++;
				if (nLotErrCnt < MAX_LOT_ERR_COUNT) 
					continue;	// ������ lot�� ���� lot�� �̸��� ���� ���� �����Ƿ� break���� �ʰ� continue ó��
				break;			// Lot Error�� 7ȸ �̻� �ݺ��Ǹ� �׳� break. 
			}

			dateId = g_sLotNetDate_Info.getLotDateId(lotId, dateName);
			if (dateId == -1)	// date range�� �Ѿ�ٸ� load���� �ʴ´�. 
			{
				strTemp.Format("Load_Log4wDir(): getLotDateId(%s/%s) Failed! naLotDateCnt[%d]=%d Range(<%d)!\n\n Can't do InsertItem", 
						lotName, dateName, lotId, g_sLotNetDate_Info.naLotDateCnt[lotId], MAX_DATE);
				ERR.Set(RANGE_OVER, strTemp); 
				ErrMsg();  ERR.Reset(); 

				nDateErrCnt++;
				if (nDateErrCnt < MAX_DATE_ERR_COUNT) 
					continue;	// ������ lot�� ���� �� �����Ƿ� break�� ���� �ʰ� continue ó��
				break;			// Date Error�� 3ȸ �̻� �ݺ��Ǹ� �׳� break
			}


			// Tree�� 'Lot' item�� �߰��Ѵ�.  
			// �������ϴ� item�̶�� hItem�� NULL�� �� ����.
			HTREEITEM  hItemInserted;
			hItemInserted = Tree_InsertLotItem(lotName, dirPath, lotId, dateId);			
			if (hItemInserted != NULL)		 
			{
				// �߰��� item
				MyTrace(PRT_BASIC, _T("%s: Inserted to Tree.\n"), (LPCTSTR)lotName);
			}

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

		// sample�� �ƴ� ���� 4W Data file�� �ϳ��� ������ FALSE ����
		return FALSE;
	}
	if (nFile == 0)
		return TRUE;

	return FALSE;
}


// CTreeCtrl�� �Է¹��� string�� Tree Item�� �߰��Ѵ�.
HTREEITEM CStatisticsDialog::Tree_InsertLotItem(LPCTSTR pStrInsert, LPCTSTR pStrPath, int nLot, int nDate)
{
	// TODO: Add your control notification handler code here
	HTREEITEM 	hItemFind= NULL, hLotItem = NULL;
	int			nLevel;
	CString		strTemp;

	HTREEITEM   hItemGet = NULL;	// return ��

	// Insert��  �ؽ�Ʈ�� ����ֳ� �˻��Ѵ�.
	if(pStrInsert == "")
	{
		strTemp.Format("Tree_InsertLotItem(): pStrInsert is NULL, Can't do InsertItem.");
		ERR.Set(SW_LOGIC_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return NULL;

	}



	// �߰��ؾ��� strInsert(Lot Name)�� ���� branch�� �����ϴ��� �˻��Ѵ�.
	// ���ο� Lot Name�̶�� ������ ������ Lot ��带 �߰��Ѵ�.
	//if ((hItemFind = Tree_FindStrItem(&m_treeCtrl, m_hRoot, pStrInsert)) == NULL)	// ���ο� Node(Lot) 
	
	// ���ο� Lot���� üũ
	hItemFind = Tree_FindLotItem(&m_treeCtrl, m_hRoot, pStrInsert);
	if (hItemFind != NULL) 				// Found: ������ �����ϴ� ���(Lot)
	{
		// ���� �̸��� ���(Lot)�� ������ �߰����� �ʵ��� return ���� NULL�� �����Ѵ�.
		hItemGet = NULL;		
		hLotItem = hItemFind;	// Found�̹Ƿ� ã�Ƴ� hItemFind�� hLotItem���� �Ҵ�.

		MyTrace(PRT_BASIC, "%s Node already exist! use existing one.\n\n", pStrInsert);

	}
	else	//if (hItemFind == NULL)	// Not Found:
	{
		// Tree InsertItem�� �״�� �̿��Ͽ� root�� ���ο� Lot Node�� �߰�
		hItemGet = m_treeCtrl.InsertItem(pStrInsert, m_hRoot); 
		m_treeCtrl.Expand(m_hRoot, TVE_EXPAND);
		hLotItem = hItemGet;	// ���� ������ hItemGet�� hLotItem���� �Ҵ�.

		// �� �������� Insert�� dir�� dirName, dirPath, hLotItem handle ���� �����ؾ� �� �� �ִ�.
		// ���� ��¥ �ʿ��ϴٸ� ��������. 

	}


#if 0
	// Lot��ȣ�� ������ Lot�� �ٸ��ٸ�, ���ݱ��� ������ lot������ ���Ϸ� �̵��Ͽ� �����ϰ�  �޸𸮸� clear�Ѵ�.
	// ���ο� Lot��ȣ�� ������ �̹� �����Ѵٸ� �޸𸮿� �ش� ������ Open�ؼ� �޸𸮿� ���ο� Lot�� �����͸� �ε��Ѵ�.
	if (nLot != m_nLoad_CurrLot)
	{
		Save_StatLotDataFile(m_nLoad_CurrLot);

		//Load_StatLotDataFile(nLot);	
		m_nLoad_CurrLot = nLot;		

	}

	// Lot ��ȣ�� ���� Lot�� ������, ���ο� ����� ��� üũ.
	else 
	{ 	
		// ù��° Lot�̶�� ������ nLot�� m_nLoad_CurrLot�� ���� �� ����.
		// �� ���� ���� ������� nLot�� ���� �ܰ迡�� ���� �޾� �Ծ�� ��.
		if (hItemFind == NULL && nLot != 0)
		{
			strTemp.Format("Tree_InsertLotItem(): pStrInsert=%s is New Node in Tree, But Not New LotId(%d). Check SW Logic!\n",
									pStrInsert, nLot);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); return NULL; 
		}
	}
#endif

	// �ű� Node, �������ϴ� Node ��� ���� ���丮�� Net Data�� Load�Ѵ�.
	nLevel = Tree_GetCurrentLevel(hLotItem);
	if (nLevel == 2)	// Lot ���丮��� ������ Net Data File�� Open�ؼ� Load
	{
		Tree_LoadLot_4WFiles(pStrPath, hLotItem, nLot, nDate);
		MyTrace(PRT_BASIC, "%s nLot=%d, nDate=%d: Tree_LoadLot_4WFiles... \n\n", pStrPath, nLot, nDate);

	}
	
	return hItemGet;
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
	CString		nodeName = "";


	hItemName = pTree->GetItemText(hItem);

	// �Է¹��� str�� hItem �ڽ��� str�̸� Found
	if ( strcmp(hItemName, pStr) == 0)	
	{
		hItemFind = hItem;
		MyTrace(PRT_BASIC, "Tree_FindLotItem2(): hItem=%s, pStr=%s Found!  return hItem! \n",  hItemName, pStr);
	}

	// �� ã������ 
	else 
	{
		// Lot ����������  ã�ƾ� �ϹǷ�, Child���� ������ �ʿ� ����.

		// ���� ������带 ã�´�. (������� recursive ȣ��)
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


// hItem���� ��� ��忡�� pStr�� ã�´�.
// Net�� ���� ������ �ʹ� ���Ƽ� �� �Լ��� ����ϸ� �ð��� �ʹ� ���� �ɸ��� ���� �߻�.
// Net�� ������ �Լ� g_sLotNetDate_Info.findLotNetId() �� ����.
//
// 2018.05.28 �� �Լ��� ������ ȣ��ÿ� stack over flow�� �״� ���� �־���.  
// ���ʿ��ϰ�  Net �������� Lot�� str�� �˻��ϴ� �κ��� �����ϱ� ���� 
// �� �Լ��� ������� �ʰ�  Tree_FindLotStrItem()�� ����ϱ�� �Ѵ�.
/*
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
*/


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

		// 2018.06.18 Tree_LoadLot_4WFiles()���� nNetDataCount >= MAX_NET_DATA ���� �������� ��� ,
		//            �� �̻� �ٸ� Time File�� ã�� �ʵ��� �Ʒ� �ڵ� �߰���.
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
// 1. 4W Data File�� Open�ϰ� Net�� Data�� read�Ͽ� �޸� ����ü�� Load
// 2. ���ο� Net ��ȣ���� Ȯ���ϰ� ���ο� Net ��ȣ���, hParentLot Tree�� Net Item�� Insert 
void CStatisticsDialog::Load_4wDataFile(CString dataFilePath, CString dataFileName, HTREEITEM hParentLot, int nLot, int nDate)
{

	FILE 	*fp; 
	char  	fName[200]; 		
	char	str[1024];			// fgets()�� ����° �о� ���� ���� buffer

	// 2018.05.25: file name�� 'Sample'�� ���ԵǾ� �ִٸ�.  
	// Auto sampling�� ���� �Ͻ������� ������ �����̹Ƿ� �м� data���� �����Ѵ�.  
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
	//  4W Raw Data ���� ��� Read 
	::ZeroMemory(str, sizeof(str));
	::ZeroMemory(&strSample, sizeof(strSample));
	fgets(str, sizeof(str), fp);		// ���� ��� read.  ���� ����� ����° ��°�� �д´�.

	// ������� data �� ������ �ľ��ϱ� ���� "S=" �� ������ ī��Ʈ�Ѵ�.
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
		if (g_sLotNetDate_Info.naLotSampleCnt[nLot] == 0)	// �ʱⰪ�̸� nFileSample ������  �ٲ۴�.
			g_sLotNetDate_Info.naLotSampleCnt[nLot] = nFileSample;			

		// ���� nLot �ȿ����� nFileSample�� ���� ��� ���ƾ� �Ѵ�.
		else
		{
			strTemp.Format("Load_4wDataFile(): nFileSample=%d is not same with g_sLotNetDate_Info.naLotSampleCnt[%d]=%d!", 
					nFileSample, nLot, g_sLotNetDate_Info.naLotSampleCnt[nLot]);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg(-1, TRUE);  ERR.Reset(); 
			return;
		}

	}


	statNetData netData;		// �ѹ� �����ؼ� �ִ� 5000������ ��Ȱ��.
								// Stat_insertNetData()���� ���� data�� copy�� �Ͼ��.

	// data file name���� time ȹ��.
	strTime = dataFileName.Mid(15, 6);
	netData.statTime.hour = atoi(strTime.Left(2));		// �Ǿ��� 2����
	netData.statTime.min  = atoi(strTime.Mid(2, 2));	// 3��°���� 2���� 
	netData.statTime.sec  = atoi(strTime.Mid(4, 2));	// 5��°���� 2����. 



#ifdef DP_SAMPLE
	netData.sampleSize = nFileSample;
	netData.pdaSample  = new double[nFileSample];	// nFileSample���� double �޸𸮸� Ȯ���Ѵ�.
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


		// loop�� �� �� ���� �Ʒ� ���۸� 
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
		int	   nSample = 0;
		for (i = 0; i < nFileSample; i++)
		{
			// NG �� üũ
			if (strcmp("NG", strSample[nSample]) == 0)
			{
				nSample++;			// NG�� skip �Ѵ�.

				// NG 999999.00 �� ��쿡�� ��¥ NG ó��
				if (strcmp("999999.00,", strSample[nSample]) == 0)
				{

				#ifdef DP_SAMPLE
					netData.pdaSample[i] = (double)(-1);
				#else
					netData.daSample[i] = (double)(-1);
				#endif
					nSample++;		// 999999.00 �� -1�� ó��
				}

				// NG 64.28 �� ����  ���ڰ� �ڿ� ���� ���
				// NG�� ���� ���� ���� ���� ǥ�� �̹Ƿ� ���ڸ�  ����ó���Ѵ�.
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

			// NG ���� 999999.00 �� �ִ� ���  NG ó��
			else if (strcmp("999999.00,", strSample[nSample]) == 0)
			{
			#ifdef DP_SAMPLE
				netData.pdaSample[i] = (double)(-1);		
			#else
				netData.daSample[i] = (double)(-1);		
			#endif
				nSample++;			// 999999.00 �� -1�� ó��
			}

			else
			{
				// daSample �� 
			#ifdef DP_SAMPLE
				netData.pdaSample[i] = (double)atof(strSample[nSample]);
			#else
				netData.daSample[i] = (double)atof(strSample[nSample]);
			#endif
				nSample++;

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
			m_treeCtrl.Expand(hParentLot, TVE_EXPAND);
			g_sLotNetDate_Info.setLotNetId(nLot, nNet);

		}
		strInsert.Empty();

#ifdef DP_SAMPLE
		if (m_nNetDataCount < MAX_NET_DATA)
		{
			// Read�� netData�� �޸𸮿� �߰��Ѵ�.
			// ���ʿ��� ����������� new, �Ҹ�����  delete ��  ���� ���� Stat_inserNetData() ȣ�� ��� ���� �ڵ� ����.
			int ret = Stat_insertNetVector(nLot, nNet, nDate);
			if (ret >= 0)
			{
				// 2K�� �Ѵ´ٸ� vector ������ �� Ȯ���Ѵ�.  
				if ( g_pvsaNetData[nLot][nNet][nDate]->size() == 2048)		
					g_pvsaNetData[nLot][nNet][nDate]->reserve(MAX_TIME_FILE);

				// �ش� vector�� ���� netData�� insert�Ѵ�.
				//   :  time ������ ���� vector�� �̹� time�� netData�� push_back �Ѵ�.
				g_pvsaNetData[nLot][nNet][nDate]->push_back(netData);
			}

			// 2018.06.18: m_nNetDataCount��  MAX_NET_DATA ������ �Ѿ�� 4W Data �ε��� �����ϴ� ����߰�
			m_nNetDataCount++;
			if (m_nNetDataCount == MAX_NET_DATA)
				MyTrace(PRT_BASIC, "m_nNetDataCount=%d, Stat_insertNetData() Stopped!! \n", m_nNetDataCount);
		}

#else
		// Read�� netData�� �޸𸮿� �߰��Ѵ�.
		Stat_insertNetData(nLot, nNet, nDate, netData);		
#endif


		row++;

	}
	fclose(fp);

	//TRACE ("g_pvsaNetData[nLot=%d][nNet=%d][nDate=%d]->size() = %d\n", 
	//					nLot, nNet, nDate, g_pvsaNetData[nLot][nNet][nDate]->size());

#ifdef DP_SAMPLE
	// ���⼭ ����� netData�� �޸𸮴� ����� ������. ????
	//delete []netData.pdaSample;  => �� �ʿ� ����.   
	//�� �Լ��� �����鼭 netData�� �Ҹ��ڰ� �ڵ�ȣ��ǰ� �ű⼭ delete[]��
#endif


}



// Stat Data�� �޸𸮿� �߰� �Ѵ�. 
void CStatisticsDialog::Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData)
{

	if (m_nNetDataCount >= MAX_NET_DATA)
		return;

	int ret = Stat_insertNetVector(nLot, nNet, nDate);
	if (ret < 0)
		return;

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

//#ifdef DP_SAMPLE
#if 0		// NetData �Ҹ��ڿ��� ó���ϱ�� �ϰ� �� �κ��� ����.
	int tupleSize;
	for (lot =0; lot < MAX_LOT; lot++)
	{
		for (net =0; net < MAX_NET_PER_LOT; net++)
		{
			for (date =0; date < MAX_DATE; date++)
			{
				if(g_pvsaNetData[lot][net][date] != NULL)
				{
					// ���� �� tuple�� pdaSample�� delete[]�Ѵ�.   
					// new[]�� �Ҵ������� delete[]�ؾ� ��.
					tupleSize = g_pvsaNetData[lot][net][date]->size();	// date���� tupleSize�� �ٸ�!
					for (int tuple=0; tuple < tupleSize; tuple++)
					{
						//  ���뿡�� �״´�.... ����.
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
					//int prevTupleSize = g_pvsaNetData[lot][net][date]->size();	// date���� tupleSize�� �ٸ�!

					// vector�� ��� data������ (��� tuple�� �����ϰ�)
					if (g_pvsaNetData[lot][net][date]->empty() == false)
						g_pvsaNetData[lot][net][date]->clear();		

				#ifdef DP_SAMPLE
					//int tupleSize = g_pvsaNetData[lot][net][date]->size();	// date���� tupleSize�� �ٸ�!
					//MyTrace(PRT_BASIC, "Stat_deleteAllNetData(): lot=%d, net=%d, date=%d, tupleSize=(%d => %d)\n", 
					//		lot, net, date, prevTupleSize, tupleSize);
				#endif

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

	// �ڽ� Lot ��带  ã�´�.
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
		// �ڽ��� ���� Lot ��带 ã�´�.
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

	// �ڽ� ��带 ã�´�.
	hItemChild = m_treeCtrl.GetChildItem(hItem);
	if ( hItemChild )
	{
		Tree_CheckNetFault(hItemChild, nLot);		// Date = ALL�� üũ
		//netName = m_treeCtrl.GetItemText(hItemChild);
		//MyTrace(PRT_BASIC, "Tree_CheckNetFault(): nLot=%d, %s\n", nLot, netName);
	}

	while (hItemChild)
	{
		// �ڽ��� ������带 ã�´�.
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
	CString	itemName; 
	int 	nNet;
	
	
	itemName = m_treeCtrl.GetItemText(hNetItem);
	nNet = atoi(itemName.Mid(4));


	//-----------------------
	// Fault ó�� ����
	
	int 	tupleSize, tuple, sample, sampleSize;
	double 	dLsl, dUsl;

	// 2018.05.31  Net ���� ���� Lsl, Usl�� ���� tuple�� Lsl, Usl�� ����ϱ�� ��.
	//dLsl   = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];	
	//dUsl   = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];	
	
	
	// USL, LSL���� time tuple���� �ٸ� ������ ó���Ϸ��� Display �ܰ迡���� fault üũ. 
	// LSL, USL ���� ��� time tuple�� �������� ���� ��������� üũ�Ϸ��� ���⿡�� �̸� üũ�Ѵ�.  
	// ��� �ΰ��� ����� ���� fault�� ���� ����� �ٸ� �� �ִ�.
	
	//int		nNetFaultCount = 0;
	g_sLotNetDate_Info.waNetFaultCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetNgCount[nLot][nNet] = 0;
	g_sLotNetDate_Info.waNetTotalCount[nLot][nNet] = 0;
	for (int date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)	
	{
		tupleSize = g_pvsaNetData[nLot][nNet][date]->size();
		for (tuple=0; tuple < tupleSize; tuple++)
		{
			// 2018.05.31  tuple�� Lsl, Usl ���
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
	
	CString strTemp;
	

	//-----------------------------
	// File Open Dialog ����
	
	char szFilter[] = "CSV Files(*.CSV)|*.CSV|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, 		// bOpenFileDialg = TRUE: Ÿ��Ʋ�ٿ� '����' ���
					NULL, 		// lpszDefExt           : �⺻ Ȯ����
					NULL, 		// lpszFileName         : "���� �̸�" Edit ��Ʈ�ѿ� ó�� ��µ� ���ϸ�.
					OFN_HIDEREADONLY |OFN_NOCHANGEDIR |OFN_ALLOWMULTISELECT, 
								// dwFlags :  �Ӽ�   (HIDEREADONLY : read only ������ ������� ����.
					szFilter);	// ��ȭ���ڿ� ��µ� ������ Ȯ���ڷ� �Ÿ��� ���� ����. 

	dlg.m_ofn.lpstrInitialDir = _T("d:\\log4w"); // �����Ҷ� �ʱ� ��� ����. �Ǿտ� �ҹ��� d: ���� ����� ������..
	
	// ���⼭ ���� ũ�� �÷���� ��.  �� �÷� �ָ� 6���̻� file ������ �� ��.
	// default 256byte => file 6�� ó�� ����.  �� fileName�� �� 43���� ������� ����.
	// fileName ����� �˳��ϰ� 60���ڷ� �ְ� 3000���� ó���ؾ� �ϹǷ�  60*3000 = 180000 
	// 2018.07.17 buffer�� ����� 180000 ���� �ſ� ũ�Ƿ� stack overflow�� ������ �� �־� static���� ������.
	static char buffer[60 * MAX_TIME_FILE] = {0}; 	// ����
	dlg.m_ofn.lpstrFile = buffer; 				// ���� ������
	dlg.m_ofn.nMaxFile = (60 * MAX_TIME_FILE); 	// ���� ũ��,  file 3000�� ó���Ϸ��� ������ buffer�ʿ���.


	if (IDOK != dlg.DoModal())
		return;



	//-----------------------------
	// Lot Name�� ���� ó��
	

	// ���õ� CSV���ϵ��� ������(dirPath)�� ȹ���Ѵ�. --------------

	CString dataFilePath, dirPath;
	POSITION pos= dlg.GetStartPosition();
	dataFilePath = dlg.GetNextPathName(pos);
	
	// 1. ReverseFind()�� �ڿ������� '\' ��ġ�� ã�´�.  
	int nSlashLoc = dataFilePath.ReverseFind('\\');	 

	// 2.  �޺κ�(fileName)��  ����� ������(dirPath)�� ���´�.
	//   ex) "D:\log4w\20180503_036782A2S\Ng_Log4WSample0_20180503_010135_NG_Net_-2558.CSV"
	//        nSlashLoc�� 27�� ���ϵ�. 27���� Left()�� �̿��ؼ� ������ �����ϸ� ����Path (dirPath).
	//        dirPath�� "D:\log4w\20180503_036782A2S"   . ��������� ������ fileName�� ������.
	dirPath = dataFilePath.Left(nSlashLoc); 


	// d:\log4w ���丮 �Ʒ��� ���丮���� Ȯ���Ѵ�. ---------------
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
	lotName = lotName.Mid(9, (length -9));		// ���� "D:\log4w\"�� �ش��ϴ� ù��° level�� �����.
							// 2018.06.28: �߶󳻴� ��ġ�� 9�� �����ϸ� D:\log4w�̿ܿ� �ٸ� ��츦 Ŀ������ ����.
							// 			   ����� �Ǽ� �̹Ƿ� �������� �����Ѵ�. d:\log4w_2 �� �����Ѵٸ� ó���� ����������
							// 			   �Ǵµ� date name�� �̻��ϰ� ��µȴ�.



	// "20170925" ó�� ��¥�� �������� �ʴ´ٸ� ����.  (Lot ���丮�� �ƴ�) -------------
	CString dateName  = lotName.Left(8);		// ��¥ str ����.
	CString dateExclu = dateName.SpanExcluding("0123456789"); 
	if (!dateExclu.IsEmpty())	// "0123456789"�� �ش����� �ʴ� ���ڸ� ����. Empty�� �ƴϸ� ���������̶� ���
	{
		strTemp.Format("OnButtonLoad4wData_SingleLot(): \"%s\" \n '4w Data' Directory has not proper DateName. Can't do InsertItem.\n\n", dirPath);
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 		
		return;
	}

	// Check Empty Directory     -------------------------
	// �� lot�� �߰��Ǵ� ���� ���� ���� Insert�� ���丮�� �� ���丮���� �˻��Ѵ�.  (�ڿ�����)
	//  : ���� ������ ������ ���̹Ƿ� ������ file�� ���°� �ƴ϶� 
	//    'sample_xxxx' ���ϸ� �������� ��츦 ����ؼ� �ʿ��ϴ�. 
	CString fileName; 
	BOOL	bFileFound = FALSE;
	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath �κ��� ����� fileName�� �����. (dirPath+'\' �����ŭ �պκ��� �ڸ�)
		int fileNameLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (fileNameLength - (nSlashLoc +1)));		 
		if (fileName.Find("Sample") != -1)
		{
			MyTrace(PRT_BASIC, "Skip %s\n", dataFilePath );
			continue;
		}

		// Sample�� �ƴ� ���� 4W data file�� �ϳ��� ������ break; 
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


	// "20170925_A30-19-C-4W-NEW-0920" ��� �Ǿ��� ��¥�� ���� tree item���� �����Ѵ�. 
	lotName = lotName.Mid(9, (length -9));		// "20170925_" ��¥ �κ��� �����.


	// �������.. Lot Name, Date Name Ȯ��.
	MyTrace(PRT_BASIC, "Load 4wData(Single): Lot=%s, Date=%s \n\n\n", lotName, dateName);
	


	//----------------------------------------------
	// UI (Tree, Date, Grid )  �� �޸� �ʱ�ȭ 
	
	ClearGrid(); // data Grid, summary Grid, edit�ڽ� �� UI�� �ʱ�ȭ�Ѵ�.

	Stat_deleteAllNetData();	// ���� netData�� ��� �޸𸮿��� �����.
	InitMember();				// CStatisticsDialog ��� ������ ��� �ʱ�ȭ �Ѵ�.
	InitTree();					// root���� Tree�� �ٽ� ����.	



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
	
	for (pos= dlg.GetStartPosition(); pos != NULL; )
	{
		dataFilePath = dlg.GetNextPathName(pos);		// pos++ ,  GetPathName()

		// dirPath �κ��� ����� fileName�� �����. (dirPath+'\' �����ŭ �պκ��� �ڸ�)
		int fileNameLength = strlen(dataFilePath);
		fileName = dataFilePath.Mid((nSlashLoc +1), (fileNameLength - (nSlashLoc +1)));		 

		//MyTrace(PRT_BASIC, "Load 4wData(Single): Path=%s, file=%s \n", dataFilePath, fileName );	 // test print

		// �ش� CSV file�� load�Ѵ�. 
		Load_4wDataFile(dataFilePath, fileName, hLotItem, lotId, dateId);
	}


	//------------------------------------------
	// �ش� Lot�� ��� Net�� Fault Check
	Tree_Check_LotNetFaults(hLotItem, lotId);
	

	strTemp.Format("\'Load 4w Data\' for \"%s\" Completed.\n\n To View 4W Data, follow these steps, please.\n 1. Select any Net in the tree.\n 2. Select the Date.", dirPath);
	AfxMessageBox(strTemp, MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, "OnButtonLoad4wData_SingleLot() for\"%s\" Completed. \n", dirPath);
	

}



// Tree Control�� ���� item�� ����� �� ȣ��
void CStatisticsDialog::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM	hParent;
	CString		strTemp;
	CString		parentName = ""; 
	CString		selectedName = "";

	
	// ���� ������ �������� �ڵ� ���� ��������� �����Ѵ�.
	m_hSelectedNode = pNMTreeView->itemNew.hItem;
	hParent 		= m_treeCtrl.GetParentItem(m_hSelectedNode);

	// test
	//m_treeCtrl.SetItemImage(m_hSelectedNode, 2, 3);		// item�� ������ ��� ������ ����  �׽�Ʈ

	// ������ �������� �̸��� ��ȭ������ ����Ʈ ���ڿ� �����Ѵ�.  
	if (hParent != NULL)
		parentName   = m_treeCtrl.GetItemText(hParent);
	selectedName = m_treeCtrl.GetItemText(m_hSelectedNode);

	int nLevel = Tree_GetCurrentLevel(m_hSelectedNode);

	if (nLevel == 3)			// ������ �������� Net��� 'lot# - net#'�� ����
		m_strSelect = parentName + " / " + selectedName;
	else
		m_strSelect = selectedName;


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

	// ���õ� Net�̳�, Lot�� Summary�� summaryGrid�� ����� �ְ�,  
	// Net data�� date���� ���ϰ� ������ �Ǿ��ٸ� dataGrid�� ����� �ش�.
	// 2018.05 Display ��ư�� ������ �� ����� �ִ� ������ ������.
	// 2018.06.15 �̻��� ���� ��û���� Date�� Tree Ŭ�������� ��ȸ�� �����ϰ� �ٽ� ������.
 	DisplayGrid(m_nTree_CurrLot, m_nTree_CurrNet, m_nCombo_CurrDate );		



	// ��ȭ������ Edit��Ʈ�ѿ� m_strSelect �� ����Ѵ�.
	UpdateData(FALSE);
	
	*pResult = 0;
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
		strTemp.Format("DisplayGrid(): Execute \"Load 4W Data to Tree\" button, First.\n And select any Tree item and Date item please.");
		ERR.Set(USER_ERR, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}

	// Ŭ���� item�� ���ٸ�  �ƹ� item�̳� Ŭ���϶�� �޽��� ���.
	if (m_hSelectedNode == NULL)
	{
		strTemp.Format("DisplayGrid(): Select any Tree item and Date item please.");
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



	// nLot, nNet, NDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
	m_GridSnap.dataCount = DisplayGrid_4wData(nLot, nNet, nDate, 0);


	//---------------------------
	// Net Info Edit �ڽ��� ����
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();
	m_editTupleNum  = tupleSize;

	m_editSampleNum = g_sLotNetDate_Info.naLotSampleCnt[nLot];


	// m_editLSL�� m_editUSL �κ���  DisplayGrid_Summary()���� ����.

	MyTrace(PRT_BASIC, "Lot=%d(%s), Net=%d, Date=%d(%s), tupleSize=%d : \n", 
			nLot, g_sLotNetDate_Info.strLot[nLot], nNet, 
			nDate, g_sLotNetDate_Info.strLotDate[nLot][nDate], tupleSize );


	//-------------------
	// Summary Grid ���

	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.
	

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
	int tupleSize = g_pvsaNetData[nLot][nNet][nDate]->size();

	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, sample, sampleSize;
	double  dSum=0;
	int		nMinMaxInitSample = 0;	// Min, Max initial ��ġ
	for (tuple=0; tuple < tupleSize; tuple++)
	{
		nMinMaxInitSample = 0;

		sampleSize =  g_sLotNetDate_Info.naLotSampleCnt[nLot];
		for (sample= 0; sample < sampleSize; sample++)
		{
			rnTotal++;
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)		// NG�� �ƴ϶��
			{
				rnCount++;
				dSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

				// Min, Max �ʱ�ȭ
				if (tuple==0 && sample==nMinMaxInitSample)
				{
					rdMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
					rdMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
				else
				{
					// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
					if (rdMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						rdMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

					// �ּҰ����� ���簪�� ������ �ּҰ� ����
					if (rdMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						rdMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}

			}
			else
			{
				// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
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
		dVariance = dDiffPowerSum / (double)rnCount;		// �л�     : (val-avg)�� ������ ������ n���� ������.
		rdSigma    = sqrt(dVariance);					// ǥ������ : �л��� ������
	}

	MyTrace(PRT_BASIC, "rnTotal=%d, rnCount=%d, dSum=%.2f, rdAvg=%.2f, dVar=%.2f, rdSigma=%.2f, rdMin=%.2f, rdMax=%.2f, nFaultCount=%d\n", 
							rnTotal, rnCount, dSum, rdAvg, dVariance, rdSigma, rdMin, rdMax, 
							g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][nDate]);
							//nFaultCount);


}



// nLot, nNet, NDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
// return : ���������� �����  tupleOffset �� �����Ѵ�.
int CStatisticsDialog::DisplayGrid_4wData(int nLot, int nNet, int nDate, int tuplePrtStart)
{

	//-------------------
	// data Grid ���


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
		int nMinMaxInitSample = 0;		// Min, Max�� �ʱ�ȭ ��ġ ǥ��

		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[nLot];
		for (sample= 0; sample < sampleSize; sample++)
		{
		#ifdef DP_SAMPLE
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample] != -1)	// NG�� �ƴ϶��
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];

				// Min, Max �ʱ�ȭ
				if (sample == nMinMaxInitSample)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
				}
				else
				{
					// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];

					// �ּҰ����� ���簪�� ������ �ּҰ� ����
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].pdaSample[sample];
				}
			}

		#else
			if ((*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample] != -1)	// NG�� �ƴ϶��
			{
				nTupleCount++;
				dTupleSum += (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

				// Min, Max �ʱ�ȭ
				if (sample == nMinMaxInitSample)
				{
					dTupleMax = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
					dTupleMin = (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
				else
				{
					// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
					if (dTupleMax <  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						dTupleMax =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];

					// �ּҰ����� ���簪�� ������ �ּҰ� ����
					if (dTupleMin >  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample])
						dTupleMin =  (*g_pvsaNetData[nLot][nNet][nDate])[tuple].daSample[sample];
				}
			}
		#endif
			else
			{
				// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
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

			dTupleVar   = dTupleDiffPowerSum / (double)nTupleCount;	// �л�     : (val-avg)�� ������ ������ n���� ������.
			dTupleSigma = sqrt(dTupleVar);							// ǥ������ : �л��� ������

		}


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
	int 	sample;

	// 2018.05.31  Net tuple ���� Lsl, Usl�� �ڸ�Ʈó���ϰ� tuple�� Lsl, Usl�� ����ϱ�� ��.
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
															
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample] != -1)		// NG�� �ƴϸ�
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

	// m_bDataGridFaultOnly�� TRUE �϶�  ���� sample �߿� Fault�� �Ѱ��� ���ٸ� ������� �ʰ� ����.
	if (m_bDataGridFaultOnly == TRUE &&  nFaultCount == 0)
		return -1;


	// 2. Display  PinInfo, R, LSL, USL  ----------------------
	
	// row 0 ����� �����ϰ� data������ row 1 (nTuple + 1)���� ���.
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
		if ((*g_pvsaNetData[nLot][nNet][nDate])[nTuple].daSample[sample] == -1)	// NG�̸�
	#endif
		{
			m_gridData.SetItemText(nPrtTuple+1, DATA_COL_DATA1+sample, "NG");	
			m_GridSnap.saData[nPrtTuple].strData[sample] = "NG";
		}

		else		// NG�� �ƴϸ�
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

	//----------------------------
	// Net Info Edit �ڽ��� ����
	
	
	// double���� ���� �����Ǿ� ��µǹǷ� CString���� �����Ͽ� �Ʒ��� ���� ����Ѵ�.
	// => double���� ��� 8.6�� 8.59�� �����Ǵ� ������ �־���.
	
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


void CStatisticsDialog::OnButtonClearGrid() 
{
	// TODO: Add your control notification handler code here
	MyTrace(PRT_BASIC, "OnButtonClearGrid()...\n" );
	
	ClearGrid();

}


void CStatisticsDialog::ClearGrid() 
{
	// Lot/Net ���û��׵� �����Ѵ�.
	m_strSelect = _T("");
	
	// Grid�� �Բ� Grid Sub Edit �ڽ��� �ʱ�ȭ�Ѵ�.
	m_editTupleNum = 0;
	m_editSampleNum = 0;
	//m_editStrLSL = _T("");
	//m_editStrUSL = _T("");

	// data�� ������Ʈ�ϱ� ���� �����. 
	ClearGrid_Summary();	
	ClearGrid_Data();	

	m_GridSnap.InitMember();

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

	//m_gridData.DeleteAllItems();
	
	// Scroll Bar �ʱ�ȭ 
	m_gridData.SetScrollPos32(SB_VERT, 0);
	m_gridData.SetScrollPos32(SB_HORZ, 0);

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

	//m_gridSummary.DeleteAllItems();
	
	// Scroll Bar �ʱ�ȭ 
	m_gridSummary.SetScrollPos32(SB_VERT, 0);
	m_gridSummary.SetScrollPos32(SB_HORZ, 0);

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
	ClearGrid_Data();	


	// nLot, nNet, nDate�� �´� ��� data Grid Tuple�� ����Ѵ�.
	int tupleOffset = 0;
	for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		tupleOffset= DisplayGrid_4wData(nLot, nNet, date, tupleOffset);
	m_GridSnap.dataCount = tupleOffset;




	//---------------------------
	// Net Info Edit �ڽ��� ����

	m_editTupleNum = 0;
	m_editSampleNum = g_sLotNetDate_Info.naLotSampleCnt[nLot];
	// m_editLSL�� m_editUSL �κ���  DisplayGrid_Summary()���� ����.





	//-------------------------
	// Summary Grid ���
	

	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.

	
	
	// 1. Calc Count, Avg, Min, Max	 ----------------
	int		tuple, sample, tupleSize, sampleSize;
	int	   	nTotal=0, nCount=0;
//	int		nFaultCount=0; 
	double  dSum=0;
	double 	dAvg=0, dMax=0, dMin=0; 
	int		nMinMaxInitSample = 0;	// Min, Max initial ��ġ
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
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample] != -1)	// NG�� �ƴ϶��
			#else
				if ((*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample] != -1)	// NG�� �ƴ϶��
			#endif
				{
					double dSampleVal/*, dLsl, dUsl*/;
				#ifdef DP_SAMPLE
					dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].pdaSample[sample];
				#else
					dSampleVal = (*g_pvsaNetData[nLot][nNet][date])[tuple].daSample[sample];
				#endif

					// Min, Max, Sum, Count ���ϱ� -----------
					nCount++;
					dSum += dSampleVal; 
	
					// Min, Max �ʱ�ȭ
					if (date== 0 && tuple==0 && sample==nMinMaxInitSample)
					{
						dMax = dSampleVal; 
						dMin = dSampleVal; 
					}
					else
					{
						// �ִ밪���� ���簪�� ũ�� �ִ밪 ����
						if (dMax <  dSampleVal)
							dMax =  dSampleVal; 
	
						// �ּҰ����� ���簪�� ������ �ּҰ� ����
						if (dMin >  dSampleVal)
							dMin =  dSampleVal;
					}


					// 2018.05.21 ���� üũ���� ���� �̹� üũ�� 
					//     g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date]���� �̿��ϵ��� 
					//     �ϱ� �ڵ带 �ڸ�Ʈ ó����.
					// Fault Count ���ϱ� -----------------------
					//dLsl = g_sLotNetDate_Info.daLotNetLsl[nLot][nNet];
					//dUsl = g_sLotNetDate_Info.daLotNetUsl[nLot][nNet];
					//if (dSampleVal < dLsl || dSampleVal > dUsl)
					//	nFaultCount++;
	
				}
				else
				{
					// Min Max �ʱ�ȭ ��ġ�� ���� NG��� �ʱ�ȭ ��ġ�� �ϳ� ���� ��Ų��. 
					if (sample == nMinMaxInitSample)		
						nMinMaxInitSample++;						
				}
			}
		}

		// 2018.07.09 ���⼭ �ƿ� �����ϰ� g_sLotNetDate_Info.waNetFaultCount[nLot][nNet]�� �׳� ����ϱ���ϰ� �ڸ�Ʈó��
		//nFaultCount += g_sLotNetDate_Info.waLotNetDate_FaultCnt[nLot][nNet][date];
	}

	double 	dDiff = 0, dDiffPowerSum = 0, dVariance=0, dSigma=0;
	if (nCount)
	{
		dAvg = dSum / (double)nCount;

		// 2. Calc Sigma   --------------------
		for (date=0; date < g_sLotNetDate_Info.naLotDateCnt[nLot]; date++)
		{
			tupleSize = g_pvsaNetData[nLot][nNet][date]->size();	// date���� tupleSize�� �ٸ�!
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
		dVariance = dDiffPowerSum / (double)nCount;		// �л�     : (val-avg)�� ������ ������ n���� ������.
		dSigma    = sqrt(dVariance);					// ǥ������ : �л��� ������
	}

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.

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
	m_editSampleNum = 0;
	ClearGrid_Data();	
	ClearGrid_Summary();	// data�� ������Ʈ�ϱ� ���� �����.


	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
	Invalidate(TRUE);		// ȭ�� ���� ����. UpdateData(False)������ Grid ȭ�� ������ ���� �ʾƼ� �߰���.


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
	UpdateData(TRUE);		// ȭ�� data �������� 

	CString strTemp;

	// Lot �����ؾ� �ϰ�, Net �����ؾ� �Ѵ�. 
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

	UpdateData(FALSE);		// ���� data ȭ�� �ݿ�
*/
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
			(m_nCombo_CurrDate == DATE_ALL) ? "DATE_ALL" : g_sLotNetDate_Info.strLotDate[nLot][nDate], 
			nNet); 					

	::ShellExecute(NULL,"open","EXCEl.EXE",strTemp,"NULL",SW_SHOWNORMAL);	

	
}


// 1. ���ݱ��� insert �� data��  binary ������ open�Ͽ�  write. 
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
	//  Summary Grid Data ���

	// ��� ���
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
	//  Data Grid Data ���

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
	// p Chart�� ���� sample data ��� 

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

	for (lot =0; lot < 1; lot++)		// Lot�� 1���� ����
	{
		int sampleSize = g_sLotNetDate_Info.naLotSampleCnt[lot];
		for (time =0; time < tupleSize; time++)
		{
			CString strTime;
			strTime.Format("%02d:%02d:%02d", 
					(*g_pvsaNetData[lot][1][0])[time].statTime.hour,
					(*g_pvsaNetData[lot][1][0])[time].statTime.min,
					(*g_pvsaNetData[lot][1][0])[time].statTime.sec);

			waTimeTotalCount[time] = (netCount * sampleSize);		// net ���� * sample ����
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


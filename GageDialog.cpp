// GageDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ACE400Statistics.h"
//#include "ACE400Gage.h"
#include "GageDialog.h"
#include "chartdir.h"
#include "FileSysInfo.h"
#include "math.h"
#include "cderr.h"
#include "XLEzAutomation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGageDialog dialog


CGageDialog::CGageDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGageDialog)
	m_editMeasDataPath = _T("");
	m_editSavedPath = _T("");
	m_edit_nMeasCnt = 0;
	m_edit_nRefInput = 0;
	m_edit_nTolInput = 1;
	m_edit_nStudyCnt = 0;
	//}}AFX_DATA_INIT
	
	m_nRef = m_edit_nRefInput;
	m_nTol = m_edit_nTolInput;
	m_dAvg = 0.0;
	m_d6StDev = 0.0;
	m_dStDev = 0.0;
	m_dBias = 0.0;
	m_dT = 0.0;
	m_dPValue = 0.0;
	m_dCg = 0.0;
	m_dCgk = 0.0;
	m_dVarRept = 0.0;
	m_dVarReptBias = 0.0;
}


void CGageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGageDialog)
	DDX_Control(pDX, IDC_LIST_MSG, 			   m_listMsg);
	DDX_Control(pDX, IDC_COMBO_MEAS_TYPE, 	   m_comboMeasType);
	DDX_Control(pDX, IDC_GRID_GAGE, 		   m_gridCtrl);
	DDX_Control(pDX, IDC_GRID_GAGE_STAT, 	   m_gridGageStat);
	DDX_Control(pDX, IDC_GRID_BIAS, 		   m_gridBias);
	DDX_Control(pDX, IDC_GRID_CAPABILITY, 	   m_gridCapability);
	DDX_Control(pDX, IDC_GRID_REPT, 		   m_gridRept);
	DDX_Text   (pDX, IDC_EDIT_4W_FILE_PATH,    m_editMeasDataPath);
	DDX_Text   (pDX, IDC_EDIT_SAVED_PATH,      m_editSavedPath);
	DDX_Text   (pDX, IDC_EDIT_MEASDATA_CNT,    m_edit_nMeasCnt);
	DDX_Text   (pDX, IDC_EDIT_REF_INPUT, 	   m_edit_nRefInput);
	DDX_Text   (pDX, IDC_EDIT_TOL_INPUT, 	   m_edit_nTolInput);
	DDX_Text   (pDX, IDC_EDIT_STUDY_CNT, 	   m_edit_nStudyCnt);
	DDV_MinMaxInt(pDX, m_edit_nStudyCnt, 0, 90);
	DDX_Control(pDX, IDC_CHART_GAGE, 		   m_ChartViewer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGageDialog, CDialog)
	//{{AFX_MSG_MAP(CGageDialog)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO_MEAS_TYPE,      OnSelchangeComboMeasType)
	ON_BN_CLICKED   (IDC_BUTTON_LOAD_MEASDATA, OnButtonLoadMeasdata)
	ON_EN_CHANGE    (IDC_EDIT_TOL_INPUT,       OnChangeEditTolInput)
	ON_EN_CHANGE    (IDC_EDIT_STUDY_CNT,       OnChangeEditStudyCnt)
	ON_EN_CHANGE    (IDC_EDIT_REF_INPUT,       OnChangeEditRefInput)
	ON_BN_CLICKED   (IDC_BUTTON_DO_STUDY,      OnButtonDoStudy)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_FILE,        OnButtonSaveFile)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_FILE,        OnButtonViewFile)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_GAGE_SAVE,   OnButtonAutoGageSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGageDialog message handlers

BOOL CGageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
__LINE_PRT__;
	
	g_sFile.Init();

	if (InitMember() == FALSE)
		return FALSE;

	if (InitView() == FALSE)
		return FALSE;


	MyTrace(PRT_BASIC, "\"4W GageStudy Dialog\" Started...\n" );

	CRect m_rectCurHist;
	this->GetWindowRect(m_rectCurHist);
	MyTrace(PRT_BASIC, "4W GageStudy Dialog:   top=%d, bottom=%d, left=%d, right=%d\n\n", 
							m_rectCurHist.top, m_rectCurHist.bottom,
							m_rectCurHist.left, m_rectCurHist.right);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CGageDialog::InitMember()
{

	// Meas Data 관련 멤버 변수 초기화
	m_nTypeCount = MAX_MEAS_TYPE;
	m_nMeasCount = MIN_MEAS_COUNT;
	m_nStudyCount = MIN_MEAS_COUNT;	
	::ZeroMemory(m_daMeasData, sizeof(m_daMeasData));

	m_edit_nMeasCnt  = m_nMeasCount; 		// UI에 표시될 meas Count
	m_edit_nStudyCnt = m_nStudyCount; 		// UI에 표시될 Study Count (보통 meas count와 같지만 작게 입력될 수 있다.)
	m_edit_nTolInput = m_nTol;				// UI에 표시되는 Tolerance

	// Grid 관련 멤버 변수 초기화
	m_nFixCols = 1;
	m_nFixRows = 1;
	m_nCols = (MAX_MEAS_TYPE + 1);
	m_nRows = (MAX_MEAS_COUNT + 1);
	m_bEditable = TRUE;

	m_nFixCols_Out = 1;
	m_nFixRows_Out = 0;
	m_nCols_Out = 2;
	m_nRows_Out = MAX_STAT_ENUM;
/*
	m_bHorzLines = TRUE;
	m_bListMode = FALSE;
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

	m_dAvg = 0.0;
	m_d6StDev = 0.0;
	m_dStDev = 0.0;
	m_dBias = 0.0;
	m_dT = 0.0;
	m_dPValue = 0.0;
	m_dCg = 0.0;
	m_dCgk = 0.0;
	m_dVarRept = 0.0;
	m_dVarReptBias = 0.0;

	m_listMsg.ResetContent();

	UpdateData(FALSE);
	
	return TRUE;
}


BOOL CGageDialog::InitView()
{
	UpdateData(TRUE);



	//----------------------------
	// Grid 초기화 
	//  : ChangeCurrType()를 combo초기화할 때 해야하므로 Grid를 Combo보다 먼저 초기화한다.

	m_gridCtrl.SetEditable(m_bEditable);
	//m_gridCtrl.SetListMode(m_bListMode);
	//m_gridCtrl.EnableDragAndDrop(TRUE);
	m_gridCtrl.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));		// 연노랑색 back ground

	m_gridGageStat.SetEditable(m_bEditable);
	m_gridGageStat.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));		// 연노랑색 back ground

	m_gridBias.SetEditable(m_bEditable);
	m_gridBias.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));		// 연노랑색 back ground

	m_gridCapability.SetEditable(m_bEditable);
	m_gridCapability.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));		// 연노랑색 back ground

	TRY {
		m_gridCtrl.SetRowCount(m_nRows);
		m_gridCtrl.SetColumnCount(m_nCols);
		m_gridCtrl.SetFixedRowCount(m_nFixRows);
		m_gridCtrl.SetFixedColumnCount(m_nFixCols);

		m_gridGageStat.SetRowCount(MAX_STAT_ENUM);
		m_gridGageStat.SetColumnCount(m_nCols_Out);
		m_gridGageStat.SetFixedRowCount(m_nFixRows_Out);
		m_gridGageStat.SetFixedColumnCount(m_nFixCols_Out);

		m_gridBias.SetRowCount(MAX_BIAS_ENUM);
		m_gridBias.SetColumnCount(m_nCols_Out);
		m_gridBias.SetFixedRowCount(m_nFixRows_Out);
		m_gridBias.SetFixedColumnCount(m_nFixCols_Out);

		m_gridCapability.SetRowCount(MAX_CAPABILITY_ENUM);
		m_gridCapability.SetColumnCount(m_nCols_Out);
		m_gridCapability.SetFixedRowCount(m_nFixRows_Out);
		m_gridCapability.SetFixedColumnCount(m_nFixCols_Out);

		m_gridRept.SetRowCount(MAX_REPT_ENUM);
		m_gridRept.SetColumnCount(m_nCols_Out);
		m_gridRept.SetFixedRowCount(m_nFixRows_Out);
		m_gridRept.SetFixedColumnCount(m_nFixCols_Out);
	}
	CATCH (CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
    END_CATCH	


	//---------------------------
	// 고정 데이터 먼저 출력
	Display_mohmGridHeader();		// Grid Header 설정.
	Display_OutputGridFixCol();		// Output Grid Fixed Col (name) 출력


	//--------------------------
	// combo box init

	// ChangeCurrType에서 제대로 초기화하기 전에 임시초기화, Load_4W_MeasData 실패 대비.
	// LoadData 전이므로 SetGridBlue는 FALSE로 초기화한다.
	ChangeCurrType(mohm_1, FALSE);		// 0 = mohm_1

	// mohm_75000 까지만 combo에 입력한다.
	for (int type = 0; type < (MAX_MEAS_TYPE -1); type++)
		m_comboMeasType.InsertString( -1, g_MeasInfoTable[type].strMeas);

	
	//-------------------------
	// 4W Meas File data 로드
	
	// ACE400의 4W_Setup_A.txt 파일 위치를 찾는다.   ex) "C:\ACE400\QC-JIG-S100-BHFlex\4W\"
	FileSysInfo01.LoadSaveView01(DATA_LOAD);		// Load	SysInfoView01.m_pStrFilePathBDL

	// 4W_Setup_A.txt 파일을 로드한다.  : 4W Meas data
	CString dataFilePath, dataFileName;
	//dataFilePath = SysInfoView01.m_pStrFilePathBDL;	// ex) "C:\ACE400\QC-JIG-S100-BHFlex\4W\"
	//dataFileName = "\\4W_Setup_A-H-10.txt";	
	
	dataFilePath = "C:\\ACE400\\BDL\\QC\\OpenRjig";
	dataFileName = "\\4W_Setup_A.txt";	
	dataFilePath += dataFileName;

	// 4W Meas Data Open Fail
	if (Load_4W_MeasData(dataFilePath) < 0)		
	{
		UpdateData(FALSE);
		Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

		// 2019.01.29  : default 파일이 없더라도 return은 하지 않고 나머지를 진행하도록 수정
		//return FALSE;
	}

	// 4W Meas Data Open 성공
	else
	{
		// Grid Data  출력
		//  : 블루로 선택할 data가 있어야 하므로 ChangeCurrType(type)은  Display_mohmGridData() 다음에 호출해야 함.
		Display_mohmGridData();			
	}


	// 현재 combo에서 선택된 type#
	ChangeCurrType(mohm_1); 			// 0 = mohm_1

	// 선택된 type에 대한 'type1 gage study' 결과를 출력 
	DoGageStudy(m_nCombo_CurrType);

	UpdateData(FALSE);
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.

	return TRUE;
}


// mohm Grid header 설정
#define		GRID_COL_NO			0
#define		GRID_COL_MOHM_BASE	1
void CGageDialog::Display_mohmGridHeader()
{

	// 0     1      2        3     4      5      6       7         8         9       10      
	// No,  1mohm, 2mohm,  3mohm, 4mohm, 5mohm, 10mhom,  20mohm,   30mohm,  40mohm,  50mohm, 
	// 11       12        13       14     15       				.... 24 
	// 100mohm  200mohm 500mohm  750mohm  1000mohm				   75000mohm  


    m_gridCtrl.SetItemText(0, GRID_COL_NO, "No");
	m_gridCtrl.SetColumnWidth(GRID_COL_NO, 40);


	for (int type=0; type < (MAX_MEAS_TYPE -1); type++)
	{
		m_gridCtrl.SetItemText(0, (GRID_COL_MOHM_BASE + type), g_MeasInfoTable[type].strMeas);
		m_gridCtrl.SetColumnWidth((GRID_COL_MOHM_BASE + type), 82);

	}

	
	//m_gridCtrl.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조정
}


void CGageDialog::Display_mohmGridData()
{
	CString	strTemp;

	for (int meas=0; meas < m_nMeasCount; meas++)
	{
		// No 출력
		strTemp.Format("%d", (meas+1) );
    	m_gridCtrl.SetItemText(meas+1, GRID_COL_NO, strTemp);

    	// 해당 No의 mohm(MeasType)에 해당하는 측정값 출력.
		for (int type=0; type < (MAX_MEAS_TYPE -1); type++)
		{
			strTemp.Format("%.2f",  m_daMeasData[type][meas]);		// 가로세로를 바꾸어서 출력.
			m_gridCtrl.SetItemText(meas+1, (GRID_COL_MOHM_BASE + type), strTemp);
		}

	}

}


void CGageDialog::Display_OutputGridFixCol()
{

	int row;

	m_gridGageStat.SetColumnWidth(0, 100);
	m_gridGageStat.SetColumnWidth(1, 80);
	for (row = 0; row < MAX_STAT_ENUM; row++)
		m_gridGageStat.SetRowHeight(row, 25);

    m_gridGageStat.SetItemText(STAT_REF, 		0, "Reference");
    m_gridGageStat.SetItemText(STAT_MEAN, 		0, "Mean");
    m_gridGageStat.SetItemText(STAT_STDEV, 		0, "StDev");
    m_gridGageStat.SetItemText(STAT_6xSTDEV, 	0, "6 x StDev (SV)");
    m_gridGageStat.SetItemText(STAT_TOL, 		0, "Tolerance (TOL)");

	m_gridBias.SetColumnWidth(0, 100);
	m_gridBias.SetColumnWidth(1, 80);
	for (row = 0; row < MAX_BIAS_ENUM; row++)
		m_gridBias.SetRowHeight(row, 25);
	m_gridBias.SetRowHeight(BIAS_PVALUE, 40);		// 2줄 공간 확보
    m_gridBias.SetItemText(BIAS_BIAS, 		0, "Bias");
    m_gridBias.SetItemText(BIAS_T, 			0, "T");
    m_gridBias.SetItemText(BIAS_PVALUE, 	0, "PValue\n(Test Bias = 0)");


	m_gridCapability.SetColumnWidth(0, 100);
	m_gridCapability.SetColumnWidth(1, 80);
	for (row = 0; row < MAX_CAPABILITY_ENUM; row++)
		m_gridCapability.SetRowHeight(row, 25);
    m_gridCapability.SetItemText(CAPABILITY_CG, 			0, "Cg");
    m_gridCapability.SetItemText(CAPABILITY_CGK, 			0, "Cgk");


	m_gridRept.SetColumnWidth(0, 170);
	m_gridRept.SetColumnWidth(1, 80);
    m_gridRept.SetItemText(REPT_VAR, 		0, "%Var (Repeatablility)");
    m_gridRept.SetItemText(REPT_VAR_BIAS, 	0, "%Var (Repeatability and Bias)");

	
	//m_gridCtrl.SetRowHeight(0, 40);	// 헤더에 글자 두줄 표시를 위해 높이 조정
}


// 창이 새로 보일때 때마다 초기화해야 하는 멤버들은 다음의 
// OnShowWindow()의 TODO 밑에 초기화한다. 
void CGageDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(bShow == TRUE)
	{
		// 멤버 초기화


		// View 초기화 

	}
	
}

#define		MAX_4W_MEAS_FILE		5
void CGageDialog::OnButtonLoadMeasdata() 
{
	// TODO: Add your control notification handler code here
	
	//-----------------------------
	// File Open Dialog 생성
	
	char szFilter[] = "txt Files(*.TXT)|*.TXT|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, 				// bOpenFileDialg = TRUE: 타이틀바에 '열기' 출력
					NULL, 				// lpszDefExt           : 기본 확장자
					"\\4W_Setup_A.txt", // lpszFileName         : "파일 이름" Edit 컨트롤에 처음 출력될 파일명.
					OFN_HIDEREADONLY |OFN_NOCHANGEDIR |OFN_ALLOWMULTISELECT, 
										// dwFlags :  속성   (HIDEREADONLY : read only 파일은 출력하지 않음.
					szFilter);			// 대화상자에 출력될 파일을 확장자로 거르기 위한 필터. 

	// 오픈할때 초기 경로 지정. 
	dlg.m_ofn.lpstrInitialDir = _T(SysInfoView01.m_pStrFilePathBDL); 	// ex) "C:\ACE400\QC-JIG-S100-BHFlex\4W\"
	dlg.m_ofn.lpstrInitialDir = _T("C:\\ACE400\\BDL\\QC\\OpenRjig"); 	// ex) "C:\ACE400\BDL\QC\OpenRjig\"

	
	// buffer의 사이즈가 매우 커지면,  stack overflow를 유발할 수 있어 static으로 설정함.
	static char buffer[60 * MAX_4W_MEAS_FILE] = {0}; 	// 버퍼
	dlg.m_ofn.lpstrFile = buffer; 						// 버퍼 포인터
	dlg.m_ofn.nMaxFile = (60 * MAX_4W_MEAS_FILE); 		// 버퍼 크기,  file 5개 처리하려면 이정도 buffer필요함.

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

	
	CString dataFilePath;
	POSITION pos= dlg.GetStartPosition();
	dataFilePath = dlg.GetNextPathName(pos);


	//-----------------------------------
	// 4w Meas File data load
	if (Load_4W_MeasData(dataFilePath) < 0 )
		return;

	// MeasurmentData Grid 반영
	ClearGrid_Data();					// 기존에 출력됐던 measumrent data를 지우고 
	Display_mohmGridData();				// 새로운 Input Grid Data  출력

	// 새로 load했으므로 type, StudyCount, Ref, Tol을 모두 default로 초기화한다.
	// : 블루로 선택할 data가 있어야 하므로 ChangeCurrType()는 Display_mohmGridData() 다음에 호출해야 함.
	ChangeCurrType(mohm_1); 

	//-----------------------------------------------
	// Output Grid 반영 및 Output 출력을 다시 수행

	// 선택된 type에 대한 'type1 gage study' 결과를 출력 
	DoGageStudy(m_nCombo_CurrType);

	UpdateData(FALSE);

}


#define		MAX_LINE_STR		2048
int CGageDialog::Load_4W_MeasData(CString dataFilePath)
{

	FILE *fp; 
	char  fName[200];  


	//----------------
	// File Open  
    ::ZeroMemory(&fName, sizeof(fName));
 	strcat(fName, dataFilePath);


	if(!FileExists(fName)) 
	{ 	
		ERR.Set(FLAG_FILE_NOT_FOUND, fName); 
		ErrMsg();  ERR.Reset(); return -1; 
	}

	fp=fopen(fName,"rt");
	if(fp==NULL) 
	{ 	
		ERR.Set(FLAG_FILE_CANNOT_OPEN, fName); 
		ErrMsg();  ERR.Reset(); return -1; 
	}


	//-------------------------
	// File 초기 정보 read
	
	int nMeasCount;
	int nTypeCount;
	CString strTemp;
	fscanf(fp, "%d, %d,  \n", &nMeasCount, &nTypeCount);		 


	char str[MAX_LINE_STR];
	char	strWord[3][200];
	::ZeroMemory(str, sizeof(str));
	fgets((char*)str, MAX_LINE_STR, fp);	// 헤더라인은 통째로 읽어서 skip

	// File 형식 체크: 맨 앞이 'NetNo'으로 시작하지 않는다면 Meas data file이 아님 
	::ZeroMemory(strWord, sizeof(strWord));
	int ret = sscanf(str, "%s %s ", strWord[0], strWord[1]); // ex) Read " NetNo.,   *,"
	if (strncmp(strWord[0],"NetNo.", 6) != 0)		
	{
		strTemp.Format("Load_4W_MeasData(): %s is not 4w measurement data File!\n", fName );
		ERR.Set(INVALID_INPUT, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return -1;	 
	}

	// TypeCount가 25 이상이면 range over이지만, 리턴하지 않고 max값인 25으로 값을 변경해서 계속 진행한다.
	if (nTypeCount > MAX_MEAS_TYPE)
	{
		strTemp.Format("Can't load this file!\n: %s\n\n nTypeCount=%d Range(<%d) Out Error!\n", 
					fName, nTypeCount, MAX_MEAS_TYPE);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		nTypeCount = MAX_MEAS_TYPE;

		//return -1;		// 2018.12.20 Gage Study는 중단하지 않고 진행하기로 한다.
	}

	// Meas Count가 10개 이하이면 에러를 출력한다.  Gage Study는 계속 진행한다.
	if (nMeasCount < MIN_MEAS_COUNT)		// MIN_MEAS_COUNT: 10
	{
		strTemp.Format("Can't load this file!\n: %s\n nMeasCount=%d.\n\nAt least %d measurement data are required, and the recommended number is 50 or more.", 
				fName, nMeasCount, MIN_MEAS_COUNT);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		//return -1;		// 2018.12.20 Gage Study는 중단하지 않고 진행하기로 한다.
	}


	// MeasCount가 90 이상이면 range over이지만, 리턴하지 않고 max값인 90으로 값을 변경해서 계속 진행한다.
	if (nMeasCount > MAX_MEAS_COUNT)
	{
		strTemp.Format("Load_4W_MeasData(): %s nMeasCount=%d Range(<%d) Over!\n\n We are changing the nMeasCount value to the maximum number of 90.", 
					fName, nMeasCount, MAX_MEAS_COUNT);
		int ret = AfxMessageBox(strTemp, MB_OKCANCEL|MB_ICONINFORMATION);
		MyTrace(PRT_BASIC, strTemp);
		nMeasCount = MAX_MEAS_COUNT;		
	}


	//-----------------------------------------
	// Meas Data 관련 멤버 변수 default 초기화
	

	// 에러로 리턴되는 경우에는 기존값을 보호해야 하므로
	// 에러체크돼서 리턴되는 경우를 모두 통과해야만 member 변수들을 업데이트할 수 있다.
	::ZeroMemory(m_daMeasData, sizeof(m_daMeasData));
	m_nMeasCount  = nMeasCount;
	m_nStudyCount = nMeasCount;
	m_nTypeCount  = nTypeCount;
	m_nTol = 1;

	m_edit_nMeasCnt  = m_nMeasCount; 		// UI의 Meas Count
	m_edit_nStudyCnt = m_nStudyCount; 		// UI의 Study Count (보통 meas count와 같지만 작게 입력될 수 있다.)
	m_edit_nTolInput = m_nTol;				// UI의 Tol 
 	m_editMeasDataPath = fName;				// UI의 file path
	m_editSavedPath   = _T("");				// UI의 saved file path 초기화

	UpdateData(FALSE); // 대화상자의 Edit컨트롤에 m_editMeasDataPath, m_edit_nMeasCnt 를 출력.


	//-----------------------------------------------------
	// m_nTypeCount 갯수만큼 data Line을 읽는 동작을 반복
	int		pin[4];
	int		nMeasType;
	int  	row=0;
	while(!feof(fp))
	{
		if (row >= m_nTypeCount  || row >= MAX_MEAS_TYPE)		// 탈출 조건
			break;

		::ZeroMemory(str, sizeof(str));
		fgets((char*)str, MAX_LINE_STR, fp);	// 한줄 통째로 읽어 오기


		// 맨 앞이 'Net'으로 시작하지 않는다면 line skip
		::ZeroMemory(strWord, sizeof(strWord));
		int ret = sscanf(str, "%s %s %d, %d, %d, %d", // ex) Read "Net1,  PIN,  1,  3,   130,  132, "
				strWord[0], strWord[1],  &pin[0], &pin[1], &pin[2], &pin[3]); // PIN 번호 read.  사용하지는 않음.
		if (strncmp(strWord[0],"Net", 3) != 0)		
		{
			row++;
			continue;	// line skip
		}

		// nMeasType이 range over인 경우 line skip
		nMeasType = -1;
		int length = strlen(strWord[0]);
		CString str1 = strWord[0];
		CString str2 = str1.Mid(3, (length-3));
		nMeasType = (atoi(str2) - 1); // nMeasType (Net번호 -1) 추출
		if (nMeasType < 0 || nMeasType >= m_nTypeCount) 
		{
			strTemp.Format("Load_4W_MeasData(): %s row=%d nMeasType=%d Range(0 <= nMeasType < %d) Over!\n", 
						fName, row, nMeasType, m_nTypeCount);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); 

			row++;		
			continue; // line skip
		}

		// Measurement data 위치 찾기.  (6번째 ',' 직후)
		int dataLoc = 0;
		if (getMeasDataLoc(str, MAX_LINE_STR, dataLoc) < 0) 	// data loc 찾기 실패라면 line skip
		{
			strTemp.Format("Load_4W_MeasData(): %s nMeasType=%d getMeasDataLoc(dataLoc=%d) Error! \n", 
					fName, nMeasType, dataLoc);
			ERR.Set(SW_LOGIC_ERR, strTemp); 
			ErrMsg();  ERR.Reset(); 

			row++;
			continue;	// line skip
		}

		//------------------------------------------------
		// m_nMeasCount 갯수만큼 Line 안에서 data read
		char *pStr = &str[dataLoc];
		int loc;
		for (int meas=0; meas < m_nMeasCount; meas++)
		{
			loc = getMeasData(pStr, (MAX_LINE_STR-dataLoc), m_daMeasData[nMeasType][meas]);	
			if (loc < 0)
			{
				strTemp.Format("Load_4W_MeasData(): %s nMeasType=%d getMeasData(dataLoc=%d) Error!  meas=%d\n", 
						fName, nMeasType, dataLoc, meas);
				ERR.Set(SW_LOGIC_ERR, strTemp); 
				ErrMsg();  ERR.Reset(); 
				break;
			}

			dataLoc += (loc +1);	// read한 data와 ','의 다음 위치를 표시한다.
			pStr += (loc +1);
				
		}

		row++;
	}

	
	fclose(fp);

	return 0;
}

int CGageDialog::getMeasDataLoc(char *pStr, int strMax, int &rLoc) 
{
	// data 위치 찾기.  (6번째 ',' 직후)
	int 	commaCnt = 0;
	for (int loc=0; loc < strMax; loc++)
	{
		if (pStr[loc] == ',')
			commaCnt++;

		if (commaCnt == 6)
		{
			rLoc = loc +1;
			return 0;		// OK : 6번째 ',' 위치 리턴
		}
	}

	return -1;		// NOK : 6번째 ',' 위치를 리턴 못한 경우
}

int CGageDialog::getMeasData(char *pStr, int strMax, double &rdMeasData) 
{
	int loc;

	for (loc=0; loc < strMax; loc++)
	{
		if (pStr[loc] == ',')
			break;
	}
	if (loc == strMax)	// ',' 찾는데에 실패.
		return -1;

	// ',' 위치 찾음 OK.
	CString str = pStr;
	CString strData = str.Left(loc);
	rdMeasData = (double)atof(strData);

	return loc;
}

BOOL CGageDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	

	m_gridCtrl.DeleteAllItems();
	m_gridGageStat.DeleteAllItems();
	m_gridBias.DeleteAllItems();
	m_gridCapability.DeleteAllItems();
	m_gridRept.DeleteAllItems();
	

	MyTrace(PRT_BASIC, "GageDialog Destroyed...\n" );
	return CDialog::DestroyWindow();
}


void CGageDialog::OnSelchangeComboMeasType() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;

	// 현재 combo에서 선택된 type#.  enum값으로 access 가능함 (ex: mohm_1, mohm_2..)	
	int comboType  = m_comboMeasType.GetCurSel(); 
	if ( comboType < 0 || comboType >= MAX_MEAS_TYPE ) // Range Check
	{
		strTemp.Format("OnSelchangeComboMeasType(): comboType=%d, Range(0 =< MeasType < %d) Over\n", 
					comboType, MAX_MEAS_TYPE );
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 
		return;
	}	

	// 입력받은 combo type으로 m_nCombo_CurrType을 바꾼다.
	ChangeCurrType(comboType);		


	// 선택된 type에 대한 'type1 gage study' 결과를 출력 
	DoGageStudy(m_nCombo_CurrType);
	
	UpdateData(FALSE); 

}


// 현재 선택된 type의 배경을 푸른색으로 설정.
void CGageDialog::SetGridBkBlue(int type)
{
	// 먼저 배경을 모두 노랑색으로 원상복구
	ClearGrid_BackGround();			

	//  선택된 mohm의 Grid cell들을 푸른 배경으로 표시
	for (int meas=0; meas < m_nMeasCount; meas++)
		m_gridCtrl.SetItemBkColour(meas+1, (GRID_COL_MOHM_BASE + type), 	// row, col
									RGB(0x00, 0xbf, 0xff));					// deep skyblue : 짙은 하늘파랑 

	// type이 mohm_50000, mhom_200이상이면 선택된 mohm가 보이도록, Grid의 가로 스크롤 위치를 바꾼다.
/*	if (type >= mohm_50000)
		m_gridCtrl.SetScrollPos32(SB_HORZ, (mohm_50000 * 82));
	
	else*/ 
	if (type >= mohm_200)
		m_gridCtrl.SetScrollPos32(SB_HORZ, (mohm_200 * 82));

	// mohm_200이하이면 가로 스크롤 원상복구
	else
		m_gridCtrl.SetScrollPos32(SB_HORZ, 0);	

}

void CGageDialog::ClearGrid_BackGround()
{

	for (int row = 1; row < m_gridCtrl.GetRowCount(); row++)		// 헤더는 제외하고
		for (int col = 0; col < m_gridCtrl.GetColumnCount(); col++)
		{
			// 배경을 원상복구 한다.
			m_gridCtrl.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

}

void CGageDialog::ClearGrid_Data()
{

	// fill rows/cols with text
	for (int row = 1; row < m_gridCtrl.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
		for (int col = 0; col < m_gridCtrl.GetColumnCount(); col++)
		{
			m_gridCtrl.SetItemText(row, col, "                 ");


			// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
			m_gridCtrl.SetItemBkColour(row, col, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
		}

	//m_gridCtrl.DeleteAllItems();  -> 이것도 넣으면 속도 느려지고 애써 초기화한 data 다 날아감
	
	// Scroll Bar 초기화 
	m_gridCtrl.SetScrollPos32(SB_VERT, 0);
	m_gridCtrl.SetScrollPos32(SB_HORZ, 0);
	

//	m_gridCtrl.AutoSize(); -> 이것 넣으면 속도 느려짐.

}


// Output Grid의 값을 모두 지우고 배경색도 노랑색으로 원상복구한다.
void CGageDialog::ClearGrid_Output()
{
	int row;

	// fill rows/cols with text :  두번째 컬럼만 초기화.
	for (row = 0; row < m_gridGageStat.GetRowCount(); row++)		// 헤더는 제외하고 클리어한다.
	{
		m_gridGageStat.SetItemText(row, 1, "                 ");

		// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
		m_gridGageStat.SetItemBkColour(row, 1, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
	}

	for (row = 0; row < m_gridBias.GetRowCount(); row++)		
	{
		m_gridBias.SetItemText(row, 1, "                 ");

		// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
		m_gridBias.SetItemBkColour(row, 1, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
	}

	for (row = 0; row < m_gridCapability.GetRowCount(); row++)		
	{
		m_gridCapability.SetItemText(row, 1, "                 ");

		// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
		m_gridCapability.SetItemBkColour(row, 1, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
	}

	for (row = 0; row < m_gridRept.GetRowCount(); row++)		
	{
		m_gridRept.SetItemText(row, 1, "                 ");

		// 혹시 배경을 fault처리한 cell이 있다면 다음번 display를 위해 원상복구 한다.
		m_gridRept.SetItemBkColour(row, 1, RGB(0xFF, 0xFF, 0xE0));	// 연노랑색 
	}
}



void CGageDialog::DoGageStudy(int type) 
{
	
	DisplayGageStudyChart(type);		// Run Chart 출력
	CalcGageStudyOutput(type);			// Type1 Gage Study Output 값 계산
	DisplayGageStudyOutput(type);		// Type1 Gage Study Output Grid 출력
	
}

// Type1 Gage Study Run Chart 출력
void CGageDialog::DisplayGageStudyChart(int type) 
{

	//----------------------------------------------------------------------
	// Ref - 0.1 *Tol, Ref, Ref + 0.1*Tol, measData(type) 로 그래프 그리기 
	//
	// Draw Chart and set to CChartViewer
	


    // The data for the bar chart
   	double ref[MAX_MEAS_COUNT];			// ref값이 array가 아니므로 출력을 위한 array를 만든다.
   	double ref_minus[MAX_MEAS_COUNT];	// ref - (0.1 * Tol)
   	double ref_plus[MAX_MEAS_COUNT];	// ref + (0.1 * Tol)
    double dataX0[MAX_MEAS_COUNT];		// X 축 label로 사용할 array 

    int nRef = g_MeasInfoTable[type].nMeasRef;
	for (int meas =0; meas < m_nStudyCount; meas++)
	{
		ref[meas] = nRef;
		ref_minus[meas] = nRef - (0.1 * m_nTol);
		ref_plus[meas]  = nRef + (0.1 * m_nTol);
		dataX0[meas] = meas;
	}


	// 차트 생성 ------
    // Create an XYChart object of size 660 x 280 pixels, with a light blue (EEEEFF) background,
    // black border, 1 pxiel 3D border effect and rounded corners
    XYChart *c = new XYChart(660, 235, 0xeeeeff, 0x000000, 1);
    //c->setRoundedFrame();

    // Set the plotarea at (55:x, 58:y) and of size 580 x 175 pixels, with white background. Turn on
    // both horizontal and vertical grid lines with light grey color (0xcccccc)
    c->setPlotArea(55, 58, 580, 125, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);


	// 범례형식 지정  ------- 
    // Add a legend box at (250:x, 30:y) (top of the chart) with horizontal layout. Use 9pt Arial Bold
    // font. Set the background and border color to Transparent.
    c->addLegend(250, 30, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);



	// X축, Y 축 타이틀 지정 ----------------
	
    // Add a title box to the chart using 12pt Arial Bold Italic font, on a light blue (CCCCFF)
    // background with glass effect. white (0xffffff) on a dark red (0x800000) background, with a 1
    // pixel 3D border.
    CString strTemp;
    strTemp.Format("Run Chart of %s", g_MeasInfoTable[type].strMeas);
	c->addTitle(strTemp, "arialbi.ttf", 12)->setBackground(0xccccff, 0x000000, Chart::glassEffect());


    // data가 실제 있는 범위만 출력되도록 Scale 조정하기. 
    //   meas[0]에 0값이 있는 의미없는 data가 있었음. meas[0]을 제외하고 출력하자 아래 코드 불필요.
    //c->yAxis()->setAutoScale(0.1, 0.1);			
    //c->yAxis()->setLinearScale(m_dMinYR, m_dMaxYR);
    //c->yAxis()->setMargin(10, 10);
    
    // Add a title to the y axis
 
    TextBox *textbox_Y = c->yAxis()->setTitle("mohm");
    textbox_Y->setFontAngle(0);					// Set the y axis title upright (font angle = 0)
    textbox_Y->setAlignment(Chart::TopLeft2);	// Put the y axis title on top of the axis


    // Add a title to the x axis
    TextBox *textbox_X = c->xAxis()->setTitle("Measure Count");
    textbox_X->setAlignment(Chart::BottomRight);

	// The labels for the bar chart
    //const char *labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
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
    									

    layer->addDataSet( DoubleArray(ref_minus, m_nStudyCount), 
	   				0xff0000, "Ref - 0.10 x Tol")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);	// 빨간색

    layer->addDataSet( DoubleArray(ref_plus, m_nStudyCount), 
	   				0xff0000, "Ref + 0.10 x Tol")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);	// 빨간색

    layer->addDataSet( DoubleArray(ref, m_nStudyCount), 
	   				0x008800, "Ref")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);		// 초록색

    layer->addDataSet( DoubleArray(&m_daMeasData[type][0], m_nStudyCount), 
	   				0x3333ff, "Data")->setDataSymbol(Chart::DiamondSymbol, 7, -1, 0x008800);		// 남색


 /*   layer->addDataSet( DoubleArray(&m_daLCL[1], g_sLotNetDate_Info.naLotNetCnt[nLot]), 
    				//0xff0000, "LCL");				// 빨간색 
    				0xff00ff, "LCL")->setDataSymbol(Chart::DiamondSymbol, 4, -1, 0xff00ff);		// 자주색

    // For demo purpose, we use a dash line color for the last line
    //layer->addDataSet(DoubleArray(m_daLCL, (int)(sizeof(m_daLCL) / sizeof(m_daLCL[0]))), 
    //				c->dashLineColor(0x3333ff, Chart::DashLine), "LCL");	// 남색 점선
  */  
    // The x-coordinates for the line layer	 : X 축 data label 설정.
	layer->setXData(DoubleArray(dataX0, m_nStudyCount ));

/*
	// Track Net이 설정된 경우, Track line도 그려준다.
    if (nTrackNet != -1)
	{
    	//trackLineLegend(c, c->getPlotArea()->getRightX());
    	
    	// Include track line for the latest data values
	    trackLineLabel(c, nTrackNet);
	}
 */   

    // XY LineChart  출력 ----------------


    // Output the chart
    m_ChartViewer.setChart(c);
	
    // Include tool tip for the chart
    m_ChartViewer.setImageMap( c->getHTMLImageMap("", "", "title='{xLabel}: US${value}K'"));

	// Save the Chart to png file
	char fName[200];
	::ZeroMemory(&fName, sizeof(fName));
	strTemp.Format(".\\Data\\ChartView_%s.png", g_MeasInfoTable[type].strMeas);
	strcat(fName, strTemp);
	m_ChartViewer.getChart()->makeChart(fName);


	// In this sample project, we do not need to chart object any more, so we 
	// delete it now.
    delete c;

}

// Type1 Gage Study Output 값 계산
void CGageDialog::CalcGageStudyOutput(int type) 
{

	//---------------------
	// Calc Avg (Mean) 
	m_dAvg = 0.0;

	int meas;
	double dSum = 0;
	for (meas=0; meas < m_nStudyCount; meas++)
		dSum += m_daMeasData[type][meas]; 

	if (m_nStudyCount)		// check devide by zero : Average
		m_dAvg = dSum / (double) m_nStudyCount;



	//-----------------------------
	// Calc StDev & 6x Stdev 	
	
	m_dStDev = 0.0;
	m_d6StDev = 0.0;
	double dDiff = 0, dDiffPowerSum = 0, dVar = 0; 
	int degFree = m_nStudyCount -1;		// degree of freedom (자유도) : n - 1 
	if (degFree)	// check devide by zero : for dVar
	{
		// Calc Variance
		for (meas=0; meas < m_nStudyCount; meas++)
		{
			dDiff = m_daMeasData[type][meas] - m_dAvg;
			dDiffPowerSum += (dDiff * dDiff);
		}

		// 분산     : (val-avg)의 제곱의 총합을 (n - 1)으로 나눈다.
		dVar = dDiffPowerSum / (double)degFree;

		// 표준편차 : 분산의 제곱근
		m_dStDev = sqrt(dVar);
		m_d6StDev = 6 * m_dStDev;

	}
	
	//-------------
	// Calc Bias 
	m_dBias = 0.0;
	m_dBias = m_dAvg - g_MeasInfoTable[type].nMeasRef;
	
	//-------------
	// Calc T 
	m_dT = 0.0;
	if (m_nStudyCount)	// check devide by zero : for stdErr 
	{
		double stdErr = m_dStDev / sqrt(m_nStudyCount);
		if (stdErr)	// check devide by zero : for m_dT
			m_dT = m_dBias / stdErr;
		if (m_dT < 0)
			m_dT = -m_dT;		// 절대값
	}
	
	//-------------
	// Calc PValue 
	m_dPValue = 0.0;
	m_dPValue = p_tdist(m_dT, degFree);


	//-------------
	// Calc Cg	
	m_dCg = 0.0;
	if (m_d6StDev)	// check devide by zero
		m_dCg = (20/(double)100 * m_nTol) / (double)m_d6StDev;


	//--------------
	// Calc Cgk  
	m_dCgk = 0.0;
	if (m_dStDev)	// check devide by zero
	{
		double absBias = (m_dBias < 0) ? (-m_dBias) : m_dBias;		// 절대값
		m_dCgk = (20/(double)200 * m_nTol - absBias) / (double)(3 * m_dStDev);
	}

	
	//---------------------------
	// Calc %Var(Repeatability)  
	m_dVarRept = 0.0;
	if (m_dCg)		// check devide by zero
		m_dVarRept = 20 / m_dCg;

	
	//------------------------------------
	// Calc %Var(Repeatability and Bias) 
	m_dVarReptBias = 0.0;
	if (m_dCgk)
		m_dVarReptBias = 20 / m_dCgk;

	UpdateData(FALSE);
}

// Type1 Gage Study Output Grid 출력
void CGageDialog::DisplayGageStudyOutput(int type) 
{
	UpdateData(TRUE);
	CString strTemp;

	ClearGrid_Output();			// Output Grid의 값을 모두 지우고 배경색도 노랑색으로 원상복구한다.
	m_listMsg.ResetContent();	// listMsg의 내용물도 비운다.
	UpdateData(FALSE);

	// Stat Grid
	strTemp.Format("%d", m_nRef);
	m_gridGageStat.SetItemText(STAT_REF, 		1, strTemp);

	strTemp.Format("%.2f", m_dAvg); 			
	m_gridGageStat.SetItemText(STAT_MEAN, 		1, strTemp);

	strTemp.Format("%.3f", m_dStDev);
	m_gridGageStat.SetItemText(STAT_STDEV, 		1, strTemp);

	strTemp.Format("%.3f", m_d6StDev); 
	m_gridGageStat.SetItemText(STAT_6xSTDEV, 	1, strTemp);

	strTemp.Format("%d", m_nTol);
	m_gridGageStat.SetItemText(STAT_TOL, 		1, strTemp);


	// Bias Grid
	strTemp.Format("%.2f", m_dBias);
	m_gridBias.SetItemText(BIAS_BIAS, 		1, strTemp);

	strTemp.Format("%.3f", m_dT);
	m_gridBias.SetItemText(BIAS_T, 			1, strTemp);

	strTemp.Format("%.3f", m_dPValue); 
	m_gridBias.SetItemText(BIAS_PVALUE, 	1, strTemp);

	// Capability Grid
	strTemp.Format("%.2f", m_dCg);
	m_gridCapability.SetItemText(CAPABILITY_CG, 			1, strTemp);

	strTemp.Format("%.2f", m_dCgk);
	m_gridCapability.SetItemText(CAPABILITY_CGK, 			1, strTemp);


	// Repeatability Grid
	strTemp.Format("%.2f %%", m_dVarRept);
	m_gridRept.SetItemText(REPT_VAR, 		1, strTemp);

	// 10% 이상이면 MAJ 알람표시(주황색) , 15% 이상이면 CRI 알람표시(빨간색)
	if (m_dVarRept >= VAR_REPT_LIMIT_CRI)		// Critical Alarm
	{
		m_gridRept.SetItemBkColour(REPT_VAR, 1,	// row, col
										RGB(0xdc, 0x24, 0x4c));		// crimson(0xdc143c)보다 약간 연한 빨강
		strTemp.Format("'%% Var (Repeatability)' exceeds the critical limit of %d%%.", 
				VAR_REPT_LIMIT_CRI);
		m_listMsg.InsertString(0, strTemp);
	}
	else if (m_dVarRept >= VAR_REPT_LIMIT_MAJ)	// Major Alarm
	{
		m_gridRept.SetItemBkColour(REPT_VAR, 1,	// row, col
										RGB(0xff, 0x63, 0x47));		// tomato : 진한 주황
		strTemp.Format("'%% Var (Repeatability)' exceeds the major limit of %d%%.", 
				VAR_REPT_LIMIT_MAJ);
		m_listMsg.InsertString(0, strTemp);

	}
	else	// Normal
	{
		m_gridRept.SetItemBkColour(REPT_VAR, 1,	// row, col
										RGB(0x95, 0xee, 0x95));		// light green : 밝은 초록색 
		strTemp.Format("'%% Var (Repeatability)' is OK.");
		m_listMsg.InsertString(0, strTemp);


	}

	strTemp.Format("%.2f %%", m_dVarReptBias);
	m_gridRept.SetItemText(REPT_VAR_BIAS, 	1, strTemp);

	UpdateData(FALSE);
	Invalidate(TRUE);		// 화면 강제 갱신. UpdateData(False)만으로 Grid 화면 갱신이 되지 않아서 추가함.
}

void CGageDialog::OnChangeEditTolInput() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	

	//=> OnButtonDoStudy()  로 이동함.
/*	
	CString strTemp;	
	GetDlgItemText(IDC_EDIT_TOL_INPUT, strTemp);
	
	UpdateData(TRUE);

	m_nTol = m_edit_nTolInput;		

	// 수정된 Tol 에 대한 'type1 gage study' 결과를 출력 
	//DoGageStudy(m_nCombo_CurrType);		=> Tol이나  StudyCount의 경우에는 OK를 클릭해야 변경이 반영되도록 수정함.
	
	UpdateData(FALSE);
*/
}

void CGageDialog::OnChangeEditRefInput() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	
}

void CGageDialog::OnChangeEditStudyCnt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
/*
// 이부분은  OnButtonDoStudy()  로 이동함.
	UpdateData(TRUE);

	// edit StudyCount값이 range over로 잘못 입력된 경우
	//if ( m_edit_nStudyCnt < MIN_MEAS_COUNT  || m_edit_nStudyCnt > MAX_MEAS_COUNT)
	if ( m_edit_nStudyCnt > MAX_MEAS_COUNT)
	{
		CString strTemp;
		strTemp.Format("GageStudy Count=%d. Range(%d<= Count <=%d) Over Error!\nRecover GageStudyCount to default %d", 
						m_edit_nStudyCnt, MIN_MEAS_COUNT, MAX_MEAS_COUNT, m_nMeasCount);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		// 잘못 입력된 edit StudyCount를 default 값인 MeasCount값으로 다시 원상복구한다.
		// 리턴하지는 않고 원상복구된 기준으로 그래프와 Output을 다시 출력한다.
		m_edit_nStudyCnt = m_nMeasCount;
	}

	m_nStudyCount = m_edit_nStudyCnt;


	// 수정된 Tol 에 대한 'type1 gage study' 결과를 출력 
	DoGageStudy(m_nCombo_CurrType);
	
	UpdateData(FALSE);
*/
	
}

void CGageDialog::OnButtonDoStudy() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	//-----------------------
	// m_edit_nStudyCnt 처리 
	//  : 숫자가 여러개 들어올 수 있으므로 EN_CHANGE 이벤트가 아니라 OK 버튼 클릭시에 값을 확인한다.

	// edit StudyCount값이 range over로 잘못 입력된 경우
	if ( m_edit_nStudyCnt < MIN_MEAS_COUNT  || m_edit_nStudyCnt > MAX_MEAS_COUNT)
	{
		CString strTemp;
		strTemp.Format("GageStudy Count=%d. Range(%d<= Count <=%d) Out Error!\nRecover GageStudyCount to default %d", 
						m_edit_nStudyCnt, MIN_MEAS_COUNT, MAX_MEAS_COUNT, m_nMeasCount);
		ERR.Set(RANGE_OVER, strTemp); 
		ErrMsg();  ERR.Reset(); 

		// 잘못 입력된 edit StudyCount를 default 값인 MeasCount값으로 다시 원상복구한다.
		// 리턴하지는 않고 원상복구된 기준으로 그래프와 Output을 다시 출력한다.
		m_edit_nStudyCnt = m_nMeasCount;
	}
	m_nStudyCount = m_edit_nStudyCnt;

	m_nRef = m_edit_nRefInput;
	m_nTol = m_edit_nTolInput;
	

	DoGageStudy(m_nCombo_CurrType);
	UpdateData(FALSE);
}

void CGageDialog::OnButtonSaveFile() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	SaveTypeFile(m_nCombo_CurrType);
	UpdateData(FALSE);
}
	

void CGageDialog::SaveTypeFile(int type, BOOL bDelete) 
{
	// 엑셀파일이 있는 경로 지정
	char chThisPath[256];
	CString szFileName ;
	GetCurrentDirectory(256, chThisPath);
	szFileName.Format(_T("%s\\Data\\Type1GageStudy_%s.xlsx"), 
						chThisPath, g_MeasInfoTable[type].strMeas );

	// File 위치를 UI에 표시해 준다.
	//m_editSavedPath.Format("%s\\Data\\", chThisPath);	
	m_editSavedPath = szFileName; 		// 1 개 file 저장시에는 파일이름까지 다 표시


	//-----------------------------------------------------
	// 기존 파일을 지우는 옵션이 켜져 있으면 파일을 지운다.
	if (bDelete == DELETE_YES)
	{
		if (FileExists(szFileName))	// 파일이 존재한다면
		{
			if (DeleteFile(szFileName) == TRUE)
				MyTrace(PRT_BASIC, "\"%s\" file deleted for saving new file\n", szFileName);
		}
	}	


	//-----------------------------
	// Worksheet 생성
	int ret;
	CXLEzAutomation XL(FALSE); 				// FALSE: 처리 과정을 화면에 보이지 않는다

	//ret = XL.OpenExcelFile(szFileName);	// overwrite하기 위해 기존파일 open은 막았음. 
											// 같은 이름의 파일이 있으면 새 파일로 대체해야 한다.

	//------------------------------
	// Worksheet에 그림파일 Insert
	CString strChartPath;
	strChartPath.Format("%s\\Data\\ChartView_%s.png", 
						chThisPath, g_MeasInfoTable[type].strMeas);
	ret = XL.InsertPictureFromFile(strChartPath, 15, 3);	// Column, Row  : 
				// <- 파일이 insert는 되는데 정해진 위치에는 아니고 항상 왼쪽 상단으로 고정임.
				

	//------------------------------
	// WorkSheet에  mohm 값 출력
	ret = XL.SetCellValue(1, 1,  g_MeasInfoTable[type].strMeas);

	CString	strTemp;
	for (int meas =0; meas < m_nStudyCount; meas++)		// StudyCount가 바뀌면 파일에도 그만큼만 보여야 한다.
	{
		strTemp.Format("%.2f",  m_daMeasData[type][meas]);		
		ret = XL.SetCellValue(1, 2+ meas, strTemp);					// Column, Row	
	}


	//----------------------------------------------
	// WorkSheet에  Output Fixed Column 제목 출력
	ret = XL.SetCellValue(3, 18,  "Type1 Gage Study - Observation");
	ret = XL.SetCellValue(3, 20,  "Reference");
	ret = XL.SetCellValue(3, 21,  "Mean");
	ret = XL.SetCellValue(3, 22,  "StDev");
	ret = XL.SetCellValue(3, 23,  "6 x StDev (SV)");
	ret = XL.SetCellValue(3, 24,  "Tolerance (TOL)");

	ret = XL.SetCellValue(7, 20,  "Bias");
	ret = XL.SetCellValue(7, 21,  "T");
	ret = XL.SetCellValue(7, 22,  "PValue\n(Test Bias = 0)");

	ret = XL.SetCellValue(10, 20,  "Cg");
	ret = XL.SetCellValue(10, 21,  "Cgk");

	ret = XL.SetCellValue(10, 23,  "%Var (Repeatablility)");
	ret = XL.SetCellValue(10, 24,  "%Var (Repeatability and Bias");

	//-----------------------------------------------------
	// WorkSheet에  Output data 출력  (UI와 동일해야 한다)
	
	// Stat Grid
	strTemp.Format("%d", m_nRef);
	ret = XL.SetCellValue(5, 20,  strTemp);

	strTemp.Format("%.2f", m_dAvg);
	ret = XL.SetCellValue(5, 21,  strTemp);

	strTemp.Format("%.3f", m_dStDev);
	ret = XL.SetCellValue(5, 22,  strTemp);

	strTemp.Format("%.3f", m_d6StDev);
	ret = XL.SetCellValue(5, 23,  strTemp);

	strTemp.Format("%d", m_nTol);
	ret = XL.SetCellValue(5, 24,  strTemp);

	// Bias Grid
	strTemp.Format("%.2f", m_dBias);	
	ret = XL.SetCellValue(8, 20,  strTemp);

	strTemp.Format("%.3f", m_dT);
	ret = XL.SetCellValue(8, 21,  strTemp);

	strTemp.Format("%.3f", m_dPValue); 
	ret = XL.SetCellValue(8, 22,  strTemp);
	
	// Capability Grid
	strTemp.Format("%.2f", m_dCg);
	ret = XL.SetCellValue(11, 20,  strTemp);

	strTemp.Format("%.2f", m_dCgk);
	ret = XL.SetCellValue(11, 21,  strTemp);


	// Repeatability Grid
	strTemp.Format("%.2f %%", m_dVarRept);
	ret = XL.SetCellValue(13, 23,  strTemp);

	strTemp.Format("%.2f %%", m_dVarReptBias);
	ret = XL.SetCellValue(13, 24,  strTemp);

	m_listMsg.GetText(0, strTemp);
	ret = XL.SetCellValue(10, 25,  strTemp);


	//  엑셀 파일 저장 
	ret = XL.SaveFileAs(szFileName);			


	// 엑셀 파일 해제
	ret = XL.ReleaseExcel();					

	
}

void CGageDialog::OnButtonViewFile() 
{
	// TODO: Add your control notification handler code here
	
	// 엑셀파일이 있는 경로 지정
	char chThisPath[256];
	CString szFileName ;
	GetCurrentDirectory(256, chThisPath);
	szFileName.Format(_T("%s\\Data\\Type1GageStudy_%s.xlsx"), 
						chThisPath, g_MeasInfoTable[m_nCombo_CurrType].strMeas );

	if(!FileExists(szFileName)) // 파일이 존재하지 않으면 에러 출력
	{ 	
		ERR.Set(FLAG_FILE_NOT_FOUND, szFileName); 
		ErrMsg();  ERR.Reset(); return; 
	}

	::ShellExecute(NULL, "open", "EXCEl.EXE", szFileName, "NULL", SW_SHOWNORMAL);	
}


void CGageDialog::OnButtonAutoGageSave() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	// 모든 저항 타입에 대해 자동으로 Gage Study를 수행하고 결과를 파일로 저장한다.
	for (int type = 0; type < (MAX_MEAS_TYPE -1); type++)
	{
		// Current Type 변경
		ChangeCurrType(type);

		// type에 맞는 GageStudy를 수행
		DoGageStudy(type);

		// type에 맞는 엑셀 파일을 출력  (기존 엑셀 파일이 존재하면 지운다)
		SaveTypeFile(type, DELETE_YES);
	}

	// File 위치를 UI에 표시해 준다.  여러개 file 저장시에는 폴더까지만 표시 (file 이름은 뺀다)
	char chThisPath[256];
	GetCurrentDirectory(256, chThisPath);
	m_editSavedPath.Format("%s\\Data\\", chThisPath);	

	UpdateData(FALSE);


	// 수행 종료 표시 및 수행위치 알림을 위한 메시지를 띄운다.
	CString strTemp;
	strTemp.Format("Auto Gage Study for all mohm Type finished.\nCheck the result files here.\n\n\"%s\"\n", m_editSavedPath);
	int ret = AfxMessageBox(strTemp, MB_OKCANCEL|MB_ICONINFORMATION);
	MyTrace(PRT_BASIC, strTemp);
						

	// CurrentType을  mohm_1로 초기화하고 작업을 완료
	ChangeCurrType(mohm_1); 
	DoGageStudy(m_nCombo_CurrType);
	
	UpdateData(FALSE);
}

void CGageDialog::ChangeCurrType(int type, BOOL	bSetGridBlue) 
{
	// type 변경
	m_nCombo_CurrType = type;			// 0
	m_comboMeasType.SetCurSel(type);	// 0 = mohm_1

	// type에 맞게 Ref, RefOutput을 변경
	m_nRef = g_MeasInfoTable[type].nMeasRef;
	m_edit_nRefInput = m_nRef;	

	// type과 Ref에 맞게 Tol을 변경
	if (type >= mohm_1 && type <= mohm_10)	// 10mohm 이하 Tol은 무조건 1 
		m_nTol = 1; 						// 오차율은 mohm_1은 10% ~ mohm_10은 1%까지 변동

	// 20mohm 이상은 오차율을 1%로 =>  Tol값이 Ref *0.1 이어야 한다. 
	// Tol은 다시 *0.1해서 사용하므로 오차율 1%가 됨
	else
		m_nTol =  (int)(m_nRef * 0.1);
	m_edit_nTolInput = m_nTol;


	// 현재 선택된 type column의 배경을 푸른색으로 설정.
	// 블루로 선택할 data가 있어야 하므로 SetGridBkBlue()는 Display_mohmGridData() 다음에 호출해야 함.
	if (bSetGridBlue == TRUE)
		SetGridBkBlue(type);	

}


// 엔터키, ESC키 메시지를 후킹해서 엔터키나 ESC 키가 눌려도 다이얼로그가 종료되지 않도록 한다.
BOOL CGageDialog::PreTranslateMessage(MSG* pMsg) 
{
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


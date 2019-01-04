#if !defined(AFX_GAGEDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_)
#define AFX_GAGEDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GageDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGageDialog dialog

#include "GridCtrl.h"
#include "Globals.h"
#include "Error.h"
#include "ChartViewer.h"


#define 	MIN_MEAS_COUNT		10
#define 	MAX_MEAS_COUNT		90

enum	MEAS_TYPE	{ 	mohm_1 = 0,			// 0	
						mohm_2 = 1,
						mohm_3 = 2,
						mohm_4 = 3,
						mohm_5 = 4,

						mohm_10 = 5,		// 5
						mohm_20 = 6,
						mohm_30 = 7,
						mohm_40 = 8,
						mohm_50 = 9,

						mohm_100  = 10,		// 10
						mohm_200  = 11,
						mohm_500  = 12,
						mohm_750  = 13,
						mohm_1000 = 14,

						mohm_2000  = 15,	// 15
						mohm_3000  = 16,
						mohm_5000  = 17,
						mohm_7500  = 18,
						mohm_10000 = 19,

						mohm_20000  = 20,	// 20
						mohm_30000  = 21,
						mohm_50000  = 22,
						mohm_75000  = 23,
						mohm_999999 = 24,	

						MAX_MEAS_TYPE = 25	// 25
};


typedef struct {
	int	 		nMeasYype;
	const char* strMeas;
	int			nMeasRef;
} sMeasInfo;

static sMeasInfo	g_MeasInfoTable[] = {
	{	mohm_1        ,  "1mohm"     ,  1     }, // 0
	{	mohm_2        ,  "2mohm"     ,  2     },
	{	mohm_3        ,  "3mohm"     ,  3     },
	{	mohm_4        ,  "4mohm"     ,  4     },
	{	mohm_5        ,  "5mohm"     ,  5     },	

	{	mohm_10       ,  "10mohm"    ,  10    },	// 5
	{	mohm_20       ,  "20mohm"    ,  20    },
	{	mohm_30       ,  "30mohm"    ,  30    },
	{	mohm_40       ,  "40mohm"    ,  40    },
	{	mohm_50       ,  "50mohm"    ,  50    },	

	{	mohm_100      ,  "100mohm"   ,  100   },	// 10
	{	mohm_200      ,  "200mohm"   ,  200   },
	{	mohm_500      ,  "500mohm"   ,  500   },
	{	mohm_750      ,  "750mohm"   ,  750   },
	{	mohm_1000     ,  "1000mohm"  ,  1000  },	

	{	mohm_2000     ,  "2000mohm"  ,  2000  },	// 15
	{	mohm_3000     ,  "3000mohm"  ,  3000  },
	{	mohm_5000     ,  "5000mohm"  ,  5000  },
	{	mohm_7500     ,  "7500mohm"  ,  7500  },
	{	mohm_10000    ,  "10000mohm" ,  10000 },	

	{	mohm_20000    ,  "20000mohm" ,  20000 },	// 20
	{	mohm_30000    ,  "30000mohm" ,  30000 },
	{	mohm_50000    ,  "50000mohm" ,  50000 },
	{	mohm_75000    ,  "75000mohm" ,  75000 },
	{	MAX_MEAS_TYPE ,  "999999mohm",  999999 }	
};

#define		VAR_REPT_LIMIT_MAJ		10	
#define		VAR_REPT_LIMIT_CRI		15	



enum GRID_STAT_ENUM {	STAT_REF     = 0,
						STAT_MEAN    = 1,
						STAT_STDEV   = 2,
						STAT_6xSTDEV = 3,	
						STAT_TOL     = 4, 
						MAX_STAT_ENUM 
};

enum GRID_BIAS_ENUM {	BIAS_BIAS    = 0,
						BIAS_T    	 = 1,
						BIAS_PVALUE  = 2,
						MAX_BIAS_ENUM
};

enum GRID_CAPABILITY_ENUM {	
						CAPABILITY_CG            = 0,
						CAPABILITY_CGK    	     = 1,
						//CAPABILITY_VAR_REPT      = 2,
						//CAPABILITY_VAR_REPT_BIAS = 3,
						MAX_CAPABILITY_ENUM
};
enum GRID_REPT_ENUM {	REPT_VAR      = 0,
						REPT_VAR_BIAS = 1,
						MAX_REPT_ENUM
};

enum DELETE_ENUM { 	DELETE_NO  = FALSE, 
					DELETE_YES = TRUE  
};

class CGageDialog : public CDialog
{
// Construction
public:
	CGageDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGageDialog)
	enum { IDD = IDD_GAGE_DIALOG };
	CListBox		m_listMsg;
	CComboBox		m_comboMeasType;		// for mohm type combo box, Input
	CGridCtrl		m_gridCtrl;				// for Grid  (Input data list per type)
	CGridCtrl		m_gridGageStat;			// for Grid  (Output Gage Stat data )
	CGridCtrl		m_gridBias;				// for Grid  (Output Gage Bias data)
	CGridCtrl		m_gridCapability;		// for Grid  (Output Gage Capability data)
	CGridCtrl		m_gridRept;				// for Grid  (Output Gage Repeatability data)
	CString			m_editMeasDataPath;		// for 4w Data file path
	CString			m_editSavedPath;
	int				m_edit_nMeasCnt;
	int				m_edit_nRefInput;		// Input
	int				m_edit_nTolInput;		// Input
	int				m_edit_nStudyCnt;		// Input
	CChartViewer	m_ChartViewer;			// for Chart,	 선택된 Lot, Date의 그래프 출력
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGageDialog)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGageDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeComboMeasType();
	afx_msg void OnChangeEditTolInput();
	afx_msg void OnButtonLoadMeasdata();
	afx_msg void OnChangeEditStudyCnt();
	afx_msg void OnButtonDoStudy();
	afx_msg void OnButtonSaveFile();
	afx_msg void OnButtonViewFile();
	afx_msg void OnButtonAutoGageSave();
	afx_msg void OnChangeEditRefInput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:

	// member for 4w Measure Data
	int			m_nTypeCount;
	int			m_nMeasCount;		// Load한 file의 meas data 갯수
	int			m_nStudyCount;		// 실제 Gage Study 하라고 입력받은 값. 
									// m_nMeasCount와 보통 같지만 작게 입력할 수 있다.
	double		m_daMeasData[MAX_MEAS_TYPE][MAX_MEAS_COUNT];
	//vector	<double>	m_vdaMeasData[MAX_MEAS_TYPE];
	
	int			m_nCombo_CurrType;	// 현재 combo에서 선택된 type#. enum값으로 access 가능함 (ex: mohm_1, mohm_2..)

	// member for Output Grid Val
	int			m_nRef;				
	int			m_nTol;				
	double		m_dAvg;				
	double		m_d6StDev;			
	double		m_dStDev;			
	double		m_dBias;			
	double		m_dT;				
	double		m_dPValue;			
	double		m_dCg;				
	double		m_dCgk;				
	double		m_dVarRept;			
	double		m_dVarReptBias;		

	// member for grid control
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
	BOOL	m_bEditable;

	int		m_nFixCols_Out;
	int		m_nFixRows_Out;
	int		m_nCols_Out;
	int		m_nRows_Out;
/*
	BOOL	m_bHorzLines;
	BOOL	m_bListMode;
	BOOL	m_bVertLines;
	BOOL	m_bSelectable;
	BOOL	m_bAllowColumnResize;
	BOOL	m_bAllowRowResize;
	BOOL	m_bHeaderSort;
	BOOL	m_bReadOnly;
	BOOL	m_bItalics;
	BOOL	m_btitleTips;
	BOOL	m_bSingleSelMode;	
*/
	BOOL 	InitMember();
	BOOL 	InitView();
	void 	Display_mohmGridHeader();
	void 	Display_mohmGridData();
	void 	SetGridBkBlue(int type);
	void 	ClearGrid_BackGround();
	void 	ClearGrid_Data();
	void 	Display_OutputGridFixCol();
	void 	ClearGrid_Output();

	int 	Load_4W_MeasData(CString dataFilePath);
	int 	getMeasDataLoc(char *pStr, int strMax, int &rLoc);
	int 	getMeasData(char *pStr, int strMax, double &rdMeasData);

	void 	DoGageStudy(int type); 
	void 	DisplayGageStudyChart(int type); 
	void 	CalcGageStudyOutput(int type); 
	void 	DisplayGageStudyOutput(int type); 
	void 	SaveTypeFile(int type, BOOL bDelete = DELETE_NO);
	void	ChangeCurrType(int type, BOOL bSetGridBlue = TRUE); 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAGEDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_)

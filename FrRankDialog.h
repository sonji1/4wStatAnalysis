#if !defined(AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_)
#define AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrRankDialog.h : header file
//


#include "GridCtrl.h"			// Fault Grid를 위해 include
#include "StatisticsDialog.h" 	// g_sLotNetDate_Info, g_pvsaNetData 정보 사용을 위해 추가


//---------------------
// Define & enum
//---------------------

// for FAULT Grid  -----

#define MAX_FR_GRID_ROW		(MAX_NET_PER_LOT + 1)	// 헤더까지 1추가


// enum for FAULT RANK GRID Column Location 
enum FR_COL_TYPE  { FR_COL_NO, 			// 0
					FR_COL_LOT, 		// 1
					FR_COL_DATE, 		// 2
					FR_COL_NET,			// 3
					FR_COL_TOTAL,		// 4
					FR_COL_NGCOUNT,		// 5
					FR_COL_COUNT,		// 6
					FR_COL_AVG,		    // 7
					FR_COL_SIGMA,		// 8
					FR_COL_MIN,			// 9
					FR_COL_MAX,			// 10 
					FR_COL_FAULT,		// 11 
					FR_COL_FR,			// 12 

					NUM_FR_GRID_COL     // (12 +1) : No까지 1추가
}; 


//---------------------
// Fault Rank Data 
//---------------------


class FaultRankData
{
	public:

	// for  FR Rank Calc 
	short  	wNet;
	short  	wCount;
	double	dAvg;
	double	dSigma;
	double	dMin;
	double	dMax;
	double 	dFR; 			// FR(불량율) = Fault / Count 
							//            = Fault / (Total - NG)
							
	FaultRankData()		// 생성자
	{
		wNet = 0;
		wCount = 0;
		dAvg = 0.0;
		dSigma = 0.0;
		dMin = 0.0;
		dMax = 0.0;
		dFR = 0.0;
	}
};




/////////////////////////////////////////////////////////////////////////////
// CFrRankDialog dialog

class CFrRankDialog : public CDialog
{
// Construction
public:
	CFrRankDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFrRankDialog)
	enum { IDD = IDD_FR_RANK_DIALOG };
	CComboBox	m_comboFrLot;
	CComboBox	m_comboFrDate;
	int			m_nNetCount;			// for editBox,  현재 Lot, Date의 Net 갯수
	int			m_nFaultNetCount;		// for editBox,  선택된 Lot, Date의 fault 발생 Net 갯수
	int			m_nFaultTotal;			// for editBox,  Total Fault for Lot_Date
	CGridCtrl	m_gridFault;			// for grid,	 선택된 Lot, Date의 Fault Data를 표에 출력
	BOOL		m_bFaultListFrRank;		// for checkBox, On이면 'Fault List' grid를 FR 항목기준으로 내림차순 정렬
	BOOL		m_bFaultListFaultOnly;	// for checkBox, On이면 'Fault List' grid에서 Fault인 Net만 골라서 출력
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrRankDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFrRankDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnStatLoad4wData(WPARAM wParam, LPARAM lParam);	// 사용자메시지처리기 리턴값은 void 안됨
	afx_msg void OnSelchangeComboFrLot();
	afx_msg void OnSelchangeComboFrDate();
	afx_msg void OnCheckSortFaultRate();
	afx_msg void OnCheckFrFaultOnly();
	afx_msg void OnButtonSaveFrListCsv();
	afx_msg void OnButtonViewFrListCsv();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	//-------------------
	// Member Variable 
	int 	m_nCombo_CurrLot;			// m_comboFrLot  컨트롤에서 선택된 lot index
	int 	m_nCombo_CurrDate;			// m_comboFrDate 컨트롤에서 선택된 date index



	// for  FR Rank Calc 
//	short  	m_waCount[MAX_NET_PER_LOT];
//	double 	m_daFR[MAX_NET_PER_LOT]; 	// FR(불량율) = Fault / Count 
										//            = Fault / (Total - NG)
										
	vector <FaultRankData> m_vsFrData;


	// member for grid control
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
	BOOL	m_bEditable;
	//BOOL	m_bListMode;

	
	
	//-------------------
	// Member Function 
	
	BOOL 	InitMember();
	BOOL 	InitView();	
	void 	ClearGrid_Fault();

	void 	ComboLot_UpdateContents();
	void 	ComboDate_UpdateContents(int nLot);

	void 	DisplayFrRank();
	void 	CalcFrRank(int nLot, int nDate);
	void 	DisplayGridFault(int nLot, int nDate);
	void 	CalcFrRank_Old(int nLot, int nDate);
	void 	DisplayGridFault_Old(int nLot, int nDate);
	void 	DisplayGrid_FaultTuple(int nRow, int nLot, int nDate, int nNet, 
					int nTotal, int nNgCount, int nCount, 
					double dAvg, double dSigma, double dMin, double dMax,
					int nFault, double dFR );


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_)

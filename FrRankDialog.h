#if !defined(AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_)
#define AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrRankDialog.h : header file
//


#include "GridCtrl.h"			// Fault Grid�� ���� include
#include "StatisticsDialog.h" 	// g_sLotNetDate_Info, g_pvsaNetData ���� ����� ���� �߰�


//---------------------
// Define & enum
//---------------------

// for FAULT Grid  -----

#define MAX_FR_GRID_ROW		(MAX_NET_PER_LOT + 1)	// ������� 1�߰�


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

					NUM_FR_GRID_COL     // (12 +1) : No���� 1�߰�
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
	double 	dFR; 			// FR(�ҷ���) = Fault / Count 
							//            = Fault / (Total - NG)
							
	FaultRankData()		// ������
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
	int			m_nNetCount;			// for editBox,  ���� Lot, Date�� Net ����
	int			m_nFaultNetCount;		// for editBox,  ���õ� Lot, Date�� fault �߻� Net ����
	int			m_nFaultTotal;			// for editBox,  Total Fault for Lot_Date
	CGridCtrl	m_gridFault;			// for grid,	 ���õ� Lot, Date�� Fault Data�� ǥ�� ���
	BOOL		m_bFaultListFrRank;		// for checkBox, On�̸� 'Fault List' grid�� FR �׸�������� �������� ����
	BOOL		m_bFaultListFaultOnly;	// for checkBox, On�̸� 'Fault List' grid���� Fault�� Net�� ��� ���
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
	afx_msg LRESULT OnStatLoad4wData(WPARAM wParam, LPARAM lParam);	// ����ڸ޽���ó���� ���ϰ��� void �ȵ�
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
	int 	m_nCombo_CurrLot;			// m_comboFrLot  ��Ʈ�ѿ��� ���õ� lot index
	int 	m_nCombo_CurrDate;			// m_comboFrDate ��Ʈ�ѿ��� ���õ� date index



	// for  FR Rank Calc 
//	short  	m_waCount[MAX_NET_PER_LOT];
//	double 	m_daFR[MAX_NET_PER_LOT]; 	// FR(�ҷ���) = Fault / Count 
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

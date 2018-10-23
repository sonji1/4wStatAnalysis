#if !defined(AFX_PCHARTDIALOG_H__C54425B9_8EBF_41DD_AC28_74E2BC28CBBD__INCLUDED_)
#define AFX_PCHARTDIALOG_H__C54425B9_8EBF_41DD_AC28_74E2BC28CBBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PChartDialog.h : header file
//

#include "ChartViewer.h"		// p-Chart�� ���� include
#include "GridCtrl.h"			// OOC Grid�� ���� include
#include "StatisticsDialog.h" 	// g_sLotNetDate_Info, g_pvsaNetData ���� ����� ���� �߰�



//---------------------
// OOC Grid 
#define MAX_OOC_GRID_ROW		(MAX_NET_PER_LOT + 1)	// ������� 1�߰�

// enum for OOC GRID Column Location 
enum OOC_COL_TYPE  { OOC_COL_NO, 			// 0
					OOC_COL_LOT, 			// 1
					OOC_COL_DATE, 			// 2
					OOC_COL_NET,			// 3
					OOC_COL_TOTAL,			// 4
					OOC_COL_NGCOUNT,		// 5
					OOC_COL_COUNT,			// 6
					OOC_COL_FAULT,			// 7
					OOC_COL_NORMAL,			// 8
					OOC_COL_YR,				// 9
					OOC_COL_CENTER,			// 10
					OOC_COL_LCL,			// 11
					OOC_COL_OOC,			// 12
					NUM_OOC_GRID_COL		// (12 + 1) : No���� 1�߰�
}; 

//enum OOCLIST_TYPE  { ALL_NET, OOC_NET}; // enum for m_nOocListMode


/////////////////////////////////////////////////////////////////////////////
// CPChartDialog dialog

class CPChartDialog : public CDialog
{
// Construction
public:
	CPChartDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPChartDialog)
	enum { IDD = IDD_PCHART_DIALOG };
	CComboBox		m_comboLot;			// for comboBox, Lot ����
	CComboBox		m_comboDate;		// for comboBox, Date ���� 
	CChartViewer	m_ChartViewer;		// for Chart,	 ���õ� Lot, Date�� �׷��� ���
	CGridCtrl		m_gridOOC;			// for grid,	 ���õ� Lot, Date�� Data�� ǥ�� ����ϴ� 'OOC List'
	int				m_nNetCount;		// for editBox,  ���� Lot, Date�� Net ����
	int				m_nOocNetCount; 	// for editBox,  ���õ�  Lot, Date�� OOC �߻� Net ����
	int				m_nFaultNetCount;	// for editBox,  ���õ�  Lot, Date�� fault �߻� Net ����

	BOOL			m_bOocListOocOnly;	// for checkBox, On�̸� 'OOC List' grid���� OOC�� Net�� ��� ���
	BOOL			m_bOocListFaultOnly;// for checkBox, On�̸� 'OOC List' grid���� Fault�� Net�� ��� ���
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPChartDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPChartDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnStatLoad4wData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelchangeComboLot();
	afx_msg void OnSelchangeComboDate();
	afx_msg void OnButtonDisPchart();
	afx_msg void OnButtonSavePchart();
	afx_msg void OnButtonViewPchartCsv();
	afx_msg void OnCheckOocnetOnly();
	afx_msg void OnCheckFaultOnly();
	afx_msg void OnGridSelChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	//-------------------
	// Member Variable 
	int 	m_nCombo_CurrLot;			// m_comboLot  ��Ʈ�ѿ��� ���õ� lot index
	int 	m_nCombo_CurrDate;			// m_comboDate ��Ʈ�ѿ��� ���õ� date index


	// for  PChart Calc 
	short  	m_waNormal[MAX_NET_PER_LOT];
	short  	m_waCount[MAX_NET_PER_LOT];
	double 	m_daLCL[MAX_NET_PER_LOT]; 	// Center - 3 * SQRT(Center* (1-Center) / Count)
	double 	m_daYR[MAX_NET_PER_LOT]; 	// YR(����) = Normal / Count 
										//          = (Count-Fault) / Count 
										//          = (Total- NG -Fault) / (Total - NG)
	bool 	m_baOOC[MAX_NET_PER_LOT];
									 	
	double	m_dMaxYR;
	double	m_dMinYR;

	int 	m_nNetSum_Normal;
	int		m_nNetSum_Count; 
	double 	m_dCenter;


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
	void 	ComboLot_UpdateContents();
	void 	ComboDate_UpdateContents(int nLot);
	void 	DisplayPchart_ALL();
	void 	CalcPChart(int nLot, int nDate);
	void 	DisplayPChart(int nLot, int nDate, int nTrackNet = -1);
	//void 	trackLineLegend(XYChart *c, int xValue);
	void 	trackLineLabel(XYChart *c, int xValue);
	void 	DisplayGrid_OOC(int nLot, int nDate, int nTrackNet = -1,  BOOL bScrollBarInit = TRUE);
	void 	DisplayGrid_OOCTuple(int nRow, int nLot, int nDate, int nNet, 
					int nTotal, int nNgCount, int nCount, int nFault, int nNormal, 
					double dYR, double dCenter, double dLCL, bool bOOC);

	void 	ClearGrid_OOC();
	void 	SavePChart_CsvFile(int nLot, int nDate);


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PCHARTDIALOG_H__C54425B9_8EBF_41DD_AC28_74E2BC28CBBD__INCLUDED_)

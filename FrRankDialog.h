#if !defined(AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_)
#define AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrRankDialog.h : header file
//


#include "GridCtrl.h"			// Fault Grid를 위해 include
#include "StatisticsDialog.h" 	// g_sLotNetDate_Info, g_pvsaNetData 정보 사용을 위해 추가


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
	afx_msg void OnSelchangeComboFrLot();
	afx_msg void OnSelchangeComboFrDate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	//-------------------
	// Member Variable 
	int 	m_nCombo_CurrLot;			// m_comboFrLot  컨트롤에서 선택된 lot index
	int 	m_nCombo_CurrDate;			// m_comboFrDate 컨트롤에서 선택된 date index
	
	
	//-------------------
	// Member Function 
	
	BOOL 	InitMember();
	BOOL 	InitView();	
	void 	ComboLot_UpdateContents();
	void 	ComboDate_UpdateContents(int nLot);

	void 	DisplayFrRank();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRRANKDIALOG_H__693040B6_4087_4E39_8699_0F6AD269F284__INCLUDED_)

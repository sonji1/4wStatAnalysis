#if !defined(AFX_CONFIGDIALOG_H__3DE95E71_32E5_4F38_B70B_2B62B3A9707F__INCLUDED_)
#define AFX_CONFIGDIALOG_H__3DE95E71_32E5_4F38_B70B_2B62B3A9707F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

#include "Globals.h"

class CConfigDialog : public CDialog
{
// Construction
public:
	void InitView();
	void InitMember();
	CConfigDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG_DIALOG };
	BOOL	m_bPrtBasic;
	BOOL	m_bPrtLevel1;
	BOOL	m_bPrtLevel2;
	BOOL	m_bPrtLevel3;
	BOOL	m_bPrtDevice;
	BOOL	m_bPrtDevD64;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialog)
	afx_msg void OnCheckPrtBasic();
	afx_msg void OnCheckPrtLevel1();
	afx_msg void OnCHECKPRTLevel2();
	afx_msg void OnCheckPrtLevel3();
	afx_msg void OnCheckPrtDevice();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCheckPrtDevD64();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__3DE95E71_32E5_4F38_B70B_2B62B3A9707F__INCLUDED_)

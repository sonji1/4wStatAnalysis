// Error.cpp: implementation of the CError class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "ACE400Cal.h"
#include "Error.h"
#include "Globals.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CError ERR;


CError::CError()
{

}

CError::~CError()
{

}

void CError::Set(int error_no, CString strState)
{	
	m_error_no = error_no;

	m_strState = strState;

#ifndef __PRT_OFF__
	// optional print (to be deleted..?)
	
	//TRACE("ERR.Set: err=%d %s\n", error_no, m_strState);
	//TRACE("Err.Set: err=%d\n", error_no);
#endif

	switch(error_no)
	{
	case NO_ERR:
		break;

	default:
		break;
	}

}

void CError::Reset()
{ 
	m_error_no = NO_ERR;
}
	
BOOL CError::GetMsg(int error_no, CString &title, CString &state, CString &sol)
{
	//TRACE("ERR.GetMsg(err=%d)", error_no);		// to be deleted..

	for (int i=0; i<ERRMSG_TABLE_LEN; i++) {
		if (ErrMsgTable[i].error_no == error_no) {
			title = ErrMsgTable[i].title;
			state = ErrMsgTable[i].state;
			sol = ErrMsgTable[i].sol;
			return TRUE;
		}
	}
    return FALSE;
}


// 호출방법1:  ErrMsg() 			:  MB_OK로 메시지박스생성 &  TRACE 출력, 호출이후 반드시 ERR.Reset() 수행
// 호출방법2:  ErrMsg(MB_OK, FALSE) :  MB_OK로 메시지박스만 생성, 호출이후 반드시 ERR.Reset() 수행
// 호출방법3:  ErrMsg(-1, TRUE)		:  TRACE만 출력, 호출 이후 ERR.Reset() 수행하지 않고 다음단계 return후에 맡김. 
//                                      ErrMsg(-1)로만 써도 되고, Errmsg(-1..) 이후에는 return후에 ErrMsg(MB_OK, FALSE)로 
//                                      메시지 박스만 호출하는 방식으로 응용이 가능함.
//                                      사실상 이 방식 대신 그냥 MyTrace(PRT_BASIC, ..) 를 호출해도 됨.

// MBoxType: 	MB_OK				(0)
// 				MB_OKCANCEL 		(1)
// 				MB_ABORTRETRYIGNORE	(2)
// 				MB_YESNOCANCEL		(3)
// 				MB_YESNO			(4)
// 				MB_RETRYCANCEL		(5)

int ErrMsg(int nMboxType, BOOL bTraceOn)
{
//	CDlgErrMsg DLG;
//	DLG.ntype = nMboxType;
//	DLG.DoModla();
//	int ret = DLG.ret;

//	if(ERR.Get() != NORMAL)	DISPMSG(strMsg);
//	if(ERR.Get() != NORMAL)	ERRMSGDLG.DoModal();

	//if(ERR.Get() == NO_ERR)	
	//	return 0;
	

//	const char* title; const char* state; const char* sol;
	CString title=""; CString state=""; CString sol="";
	ERR.GetMsg(ERR.Get(), title, state, sol);
//	state = state + "\n\n          " + ERR.m_strState;
	if(state == "") 
		state = ERR.m_strState;

	CString strMsg="", strTemp="";
	strMsg.Format("%s error. [ No. %d ]", title, ERR.Get());

	if(state != "") strTemp.Format("\n\nState : %s", state);
	strMsg += strTemp;
	if(sol != "") strTemp.Format("\n\nSolution : %s", sol), strMsg += strTemp;


	int ret = 0;	

	if (ERR.Get() != NO_ERR)	// Error인 경우에만 MsgBox 출력한다.
	{
		// Error 인 경우, MBoxType이 지정된 경우에는  MsgBox를 출력한다. 		
		// MBoxType을 -1로 지정하면 MsgBox를 출력하지 않는다. 
		if (nMboxType != -1)	
			ret = AfxMessageBox(strMsg, nMboxType | MB_ICONERROR); // modal
	}

	// bTraceOn == FALSE 이면 TRACE 출력을 하지 않는다. 
	// ErrMsg()에서 Trace 출력을 수행하면 PC에서 return값 조작이 어려운 경우.  (for debugging)
	// ErrMsg(nMboxType==MB_OK, bTraceOn==FALSE)로 호출해야 한다.
	if (bTraceOn == TRUE)
	{
		MyTrace(PRT_BASIC, "%s\n", strMsg);	// 출력이 512바이트를 넘을 수 있으므로  MyTrace() 사용
	}


// MB_ABORTRETRYIGNORE MB_OK MB_OKCANCEL MB_RETRYCANCEL MB_YESNO MB_YESNOCANCEL
// MB_ICONHAND MB_ICONSTOP MB_ICONERROR / MB_ICONQUESTION / MB_ICONEXCLAMATION MB_ICONWARNING / MB_ICONASTERISK MB_ICONINFORMATION
// IDABORT IDCANCEL IDIGNORE IDNO IDOK IDRETRY IDYES

	//ERR.Reset();

	return ret;
}

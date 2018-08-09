// Error.h: interface for the CError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__8D3D34FA_3024_4D5F_81B6_D01841D85268__INCLUDED_)
#define AFX_ERROR_H__8D3D34FA_3024_4D5F_81B6_D01841D85268__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



enum ErrID {
	// 정상
	NO_ERR=0,
	TEST_ERR=1,
	TEST_TRACE=2,

	// E-STOP 관련
	ESTOP=10,


	// 에러플래그
	FLAG=500,
		FLAG_FILE_NOT_FOUND,
		FLAG_FILE_CANNOT_OPEN,
		FLAG_FILE_UNPROPER_PARAMETER,
		FLAG_FILE_CHECK_SUM,

	MEM_ALLOC_FAIL,


	// 반환값 이상
	INVALID_RETURN=600,

	// Input Error 
	INVALID_INPUT=601,

	// 사용자 유발 에러 
	USER_ERR=650,

	// 제한값 초과
	RANGE_OVER=700,

	SW_LOGIC_ERR = 750,
		SW_NO_INIT_VAR,
		MAN_ERR_TEST_CONDITION,

	// 제한시간 초과
	OVERTIME=800,
		//OT_SETNCHK,

	// 디바이스 공통
	DEVICE=1000,

};

typedef struct {
    int error_no;
    const char* title;
    const char* state;
    const char* sol;
} sErrMsg;

static sErrMsg ErrMsgTable[] = 
{
	{ NO_ERR, "Normal",  "", "" },
	{ TEST_ERR, "Test",  "", "" },
	{ TEST_TRACE, "Test Trace",  "", "" },

	{ ESTOP, "Emergency-Stop",  "", "" },

	{ FLAG, "Flag",  "", "" },
		{ FLAG_FILE_NOT_FOUND, "File Not Found", "", "" },
		{ FLAG_FILE_CANNOT_OPEN, "File Cannot Open", "", "" },
		{ FLAG_FILE_UNPROPER_PARAMETER, "File Unproper Parameter", "", "" },
		{ FLAG_FILE_CHECK_SUM, "File CheckSum Error", "", "" },

	{ MEM_ALLOC_FAIL, "Memory Allocation Failed.", "", "" },


	{ INVALID_RETURN, "Invalid return",  "", "" },

	{ INVALID_INPUT, "Invalid Input Parameter",  "", "" },

	{ USER_ERR, "User Created Error",  "", "" },

	{ RANGE_OVER, "Range Over",  "", "" },

	{ SW_LOGIC_ERR, "SW Logic Error Found", "", "" },
		{ SW_NO_INIT_VAR, "Uninitialized variable used. Check SW Logic", "", "" },
		{ MAN_ERR_TEST_CONDITION, "ManulTest Test Contition Error", "", "" },

	{ OVERTIME, "Over time",  "", "" },


	{ DEVICE, "Device",  "", "" },

};

#define ERRMSG_TABLE_LEN  sizeof(ErrMsgTable)/sizeof(ErrMsgTable[0])


class CError  
{
private:
	int m_error_no;

public:
	CString m_strState;

	void	Set(int errcode, CString strState="");
	int		Get() 
	{ 
		//TRACE("ERR.Get() m_error_no =%d\n", m_error_no);
		return m_error_no; 
	}
	void	Reset();
	BOOL	GetMsg(int error_no, CString &title, CString &state, CString &sol);

public:
	CError();
	virtual ~CError();
};


extern CError ERR;

extern	int			ErrMsg(int nMboxType=MB_OK, BOOL bTraceOn=TRUE);

#endif // !defined(AFX_ERROR_H__8D3D34FA_3024_4D5F_81B6_D01841D85268__INCLUDED_)

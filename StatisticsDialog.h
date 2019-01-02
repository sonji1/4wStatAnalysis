#if !defined(AFX_STATISTICSDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_)
#define AFX_STATISTICSDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatisticsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStatisticsDialog dialog

#include "GridCtrl.h"
#include "Globals.h"
#include "Error.h"
#include "stdafx.h"
#include <time.h>
#include "string.h"
#include <vector>
#include <iostream>
using namespace std;


//---------------------
// Statistics Data 
//---------------------


typedef struct sSTAT_TIME
{
	BYTE	hour;		// 0~23
	BYTE	min;		// 0~59
	BYTE	sec;		// 0~59
} sSTAT_TIME;

#define	NUM_STAT_PIN		4
#define NUM_STAT_SAMPLE		12		// 이걸 늘리게 되면 Load_4wDataFile()-> sscanf의 입력 parameter를 늘려야 한다.
									// 원래는 100까지 가능하다고 함.
class statNetData
{
public:
	//short 		waPin[NUM_STAT_PIN];		// g_sLotNetDate_Info.waLotNetPin[lot][net] 로 이동함. 
	
	sSTAT_TIME		statTime;
	double			dLsl;	
	double			dRefR;	
	double			dUsl;	


#ifdef  DP_SAMPLE 
	// 동적 배열
	int				sampleSize;
	double* 		pdaSample; 		// pdaSample의 현재 갯수는 g_sLotNetDate_Info.naLotSampleCnt[lot]로 확인.

#else
	// 고정 배열
	double 			daSample[NUM_STAT_SAMPLE];
#endif

public:
	// 생성자
	statNetData()
	{
		statTime.hour = 0;
		statTime.min = 0;
		statTime.sec = 0;

		dLsl = 0;
		dRefR = 0;
		dUsl = 0;
#ifdef DP_SAMPLE
		sampleSize = 0;
		pdaSample = NULL;
#else
		::ZeroMemory(daSample, sizeof(daSample));
#endif
	}

#ifdef  DP_SAMPLE 
	// 소멸자
	~statNetData()			// 소멸자에서  pda Sample에 대해 delete[]를 수행해 주자.
	{
		if (pdaSample != NULL)
			delete []pdaSample;
	}
	
	// Deep Copy를 하는 복사 생성자.
	statNetData(const statNetData& s)
	{
		statTime.hour = s.statTime.hour;
		statTime.min  = s.statTime.min;
		statTime.sec  = s.statTime.sec; 

		dLsl  = s.dLsl;
		dRefR = s.dRefR;
		dUsl  = s.dUsl;


		// Deep copy를 위해 복사생성자에서 메모리를 새로 할당해 줘야 한다.
		// Load_4wDataFile()에서는 하나의 netData만 new로 만들어서 최대 MAX_TIME_FILE 갯수만큼 
		// g_pvsaNetData[nLot][nNet][nDate]->push_back(netData)를 호출한다.
		// g_pvsaNetData[nLot][nNet][nDate]->push_back(netData)에서 netData를 value형 인자로
		// 받을 때 새로 메모리 할당이 되도록 아래와 같이 deep copy를 위해 새로 new를 수행해 준다.
		//
		// 그렇지 않으면 같은 메모리(Load_4wDataFile() 내의 netData )를 최대  MAX_TIME_FILE 개의 tuple이 공유하는 꼴이 됨. 
		// delete할 때 특히 문제가 될 수 있다.  (하나를  MAX_TIME_FILE 횟수만큼 delete하다 죽음...)
		sampleSize = s.sampleSize;
		pdaSample = new double[sampleSize];		
		memcpy(pdaSample, s.pdaSample, (sampleSize * sizeof(double)));

	}

#else
	// daSample로 고정배열을 쓴다면 deault 생성자, default 복사생성자를 사용하자.


#endif



};


//-----------------------------------------------
// Lot, Date 기준 key 값을 획득하기 위한 구조체
//-----------------------------------------------

// LOT > NET > Date > TUPLE(time)
// 4w 파일 하나에서 해당 net의 데이터 1개를 lot>net>date>tuple 구조의 tuple로 처리한다
// 날짜당 300개 정도의 파일을 다 분석하면 각 net별로 날짜수 * 300개 정도의 tuple이 생김.

#define MAX_LOT				5	
#define MAX_NET_PER_LOT		5000			// 원래는 10000 
#define MAX_DATE			5				
#define	MAX_TIME_FILE		4000			// 원래는 1200  
#define MAX_TUPLE_PER_NET	MAX_TIME_FILE	// 1분주기로 24시간측정이면 60*24= 1440이지만
											// 실제로는 약 84초 정도로 1000개정도의 파일이 생김
											// 여분을 고려하여 1200개로 지정
											// => 84초 주기는 data 너무 많다. 5분 주기 정도로 하면 300이면 충분함. 
											//
											// 2018.06.19 : 실제 현장에서 3000개 수준의  file이 존재할 수 있음.
											// net당 300개의 기준으로는 처리불가. net당은 3000으로 늘리고, 
											// 대신 총 sample 갯수(MAX_NET_DATA)를 8백만개로 제한하기로 함.
											//
											// 2018.10.10 CCTC의 경우, 3174개 케이스 실제 있음. 약 27초 주기
											//   4096을 넘으면 vector갯수(4096)문제가 있어 4000으로 결정.


// 2018.10.16: m_nNetDataCount가  MAX_NET_DATA 갯수를 넘어서면 4W Data 로딩을 중지하는 기능추가
//             아래의 측정 data를 토대로  Max Count를 600만으로 지정한다.
//			   Lot, Date별로 Net* time tuple 수를 다 더한 수를 6000000으로 제한.
//                memSize = 1461604352
//        m_nNetDataCount = 6000000 (약, 1.46 GB 시의 Count 수, file로는 약 552MB, cctc Net 4062, sample 1개 기준 file 1980개)
//sizeof(LotNetDate_Info) = 950200
//    sizeof(statNetData) = 128   =  4 + 24 + 4 + 12*8
//
//                memSize = 1398804480
//        m_nNetDataCount = 8000000	(약, 1.4GB 시의 Count수, file로는 약 850MB, sample2개 기준 file 3000개)
//sizeof(LotNetDate_Info) = 330100
//    sizeof(statNetData) = 128   =  4 + 24 + 4 + 12*8

#define	MAX_NET_DATA		8000000			 

// Lot, Date 기준 key 값을 획득하기 위한 구조체
// Lot, Date, Net 자체에 대한 information을 관리한다. 
class LotNetDate_Info
{
public:
	// Lot data
	CString		strLot[MAX_LOT];
	int			nLotCnt;

	// Date data
	CString		strLotDate[MAX_LOT][MAX_DATE];	// g_pvsaNetData[lot]의 date string list
	int			naLotDateCnt[MAX_LOT];			// g_pvsaNetData[lot]의 date 갯수 정보

	// Sample data
	int			naLotSampleCnt[MAX_LOT];		// g_pvsaNetData[lot]의 실제 sample 갯수 정보	
												// daSample도 vector로 관리하기 위함.
												// sample은 같은 lot에서는 모든 net에 대해 갯수가 동일함.

	// Net data
	int			naLotNet[MAX_LOT][MAX_NET_PER_LOT];
						// 모두 -1로 초기화하고 실제 존재하는 net만 net 번호를 write하고 사용한다.
						// net과 date는 실제 raw data에서는  Lot > Date > Net의 관계이지만
						// Stat 프로그램내에서는 Lot > Net > Date로 정리되어야 한다. 
						// 실제로는 Net마다 date가 달라질 필요는 없으므로 LotNetDate_Info 에서는 
						//  Lot > Net,   Lot > Date 로  Net과 Date는 서로 종속관계가 아닌 것으로 정리한다.
	int			naLotNetCnt[MAX_LOT];			// 해당 Lot의 Net 갯수
						
	short 		waLotNetPin[MAX_LOT][MAX_NET_PER_LOT][NUM_STAT_PIN];	 
						// statNetData class 에 넣었다가 pin번호는 lot, net 별로는 달라져도, 
						// date, tuple 별로는 달라지지 않으므로 데이터 중복을 막기 위해 여기로 옮김.
						
	// 2018.05.31 Net의 tuple 공통 LSL, USL :  추후 사용가능성이 있어서 코멘트처리함.
	//            Fault 처리는 netData의 tuple별 dLsl, dUsl을 사용하기로 함. 
	//double	daLotNetUsl[MAX_LOT][MAX_NET_PER_LOT];		// Net 별 USL(Upper Spec Limit) 정보. Fault 기준값
	//double	daLotNetLsl[MAX_LOT][MAX_NET_PER_LOT];		// Net 별 LSL(Lower Spec Limit) 정보. Fault 기준값
	

	// Date별 종합 data
	short		waLotNetDate_FaultCnt[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// 해당 date의 time*sample별 Fault sumup
														// USL이나 LSL을 초과한 date별 fault sample의 count
														// Lot 로딩시에 모두 계산해서 Tree의 Net 장애표시 기준으로 사용한다.
	short		waLotNetDate_NgCnt[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];		// 해당 date의 time*sample 별 Ng sumup
														
	// Net 별 종합 data
	short		waNetFaultCount[MAX_LOT][MAX_NET_PER_LOT];	// 해당 Net의 총 fault 갯수 (date*time*sample별 Fault 를 sumup)
	short		waNetNgCount[MAX_LOT][MAX_NET_PER_LOT];	    // 해당 Net의 총 NG 갯수 (date*time*sample별 Fault를 sumup)
	short		waNetTotalCount[MAX_LOT][MAX_NET_PER_LOT];	// 해당 Net의 Total data 갯수 (tupleSize * sampleSize * date 개수)


	LotNetDate_Info() 
	{
		nLotCnt = 0;
		for (int lot=0; lot < MAX_LOT; lot++)
		{
			// Lot data 초기화
			strLot[lot] = "";

			// Date(날짜) data 초기화
			int date;
			for (date=0; date < MAX_DATE; date++)
				strLotDate[lot][date] = "";
			naLotDateCnt[lot] = 0;
			naLotSampleCnt[lot] = 0;
			naLotNetCnt[lot] = 0;


			int net;
			for (net=0; net < MAX_NET_PER_LOT; net++)
			{
				// Net data 초기화
				naLotNet[lot][net] = -1;

				// Pin data 초기화
				for (int pin=0; pin < NUM_STAT_PIN; pin++)
					waLotNetPin[lot][net][pin] = 0;

				// USL, LSL, FaultCount 초기화
				//daLotNetUsl[lot][net] 		= 0.0;
				//daLotNetLsl[lot][net] 		= 0.0;

				for (date=0; date < MAX_DATE; date++)
				{
					waLotNetDate_FaultCnt[lot][net][date] = 0;
					waLotNetDate_NgCnt[lot][net][date] = 0;
				}

				
				waNetFaultCount[lot][net] = 0;
				waNetNgCount[lot][net] = 0;
			}
		}
	}

	void InitMember() 
	{
		nLotCnt = 0;
		for (int lot=0; lot < MAX_LOT; lot++)
		{
			// Lot data 초기화
			strLot[lot] = "";

			// Date data 초기화
			int date;
			for (date=0; date < MAX_DATE; date++)
				strLotDate[lot][date] = "";
			naLotDateCnt[lot] = 0;
			naLotSampleCnt[lot] = 0;
			naLotNetCnt[lot] = 0;

			for (int net=0; net < MAX_NET_PER_LOT; net++)
			{
				// Net data 초기화
				naLotNet[lot][net] = -1;

				// Pin data 초기화
				for (int pin=0; pin < NUM_STAT_PIN; pin++)
					waLotNetPin[lot][net][pin] = 0;

				// USL, LSL, FaultCount 초기화
				//daLotNetUsl[lot][net] 		= 0.0;
				//daLotNetLsl[lot][net] 		= 0.0;

				for (date=0; date < MAX_DATE; date++)
				{
					waLotNetDate_FaultCnt[lot][net][date] = 0;
					waLotNetDate_NgCnt[lot][net][date] = 0;
				}

				waNetFaultCount[lot][net] = 0;
				waNetNgCount[lot][net] = 0;
			}
		}

	}


	// 같은 이름의 Lot이 이미 있다면 Lot idx를 리턴, 없다면 처음이므로 새로 할당하고 할당된 id를 리턴 
	// Lot을  string 요소로 관리한다.
	int getLotId(CString str_Lot)
	{
		int lot;
		for (lot = 0 ; lot < MAX_LOT; lot++)
		{
			if (strLot[lot] == str_Lot)
				return lot;
		}

		if (nLotCnt >= MAX_LOT)
			return -1;

		lot = nLotCnt;		// 현재 count값이 다음번 id
		strLot[lot] = str_Lot;
		nLotCnt++;
		return lot;
	}

	// 할당받은 lot Id가 소용이 없을 때, Lot 자원을 아끼기 위해 다시 돌려 놓는다.
	int putBackLotId(int lot)
	{

		// 되돌리려는  lot id가 마지막 lot이 아니라면 put back 하지 않는다.
		if (nLotCnt != (lot+1))
			return -1;

		strLot[lot] = "";
		nLotCnt--;

		// 해당 lot 관련 date data, net data도 모두 초기화
		int date;
		for (date=0; date < MAX_DATE; date++)
			strLotDate[lot][date] = "";
		naLotDateCnt[lot] = 0;
		naLotSampleCnt[lot] = 0;
		naLotNetCnt[lot] = 0;

		return 0;
	}


	// 같은 이름의 Lot이 이미 있다면 Lot idx를 리턴, 없다면 -1 리턴 
	int findLotId(CString str_Lot)
	{
		int lot= -1;
		for (lot = 0 ; lot < MAX_LOT; lot++)
		{
			if (strLot[lot] == str_Lot)
				return lot;
		}

		return lot;
	}


	// 해당 lot에서 같은 date(날짜)를 찾으면 해당 date index를 리턴, 없으면 처음이므로 새로 할당하고 할당된 id을 리턴
	// Date(날짜)를  string 요소로 관리한다.
	int	getLotDateId(int nLot, CString dateStr)
	{

		// 같은 lot에는 같은 date가 있을 수 없다. 
		// 다른 lot에는 같은 date가 있을 수 있다.
		int date;
		for (date=0; date < MAX_DATE; date++)
		{
			if (strLotDate[nLot][date] == dateStr)
				return date;
		}

		if (naLotDateCnt[nLot] >= MAX_DATE)
			return -1;

		// 기존 LotDateStr 중에서 해당 str이 없는 경우 새로 할당.
		date = naLotDateCnt[nLot];		// 현재 date count값이 다음번 date id.
		strLotDate[nLot][date] = dateStr;
		naLotDateCnt[nLot]++;

		return date; 
	}

	// 해당 lot에서 같은 date를 찾으면 해당 date index를 리턴, 없으면 -1 리턴. 
	int	findLotDateId(int nLot, CString dateStr)
	{
		int date = -1;
		for (date=0; date < MAX_DATE; date++)
		{
			if (strLotDate[nLot][date] == dateStr)
				return date;
		}

		return date; 
	}

	// nNet 이 nLot 안에 존재한다는 의미로 naLotNet[nLot][nNet]에 nNet값을 write한다.
	void setLotNetId(int nLot, int nNet)
	{
		naLotNet[nLot][nNet] = nNet;
		naLotNetCnt[nLot]++;
	}

	// 해당 Net이 존재하는지 확인. 존재하면 true, 존재하지 않으면 -1 리턴.
	int findLotNetId(int nLot, int nNet)
	{
		if (naLotNet[nLot][nNet] == nNet) 
			return TRUE;

		return -1;
	}
};


//---------------------
// Define & enum
//---------------------


// define for DataGrid
//#define NUM_DATA_GRID_COL		26
#define MAX_DATA_GRID_ROW		(MAX_TIME_FILE * MAX_DATE) 

// enum for DATA GRID Column Location 
enum DATA_COL_TYPE  { DATA_COL_NO, 			// 0
					 DATA_COL_DATE, 		// 1
					 DATA_COL_TIME, 		// 2
					 DATA_COL_PIN1,			// 3
					 DATA_COL_PIN2,			// 4
					 DATA_COL_PIN3,			// 5
					 DATA_COL_PIN4,			// 6
					 DATA_COL_R,			// 7
					 DATA_COL_LSL,			// 8
					 DATA_COL_USL,			// 9
					 DATA_COL_AVG,			// 10
					 DATA_COL_SIGMA,		// 11
					 DATA_COL_MIN,			// 12
					 DATA_COL_MAX,			// 13

					 DATA_COL_DATA1,		// 14
					 DATA_COL_DATA2,		// 15
					 DATA_COL_DATA3,		// 16
					 DATA_COL_DATA4,		// 17
					 DATA_COL_DATA5,		// 18
					 DATA_COL_DATA6,		// 19
					 DATA_COL_DATA7,		// 20
					 DATA_COL_DATA8,		// 21
					 DATA_COL_DATA9,		// 22
					 DATA_COL_DATA10,		// 23
					 DATA_COL_DATA11,		// 24
					 DATA_COL_DATA12,		// 25

					 NUM_DATA_GRID_COL		// (25 + 1) : No까지 1추가
}; 

// enum for SUMMARY GRID Column Location 
enum SUM_COL_TYPE  { SUM_COL_NET, 			// 0
					 SUM_COL_DATE, 			// 1
					 SUM_COL_TOTAL,			// 2
					 SUM_COL_NG,			// 3
					 SUM_COL_COUNT, 		// 4
					 SUM_COL_AVG,			// 6
					 SUM_COL_SIGMA,			// 6
					 SUM_COL_DATAMIN,		// 7
					 SUM_COL_DATAMAX,		// 8
					 SUM_COL_FAULT,			// 9

					 NUM_SUM_GRID_COL	 	// 10
}; 

// enum for m_nCombo_CurrDate
enum DATE_TYPE  { DATE_ALL }; 		// 0


//------------------------------------------------------
// CSV 파일 출력을 위한 현재  Grid Snap Data
//------------------------------------------------------
class SumSnapData
{
public:
	CString strNetName;
	CString strDate;
	CString strTotal;
	CString strNgCount;
	CString strCount;

	CString strAvg;
	CString strSigma;
	CString strMin;
	CString strMax;
	CString strFault;


	SumSnapData()	// 생성자
	{
		strNetName = "";
		strDate = "";
		strCount = "";
		strNgCount = "";

		strAvg = "";
		strSigma = "";
		strMin = "";
		strMax = "";
		strFault = "";

	}

	void InitMember() 
	{
		strNetName = "";
		strDate = "";
		strCount = "";
		strNgCount = "";

		strAvg = "";
		strSigma = "";
		strMin = "";
		strMax = "";
		strFault = "";

	}
};

class DataSnapTuple
{
public:
//	int nTuple; 
	int nPrtTuple;
	CString strDate; 	// 같은 화면에서 date가 달라질 수 있으므로 저장. (DATE_ALL로 조회시)
	CString strTime;	

	CString strRefR; 	// DATE_ALL인 경우 nDate를 바꿔가며 계산해야 하므로 오류를 줄이기 위해 
	CString strLSL; 	// RefR, LSL, USL, strData[] 같은 값들도 모두 snap에 넣기로 함.
	CString strUSL; 
	CString strTupleAvg; 
	CString strTupleSigma; 
	CString strTupleMin; 
	CString strTupleMax;

	CString strData[NUM_STAT_SAMPLE];

	DataSnapTuple()	// 생성자
	{
		int i;
//		nTuple = 0;
		nPrtTuple = 0;
		strDate = "";
		strTime = "";

		strRefR = ""; 
		strLSL = ""; 
		strUSL = ""; 
		strTupleAvg = ""; 
		strTupleSigma = ""; 
		strTupleMin = ""; 
		strTupleMax = "";

		for (i = 0; i < NUM_STAT_SAMPLE; i++)
			strData[i] = "";
	}

	void InitMember() 
	{
		int i;
//		nTuple = 0;
		nPrtTuple = 0;
		strDate = "";
		strTime = "";

		strRefR = ""; 
		strLSL = ""; 
		strUSL = ""; 
		strTupleAvg = ""; 
		strTupleSigma = ""; 
		strTupleMin = ""; 
		strTupleMax = "";

		for (i = 0; i < NUM_STAT_SAMPLE; i++)
			strData[i] = "";
	}
};

class GridSnapInfo
{
public:
	
	SumSnapData		Summary;

	int				dataCount;
	DataSnapTuple   saData[MAX_DATA_GRID_ROW];

	void InitMember()
	{
		Summary.InitMember();
		dataCount = 0;
		for (int i = 0; i < MAX_DATA_GRID_ROW; i++)
			saData[i].InitMember();

	}
};

//---------------------------
// 전역변수,함수  extern 선언
//---------------------------

// 2018.07.18 4W YRp-chart에서도 활용하기 위해 다음 정보를 전역변수로 변경함. 
// Stat, YR p-Chart, FR Rank 화면 모두에서 사용.
extern LotNetDate_Info		g_sLotNetDate_Info;	
extern vector <statNetData>	*g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// 동적할당 vector


// Stat 화면과 FR Rank 화면에서 사용.
extern void	CalcSummary_AvgSigmaMinMax(int nLot, int nNet, int nDate,  	// argument
		int& rnCount, int& rnTotal, double& rdAvg, double& rdSigma, double& rdMin, double& rdMax );	// reference


// FR Rank, YR p-chart 다이얼로그에 보낼 User Defined Message

#define	UWM_LOAD_LOG4W_DATA		(WM_USER+1)

class CStatisticsDialog : public CDialog
{
// Construction
public:
	CStatisticsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStatisticsDialog)
	enum { IDD = IDD_STATISTICS_DIALOG };
	CComboBox	m_comboDate;
	CTreeCtrl	m_treeCtrl;
	CGridCtrl	m_gridSummary;
	CGridCtrl	m_gridData;
	CString		m_editStrSelect;
	int			m_editTupleNum;
	int			m_editSampleNum;
	BOOL		m_bDataGridFaultOnly;
	BOOL		m_bSimulateULSL;
	BOOL		m_bApplyULSL;
	CString		m_editStrUSL;	// double 값으로 바꾸어서 g_sLotNetDate_Info.dSimulUsl에 저장한다.
								// double 처리를 위해 str으로 value형 변수를 생성. 
								// 평상시에는 기본적으로 "" 빈칸을 유지한다.
	CString		m_editStrLSL;	// double 값으로 바꾸어서 g_sLotNetDate_Info.dSimulLsl에 저장한다.
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticsDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatisticsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboDate();
	afx_msg void OnButtonLoad4wData();
	afx_msg void OnButtonLoad4wData_SingleLot();
	afx_msg void OnCheckDataFaultOnly();
	afx_msg void OnButtonSaveStatCsv();
	afx_msg void OnButtonViewStatCsv();
	afx_msg void OnCheckSimulUlsl();
	afx_msg void OnCheckApplyUlsl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	HWND m_hwnd_FrRankDlg;
	HWND m_hwnd_YrPChartDlg;

	// StatisticsDlg에서 m_FrRankDlg와, m_PChartDlg로 메시지를 보낼때 사용할 hwnd 를 넘겨 받는다.
	void DlgHwndSetter(HWND hwFrRank, HWND hwYrPChart)
  	{
  		m_hwnd_FrRankDlg   = hwFrRank;
  		m_hwnd_YrPChartDlg = hwYrPChart;
  	}

	// 4W Statistics 정보관리를 위해 다음 3가지의 data 구조가 필요함. 
	
	// 1. lot * net * date 별로 존재하는 "statNetData" vector pointer의 array 
	//     이 포인터 밑으로 time tuple이 300개까지 가능하므로 
	//     실질적으로 lot * net * date * time(300) 개의 statNetData object가 dynamic 메모리로  관리된다.
	//
	// 2. lot * net 별로 존재하는 (date단위에서는 모두 동일) "LotNetDate_Info" 클래스
	//
	// 3. UI 구조 
	//   1) Total > lot > net 구조를 표시하기 위한 "Tree", 
	//   2) Date를 표시하기 위한  "Combo", 
	//   3) Summary Data를 위한   "Summary Grid", 
	//   4) Summary Data, Raw Data의 공통정보를  위한 "EditBox"  (tupleCount, dataCount, USL, LSL)
	//   4) Raw Data를 위한 "Data Grid" 

	//---------------------------
	//  Tree & Combo & Grid 관련 
	
	// member for tree control
	HTREEITEM 	m_hRoot;
	HTREEITEM 	m_hSelectedNode;
	CImageList	m_ImageList;

	// member for grid control
	int			m_nFixCols;
	int			m_nFixRows;
	int			m_nCols;
	int			m_nRows;
	BOOL		m_bEditable;
	BOOL		m_bListMode;
/*
	BOOL		m_bHorzLines;
	BOOL		m_bVertLines;
	BOOL		m_bSelectable;
	BOOL		m_bAllowColumnResize;
	BOOL		m_bAllowRowResize;
	BOOL		m_bHeaderSort;
	BOOL		m_bReadOnly;
	BOOL		m_bItalics;
	BOOL		m_btitleTips;
	BOOL		m_bSingleSelMode;	
*/
	int			m_nTree_CurrLot;				// 현재 tree에서 선택된 lot#,  미선택이면 -1
	int			m_nTree_CurrNet;				// 현재 tree에서 선택된 net#,  미선택이면 -1,  net=0은 미사용. net=1부터 사용.
	int			m_nCombo_CurrDate;				// 현재 combo에서 선택된 date# (0=All이므로 실제 date+1),  미선택이면 -1
												// 0=All,  1~7 : 실제 date#+1
												
	//int		m_nLoad_CurrLot;				// 현재 메모리에 load된 Lot
	//											// Lot별 메모리로딩/Save 기능을 막으면서 이 값도 봉인.

	// 2018.10.01 Simulate USL, LSL 관련 
	//            현재 '4w Stat'화면에 선택된 Lot, Net, Date에 대해서만 Simulate한다.
	//            Simulate On 이 된 시점의 현재 Lot, Net, Date에 대해 SimulLSL, SimulUSL 기준으로 Fault를 계산하고
	//            Grid UI와 CSV에도 출력, FR Rank, YR p-chart에도 반영하도록 한다. 
	//            Simul On이 되는 순간에 statNetData.dLSL, statNetData.dUsl은 사용하지 않고, dSimulLsl, dSimulLsl을 사용하여
	//            Fault 계산을 다시 하고, grid UI, csv 파일 출력에 반영한다.  Fault 계산을 다시하면 YR, FR 화면에는 자동 반영이 된다.
	//            Simulate On 시점의 USL, LSL data는 Simulation Off되면 삭제되고 다시 원래의 dLSL, uLSL값으로 돌아가서 
	//            Fault계산을 다시해야 한다. Simulation 값이 저장되어야 한다면, 위의 daLotNetUsl[][]를 살려야 하는데 큰 의미 없어 보임.
	double		m_dSimulUsl;		// Simulation On 상태에서 USL(Upper Spec Limit). statNetData.dUsl 대체
	double		m_dSimulLsl;		// Simulation On 상태에서 LSL(Lower Spec Limit). statNetData.dLSL 대체

	int 		m_nPrevSimulLoT;	// Simulation On 상태에서 다른 net을 클릭할 경우를 대비
	int 		m_nPrevSimulNet;	// Simulation On 상태에서 다른 net을 클릭할 경우를 대비
	int 		m_nPrevSimulDate;	// Simulation On 상태에서 다른 net을 클릭할 경우를 대비
	HTREEITEM	m_hPrevSimulSelNode;
	


	//---------------------------
	//  sSTAT_NET DATA 관련 
	
	// 2018.04.01
	// 1) 원래는 다음과 같은 4차원 배열이 필요하지만 이대로 할당하면 stack overflow 에러 발생.
	//    NET * TUPLE * StatSize 만 해도 1000 * 1200 * 112 => 134400000  134.4M로 stack overflow 발생.
	// statNetData 		saNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE][MAX_TUPLE_PER_NET];
	//
	// 2) 메모리 관리를 위해 최하위의 item은
	// 배열이 아니라 vector를 사용하고 tuple의 관리는 vector에 맡긴다.
	// g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE]라는 3차원 vector 포인터를 동적할당하고  
	// net 단위의 vector 포인터에 대해서만 프로그램내에서 new/delete 같은 메모리관리를 책임진다.
	//statNetData 		saNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE][MAX_TUPLE_PER_NET];	
	//  => 이 방법은 메모리를 너무 많이 차지한다. 1075M 이상은 한꺼번에 열 수 없음.
	//     2(lot) * 800(Net) * 5 (date) * 1200(time tuple) * 112(tupleSize) = 1075M
	//
	// 3)  모든 Lot을 한꺼번에 메모리에 올리지 말고 lot별로 4개의 파일로 저장하고 필요시에 메모리에 load 한다면
	//    1개 lot당: 1000(Net) * 7 (date) * 1200(time tuple) * 112(tupleSize) = 940.8M   
	//
	// 4)  time tuple을 84초 주기로 하루 1200개 처리하는 것은 데이터가 과도한 것으로 판단됨.
	//     5분주기로 한다면 하루 288개 여분을 고려해도 300개면 충분함.  time tuple을 300으로 한다면
	//     여러개의 lot을 한번에 메모리에 올릴 수 있음. 결론적으로 2)의 구조로 다시 돌아감.
	//    4(lot)* 1000(Net) * 7 (date)  * 300 (time tuple) * 104(tupleSize) = 약 873.6M 
	
	// 2018.07.18 4W p-chart에서도 활용하기 위해 다음 m_pvsaNetData정보를 전역변수(g_pvsaNetData)로 변경함. 
	//vector <statNetData>	*m_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// 동적할당 vector
	//int		m_nLotCount;								// psaNetData의 메모리할당된 Lot 갯수
	//int		m_naNetCount[MAX_LOT];						// psaNetData의 메모리할당된 Net 갯수
	

	//---------------------------
	//  LotNetDate_Info  관련 
	// 2018.07.18 4W p-chart에서도 활용하기 위해 다음 m_sLotNetDate_Info 정보를 전역변수(g_sLotNetDate_Info)로 변경함. 
	//LotNetDate_Info		m_sLotNetDate_Info;		// lot, net, date 기준 key 값과 각종 Net별 정보를 관리하는 클래스

	int			m_nNetDataCount;			// Stat_insertNetData 호출 횟수 (vector 갯수, sample 수 상관없는 time tuple의 수)
											// vector의 갯수를 8000000(MAX_NET_DATA)개로 제한하기 위한 변수
											//
											// Multi Load 하고 MAX_NET_DATA 때문에 종료되었다면,  맨 마지막 Lot, 
											// 날짜 디렉토리는 중간에 중단되었으므로 뒤쪽 net이 tuple이 하나씩 모자름
											// 각 Lot, 날짜별 time tuple의 수를 다 더하면 8000000이어야 한다.
											//  ex) lot 0 date1의 1~512 net은 2371 tuple인데,  513~2604 net은 2370 tuple이라면
											//
											//      lot 0 date 0 : 2604 net * 702 tuple = 1828008 tuple
											//      lot 0 date 1 : 512 net  * 2371 tuple = 1213952 tuple
											//                     2092 net * 2370 tuple = 4958040 tuple
											//  1828008+ 1213952 + 4958040 = 8000000 tuple

	//--------------------------
	// Display Snap
	
	GridSnapInfo	m_GridSnap;			// CSV 출력을 위해 현재 Grid의 출력 이미지 data를 동일하게 저장해 둔다.

	//-------------------
	// Member Function 
	
	BOOL 		InitMember();
	BOOL 		InitView();
	void		Display_SummaryGridHeader();
	void 		Display_DataGridHeader();
	void 		InitTree();

	void 		Load_Log4wDir();
	void 		Load_Log4wDir(LPCTSTR pstr);		// D:Log4w RawData Directory 전체 분석.

	BOOL		CheckEmptyDirectory(CString dirPath);

	HTREEITEM 	Tree_GetLotItem(LPCTSTR pStrInsert, LPCTSTR pStrPath, int nLot, int nDate, int& rbInserted); 
	HTREEITEM 	Tree_FindLotItem(CTreeCtrl* pTree, HTREEITEM hParent, LPCTSTR pStr);
	HTREEITEM 	Tree_FindLotItem2(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr);
	HTREEITEM 	Tree_FindStrItem(CTreeCtrl* pTree, HTREEITEM hItem, LPCTSTR pStr);
	//HTREEITEM Tree_FindData(CTreeCtrl* pTree, HTREEITEM hItem, DWORD dwData);
	int 		Tree_GetCurrentLevel(HTREEITEM hItem);
	int			Tree_LoadLot_4WFiles(LPCTSTR pParentPath, HTREEITEM hParentLot,  int nLot, int nDate);
	int			Load_4wDataFile(CString dataFilePath, CString dataFileName, 
														HTREEITEM hParentLot, int nLot, int nDate); 
														// RawData Director의 time CSV 파일을 Lot별로 메모리에 로드
															
	int 		Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData);
	int 		Stat_insertNetVector(int nLot, int nNet, int nDate);
	void 		Stat_deleteAllNetData();

	void 		Tree_CheckAll_LotNetFaults();
	void		Tree_Check_LotNetFaults(HTREEITEM hItem, int nLot);
														// 해당 Lot에 속한  모든 Net의 Fault 여부를 판단한다.
	void 		Tree_CheckNetFault(HTREEITEM hNetItem, int nLot);
	void 		_SelChangedTree(HTREEITEM  hSelItem);

	void  		Combo_UpdateDateContents(int nLot);
	void		DisplayGrid(int nLot, int nNet, int nComboDate); 
	void 		DisplayGrid_DefaultLotNet();
	void		Display_SumupLotNetDate(int nLot, int nNet, int nDate);
	void		Display_SumupLotNet(int nLot, int nNet);
	void		Display_SumupLotDate(int nLot, int nDate);
	void		Display_SumupLot(int nLot);
	void		Display_SumupTotal();

	int			DisplayGrid_4wData(int nLot, int nNet, int nDate, int tupleOffset); 
	int 		DisplayGrid_4wData_Tuple(int nLot, int nNet, int nDate, int nTuple, int nPrtTuple,
								double dTupleAvg, double dTupleSigma, double dTupleMin, double dTupleMax);
	void 		DisplayGrid_Summary(int nLot, int nNet, int nDate, int nCount,
								int nTotal, double dAvg, double dSigma, double dMin, double dMax, 
								int nFaultCount);

	void		ClearGrid();
	void		ClearGrid_Data();
	void		ClearGrid_Summary();

	void 		SaveStat_CsvFile(int nLot, int nNet, int nComboDate); // 현재 grid data를 csv 파일에 저장.
	void		Save_StatLotDataFile();				
	//void		Save_StatLotDataFile(int nLot);		// Lot별 data를 파일에 저장.
	//void 		Load_StatLotDataFile(int nLot);		// Lot별 data 파일을 memory에 load.
	
	void		CheckOff_SimulUlsl();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATISTICSDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_)

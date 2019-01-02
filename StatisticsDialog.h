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
#define NUM_STAT_SAMPLE		12		// �̰� �ø��� �Ǹ� Load_4wDataFile()-> sscanf�� �Է� parameter�� �÷��� �Ѵ�.
									// ������ 100���� �����ϴٰ� ��.
class statNetData
{
public:
	//short 		waPin[NUM_STAT_PIN];		// g_sLotNetDate_Info.waLotNetPin[lot][net] �� �̵���. 
	
	sSTAT_TIME		statTime;
	double			dLsl;	
	double			dRefR;	
	double			dUsl;	


#ifdef  DP_SAMPLE 
	// ���� �迭
	int				sampleSize;
	double* 		pdaSample; 		// pdaSample�� ���� ������ g_sLotNetDate_Info.naLotSampleCnt[lot]�� Ȯ��.

#else
	// ���� �迭
	double 			daSample[NUM_STAT_SAMPLE];
#endif

public:
	// ������
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
	// �Ҹ���
	~statNetData()			// �Ҹ��ڿ���  pda Sample�� ���� delete[]�� ������ ����.
	{
		if (pdaSample != NULL)
			delete []pdaSample;
	}
	
	// Deep Copy�� �ϴ� ���� ������.
	statNetData(const statNetData& s)
	{
		statTime.hour = s.statTime.hour;
		statTime.min  = s.statTime.min;
		statTime.sec  = s.statTime.sec; 

		dLsl  = s.dLsl;
		dRefR = s.dRefR;
		dUsl  = s.dUsl;


		// Deep copy�� ���� ��������ڿ��� �޸𸮸� ���� �Ҵ��� ��� �Ѵ�.
		// Load_4wDataFile()������ �ϳ��� netData�� new�� ���� �ִ� MAX_TIME_FILE ������ŭ 
		// g_pvsaNetData[nLot][nNet][nDate]->push_back(netData)�� ȣ���Ѵ�.
		// g_pvsaNetData[nLot][nNet][nDate]->push_back(netData)���� netData�� value�� ���ڷ�
		// ���� �� ���� �޸� �Ҵ��� �ǵ��� �Ʒ��� ���� deep copy�� ���� ���� new�� ������ �ش�.
		//
		// �׷��� ������ ���� �޸�(Load_4wDataFile() ���� netData )�� �ִ�  MAX_TIME_FILE ���� tuple�� �����ϴ� ���� ��. 
		// delete�� �� Ư�� ������ �� �� �ִ�.  (�ϳ���  MAX_TIME_FILE Ƚ����ŭ delete�ϴ� ����...)
		sampleSize = s.sampleSize;
		pdaSample = new double[sampleSize];		
		memcpy(pdaSample, s.pdaSample, (sampleSize * sizeof(double)));

	}

#else
	// daSample�� �����迭�� ���ٸ� deault ������, default ��������ڸ� �������.


#endif



};


//-----------------------------------------------
// Lot, Date ���� key ���� ȹ���ϱ� ���� ����ü
//-----------------------------------------------

// LOT > NET > Date > TUPLE(time)
// 4w ���� �ϳ����� �ش� net�� ������ 1���� lot>net>date>tuple ������ tuple�� ó���Ѵ�
// ��¥�� 300�� ������ ������ �� �м��ϸ� �� net���� ��¥�� * 300�� ������ tuple�� ����.

#define MAX_LOT				5	
#define MAX_NET_PER_LOT		5000			// ������ 10000 
#define MAX_DATE			5				
#define	MAX_TIME_FILE		4000			// ������ 1200  
#define MAX_TUPLE_PER_NET	MAX_TIME_FILE	// 1���ֱ�� 24�ð������̸� 60*24= 1440������
											// �����δ� �� 84�� ������ 1000�������� ������ ����
											// ������ ����Ͽ� 1200���� ����
											// => 84�� �ֱ�� data �ʹ� ����. 5�� �ֱ� ������ �ϸ� 300�̸� �����. 
											//
											// 2018.06.19 : ���� ���忡�� 3000�� ������  file�� ������ �� ����.
											// net�� 300���� �������δ� ó���Ұ�. net���� 3000���� �ø���, 
											// ��� �� sample ����(MAX_NET_DATA)�� 8�鸸���� �����ϱ�� ��.
											//
											// 2018.10.10 CCTC�� ���, 3174�� ���̽� ���� ����. �� 27�� �ֱ�
											//   4096�� ������ vector����(4096)������ �־� 4000���� ����.


// 2018.10.16: m_nNetDataCount��  MAX_NET_DATA ������ �Ѿ�� 4W Data �ε��� �����ϴ� ����߰�
//             �Ʒ��� ���� data�� ����  Max Count�� 600������ �����Ѵ�.
//			   Lot, Date���� Net* time tuple ���� �� ���� ���� 6000000���� ����.
//                memSize = 1461604352
//        m_nNetDataCount = 6000000 (��, 1.46 GB ���� Count ��, file�δ� �� 552MB, cctc Net 4062, sample 1�� ���� file 1980��)
//sizeof(LotNetDate_Info) = 950200
//    sizeof(statNetData) = 128   =  4 + 24 + 4 + 12*8
//
//                memSize = 1398804480
//        m_nNetDataCount = 8000000	(��, 1.4GB ���� Count��, file�δ� �� 850MB, sample2�� ���� file 3000��)
//sizeof(LotNetDate_Info) = 330100
//    sizeof(statNetData) = 128   =  4 + 24 + 4 + 12*8

#define	MAX_NET_DATA		8000000			 

// Lot, Date ���� key ���� ȹ���ϱ� ���� ����ü
// Lot, Date, Net ��ü�� ���� information�� �����Ѵ�. 
class LotNetDate_Info
{
public:
	// Lot data
	CString		strLot[MAX_LOT];
	int			nLotCnt;

	// Date data
	CString		strLotDate[MAX_LOT][MAX_DATE];	// g_pvsaNetData[lot]�� date string list
	int			naLotDateCnt[MAX_LOT];			// g_pvsaNetData[lot]�� date ���� ����

	// Sample data
	int			naLotSampleCnt[MAX_LOT];		// g_pvsaNetData[lot]�� ���� sample ���� ����	
												// daSample�� vector�� �����ϱ� ����.
												// sample�� ���� lot������ ��� net�� ���� ������ ������.

	// Net data
	int			naLotNet[MAX_LOT][MAX_NET_PER_LOT];
						// ��� -1�� �ʱ�ȭ�ϰ� ���� �����ϴ� net�� net ��ȣ�� write�ϰ� ����Ѵ�.
						// net�� date�� ���� raw data������  Lot > Date > Net�� ����������
						// Stat ���α׷��������� Lot > Net > Date�� �����Ǿ�� �Ѵ�. 
						// �����δ� Net���� date�� �޶��� �ʿ�� �����Ƿ� LotNetDate_Info ������ 
						//  Lot > Net,   Lot > Date ��  Net�� Date�� ���� ���Ӱ��谡 �ƴ� ������ �����Ѵ�.
	int			naLotNetCnt[MAX_LOT];			// �ش� Lot�� Net ����
						
	short 		waLotNetPin[MAX_LOT][MAX_NET_PER_LOT][NUM_STAT_PIN];	 
						// statNetData class �� �־��ٰ� pin��ȣ�� lot, net ���δ� �޶�����, 
						// date, tuple ���δ� �޶����� �����Ƿ� ������ �ߺ��� ���� ���� ����� �ű�.
						
	// 2018.05.31 Net�� tuple ���� LSL, USL :  ���� ��밡�ɼ��� �־ �ڸ�Ʈó����.
	//            Fault ó���� netData�� tuple�� dLsl, dUsl�� ����ϱ�� ��. 
	//double	daLotNetUsl[MAX_LOT][MAX_NET_PER_LOT];		// Net �� USL(Upper Spec Limit) ����. Fault ���ذ�
	//double	daLotNetLsl[MAX_LOT][MAX_NET_PER_LOT];		// Net �� LSL(Lower Spec Limit) ����. Fault ���ذ�
	

	// Date�� ���� data
	short		waLotNetDate_FaultCnt[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// �ش� date�� time*sample�� Fault sumup
														// USL�̳� LSL�� �ʰ��� date�� fault sample�� count
														// Lot �ε��ÿ� ��� ����ؼ� Tree�� Net ���ǥ�� �������� ����Ѵ�.
	short		waLotNetDate_NgCnt[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];		// �ش� date�� time*sample �� Ng sumup
														
	// Net �� ���� data
	short		waNetFaultCount[MAX_LOT][MAX_NET_PER_LOT];	// �ش� Net�� �� fault ���� (date*time*sample�� Fault �� sumup)
	short		waNetNgCount[MAX_LOT][MAX_NET_PER_LOT];	    // �ش� Net�� �� NG ���� (date*time*sample�� Fault�� sumup)
	short		waNetTotalCount[MAX_LOT][MAX_NET_PER_LOT];	// �ش� Net�� Total data ���� (tupleSize * sampleSize * date ����)


	LotNetDate_Info() 
	{
		nLotCnt = 0;
		for (int lot=0; lot < MAX_LOT; lot++)
		{
			// Lot data �ʱ�ȭ
			strLot[lot] = "";

			// Date(��¥) data �ʱ�ȭ
			int date;
			for (date=0; date < MAX_DATE; date++)
				strLotDate[lot][date] = "";
			naLotDateCnt[lot] = 0;
			naLotSampleCnt[lot] = 0;
			naLotNetCnt[lot] = 0;


			int net;
			for (net=0; net < MAX_NET_PER_LOT; net++)
			{
				// Net data �ʱ�ȭ
				naLotNet[lot][net] = -1;

				// Pin data �ʱ�ȭ
				for (int pin=0; pin < NUM_STAT_PIN; pin++)
					waLotNetPin[lot][net][pin] = 0;

				// USL, LSL, FaultCount �ʱ�ȭ
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
			// Lot data �ʱ�ȭ
			strLot[lot] = "";

			// Date data �ʱ�ȭ
			int date;
			for (date=0; date < MAX_DATE; date++)
				strLotDate[lot][date] = "";
			naLotDateCnt[lot] = 0;
			naLotSampleCnt[lot] = 0;
			naLotNetCnt[lot] = 0;

			for (int net=0; net < MAX_NET_PER_LOT; net++)
			{
				// Net data �ʱ�ȭ
				naLotNet[lot][net] = -1;

				// Pin data �ʱ�ȭ
				for (int pin=0; pin < NUM_STAT_PIN; pin++)
					waLotNetPin[lot][net][pin] = 0;

				// USL, LSL, FaultCount �ʱ�ȭ
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


	// ���� �̸��� Lot�� �̹� �ִٸ� Lot idx�� ����, ���ٸ� ó���̹Ƿ� ���� �Ҵ��ϰ� �Ҵ�� id�� ���� 
	// Lot��  string ��ҷ� �����Ѵ�.
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

		lot = nLotCnt;		// ���� count���� ������ id
		strLot[lot] = str_Lot;
		nLotCnt++;
		return lot;
	}

	// �Ҵ���� lot Id�� �ҿ��� ���� ��, Lot �ڿ��� �Ƴ��� ���� �ٽ� ���� ���´�.
	int putBackLotId(int lot)
	{

		// �ǵ�������  lot id�� ������ lot�� �ƴ϶�� put back ���� �ʴ´�.
		if (nLotCnt != (lot+1))
			return -1;

		strLot[lot] = "";
		nLotCnt--;

		// �ش� lot ���� date data, net data�� ��� �ʱ�ȭ
		int date;
		for (date=0; date < MAX_DATE; date++)
			strLotDate[lot][date] = "";
		naLotDateCnt[lot] = 0;
		naLotSampleCnt[lot] = 0;
		naLotNetCnt[lot] = 0;

		return 0;
	}


	// ���� �̸��� Lot�� �̹� �ִٸ� Lot idx�� ����, ���ٸ� -1 ���� 
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


	// �ش� lot���� ���� date(��¥)�� ã���� �ش� date index�� ����, ������ ó���̹Ƿ� ���� �Ҵ��ϰ� �Ҵ�� id�� ����
	// Date(��¥)��  string ��ҷ� �����Ѵ�.
	int	getLotDateId(int nLot, CString dateStr)
	{

		// ���� lot���� ���� date�� ���� �� ����. 
		// �ٸ� lot���� ���� date�� ���� �� �ִ�.
		int date;
		for (date=0; date < MAX_DATE; date++)
		{
			if (strLotDate[nLot][date] == dateStr)
				return date;
		}

		if (naLotDateCnt[nLot] >= MAX_DATE)
			return -1;

		// ���� LotDateStr �߿��� �ش� str�� ���� ��� ���� �Ҵ�.
		date = naLotDateCnt[nLot];		// ���� date count���� ������ date id.
		strLotDate[nLot][date] = dateStr;
		naLotDateCnt[nLot]++;

		return date; 
	}

	// �ش� lot���� ���� date�� ã���� �ش� date index�� ����, ������ -1 ����. 
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

	// nNet �� nLot �ȿ� �����Ѵٴ� �ǹ̷� naLotNet[nLot][nNet]�� nNet���� write�Ѵ�.
	void setLotNetId(int nLot, int nNet)
	{
		naLotNet[nLot][nNet] = nNet;
		naLotNetCnt[nLot]++;
	}

	// �ش� Net�� �����ϴ��� Ȯ��. �����ϸ� true, �������� ������ -1 ����.
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

					 NUM_DATA_GRID_COL		// (25 + 1) : No���� 1�߰�
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
// CSV ���� ����� ���� ����  Grid Snap Data
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


	SumSnapData()	// ������
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
	CString strDate; 	// ���� ȭ�鿡�� date�� �޶��� �� �����Ƿ� ����. (DATE_ALL�� ��ȸ��)
	CString strTime;	

	CString strRefR; 	// DATE_ALL�� ��� nDate�� �ٲ㰡�� ����ؾ� �ϹǷ� ������ ���̱� ���� 
	CString strLSL; 	// RefR, LSL, USL, strData[] ���� ���鵵 ��� snap�� �ֱ�� ��.
	CString strUSL; 
	CString strTupleAvg; 
	CString strTupleSigma; 
	CString strTupleMin; 
	CString strTupleMax;

	CString strData[NUM_STAT_SAMPLE];

	DataSnapTuple()	// ������
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
// ��������,�Լ�  extern ����
//---------------------------

// 2018.07.18 4W YRp-chart������ Ȱ���ϱ� ���� ���� ������ ���������� ������. 
// Stat, YR p-Chart, FR Rank ȭ�� ��ο��� ���.
extern LotNetDate_Info		g_sLotNetDate_Info;	
extern vector <statNetData>	*g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// �����Ҵ� vector


// Stat ȭ��� FR Rank ȭ�鿡�� ���.
extern void	CalcSummary_AvgSigmaMinMax(int nLot, int nNet, int nDate,  	// argument
		int& rnCount, int& rnTotal, double& rdAvg, double& rdSigma, double& rdMin, double& rdMax );	// reference


// FR Rank, YR p-chart ���̾�α׿� ���� User Defined Message

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
	CString		m_editStrUSL;	// double ������ �ٲپ g_sLotNetDate_Info.dSimulUsl�� �����Ѵ�.
								// double ó���� ���� str���� value�� ������ ����. 
								// ���ÿ��� �⺻������ "" ��ĭ�� �����Ѵ�.
	CString		m_editStrLSL;	// double ������ �ٲپ g_sLotNetDate_Info.dSimulLsl�� �����Ѵ�.
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

	// StatisticsDlg���� m_FrRankDlg��, m_PChartDlg�� �޽����� ������ ����� hwnd �� �Ѱ� �޴´�.
	void DlgHwndSetter(HWND hwFrRank, HWND hwYrPChart)
  	{
  		m_hwnd_FrRankDlg   = hwFrRank;
  		m_hwnd_YrPChartDlg = hwYrPChart;
  	}

	// 4W Statistics ���������� ���� ���� 3������ data ������ �ʿ���. 
	
	// 1. lot * net * date ���� �����ϴ� "statNetData" vector pointer�� array 
	//     �� ������ ������ time tuple�� 300������ �����ϹǷ� 
	//     ���������� lot * net * date * time(300) ���� statNetData object�� dynamic �޸𸮷�  �����ȴ�.
	//
	// 2. lot * net ���� �����ϴ� (date���������� ��� ����) "LotNetDate_Info" Ŭ����
	//
	// 3. UI ���� 
	//   1) Total > lot > net ������ ǥ���ϱ� ���� "Tree", 
	//   2) Date�� ǥ���ϱ� ����  "Combo", 
	//   3) Summary Data�� ����   "Summary Grid", 
	//   4) Summary Data, Raw Data�� ����������  ���� "EditBox"  (tupleCount, dataCount, USL, LSL)
	//   4) Raw Data�� ���� "Data Grid" 

	//---------------------------
	//  Tree & Combo & Grid ���� 
	
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
	int			m_nTree_CurrLot;				// ���� tree���� ���õ� lot#,  �̼����̸� -1
	int			m_nTree_CurrNet;				// ���� tree���� ���õ� net#,  �̼����̸� -1,  net=0�� �̻��. net=1���� ���.
	int			m_nCombo_CurrDate;				// ���� combo���� ���õ� date# (0=All�̹Ƿ� ���� date+1),  �̼����̸� -1
												// 0=All,  1~7 : ���� date#+1
												
	//int		m_nLoad_CurrLot;				// ���� �޸𸮿� load�� Lot
	//											// Lot�� �޸𸮷ε�/Save ����� �����鼭 �� ���� ����.

	// 2018.10.01 Simulate USL, LSL ���� 
	//            ���� '4w Stat'ȭ�鿡 ���õ� Lot, Net, Date�� ���ؼ��� Simulate�Ѵ�.
	//            Simulate On �� �� ������ ���� Lot, Net, Date�� ���� SimulLSL, SimulUSL �������� Fault�� ����ϰ�
	//            Grid UI�� CSV���� ���, FR Rank, YR p-chart���� �ݿ��ϵ��� �Ѵ�. 
	//            Simul On�� �Ǵ� ������ statNetData.dLSL, statNetData.dUsl�� ������� �ʰ�, dSimulLsl, dSimulLsl�� ����Ͽ�
	//            Fault ����� �ٽ� �ϰ�, grid UI, csv ���� ��¿� �ݿ��Ѵ�.  Fault ����� �ٽ��ϸ� YR, FR ȭ�鿡�� �ڵ� �ݿ��� �ȴ�.
	//            Simulate On ������ USL, LSL data�� Simulation Off�Ǹ� �����ǰ� �ٽ� ������ dLSL, uLSL������ ���ư��� 
	//            Fault����� �ٽ��ؾ� �Ѵ�. Simulation ���� ����Ǿ�� �Ѵٸ�, ���� daLotNetUsl[][]�� ����� �ϴµ� ū �ǹ� ���� ����.
	double		m_dSimulUsl;		// Simulation On ���¿��� USL(Upper Spec Limit). statNetData.dUsl ��ü
	double		m_dSimulLsl;		// Simulation On ���¿��� LSL(Lower Spec Limit). statNetData.dLSL ��ü

	int 		m_nPrevSimulLoT;	// Simulation On ���¿��� �ٸ� net�� Ŭ���� ��츦 ���
	int 		m_nPrevSimulNet;	// Simulation On ���¿��� �ٸ� net�� Ŭ���� ��츦 ���
	int 		m_nPrevSimulDate;	// Simulation On ���¿��� �ٸ� net�� Ŭ���� ��츦 ���
	HTREEITEM	m_hPrevSimulSelNode;
	


	//---------------------------
	//  sSTAT_NET DATA ���� 
	
	// 2018.04.01
	// 1) ������ ������ ���� 4���� �迭�� �ʿ������� �̴�� �Ҵ��ϸ� stack overflow ���� �߻�.
	//    NET * TUPLE * StatSize �� �ص� 1000 * 1200 * 112 => 134400000  134.4M�� stack overflow �߻�.
	// statNetData 		saNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE][MAX_TUPLE_PER_NET];
	//
	// 2) �޸� ������ ���� �������� item��
	// �迭�� �ƴ϶� vector�� ����ϰ� tuple�� ������ vector�� �ñ��.
	// g_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE]��� 3���� vector �����͸� �����Ҵ��ϰ�  
	// net ������ vector �����Ϳ� ���ؼ��� ���α׷������� new/delete ���� �޸𸮰����� å������.
	//statNetData 		saNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE][MAX_TUPLE_PER_NET];	
	//  => �� ����� �޸𸮸� �ʹ� ���� �����Ѵ�. 1075M �̻��� �Ѳ����� �� �� ����.
	//     2(lot) * 800(Net) * 5 (date) * 1200(time tuple) * 112(tupleSize) = 1075M
	//
	// 3)  ��� Lot�� �Ѳ����� �޸𸮿� �ø��� ���� lot���� 4���� ���Ϸ� �����ϰ� �ʿ�ÿ� �޸𸮿� load �Ѵٸ�
	//    1�� lot��: 1000(Net) * 7 (date) * 1200(time tuple) * 112(tupleSize) = 940.8M   
	//
	// 4)  time tuple�� 84�� �ֱ�� �Ϸ� 1200�� ó���ϴ� ���� �����Ͱ� ������ ������ �Ǵܵ�.
	//     5���ֱ�� �Ѵٸ� �Ϸ� 288�� ������ ����ص� 300���� �����.  time tuple�� 300���� �Ѵٸ�
	//     �������� lot�� �ѹ��� �޸𸮿� �ø� �� ����. ��������� 2)�� ������ �ٽ� ���ư�.
	//    4(lot)* 1000(Net) * 7 (date)  * 300 (time tuple) * 104(tupleSize) = �� 873.6M 
	
	// 2018.07.18 4W p-chart������ Ȱ���ϱ� ���� ���� m_pvsaNetData������ ��������(g_pvsaNetData)�� ������. 
	//vector <statNetData>	*m_pvsaNetData[MAX_LOT][MAX_NET_PER_LOT][MAX_DATE];	// �����Ҵ� vector
	//int		m_nLotCount;								// psaNetData�� �޸��Ҵ�� Lot ����
	//int		m_naNetCount[MAX_LOT];						// psaNetData�� �޸��Ҵ�� Net ����
	

	//---------------------------
	//  LotNetDate_Info  ���� 
	// 2018.07.18 4W p-chart������ Ȱ���ϱ� ���� ���� m_sLotNetDate_Info ������ ��������(g_sLotNetDate_Info)�� ������. 
	//LotNetDate_Info		m_sLotNetDate_Info;		// lot, net, date ���� key ���� ���� Net�� ������ �����ϴ� Ŭ����

	int			m_nNetDataCount;			// Stat_insertNetData ȣ�� Ƚ�� (vector ����, sample �� ������� time tuple�� ��)
											// vector�� ������ 8000000(MAX_NET_DATA)���� �����ϱ� ���� ����
											//
											// Multi Load �ϰ� MAX_NET_DATA ������ ����Ǿ��ٸ�,  �� ������ Lot, 
											// ��¥ ���丮�� �߰��� �ߴܵǾ����Ƿ� ���� net�� tuple�� �ϳ��� ���ڸ�
											// �� Lot, ��¥�� time tuple�� ���� �� ���ϸ� 8000000�̾�� �Ѵ�.
											//  ex) lot 0 date1�� 1~512 net�� 2371 tuple�ε�,  513~2604 net�� 2370 tuple�̶��
											//
											//      lot 0 date 0 : 2604 net * 702 tuple = 1828008 tuple
											//      lot 0 date 1 : 512 net  * 2371 tuple = 1213952 tuple
											//                     2092 net * 2370 tuple = 4958040 tuple
											//  1828008+ 1213952 + 4958040 = 8000000 tuple

	//--------------------------
	// Display Snap
	
	GridSnapInfo	m_GridSnap;			// CSV ����� ���� ���� Grid�� ��� �̹��� data�� �����ϰ� ������ �д�.

	//-------------------
	// Member Function 
	
	BOOL 		InitMember();
	BOOL 		InitView();
	void		Display_SummaryGridHeader();
	void 		Display_DataGridHeader();
	void 		InitTree();

	void 		Load_Log4wDir();
	void 		Load_Log4wDir(LPCTSTR pstr);		// D:Log4w RawData Directory ��ü �м�.

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
														// RawData Director�� time CSV ������ Lot���� �޸𸮿� �ε�
															
	int 		Stat_insertNetData(int nLot, int nNet, int nDate, statNetData netData);
	int 		Stat_insertNetVector(int nLot, int nNet, int nDate);
	void 		Stat_deleteAllNetData();

	void 		Tree_CheckAll_LotNetFaults();
	void		Tree_Check_LotNetFaults(HTREEITEM hItem, int nLot);
														// �ش� Lot�� ����  ��� Net�� Fault ���θ� �Ǵ��Ѵ�.
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

	void 		SaveStat_CsvFile(int nLot, int nNet, int nComboDate); // ���� grid data�� csv ���Ͽ� ����.
	void		Save_StatLotDataFile();				
	//void		Save_StatLotDataFile(int nLot);		// Lot�� data�� ���Ͽ� ����.
	//void 		Load_StatLotDataFile(int nLot);		// Lot�� data ������ memory�� load.
	
	void		CheckOff_SimulUlsl();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATISTICSDIALOG_H__2ECC17B6_A785_4CA6_B46A_85C1648A9469__INCLUDED_)

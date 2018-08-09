//////////////////////////////////////////////////////////////////////
// System.h: interface for the CSystem class.
//
// 2018.03.30  written by sonji
//////////////////////////////////////////////////////////////////////

#if !defined(_SYSTEM_H_INCLUDED_)
#define _SYSTEM_H_INCLUDED_



//-------------------------
// Statistics  데이터

//  Total > Lot > Net
//      Cal 	: 지정 Count만큼의 Measure를 수행 완료시 까지
//      Measure : 1회 Calibration 측정(Test 수행결과)
//      Test    : open, short(leak), 4W같은 Measure의 Test 종류

#define MAX_MEAS_COUNT		10
#define	MAX_AD_COUNT	100	

#define	CAL_AD_COUNT			(MAX_AD_COUNT*2)		
#define CAL_AD_BUF_SIZE			CAL_AD_COUNT	

//#define MAX_CAL_DATA		128
#define MAX_CAL_DATA_org	128
#define MAX_CAL_DATA		MAX_2W_RJIG
#define NUM_CAL_RESULT_COL	(22 + 60)		// 22: 기본,   60: 개별 Measure (1~10)

typedef struct sMEAS_DATA
{
						// Col22, Col28, Col34, ... Col76  <= UI 빈칸
	double	dR;			// Col23, Col29, Col35, ... Col77
	double	dVc;		// Col24, Col30, Col36, ... Col78
	double	dI;			// Col25, Col31, Col37, ... Col79
	double	dAdVc;		// Col26, Col32, Col38, ... Col80
	double	dAdI;		// Col27, Col33, Col39, ... Col81
} sMEAS_DATA;



typedef struct sCAL_DATA
{
	int 	nUse;				// Col1		// 0: NotUse,   1: Use
	int 	nValid;				// Col2	    // 0: Bad(InValid), FALSE,  1: Good(Valid), TRUE
										    
	double 	dOrgR;				// Col7		//  Original R Value   (Real H/W) . sRJIG_DATA 의 dOrgR과 같은 값이어야 한다.
	double 	dRefR_Avg;			// Col8
	double 	dRefVcVal;			// Col9		// 추후 필요없다면 지운다.
	double 	dRefIVal;			// Col10	// 추후 필요없다면 지운다.
								// Col11    // 구분용 빈 컬럼(for UI)
								
	double 	dMeasR_Avg;			// Col12
	double 	dMeasVc_Avg;		// Col16
	double 	dMeasI_Avg;			// Col17
	double 	dMeasAdVc_Avg;		// Col18
	double 	dMeasAdI_Avg;		// Col19

	double 	dMeasR_Max;			// Col20
	double 	dMeasR_MaxDiffRate;	// Col13	// (Max - Avg)/Avg : + 오차율  .추후 +5%보다 크면 붉은색표기. TBD
	double 	dMeasR_Min;			// Col21
	double 	dMeasR_MinDiffRate; // Col14	// (Min - Avg)/Avg : - 오차율  .추후 -5%보다 작으면 붉은색표기. TBD


	sMEAS_DATA		sa_MeasData[MAX_MEAS_COUNT];

} sCAL_DATA;


//----------------
// System 정보
//  : 고유의 ACE400 계측기 H/W가 가진 특성을 셋팅하는 것.   
//    내부적으로 개발하기 위한 초기화 값. 
//    제품과 상관 없이 전체 모델을 통틀어 만든 Bioptro의 H/W 관련 데이터. 
//    ex) BlockType, RJigData 핀설정 데이터

/*
// SysInfo  enum
enum SYSTEM_TYPE	{ 
	QD =0, 			// QD: Quad Density
	DD =1, 			// DD: Double Density
	SD =2,			// SD: Single Density
	S100_1 = 3,
	S100_2 = 4,
	YAMAHA = 5,
};
*/

//enum BOX_TYPE		{ TOP, BTM, MAX_BOX };



class CSystem  
{
public:
	//for "C:\\ACE400\\SETUP\\SysSet01.ini" 관련 정보. CSystem::Load_SysInfoData()
	int 			m_nBlockNumber[MAX_BOX];	// idx(0=Top, 1=Bottom)  val(m_nBlockType의 block 갯수)
	int 			m_nBlockType[MAX_BOX];		// idx(0=Top, 1=Bottom)  val(QD=0, DD=1, SD=2, S100_1=3, S100_2=4)

public: 
	CSystem() 
	{
		::ZeroMemory(m_nBlockNumber, sizeof(m_nBlockNumber));
		::ZeroMemory(m_nBlockType, sizeof(m_nBlockType));
		//m_nRJigFileNum = -1;
		//m_nSystemType = -1;

	}

	virtual ~CSystem();
	void	InitMember();
	BOOL 	Load_4wRawData();
};


extern CSystem g_sSystem;




#endif // !defined(_SYSTEM_H_INCLUDED_)

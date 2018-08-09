#include "stdafx.h"
#include "System.h"


// 전역변수 설정
CSystem g_sSystem;


CSystem::~CSystem()
{


}

void CSystem::InitMember() 
{
	::ZeroMemory(m_nBlockNumber, sizeof(m_nBlockNumber));
	::ZeroMemory(m_nBlockType, sizeof(m_nBlockType));

	m_nRJigFileNum = -1;
	m_nSystemType = -1;

	InitCalData();
	//TRACE("CSystem::InitMember(): sizeof(m_nBlockNumber)=%d\n", sizeof(m_nBlockNumber));
	//TRACE("CSystem::InitMember(): sizeof(m_nBlockType)=%d\n", sizeof(m_nBlockType));
}





// "D:\log4w\" 디렉토리에 날짜별폴더 -> 시간별 Net Sample Raw data를  로드
BOOL 	CSystem::Load_4wRawData()
{
	FILE *fp; 
	char  fName[200]; 
	int i; 
	BOOL bRetVal = TRUE;


	::ZeroMemory(&fName, sizeof(fName));
	strcat(fName , g_sFile.ACE400_4WDataDir);		// "D:\\log4w"
/*
	if(!FileExists(fName)) 
	{ 	ERR.Set(FLAG_FILE_NOT_FOUND, fName); return FALSE; }

	fp=fopen(fName,"rt");
	if(fp == NULL)
	{ 	ERR.Set(FLAG_FILE_CANNOT_OPEN, fName); return FALSE; }

	int Num;

	// 파일 헤더 Read 
	for (i=0; i < 16; i++)
		fscanf(fp, "%10d", &Num);	// 파일 헤더는 저장하지 말자.  Skip
	fscanf(fp, "\n");

	// 파일 데이터 Read
	for (i=0; i < MAX_TEST_MODE; i++)
	{

		fscanf(fp, "%10d", &Num); 	// 첫 컬럼 Num 은 저장하지 말자. (i+1값과 같아서 불필요)
		fscanf(fp, "%10d", &g_saTestCondition[i].nCC);
		fscanf(fp, "%10d", &g_saTestCondition[i].nVBSrc);
		fscanf(fp, "%10d", &g_saTestCondition[i].nVSet);
		fscanf(fp, "%10d", &g_saTestCondition[i].nIRng);
		fscanf(fp, "%10d", &g_saTestCondition[i].nISet);
		fscanf(fp, "%10d", &g_saTestCondition[i].nDelay);
		fscanf(fp, "%10d", &g_saTestCondition[i].nAdCnt);

		fscanf(fp, "%10d", &g_saTestCondition[i].nFilter);
		fscanf(fp, "%10d", &g_saTestCondition[i].nIFilter);
		fscanf(fp, "%10d", &g_saTestCondition[i].nHvOffset);
		fscanf(fp, "%10d", &g_saTestCondition[i].nHVGain);
		fscanf(fp, "%10d", &g_saTestCondition[i].nIGain);
		fscanf(fp, "%10d", &g_saTestCondition[i].n4WGain);
		fscanf(fp, "%10d", &g_saTestCondition[i].sCount);
		fscanf(fp, "%10d", &g_saTestCondition[i].nTest);
		fscanf(fp, "\n");
	}
	fclose(fp);
*/

	return bRetVal;
}



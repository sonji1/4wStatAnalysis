// FileSysInfo.cpp: implementation of the CFileSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileSysInfo.h"
#include <mmsystem.h>



// 글로벌 변수
CFileSysInfo 	FileSysInfo01;
CSysInfoView01  SysInfoView01;
CSysInfo03  	SysInfo03;
CSysInfo25  	SysInfo25;
CSysInfo19  	SysInfo19;


//-------------------------
// class CFileSysInfo 함수
//-------------------------


int CFileSysInfo::LoadSaveView01(int type)
{
	FILE *fp; 
	char  fName[200]; 
	CString ; 
 


	::ZeroMemory(&fName, sizeof(fName));
	strcat( fName , "C:\\ACE400\\SETUP\\FilePathJig.ini" ); //sylee121202
 

	switch(type)
	{
	case DATA_SAVE: // 1
 
 
		fp = fopen(fName,"wt");
		if(fp == NULL){ 
			AfxMessageBox(" Error No 2002 !\n\\SETUP\\FilePathJig.ini.  file Data Saving Error !     file close!", MB_OK); //error messege
			
			return 0;
		}

	
        fprintf(fp, "%s\n",SysInfoView01.m_pStrFileDri1);  //set path file
        fprintf(fp, "%s\n",SysInfoView01.m_pStrFilePath1);  //set path file
		fprintf(fp, "%s\n",SysInfoView01.m_pStrFilePathJig1);  //set path file
		fprintf(fp, "%s\n",SysInfoView01.m_pStrFilePathJig2);  //set path file
		fprintf(fp, "%s\n",SysInfoView01.m_pStrFilePathBDL);  //SYLEE121202
        fprintf(fp, "%s\n",SysInfoView01.m_pStrFileBDLName);  //SYLEE121202	
		
		break;

	case DATA_LOAD: // 2
		fp=fopen(fName,"rt");
		if(fp == NULL){ 
	     	AfxMessageBox(" Error No 2002 !\n\\SETUP\\FilePathJig.ini.  file Data Loading Error", MB_OK); //error messege
    		return 0;
		}
		fscanf(fp, "%s\n", SysInfoView01.m_pStrFileDri1);		
		fscanf(fp, "%s\n", SysInfoView01.m_pStrFilePath1); 
		fscanf(fp, "%s\n", SysInfoView01.m_pStrFilePathJig1);
		fscanf(fp, "%s\n", SysInfoView01.m_pStrFilePathJig2);  //sw loading init folder
        fscanf(fp, "%s\n", SysInfoView01.m_pStrFilePathBDL); //SYLEE121202
		fscanf(fp, "%s\n", SysInfoView01.m_pStrFileBDLName); //SYLEE121202
          		
		break;

	default :
		break;
	}
	fclose(fp); 
	return 1;
}



//for Block.ini <-> SysInfo03
int CFileSysInfo::LoadSaveSub13(int type)
{
 
	int  row, col, nBlockTot;
	
	FILE *fp; 
	char  fName[200];  
	CString str, strTemp;

    ::ZeroMemory(&fName, sizeof(fName));
 	str.Format("%s", SysInfoView01.m_pStrFilePathBDL);//SYLEE121202
   	strcat( fName ,str);
 	strcat( fName , "\\SETUP2\\Block.ini" );

 // 	strcat( fName , "C:\\ACE300\\SETUP2\\Block.ini" );
 
	
	switch(type)
	{
	case DATA_SAVE: // 1
        // file 삭제 
//		DeleteFile(fName);
		fp = fopen(fName,"wt");
		if(fp == NULL) return FALSE;


		fprintf(fp, "%10d\n",SysInfo03.m_nBlock);
		fprintf(fp, "%10d\n",SysInfo03.m_nRow);	 
		fprintf(fp, "%10d\n",SysInfo03.m_nCol);
		fprintf(fp, "%10d\n",SysInfo03.m_nType);

       
		for(row=0; row < SysInfo03.m_nRow; row++)
		{
			for(col=0;col<SysInfo03.m_nCol;col++)
				fprintf(fp, "%10d",SysInfo03.m_nData[row][col]);
			fprintf(fp,"\n");
		}
		break;


	case DATA_LOAD: // 2
		fp=fopen(fName,"rt");
		if(fp==NULL) return FALSE;

		fscanf(fp, "%10d\n", &SysInfo03.m_nBlock);		 
		fscanf(fp, "%10d\n", &SysInfo03.m_nRow);		 
	    fscanf(fp, "%10d\n", &SysInfo03.m_nCol);			  
		fscanf(fp, "%10d\n", &SysInfo03.m_nType);

		// Range Check
		if (SysInfo03.m_nRow <= 0 || SysInfo03.m_nRow >= Def_MaxBlock1)
		{
			strTemp.Format("LoadSaveSub13(): SysInfo03.m_nRow=%d Range Over! (1 <= m_nRow <= %d)\n", 
					SysInfo03.m_nRow, (Def_MaxBlock1 -1));
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}
		if (SysInfo03.m_nCol <= 0 || SysInfo03.m_nCol >= Def_MaxBlock1)
		{
			strTemp.Format("LoadSaveSub13(): SysInfo03.m_nCol=%d Range Over! (1 <= m_nCol <= %d)\n", 
					SysInfo03.m_nCol, (Def_MaxBlock1 -1));
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}
		nBlockTot = SysInfo03.m_nRow * SysInfo03.m_nCol;
		if (nBlockTot <= 0 || nBlockTot >= Def_MaxBlock1)
		{
			strTemp.Format("LoadSaveSub13(): nBlockTot=%d Range Over! (1 <= nBlockTot <= %d)\n", 
					nBlockTot, (Def_MaxBlock1 -1));
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}

		for(row=0; row < SysInfo03.m_nRow; row++)
		{
			for(col=0; col < SysInfo03.m_nCol; col++)
				fscanf(fp, "%10d", &SysInfo03.m_nData[row][col]);
		} 
		break;


	default :
		break;
	}

	
	fclose(fp); 
	return TRUE;
}




//for Piece.ini <-> SysInfo25
int CFileSysInfo::LoadSaveSub25(int type)
{
 
	int  row, col, nPieceTot;;
 	FILE *fp; 
	char  fName[200]; 
	CString str, strTemp; 

    ::ZeroMemory(&fName, sizeof(fName));
 	str.Format("%s", SysInfoView01.m_pStrFilePathBDL);//SYLEE121202
   	strcat( fName ,str);
 	strcat( fName , "\\SETUP2\\Piece.ini" );
 	 
//	strcat( fName , "C:\\ACE300\\SETUP2\\Piece.ini" );
 	
	switch(type)
	{
	case DATA_SAVE: // 1
 
		nPieceTot=SysInfo25.m_nRow * SysInfo25.m_nCol;
 
		if( Def_MaxPiece1 < 300)
		{
			if(nPieceTot > 200){
				AfxMessageBox("  ERROR 901,  piece  total max 200 over! ", MB_ICONSTOP);			
				return FALSE;
			} 
		}
		else
		{
			if(nPieceTot > Def_MaxPiece1) //SYLEE170803
			{
				AfxMessageBox("  ERROR 901,  piece  total max 300 over! ", MB_ICONSTOP);			
				return FALSE;
			} 
		}

		fp = fopen(fName,"wt");
		if(fp == NULL) 
			return FALSE; 

		fprintf(fp, "%10d\n",SysInfo25.m_nBlock);
		fprintf(fp, "%10d\n",SysInfo25.m_nRow);	 
		fprintf(fp, "%10d\n",SysInfo25.m_nCol);
		fprintf(fp, "%10d\n",SysInfo25.m_nType);
       
		for(row=0; row < SysInfo25.m_nRow; row++)
		{
			for(col=0; col < SysInfo25.m_nCol; col++)
				fprintf(fp, "%10d",SysInfo25.m_nData[row][col]);
			fprintf(fp,"\n");
		}

		fprintf(fp, "%10d\n",SysInfo25.m_nRowDel);	 
		fprintf(fp, "%10d\n",SysInfo25.m_nColDel);

		break;

	case DATA_LOAD: // 2
		fp = fopen(fName,"rt");
		if (fp == NULL) 
			return FALSE;

		fscanf(fp, "%10d\n", &SysInfo25.m_nBlock);			// nPieceTot와 같아야 함.
		fscanf(fp, "%10d\n", &SysInfo25.m_nRow);			 
	    fscanf(fp, "%10d\n", &SysInfo25.m_nCol);		 
		fscanf(fp, "%10d\n", &SysInfo25.m_nType);		 

		// Range Check
		if (SysInfo25.m_nRow <= 0 || SysInfo25.m_nRow >= Def_MaxPiece1)
		{
			strTemp.Format("LoadSaveSub25(): SysInfo25.m_nRow=%d Range Over! (1 <= m_nRow <= %d)\n", 
					SysInfo25.m_nRow, (Def_MaxPiece1 -1) );
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}
		if (SysInfo25.m_nCol <= 0 || SysInfo25.m_nCol >= Def_MaxPiece1)
		{
			strTemp.Format("LoadSaveSub25(): SysInfo25.m_nCol=%d Range Over! (1 <= m_nCol <= %d)\n", 
					SysInfo25.m_nCol, (Def_MaxPiece1 -1) );
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}

	    nPieceTot=SysInfo25.m_nRow * SysInfo25.m_nCol;
		if( Def_MaxPiece1 < 300) //SYLEE170803	// 미동작  코드.. (Def_MaxPiece1은 상수 301이므로 300보다 클 수 없음)
		{
			if(nPieceTot > 200) //SYLEE17032
			{
				strTemp.Format("LoadSaveSub25() error: nPieceTot(=%d) max 200 over!", nPieceTot);			
				AfxMessageBox(strTemp, MB_ICONSTOP);
				SysInfo25.m_nRow=10;
				SysInfo25.m_nCol=10;
			} 
		}
		else		// 동작 코드
		{
			if(nPieceTot >= Def_MaxPiece1){//SYLEE170803
				strTemp.Format("LoadSaveSub25() error: nPieceTot(=%d) max 300 over!", nPieceTot);			
				AfxMessageBox(strTemp, MB_ICONSTOP);
				SysInfo25.m_nRow=10;	// 
				SysInfo25.m_nCol=10;
			} 
		}


		for(row=0; row < SysInfo25.m_nRow; row++)
		{
			for(col=0; col < SysInfo25.m_nCol; col++)
				fscanf(fp, "%10d", &SysInfo25.m_nData[row][col]);
		} 

		fscanf(fp, "%10d\n", &SysInfo25.m_nRowDel);			 
		fscanf(fp, "%10d\n", &SysInfo25.m_nColDel);

		break;

	default :
		break;
	}

	fclose(fp); 
	return TRUE;
}


// for BlockDis.ini <-> SysInfo19
int CFileSysInfo::LoadSaveSub19(int type, int nFileType)
{

	int  block, piece;
	
	FILE *fp; 
	char  fName[200]; 
 	CString str, strTemp; 

    ::ZeroMemory(&fName, sizeof(fName));
	//SYLEE130601	strcat( fName , "C:\\ACE300\\Setup\\BlockDis.ini" );
 
  	
	if(nFileType==2){
	   strcat( fName , "Z:\\BLOCKDISABLE.TXT" );
	}else{
		str.Format("%s", SysInfoView01.m_pStrFilePathBDL);//SYLEE121202
   		strcat( fName ,str);
  		strcat( fName , "\\SETUP2\\BlockDis.ini" );
	}

	

	switch(type)
	{
	case DATA_SAVE: // 1
        // file 삭제 
		DeleteFile(fName);
		fp = fopen(fName,"wt");
		if(fp == NULL)
		{ 

			SysInfo19.m_nBlockTot=SysInfo03.m_nBlock;//block
			SysInfo19.m_nPieceTot=SysInfo25.m_nBlock;//piece
			SysInfo19.m_nCellTotal=SysInfo19.m_nBlockTot*SysInfo19.m_nPieceTot;
			SysInfo19.m_nType=1;
			SysInfo19.m_nUse=0;
			//FileSysInfo01.LoadSaveSub19(1,1); //save//sylee160203

			fp = fopen(fName,"wt");//sylee160203
			if(fp == NULL){	//sylee160203			
				if(nFileType==2){	         
					AfxMessageBox(" Error No 2218, \n\n\n    Z:\\BLOCKDISABLE.TXT  FILE,  Save Error !!! \n   ");
				}else{
					AfxMessageBox(" Error No 2218, \n\n\n   C:\\ACE300\\BDL\\...\\SETUP2\\BlockDis.ini   FILE,  Save Error !!! \n   ");
				}
			}

			return FALSE;
		}

		fprintf(fp, "%10d\n",SysInfo19.m_nBlockTot);
		fprintf(fp, "%10d\n",SysInfo19.m_nPieceTot); 
        fprintf(fp, "%10d\n",SysInfo19.m_nCellTotal);
		fprintf(fp, "%10d\n",SysInfo19.m_nType);
        fprintf(fp, "%10d\n",SysInfo19.m_nUse);
 
 		// m_nUse가 1이 아니라면 모두 0으로 초기화
		if( SysInfo19.m_nUse!=1)
		{
			for(block=1; block < Def_MaxBlock1; block++) //sylee131122
			{
				for(piece=1; piece < Def_MaxPiece1; piece++)
					SysInfo19.m_nData[block][piece] = 0; 
			}
		}

		// SysInfo19.m_nData를 BlockDis.ini 파일에 write한다.
		for(block=1; block<=SysInfo19.m_nBlockTot; block++)
		{
			for(piece=1; piece<=SysInfo19.m_nPieceTot; piece++)
			{
				if(SysInfo19.m_nData[block][piece] != 1)
					SysInfo19.m_nData[block][piece] = 0;

				fprintf(fp, "%10d",SysInfo19.m_nData[block][piece]);
			}
			fprintf(fp,"\n");
		}

		break;





	case DATA_LOAD:	// 2

		fp=fopen(fName,"rt");
		if(fp==NULL){

			SysInfo19.m_nBlockTot=SysInfo03.m_nBlock;//block
			SysInfo19.m_nPieceTot=SysInfo25.m_nBlock;//piece
			SysInfo19.m_nCellTotal=SysInfo19.m_nBlockTot*SysInfo19.m_nPieceTot;
			SysInfo19.m_nType=1;
			SysInfo19.m_nUse=1;

			//FileSysInfo01.LoadSaveSub19(1,1); //save//sylee160203

			fp=fopen(fName,"rt");//sylee160203
			if(fp == NULL){	//sylee160203
				str.Format( "   Error No 2218, \n\n       FILE Loading Error !!!   \n\n     %s \n ",fName ); 
				AfxMessageBox(str);		
			}

				str.Format( "   Error No 2218, \n\n       FILE Loading Error !!!   \n\n     %s \n ",fName ); 
			return FALSE;
		}

		//-----------------------------------------------------
		// BlockDis.ini에서 block 갯수, piece갯수를 read 
		
		fscanf(fp, "%10d\n", &SysInfo19.m_nBlockTot);  //block
		fscanf(fp, "%10d\n", &SysInfo19.m_nPieceTot);  //piece

		// Range Check
		if (SysInfo19.m_nBlockTot <= 0 || SysInfo19.m_nBlockTot >= Def_MaxBlock1)
		{
			strTemp.Format("LoadSaveSub19(): SysInfo19.m_nBlockTot=%d Range Over! (1 <= m_nRow <= %d)\n", 
					SysInfo19.m_nBlockTot, (Def_MaxBlock1 -1) );
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}
		if (SysInfo19.m_nPieceTot <= 0 || SysInfo19.m_nPieceTot >= Def_MaxPiece1)
		{
			strTemp.Format("LoadSaveSub19(): SysInfo19.m_nPieceTot=%d Range Over! (1 <= m_nCol <= %d)\n", 
					SysInfo19.m_nPieceTot, (Def_MaxPiece1 -1) );
			AfxMessageBox(strTemp, MB_ICONSTOP);
			return FALSE;
		}


		//-------------------------------------
		// BlockDis.ini 파일 나머지 모두 read  
		
		if((SysInfo19.m_nBlockTot==SysInfo03.m_nBlock)&& (SysInfo19.m_nPieceTot==SysInfo25.m_nBlock))
		{ //piece  //sylee160126-5

			fscanf(fp, "%10d\n", &SysInfo19.m_nCellTotal);
			fscanf(fp, "%10d\n", &SysInfo19.m_nType);
			fscanf(fp, "%10d\n", &SysInfo19.m_nUse);

			SysInfo19.m_nCheck=0;//sylee131117


			// 먼저 0으로 초기화.
			for(block=1; block < Def_MaxBlock1; block++){
				for(piece=1; piece < Def_MaxPiece1; piece++)
					SysInfo19.m_nData[block][piece] =0; 
			}

			if(SysInfo19.m_nUse == 1) //sylee131123
			{
				for(block=1; block <= SysInfo19.m_nBlockTot; block++)
				{
					for(piece=1; piece <= SysInfo19.m_nPieceTot; piece++)
					{
						fscanf(fp, "%10d", &SysInfo19.m_nData[block][piece]);		 

						if(SysInfo19.m_nData[block][piece]==1)
							SysInfo19.m_nCheck=1;//sylee131117

					}
				} 
			}
	   }
	   else
	   {

		   strTemp.Format("LoadSaveSub19():  Logic Error! \n Check (SysInfo19.m_nBlockTot(=%d) == SysInfo03.m_nBlock(=%d))? \n Check (SysInfo19.m_nPieceTot(=%d) == SysInfo25.m_nBlock(=%d)) ?\n ", 
				   SysInfo19.m_nBlockTot, SysInfo03.m_nBlock, SysInfo19.m_nPieceTot, SysInfo25.m_nBlock);
		   AfxMessageBox(strTemp, MB_ICONSTOP);

		   // 값을 다시 할당해서 사용.
		   SysInfo19.m_nBlockTot=SysInfo03.m_nBlock;  //sylee160126-5
		   SysInfo19.m_nPieceTot=SysInfo25.m_nBlock;  //block //sylee160126-5

		   SysInfo19.m_nCellTotal=SysInfo19.m_nBlockTot*SysInfo19.m_nPieceTot; //sylee160126-5
		   SysInfo19.m_nType=1; //sylee160126-5
		   SysInfo19.m_nUse=0; //sylee160126-5

		   SysInfo19.m_nCheck=0;//sylee160126-5

		   for(block=1;block<301;block++){//sylee160126-5
			   for(piece=1;piece<201;piece++){//sylee160126-5
				   SysInfo19.m_nData[block][piece] =0; //sylee160126-5
			   }
		   }			  

	   }




	    int nCo1;//sylee170417-1


		if(SysInfo19.m_nUse)
		{
			for( block = 1 ; block <=SysInfo19.m_nBlockTot ; block++) 
			{ 
				nCo1=0;//sylee170417-1
				for( int  piece = 1 ; piece <=SysInfo19.m_nPieceTot ;  piece++) 
				{ 
					if(SysInfo19.m_nData[block][piece]==1){
						nCo1++;//sylee170417-1
					}
				}

			/* son 컴파일을 위해서 막음
				if(nCo1==SysInfo19.m_nPieceTot){//sylee170417-1
					nBlockMaxLast1=block-1;//sylee170417-1
				}else{
					nBlockMaxLast1=SysInfo19.m_nBlockTot;//sylee170417-1   nBlockMax
				} 
			*/
			}
		} 
		break;



	default :
		break;
	}

	fclose(fp);
	return TRUE;
}


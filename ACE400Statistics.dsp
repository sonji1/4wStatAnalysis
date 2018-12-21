# Microsoft Developer Studio Project File - Name="ACE400Statistics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ACE400Statistics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ACE400Statistics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ACE400Statistics.mak" CFG="ACE400Statistics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ACE400Statistics - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ACE400Statistics - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ACE400Statistics - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /O2 /I "./gridctrl_src/" /I "./ChartDirector/" /I "./XLAutomation/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ./chartdir60.lib /nologo /stack:0x300000 /subsystem:windows /incremental:yes /debug /machine:I386

!ELSEIF  "$(CFG)" == "ACE400Statistics - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./gridctrl_src/" /I "./ChartDirector/" /I "./XLAutomation/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ./chartdir60.lib /nologo /stack:0x200000 /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ACE400Statistics - Win32 Release"
# Name "ACE400Statistics - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ACE400MainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ACE400Statistics.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\FileSysInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FrRankDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\GageDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\InPlaceList.cpp
# End Source File
# Begin Source File

SOURCE=.\PChartDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StatisticsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TDist.cpp
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\TitleTip.cpp
# End Source File
# Begin Source File

SOURCE=.\XLAutomation\XLAutomation.cpp
# End Source File
# Begin Source File

SOURCE=.\XLAutomation\XLEzAutomation.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ACE400MainDlg.h
# End Source File
# Begin Source File

SOURCE=.\ACE400Statistics.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\ChartDirector\include\chartdir.h
# End Source File
# Begin Source File

SOURCE=.\ChartViewer.h
# End Source File
# Begin Source File

SOURCE=.\ConfigDialog.h
# End Source File
# Begin Source File

SOURCE=.\Error.h
# End Source File
# Begin Source File

SOURCE=.\FileSysInfo.h
# End Source File
# Begin Source File

SOURCE=.\FrRankDialog.h
# End Source File
# Begin Source File

SOURCE=.\GageDialog.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\InPlaceList.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\PChartDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StatisticsDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\gridctrl_src\TitleTip.h
# End Source File
# Begin Source File

SOURCE=.\XLAutomation\XLAutomation.h
# End Source File
# Begin Source File

SOURCE=.\XLAutomation\XLEzAutomation.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ACE400Statistics.ico
# End Source File
# Begin Source File

SOURCE=.\ACE400Statistics.rc
# End Source File
# Begin Source File

SOURCE=.\res\ACE400Statistics.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Bioptro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Bioptro.ico
# End Source File
# Begin Source File

SOURCE=.\res\tree.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

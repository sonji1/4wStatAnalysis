; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CStatisticsDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ace400statistics.h"
LastPage=0

ClassCount=6
Class1=CAboutDlg
Class2=CACE400MainDlg
Class3=CACE400StatisticsApp
Class4=CConfigDialog
Class5=CStatisticsDialog

ResourceCount=5
Resource1=IDD_MAIN_DIALOG
Resource2=IDD_STATISTICS_DIALOG
Resource3=IDD_CONFIG_DIALOG
Resource4=IDD_ABOUTBOX
Class6=CPChartDialog
Resource5=IDD_PCHART_DIALOG

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ACE400MainDlg.cpp
ImplementationFile=ACE400MainDlg.cpp
LastObject=IDOK

[CLS:CACE400MainDlg]
Type=0
BaseClass=CDialog
HeaderFile=ACE400MainDlg.h
ImplementationFile=ACE400MainDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CACE400MainDlg

[CLS:CACE400StatisticsApp]
Type=0
BaseClass=CWinApp
HeaderFile=ACE400Statistics.h
ImplementationFile=ACE400Statistics.cpp

[CLS:CConfigDialog]
Type=0
BaseClass=CDialog
HeaderFile=ConfigDialog.h
ImplementationFile=ConfigDialog.cpp
LastObject=CConfigDialog

[CLS:CStatisticsDialog]
Type=0
BaseClass=CDialog
HeaderFile=StatisticsDialog.h
ImplementationFile=StatisticsDialog.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_CHECK_DATA_FAULT_ONLY

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_MAIN_DIALOG]
Type=1
Class=CACE400MainDlg
ControlCount=6
Control1=IDOK,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_CHECK_STATISTICS,button,1342246915
Control5=IDC_CHECK_PCHART,button,1342246915
Control6=IDC_CHECK_CONFIG,button,1342246915

[DLG:IDD_CONFIG_DIALOG]
Type=1
Class=CConfigDialog
ControlCount=9
Control1=IDC_CHECK_PRT_BASIC,button,1342246915
Control2=IDC_STATIC,button,1342177287
Control3=IDC_CHECK_PRT_LEVEL1,button,1342246915
Control4=IDC_CHECK_PRT_Level2,button,1342246915
Control5=IDC_CHECK_PRT_LEVEL3,button,1342246915
Control6=IDC_CHECK_PRT_DEVICE,button,1342246915
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC_PRINT_STATE,static,1342308865
Control9=IDC_CHECK_PRT_DEV_D64,button,1342246915

[DLG:IDD_STATISTICS_DIALOG]
Type=1
Class=CStatisticsDialog
ControlCount=21
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GRID_DATA,MFCGridCtrl,1342242816
Control3=IDC_TREE,SysTreeView32,1350631479
Control4=IDC_EDIT_SELECT,edit,1350633600
Control5=IDC_GRID_SUMMARY,MFCGridCtrl,1342242816
Control6=IDC_COMBO_DATE,combobox,1344340226
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BUTTON_LOAD4W_DATA,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_TUPLE_NUM,edit,1350633600
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_SAMPLE_NUM,edit,1350633600
Control13=IDC_STATIC,static,1208090624
Control14=IDC_EDIT_LSL,edit,1216415872
Control15=IDC_STATIC,static,1208090624
Control16=IDC_EDIT_USL,edit,1216415872
Control17=IDC_BUTTON_CLEAR_GRID,button,1342242816
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_BUTTON_LOAD4W_DATA_SINGLE,button,1342242816
Control21=IDC_CHECK_DATA_FAULT_ONLY,button,1342242819

[DLG:IDD_PCHART_DIALOG]
Type=1
Class=CPChartDialog
ControlCount=19
Control1=IDC_COMBO_LOT,combobox,1344340226
Control2=IDC_COMBO_DATE2,combobox,1344340226
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_CHART,static,1342177294
Control6=IDC_STATIC,static,1342308352
Control7=IDC_BUTTON_DIS_PCHART,button,1208025088
Control8=IDC_BUTTON_SAVE_PCHART,button,1342242816
Control9=IDC_GRID_PCHART_OOC,MFCGridCtrl,1342242816
Control10=IDC_EDIT_PCAHRT_NET_CNT,edit,1350633600
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_PCAHRT_OOC_CNT,edit,1350633600
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,button,1342177287
Control15=IDC_CHECK_OOCNET_ONLY,button,1342242819
Control16=IDC_CHECK_FAULT_ONLY,button,1342242819
Control17=IDC_EDIT_PCAHRT_FAULT_CNT,edit,1350633600
Control18=IDC_STATIC,static,1342308352
Control19=IDC_BUTTON_VIEW_PCHART_CSV,button,1342242816

[CLS:CPChartDialog]
Type=0
HeaderFile=PChartDialog.h
ImplementationFile=PChartDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_BUTTON_VIEW_PCHART_CSV
VirtualFilter=dWC


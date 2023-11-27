// Module:
// Notices: Copyright(c) 2016 Jim Jian.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.1.0.6
//---------------------------------------------------------------------------

//#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "DeviceNotify.h"
#include "Hub_fun.h"
#include "logging.h"
#include "Info.h"
#include "UnitUSB.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
Port_test *PORT = new Port_test();
HANDLE lan, threadwork, mac;
DeviceNotification g_DeviceNogification;
clock_t start, end;
AnsiString Buildday = "PingTool ver 1.0.0 B20171120";
AnsiString RUNTIME;		//程式開始執行時間(yymmddhhmmss)
AnsiString LOGFILE, INIFILE; //存檔路徑
FILE * fp;
AnsiString ERRMSG;
bool PROGPass, ReadGood, isEC, CHKERR, GETDEVICE;
void SaveLogLing(AnsiString FileName,AnsiString FileLine);
AnsiString deviceVPID;
AnsiString lanname;
DWORD TestTime = 0;

GUID guid;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	OldFormWndProc = this->WindowProc;
	this->WindowProc = FormWndProc;
	// 註冊USB裝置
	guid =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);//	GUID_CLASS_USB_DEVICE_USB);
}

void __fastcall TfrmMain::FormWndProc(TMessage& Message) // 攔截設備訊息
{
	PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	OldFormWndProc(Message);
	if (Message.Msg == WM_DEVICECHANGE) { // 有設備拔插
		if (!TestWorking) {
			pDevInf = PDEV_BROADCAST_DEVICEINTERFACE(Message.LParam);
			if (Message.WParam == DBT_DEVICEARRIVAL) // 新插入了一項裝置!
			{
				if(pDevInf->dbcc_classguid == guid){
					AnsiString dbccname = pDevInf->dbcc_name;
					bool deviceout;
					lanname = "NULL";
					int timeout = 0;
					while(lanname == "NULL"){
						if(timeout == 50)
						break;
						lanname = PORT->GetLANName();
						timeout++;
						PORT->Delay(100);
					}
					if(lanname == "NULL"){
						lab_devicename->Caption = "";
						current_mac->Caption = "未插入設備";
					}
					else{
						deviceVPID = PORT->GetLanPVID(lanname);
						if(dbccname.Pos(deviceVPID)){
							deviceout = PORT->FindDevice(deviceVPID);
						}
					}
				}
			}
			if (Message.WParam == DBT_DEVICEREMOVECOMPLETE) // 新拔出了一項裝置!
			{
				if(pDevInf->dbcc_classguid == guid){
					AnsiString dbccname = pDevInf->dbcc_name;
					bool deviceout;
					deviceout = PORT->FindDevice(deviceVPID);
					if(!deviceout){
						Panel_lan->Color = clWhite;
						Panel_lan->Caption = "Wait";
						pan_10->Color = clWhite;
						pan_100->Color = clWhite;
						pan_1000->Color = clWhite;
						frmMain->Height = 255;
						plErrMsg->Caption = "";
						lab_devicename->Caption = "";
						frmMain->lab_time->Caption = "0";
						current_mac->Caption = "未插入設備";
						HaveDevice = false;
					}
				}
			}
		}
	}
}


DWORD WINAPI ThreadWorkExecute(LPVOID Param){
	int Step = 0, costTime = 0;
	while (frmMain->TestWorking){
		switch (Step) {
		case 0 :
			lanname = PORT->GetLANName();
			if(lanname != "NULL")
			{
				deviceVPID = PORT->GetLanPVID(lanname);
				TestTime = 0;

				frmMain->TestResult = true;
				frmMain->HaveDevice = true;
				frmMain->current_mac->Caption = PORT->getLanMAC(lanname); //顯示當前設備號
				Step = 1;
			}
			else
			{
				frmMain->lab_devicename->Caption = "";
				frmMain->current_mac->Caption = "未插入設備";
			}
			break;
		case 1:
			frmMain->Panel_lan->Caption = "Testing";
			if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt",
						"TEST ITEM", 2)=="LAN_true" && frmMain->TestResult)
			{
				if(PORT->Analyze_USBView(deviceVPID.SubString(5,4), deviceVPID.SubString(14,4))){
					//網路測試
					strcpy(zTestResult[1].name, "LAN");
					lan = ::CreateThread(0, 1024, LANExecute, (LPVOID) & zTestResult[1], 0,
						NULL);
						WaitForSingleObject(lan, INFINITE);
						CloseHandle(lan);
				}else{
					frmMain->pan_1000->Color = clRed;
					frmMain->TestResult = false;
					frmMain->Panel_lan->Color = clRed;
					frmMain->Panel_lan->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->plErrMsg->Caption = "USB3.0 FAIL";
				}
			}

			frmMain->Refresh();
			Step = 2;
		break;
		case 2:
			if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt","TEST ITEM", 3) != "AUTORUN_true"){
				frmMain->TestWorking = false;
			}
			if(!PORT->FindDevice(deviceVPID))
			{
				frmMain->Panel_lan->Color = clWhite;
				frmMain->Panel_lan->Caption = "Wait";
				frmMain->pan_10->Color = clWhite;
				frmMain->pan_100->Color = clWhite;
				frmMain->pan_1000->Color = clWhite;
				frmMain->Height = 255;
				frmMain->plErrMsg->Caption = "";
				frmMain->lab_devicename->Caption = "";
				frmMain->current_mac->Caption = "未插入設備";
				frmMain->HaveDevice = false;
				frmMain->lab_time->Caption = 0;
				if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
				Step = 0;
			}
			break;
		}
		frmMain->Refresh();
		Sleep(500);
	}
	CloseHandle(threadwork);
	return 1;
}
DWORD WINAPI LANExecute(LPVOID Param){
	st_ThreadParam *pParam;
	pParam = (st_ThreadParam*)Param;
	// 改用fping測速
	AnsiString PVID = PORT->GetLanPVID(lanname);
	if (PORT->ChangeLanSpeed(PVID, "{4d36e972-e325-11ce-bfc1-08002be10318}", lanname)){
		frmMain->Lan_test_result(1);
		pParam->result = 1;
		return 1;
	}
	else{
		frmMain->Lan_test_result(-1);
		pParam->result = -1;
		return -1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Lan_test_result(int result){
	switch(result) {
	case 1:
		Panel_lan->Color = clGreen;
		frmMain->Panel_lan->Caption = "PASS";
		break;
	case -1:
		frmMain->TestResult = false;
		Panel_lan->Color = clRed;
		frmMain->Panel_lan->Caption = "FAIL";
		break;
	default:
		frmMain->TestResult = false;
		Panel_lan->Color = clRed;
		frmMain->Panel_lan->Caption = "FAIL";
		break;
	}
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	lab_time->Caption = "0";
	if(!TestWorking && !AutoTest){
		if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt","TEST ITEM", 3) == "AUTORUN_true")
			AutoTest = true;
		else
			AutoTest = false;
		TestResult = true; //預設true  網路切速緊急停止會需要
		TestWorking = true;
		//初始元件
		Panel_lan->Color = clWhite;
		frmMain->Panel_lan->Caption = "Wait";
		pan_10->Color = clWhite;
		pan_100->Color = clWhite;
		pan_1000->Color = clWhite;
		frmMain->Height = 255;
		if(AutoTest){
			btn_start->Caption = L"停止";
		}else{
			btn_start->Enabled = false;
		}
		// 呼叫LAN執行檔
		strcpy(zTestResult[0].name, "ThreadWork");
		threadwork = ::CreateThread(0, 1024, ThreadWorkExecute, (LPVOID) & zTestResult[0], 0,
			NULL);
	}else{
		AutoTest = false;
		TestWorking = false;
		TestResult = false;
		btn_start->Caption = L"開始";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
/////////////////////////////////////////// 限制只能執行一次
	mx = CreateMutex( NULL, true, L"AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	frmMain->N1Click(NULL);
///////////////////////////////////////////////////////////////////
	RUNTIME = FormatDateTime("yymmddhhmmss",Now());
	lanname = "NULL";
	int timeout = 0;
	lanname = PORT->GetLANName();
	if(lanname!="NULL")
	{
		deviceVPID = PORT->GetLanPVID(lanname);
		HaveDevice = true;
	}
	else
	{
		lab_devicename->Caption = "";
		current_mac->Caption = "未插入設備";
	}
}
//---------------------------------------------------------------------------
void SaveLogLing(AnsiString FileName,AnsiString FileLine)
{
	fp = fopen(FileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
}

void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	ReleaseMutex(mx);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TestTimeTimerTimer(TObject *Sender)
{
	if(frmMain->Panel_lan->Caption == "Testing")
	{
		if(TestTime == 0)
			TestTime = GetTickCount();
		lab_time->Caption = AnsiString((GetTickCount()-TestTime)/1000);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::N1Click(TObject *Sender)
{
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = "{4d36e972-e325-11ce-bfc1-08002be10318}";
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	for (int i = 0; i <= 9999; i++) {
	//路徑名稱補0
		if (i < 10)
			LANnum = "000" + (AnsiString)i;
		if (i >= 10 && i < 100)
			LANnum = "00" + (AnsiString)i;
		if (i >= 100 && i < 1000)
			LANnum = "0" + (AnsiString)i;
		if (i >= 1000 && i < 10000)
			LANnum = (AnsiString)i;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + lanGUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");

		if(tempDeviceID.LowerCase().Pos("realtek usb")||tempDeviceID.LowerCase().Pos("displaylink")){
			if(reg->ValueExists ("NetworkAddress"))
				reg->DeleteValue("NetworkAddress");
		}
	}
	reg->CloseKey();
	delete reg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ckbNoMACClick(TObject *Sender)
{
	N1Click(NULL);
}
//---------------------------------------------------------------------------



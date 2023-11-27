// Module:
// Notices: Copyright(c) 2018 Leno.
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.0
//---------------------------------------------------------------------------

#pragma hdrstop

#include "Main.h"
#include "DeviceNotify.h"
#include "Hub_fun.h"
#include "MSGBOX.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;

Port_test *PORT = new Port_test();
HANDLE lan;
DeviceNotification g_DeviceNogification;

DWORD TestTime = 0;
int   Test_Step;

AnsiString gLAN_NAME;
GUID guid;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
/////////////////////////////////////////// 限制只能執行一次
	mx = CreateMutex( NULL, true, L"AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	N1Click(NULL);

	OldFormWndProc = this->WindowProc;
	this->WindowProc = FormWndProc;
	// 註冊USB裝置
	guid =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);//	GUID_CLASS_USB_DEVICE_USB);

	TListInitLAN = new TStringList;
	PORT->IntiNetwordCard();
	frmMsg==NULL;
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
				if(pDevInf->dbcc_classguid == guid)
				{
					gLAN_NAME = "";
					ULONG timeout = GetTickCount()+3000;
					while(gLAN_NAME == ""){
						if(GetTickCount() >= timeout)
						break;
						gLAN_NAME = PORT->GetLANName();
						PORT->Delay(100);
					}
					if(gLAN_NAME == ""){
						lab_devicename->Caption = "";
						current_mac->Caption = "Not Find Device";
					}
					else
					{
						g_DevVPID = PORT->GetLanPVID(gLAN_NAME);
						frmMain->Refresh();
					}
				}
			}
			if (Message.WParam == DBT_DEVICEREMOVECOMPLETE) // 新拔出了一項裝置!
			{
				if(pDevInf->dbcc_classguid == guid)
				{
					if(!PORT->FindDevice(g_DevVPID))
					{
						plResult->Color = clWhite;
						plResult->Caption = "Wait";
						pan_1000->Color = clWhite;
						frmMain->Height = 255;
						plErrMsg->Caption = "";
						lab_devicename->Caption = "";
						frmMain->plTime->Caption = "0";
						current_mac->Caption = "Not find Device";
					}
				}
			}
		}
	}
}

DWORD WINAPI LANExecute(LPVOID Param)
{
	AnsiString PVID = PORT->GetLanPVID(gLAN_NAME);
	PORT->ChangeLanSpeed(PVID, "{4d36e972-e325-11ce-bfc1-08002be10318}", gLAN_NAME);
	return 0;
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	TestTime = 0;
	plTime->Caption = "0";
	if(!TestWorking && !AutoTest)
	{
		plSet->Enabled = false;
		AutoTest = true;
		//初始元件
		plResult->Color = clWhite;
		plResult->Caption = "Wait";
		pan_1000->Color = clWhite;
		frmMain->Height = 255;
		if(AutoTest){
			btn_start->Caption = "Stop";
		}else{
			btn_start->Enabled = false;
		}
		// 呼叫LAN執行檔
		TestWorking = true;
		Test_Step = GET_DUT;
		TimerMain->Enabled = true;
	}else{
		TimerMain->Enabled = false;
		frmMain->g_bError = true;
		plSet->Enabled = true;
		AutoTest = false;
		TestWorking = false;
		btn_start->Caption = "Start";
		N1Click(NULL);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
 	N1Click(NULL);
	if(lan!=NULL) CloseHandle(lan);
	ReleaseMutex(mx);
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::N1Click(TObject *Sender)
{
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = "{4d36e972-e325-11ce-bfc1-08002be10318}";
	TRegistry *reg = new TRegistry();
	TStringList *ts = new TStringList;
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + lanGUID;
	reg->OpenKey(temppath.c_str(), false);
	reg->GetKeyNames(ts);
	for (int i = 0; i < ts->Count; i++) {
		LANnum = ts->Strings[i];
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + lanGUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		if(reg->ValueExists ("NetworkAddress"))
			reg->DeleteValue("NetworkAddress");
	}
	reg->CloseKey();
	delete reg;
	delete ts;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ckbNoMACClick(TObject *Sender)
{
	N1Click(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TimerMainTimer(TObject *Sender)
{
	if(frmMain->TestWorking)
	{
		frmMain->TestWorking = false;
		switch (Test_Step)
		{
			case GET_DUT :
				gLAN_NAME = PORT->GetLANName();
				if(gLAN_NAME != "")
				{
					g_DevVPID = PORT->GetLanPVID(gLAN_NAME);
					plResult->Caption = "Testing";
					ERROR_MSG		  = "";
					TestTime = GetTickCount();

					frmMain->g_bError	= false;
					frmMain->current_mac->Caption = PORT->getLanMAC(gLAN_NAME); //顯示當前設備號
					Test_Step = USB3_TEST;
				}
				else
				{
					frmMain->lab_devicename->Caption = "";
					frmMain->current_mac->Caption = "Not find Device";
				}
				break;
			case USB3_TEST:
				if(PORT->Analyze_USBView(g_DevVPID.SubString(5,4), g_DevVPID.SubString(14,4))){
					//網路測試
					Test_Step = frmMain->ckbBarcode->Checked ?  SCAN_BARCODE:LAN_TEST;
				}else{
					frmMain->pan_1000->Color = clRed;
					frmMain->plResult->Color = clRed;
					frmMain->plResult->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->plErrMsg->Caption = "USB3.0 FAIL";
					frmMain->g_bError	= true;
					Test_Step = WAIT_DUT_PLUGOUT;
				}
				break;
			case SCAN_BARCODE:
				if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);
				frmMsg->edtBarcode->Text = "" ;
				if(frmMsg->ShowModal()== mrOk)
				{
					AnsiString Mac = frmMain->current_mac->Caption;
					Mac = Mac.SubString(1,2)+Mac.SubString(4,2)+Mac.SubString(7,2)+Mac.SubString(10,2)+Mac.SubString(13,2)+Mac.SubString(16,2);
					if(Mac == frmMsg->edtBarcode->Text)
					{
						Test_Step = LAN_TEST;
						delete frmMsg;
						frmMsg = NULL;
						break;
					}
				}
				frmMain->plResult->Color = clRed;
				frmMain->plResult->Caption = "FAIL";
				frmMain->Height = 289;
				frmMain->plErrMsg->Caption = "(!)Scan Fail: " + frmMsg->edtBarcode->Text;
				frmMain->g_bError	= true;
				plTime->Caption = AnsiString(double(GetTickCount()-TestTime)/(double)1000);
				Test_Step = WAIT_DUT_PLUGOUT;
				frmMsg = NULL;
				delete frmMsg;

				break;
				//
			case LAN_TEST:
				popRegDriverClick(NULL);
				//
				frmMain->plResult->Caption = "Testing";
				//網路測試
				lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
				Test_Step = LAN_TEST_RESULT;
				break;
			case LAN_TEST_RESULT:
				if(ERROR_MSG!="")
				{
					frmMain->plErrMsg->Caption = ERROR_MSG;
					plResult->Color = clRed;
					plResult->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->pan_1000->Color = clRed;
					lan = NULL;
					CloseHandle(lan);
					Test_Step = WAIT_DUT_PLUGOUT;
					break;
				}
				else if(frmMain->pan_1000->Color == clGreen)
				{
					plResult->Color = clGreen;
					plResult->Caption = "PASS";
					lan = NULL;
					CloseHandle(lan);
					Test_Step = WAIT_DUT_PLUGOUT;
					break;
				}


				break;
			case WAIT_DUT_PLUGOUT:
				if(!PORT->FindDevice(g_DevVPID))
				{
					frmMain->plResult->Color = clWhite;
					frmMain->plResult->Caption = "Wait";
					frmMain->pan_1000->Color = clWhite;
					frmMain->Height = 255;
					frmMain->plErrMsg->Caption = "";
					frmMain->lab_devicename->Caption = "";
					frmMain->current_mac->Caption = "Not find Device";
					frmMain->plTime->Caption = 0;
					if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
					Test_Step = GET_DUT;
				}
				break;
		}
		frmMain->Refresh();
		frmMain->TestWorking = true;
	}
	if(frmMain->plResult->Caption == "Testing")
	{
		if(TestTime == 0)
			TestTime = GetTickCount();
		plTime->Caption = AnsiString(double(GetTickCount()-TestTime)/(double)1000);
	}
	else
	{
		if(!PORT->FindDevice(g_DevVPID))
		{
			if(current_mac->Caption != "Not find Device")
			{
				plResult->Color = clWhite;
				plResult->Caption = "Wait";
				pan_1000->Color = clWhite;
				frmMain->Height = 255;
				plErrMsg->Caption = "";
				lab_devicename->Caption = "";
				frmMain->plTime->Caption = "0";
				current_mac->Caption = "Not find Device";
			}
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::popRegDriverClick(TObject *Sender)
{
	if(g_DevVPID != "")
	{
		TRegistry *reg = new TRegistry();
		reg->RootKey = HKEY_LOCAL_MACHINE;
		AnsiString temppath = "SYSTEM\\CurrentControlSet\\Control\\usbflags";
		reg->OpenKey(temppath.c_str(), false);
		AnsiString tempIgnoreHWSerNum = "IgnoreHWSerNum"+g_DevVPID.SubString(5,4)+g_DevVPID.SubString(14,4);
		if(!reg->ValueExists (tempIgnoreHWSerNum))
		{
			BYTE buf[1];
			buf[0] = 0x01;
			reg->WriteBinaryData(tempIgnoreHWSerNum,buf,1);
		}
		reg->CloseKey();
		delete reg;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::plResultClick(TObject *Sender)
{
	PORT->VPIDGetLanName("VID_0B95");
}
//---------------------------------------------------------------------------


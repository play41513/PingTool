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
#pragma link "IdBaseComponent"
#pragma link "IdComponent"
#pragma link "IdIcmpClient"
#pragma link "IdRawBase"
#pragma link "IdRawClient"
#pragma resource "*.dfm"
TfrmMain *frmMain;

Port_test *PORT = new Port_test();
HANDLE lan;
DeviceNotification g_DeviceNogification;

DWORD TestTime = 0;
int   Test_Step;

AnsiString gFileName;
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
	// 註冊USB裝置
	guid =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);//	GUID_CLASS_USB_DEVICE_USB);

	TListInitLAN = new TStringList;
	PORT->IntiNetwordCard();
	gFileName = "LOG\\"+FormatDateTime("mm-dd_hh-nn-ss", Now())+".txt";
	frmMsg==NULL;
}

DWORD WINAPI LANExecute(LPVOID Param)
{
	PORT->ChangeLanSpeed(frmMain->g_DevVPID, "{4d36e972-e325-11ce-bfc1-08002be10318}", frmMain->gLAN_NAME);
	return 0;
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	TestTime = 0;
	plTime->Caption = "0";
	if(!TestWorking && !AutoTest)
	{
		SaveLogLine("Start");
		btn_start->Enabled = false;
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
		// 呼叫LAN Timer
		TestWorking = true;
		Test_Step = GET_DUT;
		//TimerMain->Enabled = true;
		btn_start->Enabled = true;
	}else{
		SaveLogLine("Stop");
		btn_start->Enabled = false;
		//TimerMain->Enabled = false;
		frmMain->g_bError = true;
		plSet->Enabled = true;
		AutoTest = false;
		TestWorking = false;
		btn_start->Caption = "Start";
		N1Click(NULL);
		btn_start->Enabled = true;
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
	gLAN_NAME = PORT->GetLANName();
	if(frmMain->TestWorking)
	{
		frmMain->TestWorking = false;
		switch (Test_Step)
		{
			case GET_DUT :
				if(gLAN_NAME != "")
				{

					g_DevVPID = PORT->GetLanPVID(gLAN_NAME);
					plResult->Caption = "Testing";
					ERROR_MSG		  = "";
					TestTime = GetTickCount();

					frmMain->g_bError	= false;
					frmMain->current_mac->Caption = PORT->getLanMAC(gLAN_NAME); //顯示當前設備號
					Test_Step = USB3_TEST;
					SaveLogLine("Get NetworkCard: [VPID]"+g_DevVPID+"[NAME]"+gLAN_NAME+"[MAC]"+AnsiString(current_mac->Caption));
				}
				break;
			case USB3_TEST:
				btn_start->Enabled = false;
				SaveLogLine("Check USB3.0");
				if(PORT->Analyze_USBView(g_DevVPID.SubString(5,4), g_DevVPID.SubString(14,4))){
					//網路測試
					SaveLogLine("USB3.0 PASS");
					Test_Step = frmMain->ckbBarcode->Checked ?  SCAN_BARCODE:LAN_TEST;
				}else{
					frmMain->pan_1000->Color = clRed;
					frmMain->plResult->Color = clRed;
					frmMain->plResult->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->plErrMsg->Caption = "USB3.0 FAIL";
					frmMain->g_bError	= true;
					Test_Step = WAIT_DUT_PLUGOUT;
					SaveLogLine("USB3.0 FAIL");
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
				SaveLogLine("Device Driver Regedit Check");
				//
				frmMain->plResult->Caption = "Testing";
				//網路測試
				lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
				SaveLogLine("Lan Test Start");
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
					frmMain->SaveLogLine("TEST FAIL");
					Test_Step = WAIT_DUT_PLUGOUT;
					frmMain->SaveLogLine("Waiting PlugOut");
					break;
				}
				else if(frmMain->pan_1000->Color == clGreen)
				{
					plResult->Color = clGreen;
					plResult->Caption = "PASS";
					lan = NULL;
					CloseHandle(lan);
					frmMain->SaveLogLine("TEST PASS");
					Test_Step = WAIT_DUT_PLUGOUT;
					frmMain->SaveLogLine("Waiting PlugOut");
					break;
				}


				break;
			case WAIT_DUT_PLUGOUT:
				btn_start->Enabled = true;
				if(gLAN_NAME == "")
				{
					frmMain->SaveLogLine("Device PlugOut :Timer");
					frmMain->plResult->Color = clWhite;
					frmMain->plResult->Caption = "Wait";
					frmMain->pan_1000->Color = clWhite;
					frmMain->Height = 255;
					frmMain->plErrMsg->Caption = "";
					frmMain->plTime->Caption = 0;
					if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
					Test_Step = GET_DUT;
					frmMain->SaveLogLine("Waiting Device PlugIn...");
					frmMain->SaveLogLine("==============");
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
void  TfrmMain::SaveLogLine(AnsiString FileLine)
{
	FileLine = (FormatDateTime("hh:nn:ss", Now()))+"  "+FileLine+="\r\n";
	FILE * fp;
	fp = fopen(gFileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
}
bool TfrmMain::Ping(AnsiString IP)
{
   moDebug->Lines->Add("Pinging \""+IP+"\"...");
   int avg_time = 0;
   gPingTime = 0;
   try
   {
	  IdIcmpClient->Host=IP;
	  for(int i=0; i<3; i++)
      {
		 try
		 {
			AnsiString buff;
			IdIcmpClient->Ping(buff,0);
			Application->ProcessMessages();
         }
         catch(Exception &e)
		 {
			frmMain->ERROR_MSG = "PING 3秒內發生失敗(PING FAIL)";
			moDebug->Lines->Add("Request timed out.");
         }
      }
   }
   __finally
   {
	  avg_time = gPingTime/3;
	  moDebug->Lines->Add(avg_time);
   }
   moDebug->Lines->Add("End!");
   if(frmMain->ERROR_MSG == "")
   {
	   if(avg_time>10)
		frmMain->ERROR_MSG = "網速未達到1000M(Speed of PING Fail)";
	   else return true;
   }
   return false;

}
void __fastcall TfrmMain::IdIcmpClientReply(TComponent *ASender, const TReplyStatus *AReplyStatus)

{
	AnsiString szReport;
   if(AReplyStatus->FromIpAddress=="0.0.0.0" && AReplyStatus->TimeToLive==0)
   {
	frmMain->ERROR_MSG = "PING 3秒內發生失敗(PING FAIL)";
	  szReport="Request timed out.";
   }
   else
   {
	  if(AReplyStatus->MsRoundTripTime==0)
	  {
		 szReport.sprintf("Reply from %s: bytes=%d time<1ms TTL=%d",
			AReplyStatus->FromIpAddress, AReplyStatus->BytesReceived,
			AReplyStatus->TimeToLive);
	  }
	  else
	  {
		 szReport.sprintf("Reply from %s: bytes=%d time=%dms TTL=%d",
			AReplyStatus->FromIpAddress, AReplyStatus->BytesReceived,
			AReplyStatus->MsRoundTripTime, AReplyStatus->TimeToLive);
	  }
	  gPingTime+= AReplyStatus->MsRoundTripTime;
   }
   moDebug->Lines->Add(szReport);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::plResultDblClick(TObject *Sender)
{
	frmMain->Width = moDebug->Width ? 591 : 792;
}
//---------------------------------------------------------------------------


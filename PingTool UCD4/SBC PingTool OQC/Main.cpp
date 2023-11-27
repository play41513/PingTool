// Module:
// Notices: Copyright(c) 2018 Leno.
// Purpose:
// Compiler: Embarcadero RAD Studio 2010
// OS: Windows 8.1 x64
// Version: 1.0.0
//---------------------------------------------------------------------------

#pragma hdrstop

#include "Main.h"
#include "Hub_fun.h"
#include "WorkThread.h"
#include "USBDevConnectionInfo.h"
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
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();

AnsiString gFileName;
AnsiString szLanMsg = "";
int gPingTime;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
/////////////////////////////////////////// ����u�����@��
	mx = CreateMutexA( NULL, true, "AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	N1Click(NULL);


	TListInitLAN = new TStringList;
	PORT->IntiNetwordCard();
	gFileName = "C:\\ASMP\\PingTool\\LOG\\"+FormatDateTime("mm-dd_hh-nn-ss", Now())+".txt";
	_mkdir( "C:\\ASMP\\");
	_mkdir( "C:\\ASMP\\PingTool\\" );
	_mkdir( "C:\\ASMP\\PingTool\\LOG\\" );
	frmMain->DoubleBuffered = true;
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	plTime->Caption = "0";
	if(btn_start->Caption.Pos("Start"))
	{
		SaveLogLine("Start");
		btn_start->Caption = "Stop";
		btn_start->Enabled = false;
		plSet->Enabled = false;
		FormUIChange(UI_CLEAR,"");
		// �I�sLAN Thread
		dThreadStatus = THREAD_START;
		TWORKTHREAD *WORKTHREAD = new TWORKTHREAD(false);
		btn_start->Enabled = true;
	}else{
		SaveLogLine("Stop");
		btn_start->Enabled = false;
		//
		dThreadStatus = THREAD_STOP;
		while(true)
		{
			if(dThreadStatus == THREAD_FINSH)
				break;
			PORT->Delay(500);
		}
		//
		btn_start->Caption = "Start";
		N1Click(NULL);
		plSet->Enabled = true;
		btn_start->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	N1Click(NULL);
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
void TfrmMain::RegDriverClick()
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

void  TfrmMain::SaveLogLine(AnsiString FileLine)
{
	FileLine = (FormatDateTime("hh:nn:ss", Now()))+"  "+FileLine+="\r\n";
	FILE * fp;
	fp = fopen(gFileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //�g�J�@�����
	fclose(fp);
}
bool TfrmMain::Ping(AnsiString IP,bool bLast)
{
	szLanMsg = "";
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
			szLanMsg = "PING ����(PING FAIL)";
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
   if(szLanMsg == "")
   {
	   if(avg_time>10)
	   {
			szLanMsg = "���t���F��100M(Speed of PING Fail)";
	   }
	   else
	   {
			if(szLanMsg=="")
				return true;
	   }
   }
   if(bLast)
		frmMain->ERROR_MSG = szLanMsg;
   return false;

}
void __fastcall TfrmMain::IdIcmpClientReply(TComponent *ASender, const TReplyStatus *AReplyStatus)

{
	AnsiString szReport;
   if(AReplyStatus->FromIpAddress=="0.0.0.0" && AReplyStatus->TimeToLive==0)
   {
		szLanMsg = "PING ����(PING FAIL)";
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
	frmMain->Width = frmMain->Width == 811 ? 592 : 811;
}
//---------------------------------------------------------------------------
void TfrmMain::FormUIChange(DWORD UIResult,AnsiString ErrorMsg)
{
	switch(UIResult)
	{
		case UI_LAN_ERROR:
			pan_1000->Color = clRed;
			plResult->Color = clRed;
			plResult->Caption = "FAIL";
			Height = 289;
			plErrMsg->Caption = ErrorMsg;
			SaveLogLine(ErrorMsg);
			break;
		case UI_ERROR:
			plResult->Color = clRed;
			plResult->Caption = "FAIL";
			Height = 289;
			plErrMsg->Caption = ErrorMsg;
			SaveLogLine(ErrorMsg);
			break;
		case UI_CLEAR:
			plResult->Color = clWhite;
			plResult->Caption = "Wait";
			pan_1000->Color = clWhite;
			plMCU->Color = clWhite;
			plTurnOnOff->Color = clWhite;
			plSwitchSBC->Color = clWhite;
			Height = 255;
			plErrMsg->Caption = "";
			plTime->Caption = 0;
			break;
		case UI_PASS:
			plResult->Color = clGreen;
			plResult->Caption = "PASS";
			SaveLogLine("TEST PASS");
			Height = 255;
			break;
	}
	frmMain->Refresh();
}





void __fastcall TfrmMain::SBC1Click(TObject *Sender)
{
	if(btn_start->Caption.Pos("Start"))
	{
		if(HID_TurnOn())
		{
			Tx("03 13 80 20 20"); //Switch SBC Mode
			Rx();
			Tx("03 13 80 80 80");//all power on
			Rx();
		}
	}
}
//---------------------------------------------------------------------------
bool TfrmMain::HID_TurnOn()
{
	//�t�mHID����
	wchar_t HID_PID[5];
	wchar_t HID_VID[5];
	wcsncpy(HID_PID,HID_PVID+8,4);
	wcsncpy(HID_VID,HID_PVID+17,4);
	HID_PID[4]='\0';
	HID_VID[4]='\0';
	if(m_hid.Configure(HID_PID,HID_VID,L"",L"")){
		if(!m_hid.Open()){
			return false;
		}
	}else{
		return false;
	}
	HID_PID[4] = 0;
	HID_PID[4] = 0;

	return true;
}
void TfrmMain::Tx(AnsiString Value){
	int szTx, szCmd;
	unsigned char * cmd;
	TStringList * list;

	szTx = m_hid.GetTxBytes();
	m_or.AllocBuf(szTx);
	cmd = (unsigned char *)m_or.GetBufAddr();

	list = new TStringList();
	list->DelimitedText = Value;

	if(list->Count > szTx){
		szCmd = szTx;
	}else{
        szCmd = list->Count;
	}

	for(int i=0; i<szCmd; i++){
		list->Strings[i] = L"0x" + list->Strings[i];
		cmd[i] = list->Strings[i].ToInt();
	}

	HidD_SetOutputReport(m_hid.m_hWrite, cmd, szTx);
	//UI_DisplayCmd(cmd, szTx);

	delete list;
}
bool TfrmMain::Rx()
{
	DWORD nBytes;
	bool bl=false;
	AnsiString Value;

	if(!m_ir.AllocBuf(m_hid.GetRxBytes())){
		MessageBox(NULL, _T("FAIL!"), _T("Allocate read buffer"), MB_ICONSTOP);
		return false;
	}

	if(m_ir.Read(m_hid.m_hRead)){
		bl = true;
	}else{
		if(GetLastError() == ERROR_IO_PENDING){
			WaitForSingleObject(m_hid.m_hRead, 1000);
			if(GetOverlappedResult(m_hid.m_hRead, &m_ir, &nBytes, false)){
				bl = true;
			}
			CancelIo(m_hid.m_hRead);
		}
	}

	return bl;
}
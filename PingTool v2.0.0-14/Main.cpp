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
#include "MSGBOX.h"
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
HANDLE lan;

DWORD TestTime = 0;
int   Test_Step;

AnsiString strTemp  = "";
GUID guid;
AnsiString szLanMsg = "";
DWORD dwTestCount = 0;
AnsiString strLoadMAC 		= "";
AnsiString strLoadLANName 	= "";
int numBarcodeResult;
DWORD BarcodeTimeOut;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
/////////////////////////////////////////// 限制只能執行一次
	mx = CreateMutex( NULL, true, "AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	N1Click(NULL);


	TListInitLAN = new TStringList;
	PORT->IntiNetwordCard();
	gFileName = "C:\\ASMP\\log\\PingTool\\TEST_LOG.txt";
	ZeroMAC = "00249B:"+FormatDateTime("nnssms", Now()).SubString(1,6);
	gTestLog = "C:\\ASMP\\log\\PingTool\\"+FormatDateTime("mm-dd_hh-nn-ss", Now())+".txt";
	_mkdir( "C:\\ASMP\\");
	_mkdir( "C:\\ASMP\\log\\" );
	_mkdir( "C:\\ASMP\\log\\PingTool\\" );
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);
	ReadRegValue();
	if(frmMsg->ShowModal()== mrOk)
	{
		edtSetWorkOrderNumber->Caption  = frmMsg->edtSetWorkOrderNumber->Text;
		edtWorkStation->Caption = frmMsg->edtWorkStation->Text;
		edtSetModel->Caption = frmMsg->edtSetModel->Text ;
		edtSetEmployeeID->Caption = frmMsg->edtSetEmployeeID->Text;
		delete frmMsg;
		frmMsg = NULL;
		SetRegValue();
	}
	else
	{
		delete frmMsg;
		frmMsg = NULL;
		Close();
	}
}

DWORD WINAPI LANExecute(LPVOID Param)
{
	if(frmMain->gLAN_NAME == "")
	{
		if(GetTickCount()>frmMain->dwTimeout)
			return 0;
		frmMain->gLAN_NAME = PORT->GetLANName();

	}
	else
		PORT->ChangeLanSpeed(frmMain->g_DevVPID, "{4d36e972-e325-11ce-bfc1-08002be10318}", frmMain->gLAN_NAME);
	return 0;
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	TestTime = 0;
	plTime->Caption = "0";
	lbBarcode->Caption = "請掃描MAC驗證條碼";
	if(!TestWorking && !AutoTest)
	{
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
		writeLOG("",true);
		SetRegValue();
	}else{
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
				if(ckbBarcode->Checked && strLoadMAC == "" && btn_start->Caption.Pos("Stop"))
				{
					numBarcodeResult = 0;
					edtBarcodeMAC->Text = "";
					plBarcode->Height = 138;
					plBarcode->Top = (frmMain->Height/2)-(plBarcode->Height/2);
					plBarcode->Left = (frmMain->Width/2)-(plBarcode->Width/2);
					edtBarcodeMAC->SetFocus();
					while(true)
					{
						if(numBarcodeResult == BARCODE_FINISH)
						{
							TestTime = GetTickCount();
							lbBarcode->Font->Size = 32;
							lbBarcode->Caption = "請掃描MAC驗證條碼";
							plBarcode->Height = 0;
							BarcodeTimeOut = GetTickCount()+(15000);
							frmMain->Refresh();
							break;
						}
						if(numBarcodeResult == BARCODE_CHANEL)
						{
							plBarcode->Height = 0;
							frmMain->Refresh();
							btn_startClick(NULL);
							break;
						}
					   PORT->Delay(100);
					}
				}
				if(BarcodeTimeOut !=0 && btn_start->Caption.Pos("Stop"))
				{
					if(GetTickCount() > BarcodeTimeOut)
					{
						lbBarcode->Font->Size = 20;
						lbBarcode->Caption = "未偵測到待測物請重新掃描MAC";
						frmMain->plResult->Color = clRed;
						plResult->Caption = "FAIL";
						frmMain->Height = 293;
						frmMain->plErrMsg->Caption = "(!)TimeOUT";
						frmMain->g_bError	= true;
						writeLOG("(!)WAIT_BARCODE_TimeOUT",false);
						Test_Step = WAIT_DUT_PLUGOUT;
						writeLOG("(!)TimeOUT",false);
						strLoadMAC = "";
					}
					else
					{
						plTime->Caption = AnsiString(double(GetTickCount()-TestTime)/(double)1000);
                    }
				}
				if(gLAN_NAME != "")
				{
					g_DevVPID = USBCONInfo->DevNameGetVPID(gLAN_NAME);
					if(g_DevVPID == "")
					{
						frmMain->pan_1000->Color = clRed;
						frmMain->plResult->Color = clRed;
						plResult->Caption = "FAIL";
						frmMain->Height = 289;
						frmMain->plErrMsg->Caption = "(!)GET VPID FAIL";
						frmMain->g_bError	= true;
						writeLOG("(!)GET_VPID_FAIL",false);
						Test_Step = WAIT_DUT_PLUGOUT;
					}
					else
					{
						bPlugOut = false;
						plResult->Caption = "Testing";
						ERROR_MSG		  = "";
						TestTime = GetTickCount();

						frmMain->g_bError	= false;
						frmMain->current_mac->Caption = PORT->getLanMAC(gLAN_NAME); //顯示當前設備號
						Test_Step = MAC_LANNAME_VERIFY;
						dwTimeout = GetTickCount()+5000;
					}
				}
				break;
			case MAC_LANNAME_VERIFY:
				if(strLoadMAC != "")
					if(!current_mac->Caption.Pos(strLoadMAC))
						current_mac->Font->Color = clRed;
				if(strLoadLANName != "")
					if(!lab_devicename->Caption.Pos(strLoadLANName))
						lab_devicename->Font->Color = clRed;
				if(current_mac->Font->Color == clRed || lab_devicename->Font->Color == clRed)
				{
					frmMain->pan_1000->Color = clRed;
					frmMain->plResult->Color = clRed;
					frmMain->plResult->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->plErrMsg->Caption = "Comparison Failure";
					frmMain->g_bError	= true;
					writeLOG("Comparison_Failure",false);
					Test_Step = WAIT_DUT_PLUGOUT;
				}
				else
				{
					Test_Step = USB3_TEST;
				}
				if(ckbBarcode->Checked)
					strLoadMAC = "";
				break;
			case USB3_TEST:
				btn_start->Enabled = false;
				if(USBCONInfo->GetDevUSB(g_DevVPID.SubString(5,4), g_DevVPID.SubString(14,4))){
					//網路測試
					Test_Step = LAN_TEST;
				}else{
					frmMain->pan_1000->Color = clRed;
					frmMain->plResult->Color = clRed;
					frmMain->plResult->Caption = "FAIL";
					frmMain->Height = 289;
					frmMain->plErrMsg->Caption = "USB3.0 FAIL";
					frmMain->g_bError	= true;
					writeLOG("USB3.0_FAIL",false);
					Test_Step = WAIT_DUT_PLUGOUT;
				}
				break;
			case LAN_TEST:
				if(frmMain->gLAN_NAME == "" && GetTickCount()>dwTimeout)
				{
					ERROR_MSG = "Not Find NetworkCard";
					Test_Step = LAN_TEST_RESULT;
				}
				else if(frmMain->gLAN_NAME != "" )
				{
					popRegDriverClick(NULL);
					//
					frmMain->plResult->Caption = "Testing";
					//網路測試
					lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
					Test_Step = LAN_TEST_RESULT;
				}
				break;
			case LAN_TEST_RESULT:
				if(ERROR_MSG!="")
				{
					writeLOG(ERROR_MSG,false);
					//
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
					writeLOG("PASS",false);
					//
					plResult->Color = clGreen;
					plResult->Caption = "PASS";
					lan = NULL;
					CloseHandle(lan);
					Test_Step = WAIT_DUT_PLUGOUT;
					break;
				}


				break;
			case WAIT_DUT_PLUGOUT:
				btn_start->Enabled = true;
				if(gLAN_NAME == ""||frmMain->bPlugOut)
				{
					frmMain->current_mac->Font->Color = clBlue;
					frmMain->lab_devicename->Font->Color = clBlue;
					frmMain->plResult->Color = clWhite;
					frmMain->plResult->Caption = "Wait";
					frmMain->pan_1000->Color = clWhite;
					frmMain->Height = 255;
					frmMain->plErrMsg->Caption = "";
					frmMain->plTime->Caption = 0;
					if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
					Test_Step = GET_DUT;
				}
				break;
		}
		frmMain->Refresh();
		if(btn_start->Caption.Pos("Stop"))
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
void  TfrmMain::SaveLogLine(AnsiString FileName,AnsiString FileLine)
{
	FileLine = (FormatDateTime("hh:nn:ss", Now()))+"  "+FileLine+="\r\n";
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
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
			szLanMsg = "PING 失敗(PING FAIL)";
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
			szLanMsg = "網速未達到1000M(Speed of PING Fail)";
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
		szLanMsg = "PING 失敗(PING FAIL)";
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
	frmMain->Width = plDebug->Width ? 591 : 892;
}
//---------------------------------------------------------------------------



void __fastcall TfrmMain::BitBtn1Click(TObject *Sender)
{
	USBCONInfo->RestartDev(g_DevVPID.SubString(5,4), g_DevVPID.SubString(14,4));
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::btnLoadMACClick(TObject *Sender)
{
	strLoadMAC =  current_mac->Caption;
	if(strLoadMAC != "")
	{
		btnLoadMAC->Caption = strLoadMAC;
		btnLoadMAC->Enabled = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnLoadNetworkCardNameClick(TObject *Sender)
{
	int i = lab_devicename->Caption.Pos("#")-1;
	if(i>0)
		strLoadLANName =  lab_devicename->Caption.SubString(1,i).Trim();
	else
		strLoadLANName =  lab_devicename->Caption.Trim();
	if(strLoadLANName != "")
	{
		btnLoadNetworkCardName->Caption = strLoadLANName;
		btnLoadNetworkCardName->Enabled = false;
	}
}
//---------------------------------------------------------------------------
bool TfrmMain::writeLOG(AnsiString Msg,bool bAction)
{
	AnsiString SS;
	AnsiString LOGFilePath = "C:\\ASMP\\log\\PingTool\\"+edtSetWorkOrderNumber->Caption;
	if(bAction)
	{  //創LOG資料夾
		if(!FileExists(LOGFilePath.c_str())){
			_mkdir( "C:\\ASMP\\");
			_mkdir( "C:\\ASMP\\log\\" );
			_mkdir( "C:\\ASMP\\log\\PingTool\\" );
			_mkdir(LOGFilePath.c_str());
		}
		SS = "\r\n==============\r\n [EmployeeID]"+ edtSetEmployeeID->Caption
			+"\n[ProductModel]"+edtSetModel->Caption+"[WorkStation]"+edtWorkStation->Caption+"\r\n=================================\r\n";
			SaveUserLogLine(LOGFilePath+"\\LOG_RD.log",SS);
	}
	else
	{
		dwTestCount++;
		strTemp.sprintf("%04d ",dwTestCount);
		if(Msg.Pos("PASS"))
		{
			SS = "\r\n"+strTemp+AnsiString(FormatDateTime(" mm-dd hh:mm:ss", Now()))
				+"[PASS]"+lab_devicename->Caption+"[DUT_MAC]"+current_mac->Caption;
			SS+= " [EmployeeID]"+edtSetEmployeeID->Caption+" [ProductModel]"+edtSetModel->Caption+" [WorkStation]"+edtWorkStation->Caption;
		}
		else
		{
			SS = "\r\n"+strTemp+AnsiString(FormatDateTime(" mm-dd hh:mm:ss", Now()))
				+" [FAIL]"+lab_devicename->Caption+" [DUT_MAC]"+current_mac->Caption;
			SS+= " [EmployeeID]"+edtSetEmployeeID->Caption+" [ProductModel]"+edtSetModel->Caption+" [WorkStation]"+edtWorkStation->Caption;
			SS+=" [ErrorMsg]"+Msg;
		}
		SaveUserLogLine(LOGFilePath+"\\LOG_RD.log",SS);
		SaveUserLogLine(LOGFilePath+"\\LOG.log",SS);
	}
}
void  TfrmMain::SaveUserLogLine(AnsiString FileName,AnsiString FileLine)
{
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
}
void __fastcall TfrmMain::btnBarcodeClearClick(TObject *Sender)
{
	edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnBarcodeChanelClick(TObject *Sender)
{
	numBarcodeResult = BARCODE_CHANEL;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::edtBarcodeMACChange(TObject *Sender)
{
	TEdit* edt=(TEdit*)Sender;
	TCHAR HexChar[15];

	edt->Text = edt->Text.UpperCase();// 把字串轉成全大寫
	_tcscpy_s(HexChar, 15, AnsiString(edt->Text).c_str());
	for(int i=0;i <edt->Text.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//非數字
		{
			if(HexChar[i] < 0x41 || HexChar[i] > 0x46)//非大寫字母a~f
			{
				edtBarcodeMAC->Text =
					edtBarcodeMAC->Text.Delete(edtBarcodeMAC->Text.Length(),1);
            }
		}
	}
	if(edtBarcodeMAC->Text.Length()> 12)
	  edtBarcodeMAC->Text = "";
	edt-> SelStart=edt-> Text.Length();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(edtBarcodeMAC->Text.Length()==12&&Key==13)
	{
		strLoadMAC = edtBarcodeMAC->Text.SubString(1,2)+":"
							+edtBarcodeMAC->Text.SubString(3,2)+":"
							+edtBarcodeMAC->Text.SubString(5,2)+":"
							+edtBarcodeMAC->Text.SubString(7,2)+":"
							+edtBarcodeMAC->Text.SubString(9,2)+":"
							+edtBarcodeMAC->Text.SubString(11,2);
		numBarcodeResult = BARCODE_FINISH;
	}
	else if(edtBarcodeMAC->Text.Length()<12&&Key==13)
		edtBarcodeMAC->Text = "";
	else if(Key == 8)
		edtBarcodeMAC->Text = "";
}
//---------------------------------------------------------------------------
void TfrmMain::ReadRegValue()
{
	AnsiString value;
	TRegistry *reg = new TRegistry();


	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("SOFTWARE", false);
	bool bl = reg->OpenKey("PingTool", true);
	bl = reg->OpenKey("v2.0.0-13", false);
	if(bl)
	{
		if(reg->ValueExists("NO_MAC")){
			value = reg->ReadString("NO_MAC");
			if(value == "true") ckbNoMAC->Checked = true;
			else ckbNoMAC->Checked = false;
		}

		if(reg->ValueExists("MAC_BARCODE_VERIFTY")){
			value = reg->ReadString("MAC_BARCODE_VERIFTY");
			if(value == "true") ckbBarcode->Checked = true;
			else ckbBarcode->Checked = false;
		}

		if(reg->ValueExists("WorkOrderNumber")){
			value = reg->ReadString("WorkOrderNumber");
			frmMsg->edtSetWorkOrderNumber->Text = value;
		}
		if(reg->ValueExists("Model")){
			value = reg->ReadString("Model");
			frmMsg->edtSetModel->Text = value;
		}
		if(reg->ValueExists("WorkStation")){
			value = reg->ReadString("WorkStation");
			frmMsg->edtWorkStation->Text = value;
		}
		if(reg->ValueExists("EmployeeID")){
			value = reg->ReadString("EmployeeID");
			frmMsg->edtSetEmployeeID->Text = value;
		}
	}

	delete reg;
}
void TfrmMain::SetRegValue()
{
	TRegistry *reg = new TRegistry;
	AnsiString SS;
	reg->RootKey = HKEY_CURRENT_USER;
	reg->OpenKey("SOFTWARE", false);
	bool bl = reg->OpenKey("PingTool", true);
	bl = reg->OpenKey("v2.0.0-13", true);
	if(bl)
	{
		SS = ckbNoMAC->Checked ? "true":"false";
		reg->WriteString("NO_MAC", SS);
		SS = ckbBarcode->Checked ? "true":"false";
		reg->WriteString("MAC_BARCODE_VERIFTY", SS);

		reg->WriteString("WorkOrderNumber", edtSetWorkOrderNumber->Caption );
		reg->WriteString("WorkStation", edtWorkStation->Caption );
		reg->WriteString("Model", edtSetModel->Caption );
		reg->WriteString("EmployeeID", edtSetEmployeeID->Caption );
	}
	delete reg;
}

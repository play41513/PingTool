// Module:
// Notices: Copyright(c) 2020 Leno.
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

AnsiString szLanMsg = "";
DWORD dwTestCount = 0;
AnsiString strLoadMAC 		= "";
AnsiString strLoadLANName 	= "";
int numBarcodeResult;
DWORD BarcodeTimeOut;
//
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
/////////////////////////////////////////// 限制只能執行一次
	frmMain->DoubleBuffered = true;
	mx = CreateMutex( NULL, true, "AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	N1Click(NULL);

	ReadRegValue();
	TListInitLAN = new TStringList;
	PORT->IntiNetwordCard();
	ZeroMAC = "00249B:"+FormatDateTime("nnssms", Now()).SubString(1,6);
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);
		ShowSettingForm();
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
		PORT->ChangeLanSpeed(frmMain->g_DevVPID,frmMain->gLAN_NAME,frmMain->bUSB3);
	return 0;
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	TestTime = 0;
	plTime->Caption = "0";
	lbBarcode->Caption = "請掃描MAC驗證條碼";
	btn_start->Enabled = false;
	if(btn_start->Caption.Pos("Start"))
	{
		//PopupMenu
		plSet->Enabled = false;
		btn_start->Caption = "Stop";
		strLoadMAC = "";
		//初始元件
		lab_devicename->Font->Color = clBlue;
		current_mac->Font->Color = clBlue;
		plResult->Color = clWhite;
		plResult->Caption = "Wait";
		frmMain->pl1000M->Color = clWhite;
		frmMain->pl100M->Color = clWhite;
		frmMain->pl10M->Color = clWhite;
		frmMain->Height = 294;
		//
		if(ckbStaticIP->Checked)
		{
			btn_start->Enabled  = false;
			plServerIP->Enabled = true;
			plServerIP->Width = 523;
			plServerIP->Left  = 30;
			plServerIP->Top   = 32;
			PORT->EnumComboBoxNetwork(cbNetwork);
			cbNetworkChange(NULL);
		}
		else
		{
			// 呼叫LAN Timer
			TestWorking = true;
			Test_Step = GET_DUT;
			writeLOG("",true);
		}
	}
	else
	{
		frmMain->PopupMenu = PopupMenu;
		btn_start->Caption = "Start";
		plResult->Color = clWhite;
		plResult->Caption = "Wait";
		plSet->Enabled = true;
		N1Click(NULL);
	}
	btn_start->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	PORT->DeleteServerIPAddress();
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
					SetForegroundWindowInternal(Handle);
					while(true)
					{
						if(numBarcodeResult == BARCODE_FINISH)
						{
							TestTime = GetTickCount();
							lbBarcode->Font->Size = 32;
							lbBarcode->Caption = "請掃描MAC驗證條碼";
							lbBarcode->Color = clTeal;
							plBarcode->Height = 0;
							BarcodeTimeOut = GetTickCount()+(20000);
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
				if(ckbBarcode->Checked && BarcodeTimeOut !=0 && btn_start->Caption.Pos("Stop"))
				{
					if(GetTickCount() > BarcodeTimeOut)
					{
						lbBarcode->Font->Size = 20;
						lbBarcode->Caption = "未偵測到待測物請重新掃描MAC";
						frmMain->plResult->Color = clRed;
						plResult->Caption = "FAIL";
						frmMain->Height = 293;
						frmMain->plErrMsg->Caption = "(!)TimeOUT";
						writeLOG("(!)WAIT_BARCODE_TimeOUT",false);
						Test_Step = WAIT_DUT_PLUGOUT;
						strLoadMAC = "";
					}
					else
					{
						plTime->Caption = AnsiString(double(GetTickCount()-TestTime)/(double)1000);
                    }
				}
				if(gLAN_NAME != "" && !btn_start->Caption.Pos("Start"))
				{
					g_DevVPID = PORT->getLanVPID(gLAN_NAME);
					if(g_DevVPID == "")
					{
						ERROR_MSG = "(!)GET VPID FAIL";
						Test_Step = LAN_TEST_RESULT;
					}
					else
					{
						bPlugOut = false;
						plResult->Caption = "Testing";
						ERROR_MSG		  = NULL;
						TestTime = GetTickCount();

						frmMain->current_mac->Caption = PORT->getLanMAC(gLAN_NAME); //顯示當前設備號
						Test_Step = MAC_LANNAME_VERIFY;
						dwTimeout = GetTickCount()+3000;
					}
				}
				break;
			case MAC_LANNAME_VERIFY:
				moDebug->Lines->Add("[MAC_LANNAME_VERIFY]");
				if(strLoadMAC != "")
					if(!current_mac->Caption.Pos(strLoadMAC))
						current_mac->Font->Color = clRed;
				if(strLoadLANName != "")
					if(!lab_devicename->Caption.Pos(strLoadLANName))
						lab_devicename->Font->Color = clRed;
				if(current_mac->Font->Color == clRed || lab_devicename->Font->Color == clRed)
				{
					ERROR_MSG = "(!)比對失敗Comparison-Failure";
					Test_Step = LAN_TEST_RESULT;
				}
				else
					Test_Step = USB3_TEST;
				if(ckbBarcode->Checked)
					strLoadMAC = "";
				break;
			case USB3_TEST:
				try
				{
					moDebug->Lines->Add("[USB3_TEST]");
					btn_start->Enabled = false;
					moDebug->Lines->Add("VPID :"+g_DevVPID);
					if(USBCONInfo->GetUSBType(g_DevVPID,moDebug) == USBTYPE_SUPER_SPEED)
					{
						bUSB3 = true;
						Test_Step = LAN_TEST;
						dwTimeout = GetTickCount()+5000;
						moDebug->Lines->Add("LAN_TEST");
						if(ckbUSB2->Checked)
						{
							ERROR_MSG = "(!)USB2.0-FAIL";
							Test_Step = LAN_TEST_RESULT;
						}
					}
					else if(USBCONInfo->GetUSBType(g_DevVPID,NULL) == USBTYPE_LOW_SPEED_PLUS)
					{
						if(GetTickCount()> dwTimeout)
						{
							ERROR_MSG = "(!)GetUSB-Type-FAIL";
							Test_Step = LAN_TEST_RESULT;
						}
					}
					else
					{
						bUSB3 = false;
						Test_Step = LAN_TEST;
						dwTimeout = GetTickCount()+5000;
						moDebug->Lines->Add("[LAN_TEST]");
						if(!ckbUSB2->Checked)
						{
							ERROR_MSG = "(!)USB3.0-FAIL";
							Test_Step = LAN_TEST_RESULT;
						}
					}
				}
				catch(...)
				{
					if(GetTickCount()> dwTimeout)
					{
						ERROR_MSG = "(!)GetUSB-Type-FAIL";
						Test_Step = LAN_TEST_RESULT;
					}
                }
				break;
			case LAN_TEST:
				if(frmMain->gLAN_NAME == "" && GetTickCount()>dwTimeout)
				{
					ERROR_MSG = "Not-Find-NetworkCard";
					Test_Step = LAN_TEST_RESULT;
				}
				else if(frmMain->gLAN_NAME != "" )
				{
					ChangeUI(NULL);
					RegeditDriver();
					//
					frmMain->plResult->Caption = "Testing";
					//網路測試
					lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
					Test_Step = LAN_TEST_RESULT;
				}
				break;
			case LAN_TEST_RESULT:
				if(ERROR_MSG != NULL && ERROR_MSG != "PASS")
				{
					if(!writeLOG(ERROR_MSG,false))
						ERROR_MSG = "(!)寫檔失敗";
					ChangeUI("Red");
					//
					frmMain->plErrMsg->Caption = ERROR_MSG;
					plResult->Color = clRed;
					plResult->Caption = "FAIL";
					frmMain->Height = 336;
					//
					lan = NULL;
					CloseHandle(lan);
					Test_Step = WAIT_DUT_PLUGOUT;
					break;
				}
				else if(ERROR_MSG == "PASS")
				{
					if(writeLOG("PASS",false))
					{
						ChangeUI("Green");
						//
						plResult->Color = clGreen;
						plResult->Caption = "PASS";
					}
					else
					{
						ChangeUI("Red");
						ERROR_MSG = "(!)寫檔失敗";
						frmMain->plErrMsg->Caption = ERROR_MSG;
						plResult->Color = clRed;
						plResult->Caption = "FAIL";
						frmMain->Height = 336;
					}
					//
					lan = NULL;
					CloseHandle(lan);
					Test_Step = WAIT_DUT_PLUGOUT;
					break;
				}
				break;
			case WAIT_DUT_PLUGOUT:
				btn_start->Enabled = true;
				if(ERROR_MSG == "(!)寫檔失敗")
				{
					MessageBoxA(Handle,"請確認LOG隨身碟狀態","WARNING", MB_ICONEXCLAMATION);
					btn_startClick(NULL);
					ShowSettingForm();
                }
				else if(gLAN_NAME == ""||frmMain->bPlugOut)
				{
					frmMain->current_mac->Font->Color = clBlue;
					frmMain->lab_devicename->Font->Color = clBlue;
					frmMain->plResult->Color = clWhite;
					frmMain->plResult->Caption = "Wait";
					frmMain->pl1000M->Color = clWhite;
					frmMain->pl100M->Color = clWhite;
					frmMain->pl10M->Color = clWhite;
					frmMain->Height = 294;
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
void TfrmMain::RegeditDriver()
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

void __fastcall TfrmMain::popBackWOPageClick(TObject *Sender)
{
	if(btn_start->Caption.Pos("Start"))
		ShowSettingForm();
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
   moDebug->Lines->Add("End");
   if(szLanMsg == "")
   {
	   if(avg_time>10)
	   {
			szLanMsg = "網速未達到(SpeedOfPingFail)";
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
		szLanMsg = "PING失敗(PING-FAIL)";
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
	bool bPASS[4] = {true,true,true,true};
	AnsiString SS;
	if(bAction)
	{  //創LOG資料夾
		AnsiString strNoMAC = ckbNoMAC->Checked ? "true":"false";
		AnsiString strBarcode = ckbBarcode->Checked ? "true":"false";
		SS = "\r\n===========\r\n"
			+AnsiString(FormatDateTime("hh-mm-ss", Now()))
			+" [名稱比對]"+strLoadLANName
			+" [ZeroMAC]"+strNoMAC
			+" [BarcodeMAC]"+strBarcode
			+" [工單號]"+edtSetWorkOrderNumber->Caption
			+" [操作人員工號]"+edtSetEmployeeID->Caption+"\r\n=================================\r\n";
		bPASS[0] = SaveLogLine(RDLOGFilePath,SS);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible)
		{
		   bPASS[1] = 	SaveLogLine(RDLOGDiskPath,SS);
		}
		for(int i = 0 ; i < 2 ; i++)
			if(!bPASS[i]) return false;
	}
	else
	{
		if(Msg=="PASS")
			dwTestCount++;
		AnsiString strTemp;
		strTemp.sprintf("%04d",dwTestCount);
		AnsiString strUSB = bUSB3 ? "3":"2";
		if(Msg.Pos("PASS"))
		{
			SS = "\n"+strTemp+",[PASS],"+AnsiString(FormatDateTime("mm-dd-yyyy,hh:mm:ss", Now()))
				+",[NAME],"+lab_devicename->Caption+",[MAC],"+current_mac->Caption+",[USB],"+strUSB;
			SS+= ",[WorkOrder],"+edtSetWorkOrderNumber->Caption+",[EmployeeID],"+edtSetEmployeeID->Caption+",[TEST-TIME],"+plTime->Caption;
		}
		else
		{
			SS = "\n"+strTemp+",[FAIL],"+AnsiString(FormatDateTime("mm-dd-yyyy,hh:mm:ss", Now()))
				+",[NAME],"+lab_devicename->Caption+",[MAC],"+current_mac->Caption+",[USB],"+strUSB;;
			SS+= ",[WorkOrder],"+edtSetWorkOrderNumber->Caption+",[EmployeeID],"+edtSetEmployeeID->Caption+",[TEST-TIME],"+plTime->Caption;
			SS+=",[ErrorMsg],"+Msg;
		}
		bPASS[0] = SaveLogLine(RDLOGFilePath,SS);
		bPASS[1] = SaveLogLine(LOGFilePath,SS);
		if(LOGDiskPath.Pos("csv") && ImgDisk->Visible )
		{
			bPASS[2] = SaveLogLine(RDLOGDiskPath,SS);
			bPASS[3] = SaveLogLine(LOGDiskPath,SS);
		}
		for(int i = 0 ; i < 4 ; i++)
			if(!bPASS[i]) return false;
	}
	return true;
}
bool  TfrmMain::SaveLogLine(AnsiString FileName,AnsiString FileLine)
{
	FILE * fp;
	fp = fopen(FileName.c_str(),"a+");
	if(NULL == fp)
		return false;
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
	return true;
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
	if(edtBarcodeMAC->Text.Length()> 12)
	  edtBarcodeMAC->Text = "";
	else
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
	bl = reg->OpenKey("v2.3", false);
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
			edtSetWorkOrderNumber->Caption = value;
		}
		if(reg->ValueExists("EmployeeID")){
			value = reg->ReadString("EmployeeID");
			edtSetEmployeeID->Caption = value;
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
	bl = reg->OpenKey("v2.3", true);
	if(bl)
	{
		SS = ckbNoMAC->Checked ? "true":"false";
		reg->WriteString("NO_MAC", SS);
		SS = ckbBarcode->Checked ? "true":"false";
		reg->WriteString("MAC_BARCODE_VERIFTY", SS);

		reg->WriteString("WorkOrderNumber", edtSetWorkOrderNumber->Caption );
		reg->WriteString("EmployeeID", edtSetEmployeeID->Caption );
	}
	delete reg;
}
bool TfrmMain::CheckDiskName()
{
	DWORD DiskInfo = GetLogicalDrives();
	AnsiString DiskName,DiskVerifty = "DEVICE_LOG",SS;
	char Disk;
	for(int i =3;i<=25;i++)
	{
		if((DiskInfo&(0x01<<i))!=0)
		{
			char Volumelabel[20];
			DWORD SerialNumber;
			DWORD MaxCLength;
			DWORD FileSysFlag;
			char FileSysName[10];
			Disk = 0x41+i;
			DiskName = AnsiString(Disk)+":\\";
			GetVolumeInformation(DiskName.c_str(),Volumelabel,255,&SerialNumber,&MaxCLength,&FileSysFlag,FileSysName,255);
			if(!strcmp(Volumelabel,DiskVerifty.c_str()))
			{
				LOGDiskPath = DiskName+"\\ASMP\\log\\"+edtSetWorkOrderNumber->Caption+"\\PingTool";
				if(!FileExists(LOGDiskPath.c_str())){
					_mkdir( DiskName.c_str());
					SS = DiskName + "\\ASMP";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log";
					_mkdir( SS.c_str());
					SS = DiskName + "\\ASMP\\log\\"+edtSetWorkOrderNumber->Caption;
					_mkdir(SS.c_str());
					_mkdir(LOGDiskPath.c_str());
				}
				return true;
			}
		}
	}
	return false;
}
void TfrmMain::ShowSettingForm()
{
	if(frmMsg==NULL)  frmMsg = new TfrmMsg(Application);

	frmMsg->edtSetWorkOrderNumber->Text = edtSetWorkOrderNumber->Caption;
	frmMsg->edtSetEmployeeID->Text = edtSetEmployeeID->Caption.Trim();


	if(frmMsg->ShowModal()== mrOk)
	{
		edtSetWorkOrderNumber->Caption  = frmMsg->edtSetWorkOrderNumber->Text;
		edtSetEmployeeID->Caption = frmMsg->edtSetEmployeeID->Text;
		TestWorking = false;
		SetRegValue();
		//取得LOGFilePath、LOGDiskPath路徑
		LOGFilePath = "C:\\ASMP\\log\\"+edtSetWorkOrderNumber->Caption+"\\PingTool";
		CheckDiskName();
		//
		FindLogFile();
		//
		delete frmMsg;
		frmMsg = NULL;
	}
	else
	{
		delete frmMsg;
		frmMsg = NULL;
		Close();
	}
}
void TfrmMain::FindLogFile()
{
	//
	NewFilePath(LOGFilePath);
	NewFilePath(LOGDiskPath);
	//
	TSearchRec Sr;
	AnsiString time = FormatDateTime("yyyymmdd-hhnnss", Now());
	if(DirectoryExists(LOGDiskPath.c_str()))
	{
		ImgDisk->Visible = true;
		if(FindFirst(LOGDiskPath+"\\*.csv",faAnyFile,Sr)==0)
		{
			do
			{
				if(Sr.Name.Pos("RD"))
				{
					if(Sr.Name.SubString(3,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath + "\\"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name.SubString(3,19);
						//
						RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name.SubString(3,19);
						break;
					}
				}
				else
				{
					if(Sr.Name.SubString(1,8) == time.SubString(1,8))
					{
						RDLOGDiskPath = LOGDiskPath +"\\RD"+Sr.Name;
						LOGDiskPath += "\\"+Sr.Name;
						//
						RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
						LOGFilePath += "\\"+Sr.Name;
						break;
					}
				}
			}
			while(FindNext(Sr)==0);
			FindClose(Sr);
		}

		if(!LOGDiskPath.Pos(".csv"))
		{
			RDLOGDiskPath = LOGDiskPath +"\\RD"+time+".csv";
			LOGDiskPath += "\\"+time+".csv";
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
		else//檔案已存在於USBDisk
		{
			if(!FileExists(LOGFilePath))
			{
				CopyFile(LOGDiskPath.c_str(),LOGFilePath.c_str(),true);
				CopyFile(RDLOGDiskPath.c_str(),RDLOGFilePath.c_str(),true);
			}
		}
	}
	else
	{
		ImgDisk->Visible = false;
		if(DirectoryExists(LOGFilePath.c_str()))
		{
			if(FindFirst(LOGFilePath+"\\*.csv",faAnyFile,Sr)==0)
			{
				do
				{
					if(Sr.Name.Pos("RD"))
					{
						if(Sr.Name.SubString(3,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath + "\\"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name.SubString(3,19);
							break;
						}
					}
					else
					{
						if(Sr.Name.SubString(1,8) == time.SubString(1,8))
						{
							RDLOGFilePath = LOGFilePath +"\\RD"+Sr.Name;
							LOGFilePath += "\\"+Sr.Name;
							break;
						}
					}
				}
				while(FindNext(Sr)==0);
				FindClose(Sr);
			}
		}
		if(!LOGFilePath.Pos(".csv"))
		{
			RDLOGFilePath = LOGFilePath +"\\RD"+time+".csv";
			LOGFilePath += "\\"+time+".csv";
		}
	}
}
void TfrmMain::NewFilePath(AnsiString Path)
{
	Path+="\\";
	if(!DirectoryExists(Path.c_str()))
	{
		AnsiString SS,Temp = "";
		while(true)
		{
			SS = Path.SubString(1,Path.Pos("\\"));
			Path.Delete(1,Path.Pos("\\"));
			Temp+=SS;
			_mkdir( Temp.c_str());
			if(Path == "")
				break;
		}
	}
}
void TfrmMain::ChangeUI(AnsiString astrColor)
{
	if(g_DevVPID.Pos("VID_0B95"))
	{
		if(bUSB3)
		{
			pl1000M->Caption = "1000M";
			if(ckbStaticIP->Checked && ckb2500MSpeed->Checked)
					pl1000M->Caption = "2.5G";
			if(astrColor == NULL)
			{
				pl10M->Width  = plLANSpeed->Width / 3;
				pl100M->Width = plLANSpeed->Width / 3;
				pl10M->Left = 1;
			}
			else
			{
				if(astrColor.Pos("Red"))
				{
					pl1000M->Color = clRed;
					frmMain->Refresh();
				}
				else
				{
					pl1000M->Color = clGreen;
					frmMain->Refresh();
					PORT->Delay(500);
					pl100M->Color = clGreen;
					frmMain->Refresh();
					PORT->Delay(500);
					pl10M->Color = clGreen;
					frmMain->Refresh();
				}
			}
		}
		else
		{
			if(astrColor == NULL)
			{
				pl10M->Width  = plLANSpeed->Width / 2;
				pl100M->Width = plLANSpeed->Width / 2;
				pl10M->Left = 1;
			}
			else
			{
				if(astrColor.Pos("Red"))
				{
					pl100M->Color = clRed;
					frmMain->Refresh();
				}
				else
				{
					pl100M->Color = clGreen;
					frmMain->Refresh();
					PORT->Delay(500);
					pl10M->Color = clGreen;
					frmMain->Refresh();
				}
			}
		}
	}
	else
	{
		pl1000M->Caption = "1000M";
		if(ckbStaticIP->Checked && ckb2500MSpeed->Checked)
				pl1000M->Caption = "2.5G";

		if(astrColor == NULL)
		{
			pl10M->Width  = 1;
			pl100M->Width = 1;
		}
		else
		{
			if(astrColor.Pos("Red"))
			{
				pl1000M->Color = clRed;
				frmMain->Refresh();
			}
			else
			{
				pl1000M->Color = clGreen;
				frmMain->Refresh();
			}
		}
    }
}
void TfrmMain::SetForegroundWindowInternal(HWND hWnd)
{
    if(!::IsWindow(hWnd)) return;

	//relation time of SetForegroundWindow lock
    DWORD lockTimeOut = 0;
    HWND  hCurrWnd = ::GetForegroundWindow();
    DWORD dwThisTID = ::GetCurrentThreadId(),
          dwCurrTID = ::GetWindowThreadProcessId(hCurrWnd,0);

    //we need to bypass some limitations from Microsoft :)
    if(dwThisTID != dwCurrTID)
    {
        ::AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

        ::SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT,0,&lockTimeOut,0);
        ::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,0,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

        ::AllowSetForegroundWindow(ASFW_ANY);
	}

    ::SetForegroundWindow(hWnd);

    if(dwThisTID != dwCurrTID)
    {
        ::SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT,0,(PVOID)lockTimeOut,SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
        ::AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
    }
}
void __fastcall TfrmMain::popReLoadNameClick(TObject *Sender)
{
	if(btn_start->Caption.Pos("Start"))
	{
		strLoadLANName = "";
		if(lab_devicename->Caption != "")
			btnLoadNetworkCardNameClick(NULL);
		else
			btnLoadNetworkCardName->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::plOk_ServerIPClick(TObject *Sender)
{
	plServerIP->Enabled = false;
	btn_start->Enabled  = true;
	plServerIP->Width = 0;
	//
	if(plNetworkCardMAC->Caption.Pos(SERVER_MAC))
	{
		// 呼叫LAN Timer
		TestWorking = true;
		Test_Step = GET_DUT;
		writeLOG("",true);
	}
	else
	{
		PORT->SetIPAddress(cbNetwork->Text);
		AnsiString VPID = PORT->getLanVPID(cbNetwork->Text);
		if(PORT->cmd_restartdevice(VPID))
		{
			// 呼叫LAN Timer
			TestWorking = true;
			if(!PORT->StringListTextPosString(TListInitLAN,cbNetwork->Text))
				TListInitLAN->Add(cbNetwork->Text);
			Test_Step = GET_DUT;
			writeLOG("",true);
		}
		else
		{
			MessageBoxA(Handle,"ServerIP 設定失敗","WARNING", MB_ICONEXCLAMATION);
			btn_startClick(NULL);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::plChanel_ServerIPClick(TObject *Sender)
{
	plServerIP->Enabled = false;
	btn_start->Enabled  = true;
	plServerIP->Width = 0;
	PORT->DeleteServerIPAddress();
	btn_startClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ImgRefreshClick(TObject *Sender)
{
	PORT->EnumComboBoxNetwork(cbNetwork);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::cbNetworkChange(TObject *Sender)
{
	plNetworkCardMAC->Caption = PORT->getLanMAC(cbNetwork->Text);
	if(plNetworkCardMAC->Caption.Pos(SERVER_MAC))
		plNetworkCardMAC->Font->Color = clBlue;
	else
		plNetworkCardMAC->Font->Color = clGray;
}
//---------------------------------------------------------------------------



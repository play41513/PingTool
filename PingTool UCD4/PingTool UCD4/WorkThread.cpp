//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WorkThread.h"
#include "Main.h"
#include "Hub_fun.h"
#include "USBDevConnectionInfo.h"
#pragma package(smart_init)
Port_test *PORT = new Port_test();
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();
HANDLE lan;
int Test_Step;
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TWORKTHREAD::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TWORKTHREAD::TWORKTHREAD(bool CreateSuspended)
	: TThread(CreateSuspended)
{
	TestTime = 0;
}
//---------------------------------------------------------------------------
void __fastcall TWORKTHREAD::Execute()
{
	NameThreadForDebugging("WORKTHREAD");
	//---- Place thread code here ----
	Test_Step = GET_DUT;
	FreeOnTerminate = true;
	while(!Terminated)
	{
		frmMain->gLAN_NAME = PORT->GetLANName();
		if(frmMain->gLAN_NAME != "" ||Test_Step == WAIT_DUT_PLUGOUT || Test_Step == LAN_TEST_RESULT)
			Synchronize(NetworkCardTest);
		if(frmMain->plResult->Caption == "Testing")
		{
			if(TestTime == 0)
				TestTime = GetTickCount();
			frmMain->plTime->Caption = AnsiString(double(GetTickCount()-TestTime)/(double)1000);
		}
		Sleep(100);
		if(frmMain->dThreadStatus == THREAD_STOP)
		{
			frmMain->dThreadStatus = THREAD_FINSH;
			break;
		}
	}
}
//----------------------------------------  -----------------------------------
void __fastcall TWORKTHREAD::NetworkCardTest()
{
	switch (Test_Step)
	{
		case GET_DUT :
			frmMain->g_DevVPID = USBCONInfo->DevNameGetVPID(frmMain->gLAN_NAME);
			if(frmMain->g_DevVPID == "")
			{
				frmMain->FormUIChange(true,"(!)GET VPID FAIL");
				frmMain->g_bError	= true;
				Test_Step = WAIT_DUT_PLUGOUT;
			}
			else
			{
				frmMain->g_bError	= false;
				frmMain->bPlugOut = false;
				frmMain->plResult->Caption = "Testing";
				frmMain->ERROR_MSG		  = "";
				TestTime = GetTickCount();

				frmMain->current_mac->Caption = PORT->getLanMAC(frmMain->gLAN_NAME); //顯示當前設備號
				frmMain->SaveLogLine("Get NetworkCard: [VPID]"+frmMain->g_DevVPID+"[NAME]"+frmMain->gLAN_NAME+"[MAC]"+AnsiString(frmMain->current_mac->Caption));
				Test_Step = LAN_TEST;
			}
			break;
		case USB3_TEST:
			frmMain->btn_start->Enabled = false;
			frmMain->SaveLogLine("Check USB3.0");
			if(USBCONInfo->GetDevUSB(frmMain->g_DevVPID.SubString(5,4),frmMain->g_DevVPID.SubString(14,4)))
			{
				//網路測試
				frmMain->SaveLogLine("USB3.0 PASS");
				Test_Step = LAN_TEST;
			}else{
				frmMain->FormUIChange(true,"(!)USB3.0 FAIL");
				frmMain->g_bError	= true;
				Test_Step = WAIT_DUT_PLUGOUT;
			}
			break;
		case LAN_TEST:
			frmMain->RegDriverClick();
			frmMain->SaveLogLine("Device Driver Regedit Check");
			//
			frmMain->plResult->Caption = "Testing";
			//網路測試
			lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
			frmMain->SaveLogLine("Lan Test Start");
			Test_Step = LAN_TEST_RESULT;
			break;
		case LAN_TEST_RESULT:
			if(frmMain->ERROR_MSG!="")
			{
				frmMain->FormUIChange(true,frmMain->ERROR_MSG);
				lan = NULL;
				CloseHandle(lan);
				Test_Step = WAIT_DUT_PLUGOUT;
				frmMain->SaveLogLine("Waiting PlugOut");
			}
			else if(frmMain->pan_1000->Color == clGreen)
			{
				frmMain->FormUIChange(UI_PASS,"");
				lan = NULL;
				CloseHandle(lan);
				Test_Step = WAIT_DUT_PLUGOUT;
				frmMain->SaveLogLine("Waiting PlugOut");

			}
			break;
		case WAIT_DUT_PLUGOUT:
			frmMain->btn_start->Enabled = true;
			if(frmMain->gLAN_NAME == ""||frmMain->bPlugOut)
			{
				frmMain->SaveLogLine("Device PlugOut");
				frmMain->FormUIChange(UI_CLEAR,"");
				if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
				Test_Step = GET_DUT;
				frmMain->SaveLogLine("Waiting Device PlugIn...");
				frmMain->SaveLogLine("==============");
			}
			break;
	}
}
DWORD WINAPI LANExecute(LPVOID Param)
{
	PORT->ChangeLanSpeed(frmMain->g_DevVPID, "{4d36e972-e325-11ce-bfc1-08002be10318}", frmMain->gLAN_NAME);
	return 0;
}

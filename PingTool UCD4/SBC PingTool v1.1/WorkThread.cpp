//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "WorkThread.h"

#include "Hub_fun.h"
#include "USBDevConnectionInfo.h"
#pragma package(smart_init)
Port_test *PORT = new Port_test();
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();
HANDLE lan;
int Test_Step;
DWORD SBCTimeOut = 0;
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
		else if(Test_Step == MCU_POWER_ON_TEST || Test_Step == MCU_POWER_OFF_TEST || Test_Step == MCU_TEST)
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
				frmMain->moDebug->Clear();
				frmMain->g_bError	= false;
				frmMain->bPlugOut = false;
				frmMain->plResult->Caption = "Testing";
				frmMain->ERROR_MSG		  = "";
				TestTime = GetTickCount();

				frmMain->current_mac->Caption = PORT->getLanMAC(frmMain->gLAN_NAME); //��ܷ��e�]�Ƹ�
				frmMain->SaveLogLine("Get NetworkCard: [VPID]"+frmMain->g_DevVPID+"[NAME]"+frmMain->gLAN_NAME+"[MAC]"+AnsiString(frmMain->current_mac->Caption));
				Test_Step = HID_TURN_ON;
			}
			break;
		case USB3_TEST:
			frmMain->btn_start->Enabled = false;
			frmMain->SaveLogLine("Check USB3.0");
			if(USBCONInfo->GetDevUSB(frmMain->g_DevVPID.SubString(5,4),frmMain->g_DevVPID.SubString(14,4)))
			{
				//��������
				frmMain->SaveLogLine("USB3.0 PASS");
				Test_Step = LAN_TEST;
			}else{
				frmMain->FormUIChange(true,"(!)USB3.0 FAIL");
				frmMain->g_bError	= true;
				Test_Step = WAIT_DUT_PLUGOUT;
			}
			break;
		case HID_TURN_ON:
				if(HID_TurnOn())
				{
					Tx("03 13 80 80 00");
					SBCTimeOut = GetTickCount()+5000;
					if(Rx())
					{
						Test_Step = MCU_POWER_OFF_TEST;
					}
					else
					{
						Tx("03 13 80 80 80");
						frmMain->ERROR_MSG = "(!)HID CMD ERROR";
						Test_Step = LAN_TEST_RESULT;
					}
				}
				else
				{
					frmMain->ERROR_MSG = "(!)NOT FIND HID DEVICE";
					Test_Step = LAN_TEST_RESULT;
				}
			break;
		case MCU_POWER_OFF_TEST:
				if(GetTickCount()< SBCTimeOut)
				{
					if(!PORT->EnumHUB())
					{
						Test_Step = MCU_POWER_ON_TEST;
						Tx("03 13 80 80 80");
						SBCTimeOut = GetTickCount()+5000;
						if(Rx())
							break;
						else
						{
							frmMain->ERROR_MSG = "(!)HID CMD ERROR";
							Test_Step = LAN_TEST_RESULT;
						}
					}
				}
				else
				{
					frmMain->ERROR_MSG = "(!)MCU Power Error";
					frmMain->plMCU->Color = clRed;
					Test_Step = LAN_TEST_RESULT;
				}
			break;
		case MCU_POWER_ON_TEST:
				if(GetTickCount()< SBCTimeOut)
				{
					if(PORT->EnumHUB())
					{
						Test_Step = MCU_TEST;
						SBCTimeOut = GetTickCount()+frmMain->edtTimeOut->Text.ToInt();
						break;
					}
				}
				else
				{
					frmMain->ERROR_MSG = "(!)MCU Power Error";
					frmMain->plMCU->Color = clRed;
					Test_Step = LAN_TEST_RESULT;
				}
			break;
		case MCU_TEST:
				if(GetTickCount()> SBCTimeOut)
				{
					Tx("03 1E");   //031D03
					PORT->Delay(frmMain->edt031ETimeOut->Text.ToInt());
					if(!Rx())
						frmMain->ERROR_MSG ="(!)HID CMD ERROR";
					Test_Step = frmMain->ERROR_MSG=="" ? LAN_TEST : LAN_TEST_RESULT;
					Test_Step = CheckSBCBoard() ? LAN_TEST : LAN_TEST_RESULT;
				}
			break;
		case LAN_TEST:
			frmMain->RegDriverClick();
			frmMain->SaveLogLine("Device Driver Regedit Check");
			//
			frmMain->plResult->Caption = "Testing";
			//��������
			lan = ::CreateThread(0, 1024, LANExecute, NULL, 0,NULL);
			//frmMain->pan_1000->Color = clGreen;
			frmMain->SaveLogLine("Lan Test Start");
			Test_Step = LAN_TEST_RESULT;
			break;
		case LAN_TEST_RESULT:
			if(frmMain->ERROR_MSG!="")
			{
				if(frmMain->ERROR_MSG.Pos("MCU")||frmMain->ERROR_MSG.Pos("SBC")||frmMain->ERROR_MSG.Pos("HID"))
					frmMain->FormUIChange(UI_ERROR,frmMain->ERROR_MSG);
				else frmMain->FormUIChange(UI_LAN_ERROR,frmMain->ERROR_MSG);
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
bool TWORKTHREAD::HID_TurnOn(){
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
//---------------------------------------------------------------------------
void TWORKTHREAD::Tx(AnsiString Value){
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
	UI_DisplayCmd(cmd, szTx);

	delete list;
}
void TWORKTHREAD::UI_DisplayCmd(unsigned char *pBuf, int size){
	String str, str2;
	str = _T("write : ");

	for(int i=0; i<size; i++){
		if(i == 0){
			str2.printf(_T("%02X"), pBuf[i]);
		}else{
			str2.printf(_T(" %02X"), pBuf[i]);
		}

		str += str2;
	}
	frmMain->moDebug->Lines->Add(str);

	//DEBUG(str);
}
//---------------------------------------------------------------------------
bool TWORKTHREAD::Rx()
{
	DWORD nBytes;
	bool bl=false;
	AnsiString Value;

	if(!m_ir.AllocBuf(m_hid.GetRxBytes())){
		MessageBox(NULL, _T("FAIL!"), _T("Allocate read buffer"), MB_ICONSTOP);
		return false;
	}

	if(m_ir.Read(m_hid.m_hRead)){
		if(HID_ReadReport(Value)) bl = true;
	}else{
		if(GetLastError() == ERROR_IO_PENDING){
			WaitForSingleObject(m_hid.m_hRead, 1000);
			if(GetOverlappedResult(m_hid.m_hRead, &m_ir, &nBytes, false)){
				if(HID_ReadReport(Value)) bl = true;
			}
			CancelIo(m_hid.m_hRead);
		}
	}

	return bl;
}
bool __fastcall TWORKTHREAD::HID_ReadReport(AnsiString Value){
	String buf, buf2;
	const unsigned char *report = (const unsigned char *)m_ir.GetBufAddr();

	buf = _T("read : ");

	for(int i=0; i!=m_ir.GetBufSize(); i++){
		if(i==0){
			buf2.printf(_T("%02X"), report[i]);
		}else{
			buf2.printf(_T(" %02X"), report[i]);
		}

		buf += buf2;
	}
	//DEBUG(buf);
	  frmMain->moDebug->Lines->Add(buf);
	AnsiString buf3 = AnsiString(buf.c_str());
	if(Value!="")
	{
		return false;
	}
	else
	{
		if(Test_Step == MCU_TEST)
		{
			if(buf3.Pos("03 1E 00 00 00"))
			{
				Rx_ValueAnalyze(buf3.SubString(23,26));
				return true;
			}
			return false;
		}
		else if(buf3.Pos("03 13 80 80 80"))//power on
		{
			return true;
		}
		else if(buf3.Pos("03 13 80 80 00"))//power off
		{
			return true;
		}
		else return false;
	}
}
bool TWORKTHREAD::Rx_ValueAnalyze(AnsiString Value){
	frmMain->ERROR_MSG=="";
	frmMain->moDebug->Lines->Add("Read :03 1E 00 00 00 "+Value);
	AnsiString BYTE5_HUB1_Status 		= Value.SubString(1,2);
	BYTE5_HUB1_Status = HexToInt(BYTE5_HUB1_Status);
	BYTE5_HUB1_Status = IntToBin(BYTE5_HUB1_Status.ToInt());
	BYTE5_HUB1_Status = BYTE5_HUB1_Status.SubString(BYTE5_HUB1_Status.Length()-2,3);
	if(BYTE5_HUB1_Status != "011")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE5 HUB1 Error :"+BYTE5_HUB1_Status);
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}

	AnsiString BYTE6_HUB2_Status 		= Value.SubString(4,2);
	BYTE6_HUB2_Status = HexToInt(BYTE6_HUB2_Status);
	BYTE6_HUB2_Status = IntToBin(BYTE6_HUB2_Status.ToInt());
	BYTE6_HUB2_Status = BYTE6_HUB2_Status.SubString(BYTE6_HUB2_Status.Length(),1);
	if(BYTE6_HUB2_Status != "1")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE6 HUB2 Error :"+BYTE6_HUB2_Status);
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}

	AnsiString BYTE7_DL_Video_Status 	= Value.SubString(7,2);
	BYTE7_DL_Video_Status = HexToInt(BYTE7_DL_Video_Status);
	BYTE7_DL_Video_Status = IntToBin(BYTE7_DL_Video_Status.ToInt());
	BYTE7_DL_Video_Status = BYTE7_DL_Video_Status.SubString(BYTE7_DL_Video_Status.Length()-4,5);
	if(BYTE7_DL_Video_Status != "00001")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE7 DL_Video Error :"+BYTE7_DL_Video_Status);
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}

	AnsiString BYTE8_LAN_Audeo_Status 	= Value.SubString(10,2);
	BYTE8_LAN_Audeo_Status = HexToInt(BYTE8_LAN_Audeo_Status);
	BYTE8_LAN_Audeo_Status = IntToBin(BYTE8_LAN_Audeo_Status.ToInt());
	BYTE8_LAN_Audeo_Status = BYTE8_LAN_Audeo_Status.SubString(BYTE8_LAN_Audeo_Status.Length()-7,8);
	if(BYTE8_LAN_Audeo_Status.SubString(BYTE8_LAN_Audeo_Status.Length()-2,3) != "000")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE8 LAN Error :"+BYTE8_LAN_Audeo_Status.SubString(BYTE8_LAN_Audeo_Status.Length()-2,3));
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}
	if(BYTE8_LAN_Audeo_Status.SubString(1,2) != "00")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE8 Audio Error :"+BYTE8_LAN_Audeo_Status.SubString(1,2));
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}

	AnsiString BYTE9_GPIO_Status 		= Value.SubString(13,2);
	BYTE9_GPIO_Status = HexToInt(BYTE9_GPIO_Status);
	BYTE9_GPIO_Status = IntToBin(BYTE9_GPIO_Status.ToInt());
	BYTE9_GPIO_Status = BYTE9_GPIO_Status.SubString(BYTE9_GPIO_Status.Length()-7,8);
	if(BYTE9_GPIO_Status != "11110000")
	{
		frmMain->moDebug->Lines->Add("(!)BYTE9 GPIO Error :"+BYTE9_GPIO_Status);
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
	}
	if(frmMain->plMCU->Color != clRed)
		frmMain->plMCU->Color = clGreen;

	AnsiString BYTE10_11_SBC_Voltage    = Value.SubString(16,2)+Value.SubString(19,2);
	BYTE10_11_SBC_Voltage.printf("%.3f",float(HexToInt(BYTE10_11_SBC_Voltage))*0.02112);
	frmMain->moDebug->Lines->Add(BYTE10_11_SBC_Voltage + "V");
	if(BYTE10_11_SBC_Voltage.ToDouble()< frmMain->edtSBC20V_min->Text.ToDouble()
		||BYTE10_11_SBC_Voltage.ToDouble()> frmMain->edtSBC20V_max->Text.ToDouble())
	{
		frmMain->moDebug->Lines->Add("(!)BYTE10�B11 SBC 20V Error :"+BYTE9_GPIO_Status);
		frmMain->ERROR_MSG = "(!)SBC 20V Error";
		frmMain->plSBC->Color = clRed;
	}
	else
	{
		frmMain->plSBC->Color = clGreen;
    }
}
int  TWORKTHREAD::HexToInt(AnsiString HexStr) {//16�i����10�i��
	int iIndex, iHex, totalChar;
	TCHAR HexChar[8];

	HexStr.UpperCase(); // ��r���ন���j�g
	_tcscpy_s(HexChar, 8, WideString(HexStr).c_bstr());
//	strcpy(HexChar, HexStr.c_str()); // ��r���ন�r���}�C
	iHex = 0;
	totalChar = HexStr.Length(); // ���o�r�ꪺ����
	for (iIndex = 0; iIndex < totalChar; iIndex++) {
		iHex <<= 4;
		if ((HexChar[iIndex] >= 0x30) && (HexChar[iIndex] <= 0x39)) {
			iHex += (HexChar[iIndex] - 0x30); // �� 0 - 9 �r���ন�Ʀr
		}
		else if ((HexChar[iIndex] >= 0x41) && (HexChar[iIndex] <= 0x46)) {
			iHex += (HexChar[iIndex] - 0x37); // ?�� A - F �r���ন�Ʀr
		}
		else {
			iHex = 0;
			break;
		}
	}
	return iHex;
}
bool TWORKTHREAD::CheckSBCBoard()
{
	if(frmMain->plMCU->Color != clGreen && frmMain->plMCU->Color != clRed )
	{
		//USBCONInfo->RestartDev("0895","772B");
		frmMain->moDebug->Lines->Add("(!)MCU�S���^��");
		frmMain->ERROR_MSG = "(!)MCU Error";
		frmMain->plMCU->Color = clRed;
		return false;
	}
	return true;
}
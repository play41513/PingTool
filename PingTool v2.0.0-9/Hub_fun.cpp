// ---------------------------------------------------------------------------

#pragma hdrstop

#include "Main.h"
#include "USBDevConnectionInfo.h"
#include "Hub_fun.h"
// ---------------------------------------------------------------------------
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();
#pragma package(smart_init)


Port_test *PORT = new Port_test();
AnsiString Port_test::getLanGUID(AnsiString LANname) { // 得到網卡名稱IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			// 取得網卡名稱並儲存
			AnsiString LanName = (AnsiString)pipai->Description;
			if(LanName.Pos(LANname))
			{
				delete pbBuf1;
				return (AnsiString)pipai->AdapterName;
			}
			pipai = pipai->Next;
			IpCount++;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "";
}

AnsiString Port_test::getLanIP(AnsiString LANname) { // 得到網卡名稱IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			AnsiString LanName = (AnsiString)pipai->Description;
			// 取得網卡名稱並儲存
			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {
				if(LanName.Pos(LANname))
				{
					delete pbBuf1;
					return (AnsiString)pipas->IpAddress.String;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
			IpCount++;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "";
}

AnsiString Port_test::getLanMAC(AnsiString LANname) { // 得到網卡名稱IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			AnsiString LanName = (AnsiString)pipai->Description;
			// 取得網卡名稱並儲存
			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {
				if(LanName.Pos(LANname))
				{
					// Mac ID
					AnsiString szTemp;
					szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
					pipai->Address[1], pipai->Address[2], pipai->Address[3],
					pipai->Address[4], pipai->Address[5]);
					delete pbBuf1;
					return szTemp;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
			IpCount++;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
    return "NO Device";
}

void Port_test::AddNewMAC(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname, AnsiString value) {
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = getLanGUID(frmMain->gLAN_NAME);
	TRegistry *reg = new TRegistry();
	TStringList *ts = new TStringList;
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + LANHUID;
	reg->OpenKey(temppath.c_str(), false);
	reg->GetKeyNames(ts);
	for (int i = 0; i < ts->Count; i++) {
	//路徑名稱補0
		LANnum = ts->Strings[i];
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + LANHUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
		AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");

		if(tempID.Pos(VPID)&&tempGUID.Pos(lanGUID))
		{
			reg->WriteString("*SpeedDuplex", value);
			reg->WriteString("NetworkAddress",  frmMain->ZeroMAC);
		}
	}
	reg->CloseKey();
	delete reg,ts;
}
bool Port_test::ChangeLanSpeed(AnsiString VPID, AnsiString GUID,
	AnsiString LANname) {
	int pingnum = 0;
	AnsiString ip = "";
	if(frmMain->ckbNoMAC->Checked)
	{
		AddNewMAC(GUID, VPID, LANname, "6");  //切速度
		if (!cmd_restartdevice())
		{
			frmMain->ERROR_MSG = "(!)RESTART FAIL";
			return false;
		}
		else frmMain->current_mac->Caption= frmMain->ZeroMAC;
	}
	ULONG timeout = GetTickCount()+20000;
	while(!ip.Pos("192.168")){
		if(frmMain->gLAN_NAME == ""&&GetTickCount() >= (timeout-10000))
		{
			frmMain->bPlugOut = true;
			frmMain->ERROR_MSG = "Not Find DUT";
			frmMain->SaveLogLine(frmMain->gFileName,frmMain->ERROR_MSG);
			return false;
		}
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "(!)Not Find IP";
			frmMain->SaveLogLine(frmMain->gFileName,frmMain->ERROR_MSG);
			return false;
		}
		ip = getLanIP(LANname);
		if(frmMain->g_bError) return false;
		PORT->Delay(100);
	}
	// 得到ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	timeout = GetTickCount()+5000;
	while(!sList->DelimitedText.Pos("192.168"))
	{
		sList->DelimitedText = getDHCP_Server_IP(LANname);
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "(!)Not Find DHCP Server IP";
			frmMain->SaveLogLine(frmMain->gFileName,frmMain->ERROR_MSG);
			delete sList;
			return false;
		}
		if(frmMain->gLAN_NAME == "")
		{
			frmMain->bPlugOut = true;
			frmMain->ERROR_MSG = "Not Find DUT";
			frmMain->SaveLogLine(frmMain->gFileName,frmMain->ERROR_MSG);
			delete sList;
			return false;
		}
		if(frmMain->g_bError) return false;
		PORT->Delay(100);
	}
	AnsiString PingIP;
	if(sList->DelimitedText.Pos("192.168"))
		PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	delete sList;
	if(frmMain->g_bError) return false;
	//
	frmMain->SaveLogLine(frmMain->gFileName,"Start Ping");
	if(!frmMain->Ping(PingIP,false))
	{
		frmMain->ERROR_MSG = "";
		PORT->Delay(100);
		if(!frmMain->Ping(PingIP,true))
		{
			frmMain->SaveLogLine(frmMain->gFileName,frmMain->ERROR_MSG);
			return false;
		}
		else frmMain->ERROR_MSG = "";
	}
	frmMain->pan_1000->Color = clGreen;
	return true;
}
// ---------------------------------------------------------------------------
void Port_test::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}

bool Port_test::cmd_restartdevice() // 重啟設備
{
	DWORD RestartDutTimeOut = 0;
	USBCONInfo->RestartDev(frmMain->g_DevVPID.SubString(5,4), frmMain->g_DevVPID.SubString(14,4));

	RestartDutTimeOut = GetTickCount()+10000;
	while (frmMain->gLAN_NAME == "")
	{
		if(GetTickCount() >= RestartDutTimeOut)
			return false;
		Delay(100);
	}
	return true;
}
char* Port_test::lan_find_inf(char *pData, char *pname) { // 查找字串回傳後面的字串
	char *p, *p1;
	p = pData;
	p1 = strstr(p, pname);
	if (p1 == NULL)
		return NULL;
	p = p1 + strlen(pname);
	return p;
}
AnsiString Port_test::GetLANName(){
	//取得網卡資訊
	ULONG ulLen1=8192;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	AnsiString szTemp;
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//網卡資訊
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			if(!frmMain->TListInitLAN->Text.Pos(lanname))
			{
				if(!lanname.Pos("Wi-Fi")&& !lanname.Pos("Wireless"))
				{
					frmMain->lab_devicename->Caption = lanname;

					szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
					pipai->Address[1], pipai->Address[2], pipai->Address[3],
					pipai->Address[4], pipai->Address[5]);
					frmMain->current_mac->Caption  =  szTemp;

					delete pbBuf1;
					return lanname;
				}
			}
			else
			{
				frmMain->lab_devicename->Caption = "";
				frmMain->current_mac->Caption = "Not find Device";
				frmMain->gLAN_NAME = "";
            }
			pipai=pipai->Next;
		} while(pipai!=NULL);
	}
	frmMain->lab_devicename->Caption = "";
	frmMain->current_mac->Caption = "Not find Device";
	frmMain->gLAN_NAME = "";
	delete pbBuf1;
	return "";
}
AnsiString Port_test::getDHCP_Server_IP(AnsiString LANname) { // 得到DHCP_SERVER IP
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			// 取得網卡名稱並儲存
			AnsiString LanName = (AnsiString)pipai->Description;
			if (LanName.Pos(LANname)) {
				delete pbBuf1;
				AnsiString DHCP_ServerIP = (AnsiString)pipai->DhcpServer.IpAddress.String;
				if(DHCP_ServerIP=="")
				{
					pipas = &(pipai->GatewayList);
					DHCP_ServerIP = (AnsiString)pipas->IpAddress.String;

                }
				return DHCP_ServerIP;
			}
			pipai = pipai->Next;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "";
}
void Port_test::IntiNetwordCard()
{
	//取得網卡資訊
	ULONG ulLen1=8192;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	frmMain->TListInitLAN->Clear();
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//網卡資訊
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			frmMain->TListInitLAN->Add(pipai->Description);
			pipai=pipai->Next;
		}
		while(pipai!=NULL);
	}
	delete pbBuf1;
}


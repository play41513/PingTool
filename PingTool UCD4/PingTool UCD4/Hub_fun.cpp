// ---------------------------------------------------------------------------
#include <windows.h>    // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <cfgmgr32.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <SetupAPI.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
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
			reg->WriteString("NetworkAddress",  "00249B000000");
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
		else frmMain->current_mac->Caption="00:24:9B:00:00:00";
	}
	ULONG timeout = GetTickCount()+20000;
	int NotFindDevCount = 0;
	while(!ip.Pos("192.168")){
		if(frmMain->gLAN_NAME == "")
		{
			NotFindDevCount++;
			if(NotFindDevCount ==4)
			{
				frmMain->bPlugOut = true;
				frmMain->ERROR_MSG = "Not Find DUT";
				frmMain->SaveLogLine(frmMain->ERROR_MSG);
				return false;
			}
		}
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "(!)Not Find IP";
			frmMain->SaveLogLine(frmMain->ERROR_MSG);
			return false;
		}
		ip = getLanIP(LANname);
		if(frmMain->g_bError) return false;
		Sleep(500);
	}
	// 得到ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	timeout = GetTickCount()+5000;
	NotFindDevCount = 0;
	while(!sList->DelimitedText.Pos("192.168"))
	{
		sList->DelimitedText = getDHCP_Server_IP(LANname);
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "(!)Not Find DHCP Server IP";
			frmMain->SaveLogLine(frmMain->ERROR_MSG);
			delete sList;
			return false;
		}
		if(frmMain->gLAN_NAME == "")
		{
			NotFindDevCount++;
			if(NotFindDevCount ==4)
			{
				frmMain->bPlugOut = true;
				frmMain->ERROR_MSG = "Not Find DUT";
				frmMain->SaveLogLine(frmMain->ERROR_MSG);
				delete sList;
				return false;
			}
		}
		if(frmMain->g_bError) return false;
		Sleep(500);
	}
	AnsiString PingIP;
	if(sList->DelimitedText.Pos("192.168"))
		PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	delete sList;
	if(frmMain->g_bError) return false;
	//
	frmMain->SaveLogLine("Start Ping");
	if(!frmMain->Ping(PingIP,false))
	{
		frmMain->ERROR_MSG = "";
		Sleep(100);
		if(!frmMain->Ping(PingIP,true))
		{
			frmMain->SaveLogLine(frmMain->ERROR_MSG);
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
	int res =  GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1);
	if(ERROR_SUCCESS== res)
	{
		//網卡資訊
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			lanname = lanname.Trim();
			int res = frmMain->TListInitLAN->Text.Pos(lanname);
			if(!res)
			{
				if(lanname.Pos("ASIX"))
				{
					if(EnumHUB())
					{
						if(frmMain->lab_devicename->Caption != lanname)
						{
							frmMain->moDebug->Lines->Add("OLD:"+AnsiString(frmMain->lab_devicename->Caption));
							frmMain->lab_devicename->Caption = lanname;
							frmMain->moDebug->Lines->Add("NEW:"+lanname);
						}

						szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
						pipai->Address[1], pipai->Address[2], pipai->Address[3],
						pipai->Address[4], pipai->Address[5]);
						if(frmMain->current_mac->Caption != szTemp)
						{
							frmMain->current_mac->Caption  =  szTemp;
							//frmMain->moDebug->Lines->Add(szTemp);
						}
					}
					else
					{
						frmMain->lab_devicename->Caption = "";
						if(AnsiString(frmMain->current_mac->Caption) != "Not find Device")
						{
							frmMain->current_mac->Caption = "Not find Device";
							frmMain->moDebug->Lines->Add(szTemp);
						}
						delete pbBuf1;
						return "";
					}
				}
				else
				{
					if(frmMain->lab_devicename->Caption != lanname)
						frmMain->lab_devicename->Caption = lanname;
					szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
					pipai->Address[1], pipai->Address[2], pipai->Address[3],
					pipai->Address[4], pipai->Address[5]);
					if(frmMain->current_mac->Caption != szTemp)
						frmMain->current_mac->Caption  =  szTemp;
				}

				delete pbBuf1;
				return lanname;
			}
			pipai=pipai->Next;
		} while(pipai!=NULL);
	}
	delete pbBuf1;
	frmMain->lab_devicename->Caption = "";
	frmMain->current_mac->Caption = "Not find Device";
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
			if(!AnsiString(pipai->Description).Pos("ASIX"))
				frmMain->TListInitLAN->Add(pipai->Description);
			pipai=pipai->Next;
		}
		while(pipai!=NULL);
	}
	delete pbBuf1;
}
bool Port_test::EnumHUB(){
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_HUB =StringToGUID(GUID_USB_HUB);
	//--------------------------------------------------------------------------
	//   獲取設備資訊
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_HUB,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if   (hDevInfo   ==   INVALID_HANDLE_VALUE){
		//DEBUG("ERROR - SetupDiGetClassDevs()"); //   查詢資訊失敗
	}
	else{
	//--------------------------------------------------------------------------
		SP_DEVICE_INTERFACE_DATA   ifdata;
		DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   枚舉每個USB設備
		{
			ifdata.cbSize   =   sizeof(ifdata);
			if (SetupDiEnumDeviceInterfaces(								//   枚舉符合該GUID的設備介面
			hDevInfo,           //   設備資訊集控制碼
			NULL,                         //   不需額外的設備描述
			(LPGUID)&GUID_HUB,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   設備資訊集�堛熙]備序號
			&ifdata))                 //   設備介面資訊
			{
				ULONG predictedLength   =   0;
				ULONG requiredLength   =   0;
				//   取得該設備介面的細節(設備路徑)
				SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
					&ifdata,          //   設備介面資訊
					NULL,             //   設備介面細節(設備路徑)
					0,         	      //   輸出緩衝區大小
					&requiredLength,  //   不需計算輸出緩衝區大小(直接用設定值)
					NULL);            //   不需額外的設備描述
				//   取得該設備介面的細節(設備路徑)
				predictedLength=requiredLength;
				pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
				pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   設備資訊集控制碼
					&ifdata,             //   設備介面資訊
					pDetail,             //   設備介面細節(設備路徑)
					predictedLength,     //   輸出緩衝區大小
					&requiredLength,     //   不需計算輸出緩衝區大小(直接用設定值)
					NULL))               //   不需額外的設備描述
				{
					try
					{
						char   ch[512];
						for(j=0;j<predictedLength;j++)
						{
						ch[j]=*(pDetail->DevicePath+8+j);
						}
						SS=ch;
						SS = SS.SubString(1,SS.Pos("{")-2);
						if(SS.Pos("vid"))
						{
							SS = getUSB_Rev(SS);
							if(SS.Pos("VID_05E3&PID_0610&REV_6954"))
							{
								GlobalFree(pDetail);
								SetupDiDestroyDeviceInfoList(hDevInfo);
								return true;
							}
						}

						//Memo1->Lines->Add(SS);
						//DEBUG(SS);
					}
					catch(...)
					{
						//DEBUG("列舉失敗");
                    }
				}
				GlobalFree(pDetail);
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return false;
}
AnsiString Port_test::getUSB_Rev(AnsiString DEV_PATH)
{
	DEV_PATH = StringReplace(DEV_PATH, "#", "\\", TReplaceFlags()<<rfReplaceAll);
	//AnsiString VPID = DEV_PATH.SubString(1,DEV_PATH.Pos("\\")-1);
	//AnsiString PATH = DEV_PATH.SubString(DEV_PATH.Pos("\\"),DEV_PATH.Length()-DEV_PATH.Pos("\\")+1);
	//DEV_PATH = VPID.UpperCase()+PATH;
	AnsiString DevInfo="";
	AnsiString DevRev ="";

	HKEY hSoftKey = NULL;
	hSoftKey = NULL;
	AnsiString temppath = "system\\CurrentControlSet\\Enum\\USB\\"+DEV_PATH;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, temppath.c_str(), 0, KEY_READ,&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dwType=REG_SZ;
		DWORD dwCount=0;
		LONG lResult = RegQueryValueEx(hSoftKey, "HardwareID", NULL, &dwType,NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			LPBYTE company_Get=new BYTE [dwCount];
			lResult = RegQueryValueEx(hSoftKey, "HardwareID", NULL, &dwType,
			company_Get, &dwCount);
			if(lResult == ERROR_SUCCESS)
			{
				DevRev = AnsiString((char*)company_Get);
			}
			delete company_Get;
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	return DevRev;
}



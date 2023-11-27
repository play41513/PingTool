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
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "";
}
AnsiString Port_test::getLanVPID(AnsiString LANname)
{ // 得到網卡PVID
	AnsiString LAN_GUID = getLanGUID(LANname);
	if(LAN_GUID != "")
	{     //HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\Interfaces\{4837808D-0D1C-40A6-AC90-0B825249F61E}
		AnsiString temppath, temp, LANnum;
		AnsiString lanGUID = getLanGUID(frmMain->gLAN_NAME);
		TRegistry *reg = new TRegistry();
		TStringList *ts = new TStringList;
		reg->RootKey = HKEY_LOCAL_MACHINE;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + GUID_NETWORKCARD;
		reg->OpenKey(temppath.c_str(), false);
		reg->GetKeyNames(ts);
		for (int i = 0; i < ts->Count; i++) {
		//路徑名稱補0
			LANnum = ts->Strings[i];
			temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + GUID_NETWORKCARD +
				"\\" + LANnum;
			reg->CloseKey();
			reg->OpenKey(temppath.c_str(), false);
			long tollnum = reg->InstanceSize();
			AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
			AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
			if(tempGUID.Pos(LAN_GUID))
			{
				reg->CloseKey();
				delete reg,ts;
				return tempID.Delete(1,4);
			}
		}
		reg->CloseKey();
		delete reg,ts;
	}
	return "";
}
AnsiString Port_test::getLanIP(AnsiString LANname) { // 得到網卡名稱IP
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
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "";
}
void Port_test::EnumComboBoxNetwork(TComboBox* cbNetwork)
{ // 得到網卡名稱IP
	DWORD dwCount = 0,dwIndex = 0;
	cbNetwork->Clear();
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			AnsiString LanName = (AnsiString)pipai->Description;
			cbNetwork->Items->Add(LanName);

			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {
				if(AnsiString(pipas->IpAddress.String).Pos(SERVER_IP))
				{
					dwIndex = dwCount;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
			dwCount++;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	cbNetwork->ItemIndex = dwIndex;
}
AnsiString Port_test::getLanMAC(AnsiString LANname) { // 得到網卡名稱IP

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
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return "NO Device";
}

void Port_test::AddNewMAC(AnsiString LANNAME, AnsiString value,AnsiString MAC) {
	if(MAC.Pos(":"))
	{
		MAC = StringReplace(MAC, ":", "",  TReplaceFlags()<<rfReplaceAll);
	}
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = getLanGUID(LANNAME);
	TRegistry *reg = new TRegistry();
	TStringList *ts = new TStringList;
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + GUID_NETWORKCARD;
	reg->OpenKey(temppath.c_str(), false);
	reg->GetKeyNames(ts);
	for (int i = 0; i < ts->Count; i++) {
	//路徑名稱補0
		LANnum = ts->Strings[i];
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + GUID_NETWORKCARD +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
		AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");

		if(tempGUID.Pos(lanGUID))
		{
			if(value != "")
				reg->WriteString("*SpeedDuplex", value);
			reg->WriteString("NetworkAddress",  MAC);
		}
	}
	reg->CloseKey();
	delete reg,ts;
}
bool Port_test::ChangeLanSpeed(AnsiString VPID,AnsiString LANname,bool bUSB3)
{
	AnsiString DEV_IP = NULL,ServerIP = NULL;
	//Check ZeroMAC
	if(frmMain->ckbNoMAC->Checked)
	{
		if(bUSB3)
			AddNewMAC(frmMain->gLAN_NAME, "6",frmMain->ZeroMAC);
		else
			AddNewMAC(frmMain->gLAN_NAME, "4",frmMain->ZeroMAC);
		if (!cmd_restartdevice(VPID))
		{
			frmMain->ERROR_MSG = "(!)RESTART_FAIL";
			return false;
		}
		else if(frmMain->ckbNoMAC->Checked)
			frmMain->current_mac->Caption= frmMain->ZeroMAC;
	}
	//
	ULONG timeout = GetTickCount()+20000;
	while(!DEV_IP.Pos("192.168")){
		if(frmMain->gLAN_NAME == ""&&GetTickCount() >= (timeout-10000))
		{
			frmMain->bPlugOut = true;
			frmMain->ERROR_MSG = "(!)Not_Find_DUT";
			return false;
		}
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "(!)Not_Find_IP";
			frmMain->moDebug->Lines->Add("IP :"+DEV_IP);
			frmMain->moDebug->Lines->Add("LANname :"+LANname);
			return false;
		}
		if(frmMain->ckbStaticIP->Checked)
		{
			if(DEV_IP.Pos("169.254"))
				break;
        }
		DEV_IP = getLanIP(LANname);
		PORT->Delay(100);
	}
	// 得到ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	timeout = GetTickCount()+5000;
	while(!sList->DelimitedText.Pos("192.168"))
	{            	// Check ServerIP
		if(frmMain->ckbStaticIP->Checked)
		{
			if(CheckServerIP())
			{
				sList->DelimitedText = SERVER_IP;
				break;
			}
		}
		else
			sList->DelimitedText = getDHCP_Server_IP(LANname);
		if(GetTickCount() >= timeout)
		{
			frmMain->moDebug->Lines->Add("Server_IP :"+sList->DelimitedText);
			frmMain->ERROR_MSG = "(!)Not_Find_Server_IP";
			delete sList;
			return false;
		}
		if(frmMain->gLAN_NAME == "")
		{
			frmMain->bPlugOut = true;
			frmMain->ERROR_MSG = "(!)Not_Find_DUT";
			delete sList;
			return false;
		}
		PORT->Delay(100);
	}
	AnsiString PingIP;
	if(sList->DelimitedText.Pos("192.168"))
		PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	else
		PingIP = sList->DelimitedText;
	delete sList;
	//
	if(CheckSpeed(LANname,bUSB3))
	{
		if(!frmMain->Ping(PingIP,false))
		{
			frmMain->ERROR_MSG = NULL;
			PORT->Delay(100);
			if(!frmMain->Ping(PingIP,true))
			{
				return false;
			}
			else frmMain->ERROR_MSG = NULL;
		}
	}
	else
	{
		frmMain->ERROR_MSG = "(!)網速未達到(SpeedOfPingFail)";
		return false;
	}
	frmMain->ERROR_MSG = "PASS";
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

bool Port_test::cmd_restartdevice(AnsiString strVPID) // 重啟設備
{
	if(strVPID == "")
		return false;
	DWORD dwRestartDutTimeOut = 0;
	AnsiString DevVPID    = strVPID;
	AnsiString DevVID     = strVPID.SubString(5,4);
	AnsiString DevPID     = strVPID.SubString(14,4);
	AnsiString DevPath = strVPID.Delete(1,strVPID.Pos("\\"));
	USBCONInfo->RestartDev(DevVID, DevPID,DevPath);
	dwRestartDutTimeOut = GetTickCount()+10000;
	while (!EnumUSB(DevVPID))
	{
		if(GetTickCount() >= dwRestartDutTimeOut)
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
	AnsiString szTlemp;
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//網卡資訊
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			if(!StringListTextPosString(frmMain->TListInitLAN,lanname))
			{
				if(!lanname.Pos("Wi-Fi")&& !lanname.Pos("Wireless"))
				{
					frmMain->lab_devicename->Caption = lanname;
					AnsiString szTemp;
					szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
					pipai->Address[1], pipai->Address[2], pipai->Address[3],
					pipai->Address[4], pipai->Address[5]);
					if(szTemp != frmMain->current_mac->Caption)
						frmMain->current_mac->Caption  =  szTemp;

					delete pbBuf1;
					return lanname;
				}
			}
			else
			{
				frmMain->lab_devicename->Caption = "";
				if(frmMain->current_mac->Caption != "Not find Device")
					frmMain->current_mac->Caption = "Not find Device";
				frmMain->gLAN_NAME = "";
            }
			pipai=pipai->Next;
		} while(pipai!=NULL);
	}
	frmMain->lab_devicename->Caption = "";
	if(frmMain->current_mac->Caption != "Not find Device")
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
bool Port_test::CheckSpeed(AnsiString LANNAME,bool bUSB3)
{
    MIB_IFTABLE   *pIfTable = NULL;
    ULONG          dwSize = 0;
	DWORD          dwRet,dwSpeed;
	dwRet = GetIfTable(pIfTable, &dwSize, TRUE);
	bool bHave = false;
	if(bUSB3)
		dwSpeed = 1000;
	else
		dwSpeed = 100;
	if(frmMain->pl1000M->Caption.Pos("2.5"))
    	dwSpeed = 2500;

    if (dwRet == ERROR_INSUFFICIENT_BUFFER)
    {
        pIfTable = (MIB_IFTABLE *) new char[dwSize];
        if (pIfTable != NULL)
        {
			dwRet = GetIfTable(pIfTable, &dwSize, TRUE);
			if (dwRet == NO_ERROR)
            {
				//printf("dwNumEntries = %u\n", pIfTable->dwNumEntries);
				for (DWORD i = 0; i < pIfTable->dwNumEntries; i++)
				{
                    //MIB_IF_TYPE_ETHERNET ->以太网
					if ((pIfTable->table[i]).dwType == MIB_IF_TYPE_ETHERNET && (pIfTable->table[i]).dwAdminStatus == 1
                         && ((pIfTable->table[i].dwOperStatus) == MIB_IF_OPER_STATUS_OPERATIONAL))
					{
						if(pIfTable->table[i].dwSpeed != 0)
						{

							AnsiString str=AnsiString((char*)(pIfTable->table[i]).bDescr);
							if(str.Pos(LANNAME))
							{
								frmMain->moDebug->Lines->Add(str);
								DWORD NowSpeed = (pIfTable->table[i]).dwSpeed /1000000;
								frmMain->moDebug->Lines->Add("dwSpeed ="+AnsiString(NowSpeed)+"M");
								frmMain->moDebug->Lines->Add("*********");
								if(NowSpeed < dwSpeed)
									return false;
								else bHave = true;
							}
						}

					}
				}
				if(bHave) return true;
			}
		}
	}
	return false;
}
bool Port_test::SetIPAddress(AnsiString LANname)
{
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = getLanGUID(LANname);
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\" + lanGUID;
	if(reg->OpenKey(temppath.c_str(), false))
	{
		reg->WriteInteger("DisableDhcpOnConnect",1);
		reg->WriteString("IPAddress",SERVER_IP);
		reg->WriteInteger("EnableDHCP",0);
		reg->WriteString("SubnetMask","255.255.255.0");
		reg->CloseKey();
	}

	temppath = "SYSTEM\\CurrentControlSet\\Services\\"+lanGUID+"\\Parameters\\Tcpip";
	if(reg->OpenKey(temppath.c_str(), false))
	{
		reg->WriteInteger("EnableDHCP",0);
		reg->WriteString("IPAddress",SERVER_IP);
		reg->WriteString("SubnetMask","255.255.255.0");
		reg->CloseKey();
	}
	delete reg;
	//
	AddNewMAC(LANname,"",SERVER_MAC);
	AnsiString strVPID = getLanVPID(LANname);
	//
	return true;
}
AnsiString Port_test::SetServerIPAddress()
{
//
	AnsiString LANname = NULL;
	AnsiString lanGUID = NULL;
	AnsiString ServerIP = NULL;
	AnsiString lanMAC  = NULL;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {

				if(AnsiString(pipas->IpAddress.String).Pos(SERVER_IP))
				{
					// Mac ID
					lanMAC.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
					pipai->Address[1], pipai->Address[2], pipai->Address[3],
					pipai->Address[4], pipai->Address[5]);
					//
					LANname = (AnsiString)pipai->Description;
					lanGUID = (AnsiString)pipai->AdapterName;
					ServerIP = (AnsiString)pipas->IpAddress.String;
					break;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
		}
		while (pipai != NULL && LANname ==NULL);
	}
	delete pbBuf1;
	//
	if(ServerIP.Pos(SERVER_IP) && lanMAC.Pos(SERVER_MAC))
		return ServerIP;
	//
	AnsiString temppath;
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\" + lanGUID;

	if(reg->OpenKey(temppath.c_str(), false))
	{
		reg->WriteInteger("DisableDhcpOnConnect",1);
		reg->WriteString("IPAddress",SERVER_IP);
		reg->WriteInteger("EnableDHCP",0);
		reg->WriteString("SubnetMask","255.255.255.0");
		reg->CloseKey();

		temppath = "SYSTEM\\CurrentControlSet\\Services\\"+lanGUID+"\\Parameters\\Tcpip";
		if(reg->OpenKey(temppath.c_str(), false))
		{
			reg->WriteInteger("EnableDHCP",0);
			reg->WriteString("IPAddress",SERVER_IP);
			reg->WriteString("SubnetMask","255.255.255.0");
			reg->CloseKey();
		}
	}
	delete reg;
	//
	AddNewMAC(LANname,"",SERVER_MAC);
	//
	AnsiString strVPID = getLanVPID(LANname);
	cmd_restartdevice(strVPID);
	AnsiString strIP = getLanIP(LANname);
	//
	return SERVER_IP;
}
bool Port_test::DeleteServerIPAddress()
{
	AnsiString LANname = "";
	AnsiString lanGUID = "";
	AnsiString lanMAC  = "";
	AnsiString ServerIP = "";
	AnsiString strTemp = "";
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {
				// Mac ID
				strTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
				pipai->Address[1], pipai->Address[2], pipai->Address[3],
				pipai->Address[4], pipai->Address[5]);
				if(strTemp.Pos(SERVER_MAC))
				{
					LANname = (AnsiString)pipai->Description;
					lanGUID = (AnsiString)pipai->AdapterName;
					ServerIP = (AnsiString)pipas->IpAddress.String;
					break;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
		}
		while (pipai != NULL && LANname ==NULL);
	}
	delete pbBuf1;
	//
	AnsiString temppath;
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	temppath = "SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\" + lanGUID;
	if(reg->OpenKey(temppath.c_str(), false))
	{
		reg->WriteInteger("DisableDhcpOnConnect",0);
		reg->DeleteValue("IPAddress");
		reg->WriteInteger("EnableDHCP",1);
		reg->DeleteValue("SubnetMask");
		reg->CloseKey();

		temppath = "SYSTEM\\CurrentControlSet\\Services\\"+lanGUID+"\\Parameters\\Tcpip";
		if(reg->OpenKey(temppath.c_str(), false))
		{
			reg->WriteInteger("EnableDHCP",1);
			reg->DeleteValue("IPAddress");
			reg->DeleteValue("SubnetMask");;
			reg->CloseKey();
		}
	delete reg;
	}
	// Delete MAC
	frmMain->N1Click(NULL);
	//
	AnsiString strVPID = getLanVPID(LANname);
	cmd_restartdevice(strVPID);
	//
	return true;
}
bool Port_test::CheckServerIP()
{
	AnsiString ServerIP = "";
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			pipas = &(pipai->IpAddressList); // 取得網卡IP並儲存
			while (pipas != NULL) {
					// Mac ID
				ServerIP = (AnsiString)pipas->IpAddress.String;
				if(ServerIP.Pos(SERVER_IP))
				{
					delete pbBuf1;
					return true;
				}
				pipas = pipas->Next;
			}
			pipai = pipai->Next;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return false;
}
bool Port_test::EnumUSB(AnsiString VPID)
{
	VPID = VPID.SubString(VPID.Pos("VID"),17);
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_USB =StringToGUID(GUID_USB_DEVICE);
	//--------------------------------------------------------------------------
	//   獲取設備資訊
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_USB,
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
			(LPGUID)&GUID_USB,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   設備資訊集裏的設備序號
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
						if(SS.UpperCase().Pos(VPID))
						{
							GlobalFree(pDetail);
							SetupDiDestroyDeviceInfoList(hDevInfo);
							return true;
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
bool Port_test::StringListTextPosString(TStringList*StringList,AnsiString astrNetworkCardName)
{
	for(int i = 0 ; i < StringList->Count ; i++)
	{
		if(StringList->Strings[i] == astrNetworkCardName)
			return true;
	}
	return false;
}


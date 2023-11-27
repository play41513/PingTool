// ---------------------------------------------------------------------------
#include <windows.h>    // �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <cfgmgr32.h>	// �w������USB�˸m�� *�n�� vcl.h ���sĶ
#include <SetupAPI.h>	// �w������USB�˸m�� *�n�� vcl.h ���sĶ
#pragma hdrstop

#include "Main.h"
#include "USBDevConnectionInfo.h"
#include "Hub_fun.h"
// ---------------------------------------------------------------------------
USBDevConnectionInfo *USBCONInfo = new USBDevConnectionInfo();
#pragma package(smart_init)


Port_test *PORT = new Port_test();
AnsiString Port_test::getLanGUID(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			// ���o���d�W�٨��x�s
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

AnsiString Port_test::getLanIP(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			AnsiString LanName = (AnsiString)pipai->Description;
			// ���o���d�W�٨��x�s
			pipas = &(pipai->IpAddressList); // ���o���dIP���x�s
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

AnsiString Port_test::getLanMAC(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			AnsiString LanName = (AnsiString)pipai->Description;
			// ���o���d�W�٨��x�s
			pipas = &(pipai->IpAddressList); // ���o���dIP���x�s
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
	//���|�W�ٸ�0
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
		AddNewMAC(GUID, VPID, LANname, "6");  //���t��
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
	// �o��ping ip
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
	if(CheckSpeed(LANname,4))
	{
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
	}
	frmMain->pan_1000->Color = clGreen;
	return true;
}
// ---------------------------------------------------------------------------
void Port_test::Delay(DWORD iMilliSeconds) // �쪩delay time �Φbthread�̭�
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}

bool Port_test::cmd_restartdevice() // ���ҳ]��
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
char* Port_test::lan_find_inf(char *pData, char *pname) { // �d��r��^�ǫ᭱���r��
	char *p, *p1;
	p = pData;
	p1 = strstr(p, pname);
	if (p1 == NULL)
		return NULL;
	p = p1 + strlen(pname);
	return p;
}
AnsiString Port_test::GetLANName(){
	//���o���d��T
	ULONG ulLen1=8192;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	AnsiString szTemp;
	int res =  GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1);
	if(ERROR_SUCCESS== res)
	{
		//���d��T
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			lanname = lanname.Trim();

			if(lanname.Pos("ASIX"))
			{
				if(EnumHUB(true))
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
					delete pbBuf1;
					return lanname;
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
			pipai=pipai->Next;
		}
		while(pipai!=NULL);
	}
	delete pbBuf1;
	frmMain->lab_devicename->Caption = "";
	frmMain->current_mac->Caption = "Not find Device";
	return "";
}
AnsiString Port_test::getDHCP_Server_IP(AnsiString LANname) { // �o��DHCP_SERVER IP
	ULONG ulLen1 = 8192;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			// ���o���d�W�٨��x�s
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
	//���o���d��T
	ULONG ulLen1=8192;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	frmMain->TListInitLAN->Clear();
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//���d��T
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
bool Port_test::EnumHUB(bool bUSB3){
	bool bPID0620_REV6942 = false;
	bool bPID0625_REV6954 = false;
	bool bPID0610_REV6954 = false;
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i,j;
	AnsiString SS,USBPath;
	PSP_DEVICE_INTERFACE_DETAIL_DATA   pDetail   =NULL;
	GUID GUID_HUB =StringToGUID(GUID_USB_HUB);
	//--------------------------------------------------------------------------
	//   ����]�Ƹ�T
	hDevInfo = SetupDiGetClassDevs((LPGUID)&GUID_HUB,
	0,   //   Enumerator
	0,
	DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if   (hDevInfo   ==   INVALID_HANDLE_VALUE){
		//DEBUG("ERROR - SetupDiGetClassDevs()"); //   �d�߸�T����
	}
	else{
	//--------------------------------------------------------------------------
		SP_DEVICE_INTERFACE_DATA   ifdata;
		DeviceInfoData.cbSize   =   sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData);i++)	//   �T�|�C��USB�]��
		{
			ifdata.cbSize   =   sizeof(ifdata);
			if (SetupDiEnumDeviceInterfaces(								//   �T�|�ŦX��GUID���]�Ƥ���
			hDevInfo,           //   �]�Ƹ�T������X
			NULL,                         //   �����B�~���]�ƴy�z
			(LPGUID)&GUID_HUB,//GUID_CLASS_USB_DEVICE,                     //   GUID
			(ULONG)i,       //   �]�Ƹ�T���ت��]�ƧǸ�
			&ifdata))                 //   �]�Ƥ�����T
			{
				ULONG predictedLength   =   0;
				ULONG requiredLength   =   0;
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,          //   �]�Ƥ�����T
					NULL,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					0,         	      //   ��X�w�İϤj�p
					&requiredLength,  //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					NULL);            //   �����B�~���]�ƴy�z
				//   ���o�ӳ]�Ƥ������Ӹ`(�]�Ƹ��|)
				predictedLength=requiredLength;
				pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT,   predictedLength);
				pDetail->cbSize   =   sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

				if(SetupDiGetInterfaceDeviceDetail(hDevInfo,         //   �]�Ƹ�T������X
					&ifdata,             //   �]�Ƥ�����T
					pDetail,             //   �]�Ƥ����Ӹ`(�]�Ƹ��|)
					predictedLength,     //   ��X�w�İϤj�p
					&requiredLength,     //   ���ݭp���X�w�İϤj�p(�����γ]�w��)
					NULL))               //   �����B�~���]�ƴy�z
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
								bPID0610_REV6954 = true;
							}
							else if(SS.Pos("VID_05E3&PID_0625&REV_6954"))
							{
								bPID0625_REV6954 = true;
							}
							else if(SS.Pos("VID_05E3&PID_0620&REV_6942"))
							{
								bPID0620_REV6942 = true;
							}
						}

						//Memo1->Lines->Add(SS);
						//DEBUG(SS);
					}
					catch(...)
					{
						//DEBUG("�C�|����");
                    }
				}
				GlobalFree(pDetail);
			}
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	if(bUSB3)
	{
		if(bPID0610_REV6954 && bPID0625_REV6954 && bPID0620_REV6942)
			return true;
	}
	else
	{
		if(bPID0610_REV6954 && !bPID0625_REV6954 && !bPID0620_REV6942)
			return true;
    }
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
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, temppath.c_str(), 0, KEY_READ,&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dwType=REG_SZ;
		DWORD dwCount=0;
		LONG lResult = RegQueryValueExA(hSoftKey, "HardwareID", NULL, &dwType,NULL, &dwCount);
		if (lResult == ERROR_SUCCESS)
		{
			LPBYTE company_Get=new BYTE [dwCount];
			lResult = RegQueryValueExA(hSoftKey, "HardwareID", NULL, &dwType,
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
bool Port_test::CheckSpeed(AnsiString LANNAME,DWORD dwSpeed)
{
    MIB_IFTABLE   *pIfTable = NULL;
    ULONG          dwSize = 0;
    DWORD          dwRet;
	dwRet = GetIfTable(pIfTable, &dwSize, TRUE);
	bool bHave = false;
	if(dwSpeed==6) dwSpeed = 1000;
	if(dwSpeed==4) dwSpeed = 100;
	if(dwSpeed==2) dwSpeed = 10;
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
                    //MIB_IF_TYPE_ETHERNET ->�H���I
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
								if(NowSpeed != dwSpeed)
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


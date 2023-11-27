// ---------------------------------------------------------------------------

#pragma hdrstop

#include "Main.h"
#include "Hub_fun.h"
// ---------------------------------------------------------------------------

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
	AnsiString lanname = VPIDGetLanName(VPID);
	AnsiString lanGUID = getLanGUID(lanname);
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
		if (!cmd_restartdevice(VPID))
		{
			frmMain->ERROR_MSG = "(!)RESTART FAIL";
			return false;
		}
		else frmMain->current_mac->Caption="00:24:9B:00:00:00";
	}
	ULONG timeout = GetTickCount()+15000;
	while(!ip.Pos("192.168")){
		if(GetTickCount() >= timeout)
		{
			frmMain->ERROR_MSG = "Not Find IP...";
			frmMain->SaveLogLine(frmMain->ERROR_MSG);
			return false;
		}
		ip = getLanIP(LANname);
		if(frmMain->g_bError) return false;
		PORT->Delay(100);
	}
	// 得到ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	sList->DelimitedText = getDHCP_Server_IP(LANname);
	AnsiString PingIP;
	if(sList->DelimitedText.Pos("192.168"))
		PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	delete sList;
	if(frmMain->g_bError) return false;
	if(!PingIP.Pos("192.168"))
	{
		frmMain->ERROR_MSG = "(!)Not Find DHCP Server IP";
		frmMain->SaveLogLine(frmMain->ERROR_MSG);
		return false;
	}
	//
	frmMain->SaveLogLine("Start Ping");
	if(!frmMain->Ping(PingIP))
	{
		frmMain->SaveLogLine(frmMain->ERROR_MSG);
		return false;
	}
	frmMain->pan_1000->Color = clGreen;
	return true;
}
bool Port_test::cmd_ping(AnsiString DevIP,AnsiString PingIP,int speed) { // fping
	DeleteFile("pingmsg.log");
	if (CallCommand_C("ping " + PingIP + " -S "+DevIP+" -l 65500 -n 3 \> pingmsg.log"))
	{
		if (Findfilemsg("pingmsg.log", "已遺失 = 0", 0) != NULL){  //為了加快速度改為2秒
			if(ReadPingSpeed("pingmsg.log",speed))
			{
				return true;
			}
			else
			{
				frmMain->ERROR_MSG = "網速未達到1000M(Speed of PING Fail)";
				return false;
			}
			return true;
		}
		frmMain->ERROR_MSG = "PING 3秒內發生失敗(PING FAIL)";
		return false;
	}
	else{
		frmMain->ERROR_MSG = "PING 3秒內發生失敗(PING FAIL)";
		return false;
	}
}
// ---------------------------------------------------------------------------
void Port_test::Delay(DWORD iMilliSeconds) // 原版delay time 用在thread裡面
{
	DWORD iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}

bool Port_test::cmd_restartdevice(AnsiString PVID) // 重啟設備
{
	DWORD RestartDutTimeOut = 0;
	DosCommand("TEST_TOOL\\devcon6 restart \"usb\\"+PVID+"\*\"");

	RestartDutTimeOut = GetTickCount()+10000;
	while (!FindDevice(PVID))
	{
		if(GetTickCount() >= RestartDutTimeOut)
			return false;
		Delay(100);
	}
	return true;
}

bool Port_test::CallCommand_C(AnsiString Command_line) {
	// 下Command指令 BY CreateProcess
	UINT Result;
	DWORD dwExitCode;
	STARTUPINFOA si = {
		0
	};
	PROCESS_INFORMATION pi;
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoA(&si);
	si.wShowWindow = SW_HIDE;
	AnsiString cmd = getenv("COMSPEC");
	AnsiString Command = cmd + " /c " + Command_line;
	Result = CreateProcessA(NULL, Command.c_str(), NULL, NULL, false, 0, NULL,
		NULL, &si, &pi);
	if (Result) {
		CloseHandle(pi.hThread);
		if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_FAILED) {
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
		}
		CloseHandle(pi.hProcess);
		return true;
	}
	return false;
}

AnsiString Port_test::Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum) { // 找檔案找到字串行回傳幾行後的字串
	ifstream lanfile(filename.c_str());
	std::string filemsg;
	if (lanfile.is_open()) {
		while (!lanfile.eof()) {
			getline(lanfile, filemsg);
			if (strstr(filemsg.c_str(), findmsg.c_str())) {
				for (int i = 0; i < rownum; i++)
					getline(lanfile, filemsg);
				lanfile.close();
				return(AnsiString)filemsg.c_str();
			}
		}
		lanfile.close();
		return "";
	}
	else
		return "";
}

AnsiString Port_test::VPIDGetLanName(AnsiString PVID){ // 由VIDPID取得網卡名稱
	AnsiString sResult = DosCommand("TEST_TOOL\\devcon6 hwids \*\"" + PVID + "\"\*");
	if(sResult.Pos(PVID))
	{
		char *pch = strtok(sResult.c_str(), "\r\n");
		while (pch != NULL)
		{
			if(AnsiString(pch).Pos("Name:"))
			{
				AnsiString Temp = AnsiString(pch).SubString(AnsiString(pch).Pos("Name:")+5, AnsiString(pch).Length());
				return Temp.Trim();
			}
			pch = strtok(NULL, "\r\n");
		}
	}
	return "";
}

AnsiString Port_test::GetLanPVID(AnsiString Lanname){ // 由網卡名稱取得VPID
	AnsiString sResult = DosCommand("TEST_TOOL\\devcon6 hwids usb\* ");
	if(sResult.Pos(Lanname))
	{
		char *pch = strtok(sResult.c_str(), "\r\n");
		while (pch != NULL)
		{
			if(AnsiString(pch).Pos(Lanname))
			{
				pch = strtok(NULL, "\r\n");
				pch = strtok(NULL, "\r\n");
				pch = strtok(NULL, "\r\n");
				return AnsiString(pch).SubString(AnsiString(pch).Pos("VID_"), 17);
			}
			pch = strtok(NULL, "\r\n");
		}
	}
	return "";
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

bool Port_test::FindDevice(AnsiString PVID) { // 查找設備
	AnsiString sResult = DosCommand("TEST_TOOL\\devcon6 find usb\*");
	if(sResult.Pos(PVID))
		return true;
	else return false;
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
				frmMain->lab_devicename->Caption = lanname;

				szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
				pipai->Address[1], pipai->Address[2], pipai->Address[3],
				pipai->Address[4], pipai->Address[5]);
				frmMain->current_mac->Caption  =  szTemp;

				delete pbBuf1;
				return lanname;
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
	delete pbBuf1;
	return "";
}

bool Port_test::Analyze_USBView(AnsiString VID,AnsiString PID)
{
 if(FileExists("USB_INFO.txt")) DeleteFile("USB_INFO.txt");
 DosCommand("TEST_TOOL\\DisplayUSB.exe SAVE");
 DosCommand("Taskkill /im DisplayUSB.exe /F"); //有時USBVIEW無法完全關閉
 if(FileExists("USB_INFO.txt"))
 {
  AnsiString filename = "USB_INFO.txt";
  ifstream lanfile(filename.c_str());
  std::string filemsg;
  if (lanfile.is_open()) {
   while (!lanfile.eof()) {
	getline(lanfile, filemsg);
	if (strstr(filemsg.c_str(),("wVendorID :0x"+VID).c_str()))//確認裝置VID
    {
     getline(lanfile, filemsg);
     if (strstr(filemsg.c_str(),("wProductID :0x"+PID).c_str()))//確認裝置PID
	 {
	  while(!lanfile.eof())
	  {
       getline(lanfile, filemsg);
       if(strstr(filemsg.c_str(),"wMaxPacketSize "))
       {
		if(atoi(filemsg.substr(20,4).c_str())==400)
		{
		 lanfile.close();
		 DeleteFile("USB_INFO.txt");
		 return true;
		}
		else if(atoi(filemsg.substr(20,41).c_str())==200)
		{
		 lanfile.close();
		 DeleteFile("USB_INFO.txt");
		 return false;
		}
	   }
	  }
	 }
	}
   }
   lanfile.close();
   DeleteFile("USB_INFO.txt");
   return false;
  }
  else
  {
   return false;
  }
 }
 return false;
}
bool Port_test::ReadPingSpeed(AnsiString File,int LanSpeed)
{
	AnsiString SS = Findfilemsg(File, "平均 =", 0);
	if(SS!=NULL)
	{
		AnsiString sTIME = AnsiString(lan_find_inf(SS.c_str(),"平均 = "));
		double TIME = sTIME.SubString(1,sTIME.Length()-2).ToDouble();
		switch(LanSpeed)
		{
			case 10:
				if(TIME>100) return true;
			break;
			case 100:
				if(100>TIME&&TIME>10) return true;
			break;
			case 1000:
				if(10>TIME) return true;
			break;
		}
	}
	return false;
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
AnsiString Port_test::DosCommand(AnsiString sCmdline)
{
	PROCESS_INFORMATION proc = {0}; //關於進程資訊的一個結構
	long ret;
	bool sPipe;
	STARTUPINFOA start = {0};
	SECURITY_ATTRIBUTES sa = {0};
	HANDLE hReadPipe ;
	HANDLE hWritePipe;
	AnsiString sOutput;
	AnsiString sBuffer;
	unsigned long lngBytesRead;
	char cBuffer[256];
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor= 0;
	sa.bInheritHandle = TRUE;
	sPipe=::CreatePipe(&hReadPipe, &hWritePipe,&sa, 0); //創建管道
	if (!sPipe)
	{
		sOutput="CreatePipe failed. Error: "+AnsiString(GetLastError());
		return sOutput;
	}
	start.cb = sizeof(STARTUPINFOA);
	start.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	start.hStdOutput = hWritePipe;
	start.hStdError = hWritePipe;
	start.wShowWindow = SW_HIDE;
	sBuffer = sCmdline;
	ret =::CreateProcessA(0, sBuffer.c_str(), &sa, &sa, TRUE, NORMAL_PRIORITY_CLASS, 0, 0, &start, &proc);
	if (ret == 0)
	{
	sOutput="Bad command or filename";
	return sOutput;
	}
	::CloseHandle(hWritePipe);
	do
	{
	memset(cBuffer,'\0',sizeof(cBuffer));
	ret = ::ReadFile(hReadPipe, &cBuffer, 255, &lngBytesRead, 0);
	//替換字串
	for(unsigned long i=0; i< lngBytesRead; i++){
		if(cBuffer[i] == '\0'){
			cBuffer[i] = ' ';
		}else if(cBuffer[i] == '\n'){
			cBuffer[i] = ' ';
		}
	}
	sBuffer=StrPas(cBuffer);
	sOutput = sOutput+sBuffer;
	Application->ProcessMessages();

	} while (ret != 0 );
	::CloseHandle(proc.hProcess);
	::CloseHandle(proc.hThread);
	::CloseHandle(hReadPipe);
	return sOutput;
}



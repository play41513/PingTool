// ---------------------------------------------------------------------------

#pragma hdrstop

#include "Main.h"
#include "Hub_fun.h"
// ---------------------------------------------------------------------------

#pragma package(smart_init)

Port_test *PORT = new Port_test();

bool Port_test::USB_RT(AnsiString filename) { // USBŪ�g�ɮפ��
	bool Result = true;
	AnsiString tempN;
	for (char i = 'A'; i < 'Z'; i++) {
		// filename = "F://USB_TEST4.txt";
		tempN = (AnsiString)i + "://" + filename;
		ifstream Ufile(tempN.c_str());
		ifstream Lfile("USB_TEST.txt");
		std::string Ufilemsg;
		std::string Lfilemsg;
		if (Ufile.is_open() && Lfile.is_open()) {
			//Ū�ɤ��
			while (!Ufile.eof() && !Lfile.eof()) {
				getline(Ufile, Ufilemsg);
				getline(Lfile, Lfilemsg);
				if (Ufilemsg == Lfilemsg) {
					//�g�ɤ��
					tempN = (AnsiString)i + "://UWT.txt";
					fstream fp;
					fp.open(tempN.c_str(), ios::out); // �}���ɮ�
					if(!fp)
						return false;
					fp << "ACTIONSTAR_USB3.0_TEST" << endl;
					fp.close(); // �����ɮ�
					if(Findfilemsg(tempN, "ACTIONSTAR_USB3.0_TEST", 0)!=NULL){
						remove(tempN.c_str());
						return true;
					}
				}
			}
			Lfile.close();
			Ufile.close();
		}
	}
	return false;
}

AnsiString Port_test::getLanGUID(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 4096;
	BYTE *pbBuf1 = new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai = NULL;
	IP_ADDR_STRING *pipas = NULL;
	if (ERROR_SUCCESS == GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1)) {
		pipai = (IP_ADAPTER_INFO*)pbBuf1;
		do {
			// ���o���d�W�٨��x�s
			AnsiString LanName = (AnsiString)pipai->Description;
			//AnsiString LanGUID = (AnsiString)pipai->AdapterName;
			if (PORT->lan_find_inf(LanName.c_str(), LANname.c_str())) {
				delete pbBuf1;
				return (AnsiString)pipai->AdapterName;
			}
			pipai = pipai->Next;
			IpCount++;
		}
		while (pipai != NULL);
	}
	delete pbBuf1;
	return -1;
}

AnsiString Port_test::getLanIP(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 4096;
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
				if (PORT->lan_find_inf(LanName.c_str(), LANname.c_str())) {
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
}

AnsiString Port_test::getLanMAC(AnsiString LANname) { // �o����d�W��IP
	int IpCount = 0;
	ULONG ulLen1 = 4096;
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
				if (PORT->lan_find_inf(LanName.c_str(), LANname.c_str())) {
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

void Port_test::Search_System_Exist_Lan_Speed(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname, AnsiString value) { // �ק�����t�׮ھں��d�W�٥��� (�̾ڵn����)
	AnsiString temppath, temp, LANnum;
	AnsiString lanname = GetLanName(VPID);
	AnsiString lanGUID = getLanGUID(lanname);
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	for (int i = 0; i <= 9999; i++) {
	//���|�W�ٸ�0
		if (i < 10)
			LANnum = "000" + (AnsiString)i;
		if (i >= 10 && i < 100)
			LANnum = "00" + (AnsiString)i;
		if (i >= 100 && i < 1000)
			LANnum = "0" + (AnsiString)i;
		if (i >= 1000 && i < 10000)
			LANnum = (AnsiString)i;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + LANHUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
		AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");
		if (lan_find_inf(tempID.c_str(), VPID.c_str()) && lan_find_inf(tempGUID.c_str(), lanGUID.c_str())) {
			if(tempDeviceID == "ASIX AX88772B USB2.0 to Fast Ethernet Adapter (Test Driver)"){
				reg->WriteString("ConnectionType", value);
			}
			else{
				reg->WriteString("*SpeedDuplex", value);
			}
		}
	}
	reg->CloseKey();
	delete reg;
}

void Port_test::AddNewMAC(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname, AnsiString value) {
	AnsiString temppath, temp, LANnum;
	AnsiString lanname = GetLanName(VPID);
	AnsiString lanGUID = getLanGUID(lanname);
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	for (int i = 0; i <= 9999; i++) {
	//���|�W�ٸ�0
		if (i < 10)
			LANnum = "000" + (AnsiString)i;
		if (i >= 10 && i < 100)
			LANnum = "00" + (AnsiString)i;
		if (i >= 100 && i < 1000)
			LANnum = "0" + (AnsiString)i;
		if (i >= 1000 && i < 10000)
			LANnum = (AnsiString)i;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + LANHUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
		AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");
		if (lan_find_inf(tempID.c_str(), VPID.c_str()) && lan_find_inf(tempGUID.c_str(), lanGUID.c_str())) {
			if(tempDeviceID.Pos("Realtek USB")){
				reg->WriteString("*SpeedDuplex", value);
				reg->WriteString("NetworkAddress",  "00249B000000");
			}

		}
	}
	reg->CloseKey();
	delete reg;
}

AnsiString Port_test::READ_System_Exist_Lan_Speed(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname) { // �ק�����t�׮ھں��d�W�٥��� (�̾ڵn����)
	AnsiString temppath, temp, LANnum;
	AnsiString lanname = GetLanName(VPID);
	AnsiString lanGUID = getLanGUID(lanname);
	AnsiString LanSpeed;
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	for (int i = 0; i <= 9999; i++) {
	//���|�W�ٸ�0
		if (i < 10)
			LANnum = "000" + (AnsiString)i;
		if (i >= 10 && i < 100)
			LANnum = "00" + (AnsiString)i;
		if (i >= 100 && i < 1000)
			LANnum = "0" + (AnsiString)i;
		if (i >= 1000 && i < 10000)
			LANnum = (AnsiString)i;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + LANHUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempID = (AnsiString)reg->ReadString("DeviceInstanceID");
		AnsiString tempGUID = (AnsiString)reg->ReadString("NetCfgInstanceId");
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");
		//frmMain->Memo2->Lines->Add(tempDeviceID);
		//frmMain->Memo2->Lines->Add("ASIX AX88772B USB2.0 to Fast Ethernet Adapter (Test Driver)");
		if (lan_find_inf(tempID.c_str(), VPID.c_str()) && lan_find_inf(tempGUID.c_str(), lanGUID.c_str())) {
			if(tempDeviceID == "ASIX AX88772B USB2.0 to Fast Ethernet Adapter (Test Driver)"){
				LanSpeed = (AnsiString)reg->ReadString("ConnectionType");
				//reg->WriteString("ConnectionType", value);
				//frmMain->Memo2->Lines->Add("is same!");
			}
			else{
				LanSpeed = (AnsiString)reg->ReadString("*SpeedDuplex");
			}
		}
	}
	reg->CloseKey();
	delete reg;
	return LanSpeed;
}


bool Port_test::ChangeLanSpeed(AnsiString VPID, AnsiString GUID,
	AnsiString LANname) {
	int pingnum = 0;
	AnsiString ip = "";
	pingnum = 0;
	ip="";
	if(frmMain->ckbNoMAC->Checked)
	{
		AddNewMAC(GUID, VPID, LANname, "6");  //���t��
		if (!cmd_restartdevice(VPID))
			return false;
		else frmMain->current_mac->Caption="00:24:9B:00:00:00";
	}

	while(!lan_find_inf(ip.c_str(), "192.168")){
		pingnum++;
		Delay(100);
		ip = getLanIP(LANname);
		if(pingnum == 100){
			frmMain->Height = 289;
			frmMain->plErrMsg->Caption = "�L�k�t��IP...";
			frmMain->pan_1000->Color = clRed;  //20161103 jim�[�O��
			return false;
		}
	}
	// �o��ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	sList->DelimitedText = ip;
	AnsiString PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	delete sList;
	//
	if(!Fping(PingIP)){
		frmMain->Height = 289;
		frmMain->plErrMsg->Caption = "PING 3�����o�ͥ���";
		frmMain->pan_1000->Color = clRed;  //20161103 jim�[�O��
		return false;
	}
	frmMain->pan_1000->Color = clGreen;   //20161103 jim�[�O��
	if(!frmMain->TestResult)
			return false;
	Search_System_Exist_Lan_Speed(GUID, VPID, LANname, "4");  //���t�� 100M
	if (!cmd_restartdevice(VPID))
		return false;
	pingnum = 0;
	ip="";
	while(!lan_find_inf(ip.c_str(), "192.168")){
		pingnum++;
		Delay(100);
		ip = getLanIP(LANname);
		if(pingnum == 100){
			frmMain->Height = 289;
			frmMain->plErrMsg->Caption = "�L�k�t��IP...";
			frmMain->pan_100->Color = clRed;
			return false;
		}
	}
	if(!Fping(PingIP)){
		frmMain->Height = 289;
		frmMain->plErrMsg->Caption = "PING 3�����o�ͥ���";
		frmMain->pan_100->Color = clRed;
		return false;
	}
	frmMain->pan_100->Color = clGreen;
		if(!frmMain->TestResult)
				return false;
		Search_System_Exist_Lan_Speed(GUID, VPID, LANname, "2");  //���t�� 10M
		if (!cmd_restartdevice(VPID))
			return false;
		pingnum = 0;
		ip="";
		while(!lan_find_inf(ip.c_str(), "192.168")){
			pingnum++;
			Delay(100);
			ip = getLanIP(LANname);
			if(pingnum == 100){
				frmMain->Height = 289;
				frmMain->plErrMsg->Caption = "�L�k�t��IP...";
				frmMain->pan_10->Color = clRed;
				return false;
			}
		}
		if(!Fping(PingIP)){
			frmMain->Height = 289;
			frmMain->plErrMsg->Caption = "PING 3�����o�ͥ���";
			frmMain->pan_10->Color = clRed;
			return false;
		}
		frmMain->pan_10->Color = clGreen;
		Search_System_Exist_Lan_Speed(GUID, VPID, LANname, "6");  //���t�� 1000M
	return true;
}

bool Port_test::ChangeLanSpeed2(AnsiString VPID, AnsiString GUID,
	AnsiString LANname) {
	int pingnum = 0;
	AnsiString ip = "";
	pingnum = 0;
	ip="";
	while(!lan_find_inf(ip.c_str(), "192.168")){
		pingnum++;
		Delay(100);
		ip = getLanIP(LANname);
		if(pingnum == 100){
			frmMain->Height = 289;   //20161130 jim �[��ܿ��~�T��
			frmMain->plErrMsg->Caption = "�L�k�t��IP...";
			frmMain->pan_100->Color = clRed;  //20161103 jim�[�O��
			return false;
		}
	}
	// �o��ping ip
	TStringList *sList = new TStringList();
	sList->Delimiter = '.';
	sList->DelimitedText = ip;
	AnsiString PingIP = sList->Strings[0]+"."+sList->Strings[1]+"."+sList->Strings[2]+".1";
	delete sList;
	//
	if(!Fping(PingIP)){
		frmMain->Height = 289;
		frmMain->plErrMsg->Caption = "PING 3�����o�ͥ���";
		frmMain->pan_100->Color = clRed;  //20161103 jim�[�O��
		return false;
	}
	frmMain->pan_100->Color = clGreen;   //20161103 jim�[�O��
	if(!frmMain->TestResult)
			return false;
	Search_System_Exist_Lan_Speed(GUID, VPID, LANname, "2");  //���t�� 10M
	if (!cmd_restartdevice(VPID))
		return false;
	pingnum = 0;
	ip="";
	while(!lan_find_inf(ip.c_str(), "192.168")){
		pingnum++;
		Delay(100);
		ip = getLanIP(LANname);
		if(pingnum == 100){
			frmMain->Height = 289;
			frmMain->plErrMsg->Caption = "�L�k�t��IP...";
			frmMain->pan_10->Color = clRed;
			return false;
		}
	}
	if(!Fping(PingIP)){
		frmMain->Height = 289;
		frmMain->plErrMsg->Caption = "PING 3�����o�ͥ���";
		frmMain->pan_10->Color = clRed;
		return false;
	}
	frmMain->pan_10->Color = clGreen;
	Search_System_Exist_Lan_Speed(GUID, VPID, LANname, "4");  //���t�� 100M
	return true;
}

bool Port_test::Fping(AnsiString PingIP) { // fping
	DeleteFile("pingmsg.log");
	if (CallCommand_C("ping " + PingIP + " -n 3 \> pingmsg.log")) {//20170816 jim ���F��88772 10M 100M �b�I��ping �P�ɴ��շ|�Ĭ���D�ӧ�
		if (Findfilemsg("pingmsg.log", "�w�� = 0", 0) != NULL){  //���F�[�ֳt�קאּ2��

			return true;
		}
		//DeleteFile("pingmsg.log");
		return false;
	}
	else{
		//DeleteFile("pingmsg.log");
		return false;
	}
}
// ---------------------------------------------------------------------------
void Port_test::Delay(int iMilliSeconds) // �쪩delay time �Φbthread�̭�
{
	int iStart;
	iStart = GetTickCount();
	while (GetTickCount() - iStart <= iMilliSeconds)
		Application->ProcessMessages();
}

bool Port_test::cmd_restartdevice(AnsiString PVID) // ���ҳ]��
{
	DWORD RestartDutTimeOut = 0;
	if(CallCommand_C("TEST_TOOL\\devcon6 restart \"usb\\"+PVID+"\*\"")){
		RestartDutTimeOut = GetTickCount()+10000;
		while (!FindDevice(PVID))
		{
			if(GetTickCount() >= RestartDutTimeOut)
				return false;
			Delay(100);
		}
		return true;
	}
	else
		return false;
}

bool Port_test::CallCommand_C(AnsiString Command_line) {
	// �UCommand���O BY CreateProcess
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
}

AnsiString Port_test::Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum) { // ���ɮק��r���^�ǴX��᪺�r��
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
		return NULL;
	}
	else
		return "�ɮ׶}�ҥ���!";
}

AnsiString Port_test::GetLanName(AnsiString PVID){ // ��VIDPID���o���d�W��
	AnsiString tempC = "TEST_TOOL\\devcon6 hwids \*\"" + PVID + "\"\* \> templog.txt";
	if (CallCommand_C(tempC.c_str())) {
		AnsiString lanname = Findfilemsg("templog.txt", "Name:", 0);
		if(lanname == 0){
			DeleteFile("templog.txt");
			return "NOT FIND LAN CARD";
		}
		lanname = lanname.SubString(11, lanname.Length());
		if(lan_find_inf(lanname.c_str(), "#")){
			AnsiString tempS = lan_find_inf(lanname.c_str(), "#");
			lanname = lanname.SubString(0, (lanname.Length()-tempS.Length()-2));
		}
		DeleteFile("templog.txt");
		return lanname;
	}
}

AnsiString Port_test::GetLanPVID(AnsiString Lanname){ // �Ѻ��d�W�٨��oVPID
	AnsiString tempC = "TEST_TOOL\\devcon6 hwids usb\* \> templog.txt";
	if (CallCommand_C(tempC.c_str())) {
		AnsiString lanname = Findfilemsg("templog.txt", Lanname, 3);
		lanname = lanname.SubString(13, lanname.Length());
		DeleteFile("templog.txt");
		return lanname;
	}
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

bool Port_test::FindDevice(AnsiString PVID) { // �d��]��
	if (CallCommand_C("TEST_TOOL\\devcon6 find usb\* \> tempmsg.log")) {
		AnsiString Tempmsg = Findfilemsg("tempmsg.log", PVID, 0);
		DeleteFile("tempmsg.log");
		if (Tempmsg == NULL)
			return false;
		return true;
	}
}

WideString Port_test::Str2Unicode(AnsiString msg){
	MultiByteToWideChar(950, 0, msg.c_str(), -1, wcs, 256);
	WideString ws=wcs;
	return ws;
}

bool Port_test::CheckDir(){
	//���o���d��T
	int lan_index = 0;
	ULONG ulLen1=4096;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	IP_ADDR_STRING *pipas=NULL;
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//���d��T
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			if(PORT->lan_find_inf(lanname.c_str(), "AX88179")){
				frmMain->lab_devicename->Caption = "AX88179";
			}else if(PORT->lan_find_inf(lanname.c_str(), "AX88772B")){
				frmMain->lab_devicename->Caption = "AX88772B";
			}else if(PORT->lan_find_inf(lanname.c_str(), "AX88772C")){
				frmMain->lab_devicename->Caption = "AX88772C";
			}
			else if(PORT->lan_find_inf(lanname.c_str(), "Realtek USB GBE Family Controller")){
				frmMain->lab_devicename->Caption = lanname;
				delete pbBuf1;
				return true;
			}
			else if(PORT->lan_find_inf(lanname.c_str(), "Realtek USB GbE Family Controller")){
				frmMain->lab_devicename->Caption = lanname;
				delete pbBuf1;
				return true;
			}
			if(PORT->lan_find_inf(((AnsiString)pipai->Description).c_str(), "ASIX")){
				if(PORT->lan_find_inf(((AnsiString)pipai->Description).c_str(), "Test Driver")){
					delete pbBuf1;
					return true;
				}
			}
			pipas=&(pipai->IpAddressList);
			pipai=pipai->Next;
			lan_index++;
		} while(pipai!=NULL);
	}
	delete pbBuf1;
	return false;
}

AnsiString Port_test::GetASIXname(){
	//���o���d��T
	int lan_index = 0;
	ULONG ulLen1=4096;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	IP_ADDR_STRING *pipas=NULL;
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//���d��T
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			if(PORT->lan_find_inf(((AnsiString)pipai->Description).c_str(), "ASIX")){
				delete pbBuf1;
				return (AnsiString)pipai->Description;
			}
			pipas=&(pipai->IpAddressList);
			pipai=pipai->Next;
			lan_index++;
		} while(pipai!=NULL);
	}
	delete pbBuf1;
	return "NULL";
}
AnsiString Port_test::GetSlayerName(){
	//���o���d��T
	int lan_index = 0;
	ULONG ulLen1=4096;
	BYTE *pbBuf1=new BYTE[ulLen1];
	IP_ADAPTER_INFO *pipai=NULL;
	IP_ADDR_STRING *pipas=NULL;
	AnsiString szTemp;
	if(ERROR_SUCCESS==GetAdaptersInfo((IP_ADAPTER_INFO*)pbBuf1, &ulLen1))
	{
		//���d��T
		pipai=(IP_ADAPTER_INFO*)pbBuf1;
		do
		{
			AnsiString lanname = pipai->Description;
			if(PORT->lan_find_inf(((AnsiString)pipai->Description).c_str(), "Realtek USB GBE Family Controller")){
				frmMain->lab_devicename->Caption = lanname;

				szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
				pipai->Address[1], pipai->Address[2], pipai->Address[3],
				pipai->Address[4], pipai->Address[5]);
				frmMain->current_mac->Caption  =  szTemp;

				delete pbBuf1;
				return (AnsiString)pipai->Description;
			}
			else if(PORT->lan_find_inf(((AnsiString)pipai->Description).c_str(), "Realtek USB GbE Family Controller")){
				frmMain->lab_devicename->Caption = lanname;

				szTemp.sprintf("%02X:%02X:%02X:%02X:%02X:%02X", pipai->Address[0],
				pipai->Address[1], pipai->Address[2], pipai->Address[3],
				pipai->Address[4], pipai->Address[5]);
				frmMain->current_mac->Caption  =  szTemp;

				delete pbBuf1;
				return (AnsiString)pipai->Description;
			}
			pipas=&(pipai->IpAddressList);
			pipai=pipai->Next;
			lan_index++;
		} while(pipai!=NULL);
	}
	delete pbBuf1;
	return "NULL";
}

bool Port_test::Analyze_USBView(AnsiString VID,AnsiString PID)
{
 if(FileExists("USB_INFO.txt")) DeleteFile("USB_INFO.txt");
 CallCommand_C("Taskkill /im DisplayUSB.exe /F"); //����USBVIEW�L�k��������
 CallCommand_C("TEST_TOOL\\DisplayUSB.exe SAVE");
 CallCommand_C("Taskkill /im DisplayUSB.exe /F"); //����USBVIEW�L�k��������
 if(FileExists("USB_INFO.txt"))
 {
  AnsiString filename = "USB_INFO.txt";
  ifstream lanfile(filename.c_str());
  std::string filemsg;
  if (lanfile.is_open()) {
   while (!lanfile.eof()) {
	getline(lanfile, filemsg);
	if (strstr(filemsg.c_str(),("wVendorID :0x"+VID).c_str()))//�T�{�˸mVID
    {
     getline(lanfile, filemsg);
     if (strstr(filemsg.c_str(),("wProductID :0x"+PID).c_str()))//�T�{�˸mPID
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


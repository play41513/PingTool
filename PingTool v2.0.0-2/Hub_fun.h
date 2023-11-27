//---------------------------------------------------------------------------

#ifndef Hub_funH
#define Hub_funH
#include <vcl.h>
#include <fstream>
#include <string>
#include <Registry.hpp>
#include "Iphlpapi.h"
#include <iostream>
#pragma link "Iphlpapi.lib"
using namespace std;
//---------------------------------------------------------------------------
class Port_test {
public:
	void IntiNetwordCard();
	bool ChangeLanSpeed(AnsiString VPID, AnsiString GUID,
	AnsiString LANname);//�������t
	void Delay(DWORD iMilliSeconds); // delay time
	bool CallCommand_C(AnsiString Command_line);// �UCommand���O BY CreateProcess
	AnsiString Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum); // ���ɮק��r���^�ǴX��᪺�r��
	AnsiString GetLanPVID(AnsiString Lanname);// �Ѻ��d�W�٨��oVPID
	char* lan_find_inf(char *pData, char *pname); // �d��r��^�ǫ᭱���r��
	bool FindDevice(AnsiString PVID); // �d��]��
	AnsiString getLanMAC(AnsiString LANname);//���o���dMAC
	bool Analyze_USBView(AnsiString VID,AnsiString PID);
	AnsiString GetLANName();
	void AddNewMAC(AnsiString LANHUID,AnsiString VPID, AnsiString LANname
		, AnsiString value);//�s�W�Ȯ�MAC
		AnsiString VPIDGetLanName(AnsiString PVID);// ��VIDPID���o���d�W��
private:
	AnsiString getDHCP_Server_IP(AnsiString LANname); // �o��DHCP Server IP
	AnsiString getLanIP(AnsiString LANname); // �o����dIP
	AnsiString getLanGUID(AnsiString LANname); // �o����dGUID

	bool ReadPingSpeed(AnsiString File,int LanSpeed);
	bool cmd_restartdevice(AnsiString PVID);//���ҳ]��
	bool cmd_ping(AnsiString DevIP,AnsiString PingIP,int speed);//ping
	AnsiString DosCommand(AnsiString sCmdline);

};
#endif

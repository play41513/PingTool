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
const char GUID_USB_HUB[] = "{F18A0E88-C30C-11D0-8815-00A0C906BED8}";
//---------------------------------------------------------------------------
class Port_test {
public:
	void IntiNetwordCard();
	bool ChangeLanSpeed(AnsiString VPID, AnsiString GUID,
	AnsiString LANname);//�������t
	bool EnumHUB(bool bUSB3);
	void Delay(DWORD iMilliSeconds); // delay time
	char* lan_find_inf(char *pData, char *pname); // �d��r��^�ǫ᭱���r��
	AnsiString getLanMAC(AnsiString LANname);//���o���dMAC
	AnsiString GetLANName();
	void AddNewMAC(AnsiString LANHUID,AnsiString VPID, AnsiString LANname
		, AnsiString value);//�s�W�Ȯ�MAC
private:
	AnsiString getDHCP_Server_IP(AnsiString LANname); // �o��DHCP Server IP
	AnsiString getLanIP(AnsiString LANname); // �o����dIP
	AnsiString getLanGUID(AnsiString LANname); // �o����dGUID

	bool cmd_restartdevice();//���ҳ]��
	AnsiString getUSB_Rev(AnsiString DEV_PATH);
	bool CheckSpeed(AnsiString LANNAME,DWORD dwSpeed);

};
#endif

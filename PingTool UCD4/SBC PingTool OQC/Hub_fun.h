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
	AnsiString LANname);//切換網速
	bool EnumHUB(bool bUSB3);
	void Delay(DWORD iMilliSeconds); // delay time
	char* lan_find_inf(char *pData, char *pname); // 查找字串回傳後面的字串
	AnsiString getLanMAC(AnsiString LANname);//取得網卡MAC
	AnsiString GetLANName();
	void AddNewMAC(AnsiString LANHUID,AnsiString VPID, AnsiString LANname
		, AnsiString value);//新增暫時MAC
private:
	AnsiString getDHCP_Server_IP(AnsiString LANname); // 得到DHCP Server IP
	AnsiString getLanIP(AnsiString LANname); // 得到網卡IP
	AnsiString getLanGUID(AnsiString LANname); // 得到網卡GUID

	bool cmd_restartdevice();//重啟設備
	AnsiString getUSB_Rev(AnsiString DEV_PATH);
	bool CheckSpeed(AnsiString LANNAME,DWORD dwSpeed);

};
#endif

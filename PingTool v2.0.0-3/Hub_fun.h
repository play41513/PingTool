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
	AnsiString LANname);//切換網速
	void Delay(DWORD iMilliSeconds); // delay time
	bool CallCommand_C(AnsiString Command_line);// 下Command指令 BY CreateProcess
	AnsiString Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum); // 找檔案找到字串行回傳幾行後的字串
	AnsiString GetLanPVID(AnsiString Lanname);// 由網卡名稱取得VPID
	char* lan_find_inf(char *pData, char *pname); // 查找字串回傳後面的字串
	bool FindDevice(AnsiString PVID); // 查找設備
	AnsiString getLanMAC(AnsiString LANname);//取得網卡MAC
	bool Analyze_USBView(AnsiString VID,AnsiString PID);
	AnsiString GetLANName();
	void AddNewMAC(AnsiString LANHUID,AnsiString VPID, AnsiString LANname
		, AnsiString value);//新增暫時MAC
		AnsiString VPIDGetLanName(AnsiString PVID);// 由VIDPID取得網卡名稱
private:
	AnsiString getDHCP_Server_IP(AnsiString LANname); // 得到DHCP Server IP
	AnsiString getLanIP(AnsiString LANname); // 得到網卡IP
	AnsiString getLanGUID(AnsiString LANname); // 得到網卡GUID

	bool ReadPingSpeed(AnsiString File,int LanSpeed);
	bool cmd_restartdevice(AnsiString PVID);//重啟設備
	bool cmd_ping(AnsiString DevIP,AnsiString PingIP,int speed);//ping
	AnsiString DosCommand(AnsiString sCmdline);

};
#endif

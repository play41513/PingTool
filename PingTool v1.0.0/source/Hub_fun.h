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
	bool USB_RT(AnsiString filename); // USB讀寫檔案比對
	void Search_System_Exist_Lan_Speed(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname, AnsiString value);//切換網速
	void AddNewMAC(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname, AnsiString value);//新增暫時MAC
	AnsiString READ_System_Exist_Lan_Speed(AnsiString LANHUID,
	AnsiString VPID, AnsiString LANname);  //讀取網速
	bool ChangeLanSpeed(AnsiString VPID, AnsiString GUID,
	AnsiString LANname);//切換網速
	bool ChangeLanSpeed2(AnsiString VPID, AnsiString GUID,
	AnsiString LANname);//切換網速
	bool Fping(AnsiString PingIP);//ping
	void Delay(int iMilliSeconds); // delay time
	bool cmd_restartdevice(AnsiString PVID);//重啟設備
	bool CallCommand_C(AnsiString Command_line);// 下Command指令 BY CreateProcess
	AnsiString Findfilemsg(AnsiString filename, AnsiString findmsg,
	int rownum); // 找檔案找到字串行回傳幾行後的字串
	AnsiString GetLanName(AnsiString PVID);// 由VIDPID取得網卡名稱
	AnsiString GetLanPVID(AnsiString Lanname);// 由網卡名稱取得VPID
	char* lan_find_inf(char *pData, char *pname); // 查找字串回傳後面的字串
	bool FindDevice(AnsiString PVID); // 查找設備
	AnsiString getLanGUID(AnsiString LANname); // 得到網卡GUID
	AnsiString getLanIP(AnsiString LANname); // 得到網卡名稱IP
	AnsiString getLanMAC(AnsiString LANname);//取得網卡MAC
	WideString Str2Unicode(AnsiString msg); //字串轉換UNIcode
	AnsiString GetASIXname(); //取得亞信網卡名稱
	wchar_t wcs[256]; //字串轉換UNIcode
	bool CheckDir();//檢查Dirver是否正確
	bool Analyze_USBView(AnsiString VID,AnsiString PID);
	AnsiString GetSlayerName();
private:
};
#endif

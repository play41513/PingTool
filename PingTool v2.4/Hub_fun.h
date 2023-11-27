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

const AnsiString GUID_NETWORKCARD = "{4d36e972-e325-11ce-bfc1-08002be10318}";
const char GUID_USB_HUB[] = "{F18A0E88-C30C-11D0-8815-00A0C906BED8}";
const char GUID_USB_DEVICE[] = "{A5DCBF10-6530-11D2-901F-00C04FB951ED}";
const char GUID_USBSTOR[] = "{53F5630D-B6BF-11D0-94F2-00A0C91EFB8B}";
const AnsiString SERVER_MAC = "00:24:9B:FF:FF:FF";
const AnsiString SERVER_IP  = "10.10.10.100";
using namespace std;
//---------------------------------------------------------------------------
class Port_test {
public:
	void IntiNetwordCard();
	bool ChangeLanSpeed(AnsiString VPID,AnsiString LANname,bool bUSB3);//
	void Delay(DWORD iMilliSeconds); // delay time
	char* lan_find_inf(char *pData, char *pname); // �d��r��^�ǫ᭱���r��
	AnsiString getLanMAC(AnsiString LANname);//���o���dMAC
	AnsiString GetLANName();
	void AddNewMAC(AnsiString LANNAME,AnsiString value,AnsiString MAC);//�s�W�Ȯ�MAC
	bool SetIPAddress(AnsiString LANname);
	AnsiString getLanVPID(AnsiString LANname);
	AnsiString Port_test::SetServerIPAddress();
	bool DeleteServerIPAddress();
	void EnumComboBoxNetwork(TComboBox* cbNetwork);
	bool cmd_restartdevice(AnsiString strVPID);//���ҳ]��
	bool CheckServerIP();
	bool StringListTextPosString(TStringList*StringList,AnsiString astrNetworkCardName);
private:
	AnsiString getDHCP_Server_IP(AnsiString LANname); // �o��DHCP Server IP
	AnsiString getLanIP(AnsiString LANname); // �o����dIP
	AnsiString getLanGUID(AnsiString LANname); // �o����dGUID
	bool  CheckSpeed(AnsiString LANNAME,bool bUSB3);
	bool  EnumUSB(AnsiString VPID);

};
#endif

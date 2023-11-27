//---------------------------------------------------------------------------

#ifndef MainThreadH
#define MainThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "ConstantString.h"
#include "USBDevConnectionInfo.h"
#include "LoaderThread.h"
#include "FileControl.h"
#include "LogMsg.h"
#include "HID.h"
#include "Iphlpapi.h"

#pragma link "Iphlpapi.lib"

#define STEP_CHECK_DEV_ONLINE	1
#define STEP_AUTO_TEST			2
#define STEP_MANUAL_TESTING     3
#define STEP_VOLTAGE_TEST		4
#define STEP_CHECK_DEV_OFFLINE	5

#define HID_CONTROL						0
#define HID_CHANGE_VOLTAGE_DELAYTIME	1
#define HID_CMD_NO_LOAD					2
#define READ_NO_LOAD_VOLTAGE_VALUE		3
#define HID_CMD_FULL_LOAD				4
#define READ_FULL_LOAD_VOLTAGE_VALUE	5
#define TEST_VOLTAGE_END	6
#define HID_NOT_FIND		7
#define DL_NOT_FIND		    8
#define TEST_PASS		    9
#define TEST_FAIL		    10
//---------------------------------------------------------------------------
class TMainThread : public TThread
{
private:
	INI_DESCRIPOR INI_Desc;
	HID m_hid;
	HWND mHwnd;

	USBDevConnectionInfo *CL_USB;
	cLogMsg *mLogMsg;
	TLoaderThread *mLoaderThread;
	queue<AnsiString> que_cmd;

	void MainTestFun();
	int  DL_CMD(AnsiString CMD,int index);
	AnsiString getLanMAC(AnsiString LANname);
protected:
	void __fastcall Execute();
public:
	__fastcall TMainThread(bool CreateSuspended,INI_DESCRIPOR info,HWND hWnd,cLogMsg* LogMsg);
	INI_DESCRIPOR ReadINIDesc();
	void SetINIDesc(INI_DESCRIPOR info);
	void Voltage_Test(int index);
	void SetManualTestingStep(DWORD Index,bool bSingleVoltageTest);
};
//---------------------------------------------------------------------------
#endif

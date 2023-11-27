#ifndef ConstantStringH
#define ConstantStringH
//---------------------------------------------------------------------------
//標題
#define APP_TITLE "PingTool v3.0 ( ActionStar.)";
//Debug
#define DEBUG_TEST_START_VPID_AUTO      		"[TEST_START_VPID_AUTO]"
#define DEBUG_TEST_START_VOLTAGE_AUTO   		"[TEST_START_VOLTAGE_AUTO]"              T
#define DEBUG_AUTO_TEST_START   				"[AUTO_TEST_START]"
#define DEBUG_AUTO_TEST_START   				"[AUTO_TEST_START]"
#define DEBUG_READY_TO_CHANGE_VOLTALE   		"[READY_TO_CHANGE_VOLTALE]"
#define DEBUG_AFTER_CHANGE_VOLTALE   			"[AFTER_CHANGE_VOLTALE]"
#define DEBUG_NO_LOAD_TEST   					"[NO_LOAD_TEST]"
#define DEBUG_FULL_LOAD_TEST   					"[FULL_LOAD_TEST]"
#define DEBUG_SLEEP   					"Sleep..."
#define DEBUG_VOLTAGE_TEST_PASS   				"VOLTAGE_PASS"
#define DEBUG_VOLTAGE_TEST_FAIL   				"VOLTAGE_FAIL"
#define DEBUG_CAN_NOT_FIND_PDBOARD   	"(!)Can Not Find PD-Board"
#define DEBUG_CAN_NOT_FIND_LOADER   	"(!)Can Not Find Loader"

#define WM_UI_PANEL_RESULT				WM_USER+100
#define WM_UI_PANEL_NOLOAD				WM_USER+101
#define WM_UI_PANEL_FULLLOAD			WM_USER+102

#define UI_PANEL_RESULT_WAIT				1
#define UI_PANEL_RESULT_TESTING				2
#define UI_PANEL_RESULT_PASS				3
#define UI_PANEL_RESULT_FAIL				4

#define UI_PANEL_FONT_GREEN				1
#define UI_PANEL_FONT_RED				2
#define UI_PANEL_FONT_BLUE				3

//LoaderThread
#define STEP_CHECK_COMPORT			1
#define STEP_COMPORT_WRITE			2
#define STEP_COMPORT_READ			3
#define STEP_TEST_END				4
#define STEP_LOAD_TEST_PASS			5
#define STEP_LOAD_TEST_FAIL			6
#define STEP_COMPORT_NO_RESPOND		7

typedef struct _ITEM_PD_DESCRIPOR
{
  bool	 bTest;
  //Setting Value
  AnsiString FullLoadCurrent;
  AnsiString FullInitLoadCurrent;
  AnsiString NoLoadMaxVoltage;
  AnsiString NoLoadMinVoltage;
  AnsiString FullLoadMaxVoltage;
  AnsiString FullLoadMinVoltage;
  AnsiString LossVol;
  //
  TPanel* plNoLoadValue;
  TPanel* plFullLoadValue;
  AnsiString PD_Voltage;
  AnsiString SetVoltageCMD;
  bool bWattIsOver;
  //
  DWORD dwLoadTime;
  DWORD dwBeforeChangeVoltageDelay;
  DWORD dwAfterChangeVoltageDelay;
  DWORD dwBeforeFullLoadDelay;
  DWORD dwAfterFullLoadDelay;
  DWORD dwAfterTestDelay;
}ITEM_PD_DESCRIPOR;

typedef struct _WNet_LOG_DESCRIPOR
{
  bool bWNetLog;
  AnsiString IP;
  AnsiString User;
  AnsiString Password;
  AnsiString ufolder;

  bool  bRecordMAC;
  AnsiString NetworkCardName;
  AnsiString NetworkCardRecordMAC;
}WNet_LOG_DESCRIPOR;

typedef struct _INI_DESCRIPOR
{
  AnsiString ININame;
  bool VPID_AutoTest;
  AnsiString Auto_VID;
  AnsiString Auto_PID;
  DWORD dwDelayTime_Auto_VPID;
  //
  bool Voltage_AutoTest;
  AnsiString Auto_VoltageValue;
  AnsiString LossVol;
  DWORD dwDelayTime_Auto_Voltage;
  //
  DWORD dwDelayTime_ChangeTo5V;
  DWORD dwDelayTime_ChangeTo20V;
  //
  ITEM_PD_DESCRIPOR PITEM_PD_DESCRIPOR[5];

  bool bSetting;
  bool bTesting;
  bool bError;
  bool bHIDError;
  bool bSwitchBack5V;
  bool bExitWhenPASS;

  AnsiString strReadValueNow;
  AnsiString strLoaderName;
  DWORD dwTestStatus;
  DWORD dwExitWhenPASSCount;
  //
  WNet_LOG_DESCRIPOR WNetLOG;

}INI_DESCRIPOR;

#define DEBUG(String)    FrmMain->moDebug->Lines->Add(String)


//---------------------------------------------------------------------------




#define MB_WARNING(handle, msg) MessageBox(handle, msg, MB_CAPTIONWARNING, MB_ICONWARNING)
//---------------------------------------------------------------------------
#endif

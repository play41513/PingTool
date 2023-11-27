//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
/////////////////////////////////////////
#include <windows.h>
#include <basetyps.h>
#include <winioctl.h>
#include <setupapi.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include <pngimage.hpp>
#include <Buttons.hpp>
#include "IdBaseComponent.hpp"
#include "IdComponent.hpp"
#include "IdIcmpClient.hpp"
#include "IdRawBase.hpp"
#include "IdRawClient.hpp"

#define D_MAX_TEST_ITEM		4

#define WAIT_SCAN		0
#define SCAN_OVER		1
#define SCAN_CHANCEL	2

#define GET_DUT				0
#define SCAN_BARCODE		1
#define LAN_TEST            2
#define LAN_TEST_RESULT		3
#define USB3_TEST			4
#define WAIT_DUT_PLUGOUT	5

//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *lab_devicename;
	TPanel *Panel3;
	TPanel *pan_1000;
	TPanel *Panel4;
	TButton *btn_start;
	TGroupBox *current_mac_group;
	TLabel *current_mac;
	TPanel *plResult;
	TPanel *plErrMsg;
	TPanel *plSet;
	TCheckBox *ckbNoMAC;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	TPanel *Panel6;
	TGroupBox *GroupBox2;
	TImage *Image1;
	TCheckBox *ckbBarcode;
	TPanel *Panel10;
	TLabel *Label2;
	TPanel *plTime;
	TTimer *TimerMain;
	TMenuItem *popRegDriver;
	TIdIcmpClient *IdIcmpClient;
	TMemo *moDebug;
	void __fastcall btn_startClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall ckbNoMACClick(TObject *Sender);
	void __fastcall TimerMainTimer(TObject *Sender);
	void __fastcall popRegDriverClick(TObject *Sender);
	void __fastcall IdIcmpClientReply(TComponent *ASender, const TReplyStatus *AReplyStatus);
	void __fastcall plResultDblClick(TObject *Sender);
	void __fastcall BitBtn1Click(TObject *Sender);

private:	// User declarations


public:		// User declarations
	HANDLE mx;
	AnsiString ERROR_MSG;
	int  numScanResult,gPingTime;
	AnsiString gFileName,gTestLog;
	AnsiString g_DevVPID;
	bool AutoTest;
	bool TestWorking;
	bool g_bError;
	AnsiString gLAN_NAME;
	bool Ping(AnsiString IP,bool bLast);
	bool bPlugOut;

	TStringList *TListInitLAN;
	void SaveLogLine(AnsiString FileName,AnsiString FileLine);
	__fastcall TfrmMain(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
extern DWORD WINAPI LANExecute(LPVOID Param);
//---------------------------------------------------------------------------
#endif

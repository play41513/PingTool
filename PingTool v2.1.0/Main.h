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
#define MAC_LANNAME_VERIFY	6

#define BARCODE_FINISH	1
#define BARCODE_CHANEL  2

//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *lab_devicename;
	TPanel *plLANSpeed;
	TPanel *pl1000M;
	TPanel *Panel4;
	TButton *btn_start;
	TGroupBox *current_mac_group;
	TLabel *current_mac;
	TPanel *plResult;
	TPanel *plErrMsg;
	TPanel *plSet;
	TCheckBox *ckbNoMAC;
	TPopupMenu *PopupMenu;
	TMenuItem *N1;
	TPanel *Panel6;
	TGroupBox *GroupBox2;
	TImage *Image1;
	TCheckBox *ckbBarcode;
	TPanel *Panel10;
	TLabel *Label2;
	TPanel *plTime;
	TTimer *TimerMain;
	TMenuItem *popBackWOPage;
	TIdIcmpClient *IdIcmpClient;
	TPanel *plDebug;
	TMemo *moDebug;
	TPanel *plBarcode;
	TLabel *lbBarcode;
	TPanel *Panel12;
	TBitBtn *btnBarcodeChanel;
	TBitBtn *btnBarcodeClear;
	TEdit *edtBarcodeMAC;
	TPanel *Panel7;
	TPanel *Panel8;
	TPanel *Panel9;
	TPanel *edtSetEmployeeID;
	TPanel *edtSetWorkOrderNumber;
	TPanel *pl100M;
	TPanel *pl10M;
	TBitBtn *btnLoadNetworkCardName;
	TPanel *Panel3;
	TImage *ImgDisk;
	TMenuItem *popReLoadName;
	void __fastcall btn_startClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall ckbNoMACClick(TObject *Sender);
	void __fastcall TimerMainTimer(TObject *Sender);
	void __fastcall popBackWOPageClick(TObject *Sender);
	void __fastcall IdIcmpClientReply(TComponent *ASender, const TReplyStatus *AReplyStatus);
	void __fastcall plResultDblClick(TObject *Sender);
	void __fastcall btnLoadNetworkCardNameClick(TObject *Sender);
	void __fastcall btnBarcodeClearClick(TObject *Sender);
	void __fastcall btnBarcodeChanelClick(TObject *Sender);
	void __fastcall edtBarcodeMACChange(TObject *Sender);
	void __fastcall edtBarcodeMACKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall popReLoadNameClick(TObject *Sender);


private:	// User declarations
	bool CheckDiskName();
	void ShowSettingForm();
	void TfrmMain::FindLogFile();
	void TfrmMain::NewFilePath(AnsiString Path);
	void TfrmMain::ChangeUI(AnsiString astrColor);
	void TfrmMain::RegeditDriver();
	void TfrmMain::SetForegroundWindowInternal(HWND hWnd);

	AnsiString LOGFilePath,LOGDiskPath,RDLOGDiskPath,RDLOGFilePath;


public:		// User declarations
	HANDLE mx;
	DWORD dwTimeout;

	AnsiString ERROR_MSG,ZeroMAC;
	int  numScanResult,gPingTime;
	AnsiString gFileName,gTestLog;
	AnsiString g_DevVPID;

	bool TestWorking;
	bool bUSB3;
	AnsiString gLAN_NAME;
	bool Ping(AnsiString IP,bool bLast);
	bool bPlugOut;

	TStringList *TListInitLAN;
	bool SaveLogLine(AnsiString FileName,AnsiString FileLine);
	bool writeLOG(AnsiString Msg,bool bAction);
	void ReadRegValue();
	void SetRegValue();
	__fastcall TfrmMain(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
extern DWORD WINAPI LANExecute(LPVOID Param);
//---------------------------------------------------------------------------
#endif

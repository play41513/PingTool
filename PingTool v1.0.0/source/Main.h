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

#define D_MAX_TEST_ITEM		4

struct st_thread_parameter {
		int id;
		char name[40];
		int result;
	};

typedef struct st_thread_parameter st_ThreadParam;
st_ThreadParam zTestResult[D_MAX_TEST_ITEM];
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TLabel *new_mac;
	TButton *WRITE;
	TMemo *Memo2;
	TButton *READ;
	TCheckBox *LoopCheck;
	TButton *CHECK;
	TEdit *Edit1;
	TEdit *Edit2;
	TEdit *Edit3;
	TEdit *Edit4;
	TEdit *Edit5;
	TEdit *Edit6;
	TEdit *Edit7;
	TEdit *Edit8;
	TEdit *Edit9;
	TButton *MACP;
	TButton *LOAD;
	TButton *SAVE;
	TPanel *Panel_mac;
	TButton *changedevice;
	TButton *SaveLog;
	TCheckBox *SpeedTest;
	TCheckBox *DoReadXML;
	TButton *CHKDEVICE;
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *lab_devicename;
	TPanel *Panel3;
	TPanel *pan_1000;
	TPanel *pan_100;
	TPanel *pan_10;
	TPanel *Panel4;
	TButton *btn_start;
	TButton *btn_set;
	TGroupBox *current_mac_group;
	TLabel *current_mac;
	TGroupBox *GroupBox2;
	TLabel *lab_time;
	TLabel *Label2;
	TPanel *Panel_lan;
	TPanel *plErrMsg;
	TPanel *Panel5;
	TCheckBox *ckbNoMAC;
	TTimer *TestTimeTimer;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	void __fastcall btn_startClick(TObject *Sender);
	void __fastcall btn_setClick(TObject *Sender);
	void __fastcall Lan_test_result(int result);
	void __fastcall WRITEClick(TObject *Sender);
	void __fastcall READClick(TObject *Sender);
	void __fastcall CHECKClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall MACPClick(TObject *Sender);
	void __fastcall LOADClick(TObject *Sender);
	void __fastcall SAVEClick(TObject *Sender);
	void __fastcall FormWndProc(TMessage& Message);
	void __fastcall Memo2KeyPress(TObject *Sender, wchar_t &Key);
	void __fastcall changedeviceClick(TObject *Sender);
	void __fastcall SaveLogClick(TObject *Sender);
	void __fastcall CHKDEVICEClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall TestTimeTimerTimer(TObject *Sender);
	void __fastcall N1Click(TObject *Sender);
	void __fastcall ckbNoMACClick(TObject *Sender); // �d�I�]�ưT��
private:	// User declarations
	TWndMethod OldFormWndProc;

public:		// User declarations
	HANDLE mx;
	bool AutoTest;
	bool HaveDevice;
	bool TestResult;
	bool TestWorking;
	bool ResetMAC;
	bool LANU3GET;
	AnsiString CHIPSET;
	__fastcall TfrmMain(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
extern DWORD WINAPI LANExecute(LPVOID Param);
extern DWORD WINAPI MACExecute(LPVOID Param);
extern DWORD WINAPI ThreadWorkExecute(LPVOID Param);
//---------------------------------------------------------------------------
#endif

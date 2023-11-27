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
	void __fastcall Lan_test_result(int result);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormWndProc(TMessage& Message);
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
	__fastcall TfrmMain(TComponent* Owner);

};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
extern DWORD WINAPI LANExecute(LPVOID Param);
extern DWORD WINAPI ThreadWorkExecute(LPVOID Param);
//---------------------------------------------------------------------------
#endif

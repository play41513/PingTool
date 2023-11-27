//---------------------------------------------------------------------------

#ifndef MSG_DIALOGH
#define MSG_DIALOGH
//---------------------------------------------------------------------------
#include <windows.h>    // 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <SetupAPI.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <cfgmgr32.h>	// 安全移除USB裝置用 *要比 vcl.h 早編譯
#include <vcl.h>
#include "Iphlpapi.h"
#include <winsock.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class Tmessagebox_form : public TForm
{
__published:	// IDE-managed Components
	TButton *OK_btn;
	TButton *FAIL_btn;
	TLabel *MSG_Label;
	TEdit *Edit_Password;
	TMaskEdit *Edit_resetMAC;
	void __fastcall OK_btnClick(TObject *Sender);
	void __fastcall FAIL_btnClick(TObject *Sender);
	void __fastcall Edit_PasswordKeyPress(TObject *Sender, wchar_t &Key);

private:	// User declarations
public:		// User declarations
	int myStyle;
	void changeform(int style, AnsiString Dump_MAC);
	__fastcall Tmessagebox_form(TComponent* Owner);
	int __fastcall run_mesagebox(AnsiString title, AnsiString msgbuf, int style);
};
//---------------------------------------------------------------------------
extern PACKAGE Tmessagebox_form *messagebox_form;
//---------------------------------------------------------------------------
#endif

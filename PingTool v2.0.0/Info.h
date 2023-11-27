//---------------------------------------------------------------------------

#ifndef InfoH
#define InfoH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <fstream>
#include <iostream>
using namespace std;
//---------------------------------------------------------------------------
class TfrmInfo : public TForm
{
__published:	// IDE-managed Components
	TButton *btn_set;
	TGroupBox *GroupBox1;
	TEdit *LAN_Ping;
	TLabel *Label1;
	TGroupBox *GroupBox2;
	TCheckBox *mac_check;
	TCheckBox *lan_check;
	TGroupBox *GroupBox3;
	TMaskEdit *macset_edit;
	TCheckBox *autoRun_check;
	void __fastcall btn_setClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Logging();
private:	// User declarations
public:		// User declarations
	__fastcall TfrmInfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmInfo *frmInfo;
//---------------------------------------------------------------------------
#endif
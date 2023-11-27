//---------------------------------------------------------------------------
#pragma hdrstop
#include "MSG_DIALOG.h"
#include "Hub_fun.h"
#include <initguid.h>	//GUID ���O
#include "usbdi.h"		// �w������USB�˸m��
#include <dbt.h>		//�˸m�ޮt�ƥ��
#pragma link "Iphlpapi.lib"
#pragma comment(lib, "wsock32.lib")
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tmessagebox_form *messagebox_form;
Port_test *PORT = new Port_test();
int msgboxresult=0;
//---------------------------------------------------------------------------
__fastcall Tmessagebox_form::Tmessagebox_form(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tmessagebox_form::OK_btnClick(TObject *Sender)
{
	ModalResult = 1;
	msgboxresult = 1;
}
//---------------------------------------------------------------------------
void __fastcall Tmessagebox_form::FAIL_btnClick(TObject *Sender)
{
	if(Edit_Password->Visible){
		if(Edit_Password->Text == "admin")
			ModalResult = 3;
		else
            ModalResult = 1;
	}else{
		ModalResult = 2;
		msgboxresult = 2;
	}
}
//---------------------------------------------------------------------------
int __fastcall Tmessagebox_form::run_mesagebox(AnsiString title, AnsiString msgbuf, int style){
	messagebox_form->Caption = title.c_str();
	messagebox_form->MSG_Label->Caption = msgbuf.c_str();
	OK_btn->Visible = true;
	FAIL_btn->Visible = true;
	//���s���A
	if(style == 1){
		OK_btn->Caption = L"���\";
		FAIL_btn->Caption = L"����";
	}
	if(style == 2){
		FAIL_btn->Visible = false;
		OK_btn->Caption = L"�T�w";
	}
	if(style == 3){
		OK_btn->Caption = L"���T";
		FAIL_btn->Caption = L"���~";
	}

	//���ݫ��U�ƥ�
	msgboxresult=0;
	messagebox_form->Show();
	while(msgboxresult==0){
		PORT->Delay(100);
	}
	messagebox_form->Close();
	return msgboxresult;
	////////////////////////
}

void Tmessagebox_form::changeform(int style, AnsiString Dump_MAC){
	Edit_Password->Visible = false;
	Edit_resetMAC->Visible = false;
	FAIL_btn->Visible = false;
	OK_btn->Visible = false;
	OK_btn->Width = 90;
	if(style == 1){
		FAIL_btn->Visible = true;
		OK_btn->Visible = true;
		MSG_Label->Caption = L"�ЧP�_�ù����զ��\�P�_";
		OK_btn->Caption = L"���\";
		FAIL_btn->Caption = L"����";
	}
	if(style == 2){
		FAIL_btn->Visible = true;
		OK_btn->Visible = true;
		MSG_Label->Caption = L"�ЧP�_�n�����զ��\�P�_";
		OK_btn->Caption = L"���\";
		FAIL_btn->Caption = L"����";
	}
	if(style == 3){
		MSG_Label->Caption = L"�Ч󴫬�VGA";
		OK_btn->Visible = true;
		OK_btn->Caption = L"�T�w";
		OK_btn->Width = 407;
	}
	if(style == 4){
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
		OK_btn->Visible = true;
		OK_btn->Caption = L"�T�w";
		OK_btn->Width = 407;
	}
	if(style == 5){
		Edit_Password->Visible = true;
		MSG_Label->Caption = L"�п�J�]�w���_";
		FAIL_btn->Visible = true;
		FAIL_btn->Caption = L"��J";
	}
	if(style == 6){
		OK_btn->Visible = true;
		OK_btn->Caption = L"�T�w";
		OK_btn->Width = 407;
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
	}
	if(style == 7){
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
		FAIL_btn->Visible = true;
		Edit_resetMAC->Visible = true;
		FAIL_btn->Caption = L"�T�{";
	}
}



void __fastcall Tmessagebox_form::Edit_PasswordKeyPress(TObject *Sender, wchar_t &Key)
{
	if(Key == VK_RETURN)
		FAIL_btn->Click();
}
//---------------------------------------------------------------------------

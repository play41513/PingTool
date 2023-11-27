//---------------------------------------------------------------------------
#pragma hdrstop
#include "MSG_DIALOG.h"
#include "Hub_fun.h"
#include <initguid.h>	//GUID 類別
#include "usbdi.h"		// 安全移除USB裝置用
#include <dbt.h>		//裝置拔差事件用
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
	//按鈕型態
	if(style == 1){
		OK_btn->Caption = L"成功";
		FAIL_btn->Caption = L"失敗";
	}
	if(style == 2){
		FAIL_btn->Visible = false;
		OK_btn->Caption = L"確定";
	}
	if(style == 3){
		OK_btn->Caption = L"正確";
		FAIL_btn->Caption = L"錯誤";
	}

	//等待按下事件
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
		MSG_Label->Caption = L"請判斷螢幕測試成功與否";
		OK_btn->Caption = L"成功";
		FAIL_btn->Caption = L"失敗";
	}
	if(style == 2){
		FAIL_btn->Visible = true;
		OK_btn->Visible = true;
		MSG_Label->Caption = L"請判斷聲音測試成功與否";
		OK_btn->Caption = L"成功";
		FAIL_btn->Caption = L"失敗";
	}
	if(style == 3){
		MSG_Label->Caption = L"請更換為VGA";
		OK_btn->Visible = true;
		OK_btn->Caption = L"確定";
		OK_btn->Width = 407;
	}
	if(style == 4){
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
		OK_btn->Visible = true;
		OK_btn->Caption = L"確定";
		OK_btn->Width = 407;
	}
	if(style == 5){
		Edit_Password->Visible = true;
		MSG_Label->Caption = L"請輸入設定金鑰";
		FAIL_btn->Visible = true;
		FAIL_btn->Caption = L"輸入";
	}
	if(style == 6){
		OK_btn->Visible = true;
		OK_btn->Caption = L"確定";
		OK_btn->Width = 407;
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
	}
	if(style == 7){
		MSG_Label->Caption = PORT->Str2Unicode(Dump_MAC);
		FAIL_btn->Visible = true;
		Edit_resetMAC->Visible = true;
		FAIL_btn->Caption = L"確認";
	}
}



void __fastcall Tmessagebox_form::Edit_PasswordKeyPress(TObject *Sender, wchar_t &Key)
{
	if(Key == VK_RETURN)
		FAIL_btn->Click();
}
//---------------------------------------------------------------------------


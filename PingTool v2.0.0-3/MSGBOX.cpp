//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MSGBOX.h"
#include "Main.h"
#include "Hub_fun.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMsg *frmMsg;
//---------------------------------------------------------------------------
__fastcall TfrmMsg::TfrmMsg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::BitBtn1Click(TObject *Sender)
{
	edtBarcode->Text = "";
	edtBarcode->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::BitBtn2Click(TObject *Sender)
{
	ModalResult = mrNo;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::edtBarcodeChange(TObject *Sender)
{
	TEdit* edt=(TEdit*)Sender;
	TCHAR HexChar[15];

	edt->Text = edt->Text.UpperCase();// ��r���ন���j�g
	_tcscpy_s(HexChar, 15, edtBarcode->Text.c_str());
	for(int i=0;i <edt->Text.Length();i++)
	{
		if(HexChar[i] < 0x30 || HexChar[i] > 0x39)//�D�Ʀr
		{
			if(HexChar[i] < 0x41 || HexChar[i] > 0x46)//�D�j�g�r��a~f
			{
				edtBarcode->Text =
					edtBarcode->Text.Delete(edtBarcode->Text.Length(),1);
			}
		}
	}
	if(edtBarcode->Text.Length()> 12)
	  edtBarcode->Text = "";
	edt-> SelStart=edt-> Text.Length();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMsg::edtBarcodeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(edtBarcode->Text.Length()==12&&Key==13)
	{
		ModalResult = mrOk;
	}
	else if(edtBarcode->Text.Length()<12&&Key==13)
		edtBarcode->Text = "";
	else if(Key == 8)
		edtBarcode->Text = "";
}
//---------------------------------------------------------------------------
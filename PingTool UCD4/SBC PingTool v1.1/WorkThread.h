//---------------------------------------------------------------------------

#ifndef WorkThreadH
#define WorkThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "IdGlobalProtocols.hpp"

const wchar_t HID_PVID[] = L"hid#vid_0835&pid_fe02";
//---------------------------------------------------------------------------
extern DWORD WINAPI LANExecute(LPVOID Param);
class TWORKTHREAD : public TThread
{
protected:
	void __fastcall Execute();
	void __fastcall NetworkCardTest();
	bool TWORKTHREAD::HID_TurnOn();
	void TWORKTHREAD::Tx(AnsiString Value);
	void TWORKTHREAD::UI_DisplayCmd(unsigned char *pBuf, int size);
	bool TWORKTHREAD::Rx();
	bool __fastcall HID_ReadReport(AnsiString Value);
	bool TWORKTHREAD::Rx_ValueAnalyze(AnsiString Value);
	int  TWORKTHREAD::HexToInt(AnsiString HexStr);
	bool TWORKTHREAD::CheckSBCBoard();
public:
	HID m_hid;
	IO_Request m_ir, m_or;
	DWORD TestTime;
	__fastcall TWORKTHREAD(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
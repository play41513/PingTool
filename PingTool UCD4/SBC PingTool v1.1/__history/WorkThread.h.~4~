//---------------------------------------------------------------------------

#ifndef WorkThreadH
#define WorkThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
extern DWORD WINAPI LANExecute(LPVOID Param);
class TWORKTHREAD : public TThread
{
protected:
	void __fastcall Execute();
	void __fastcall NetworkCardTest();
public:
	DWORD TestTime;
	__fastcall TWORKTHREAD(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif

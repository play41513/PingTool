// Module:
// Notices: Copyright(c) 2016 Jim Jian.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.1.0.6
//---------------------------------------------------------------------------

//#include <vcl.h>
#pragma hdrstop

#include "MSG_DIALOG.h"
#include "Main.h"
#include "DeviceNotify.h"
#include "Hub_fun.h"
#include "logging.h"
#include "Info.h"
#include "UnitUSB.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
Port_test *PORT = new Port_test();
Tmessagebox_form *SETBOX, *RESETMACBOX;
HANDLE lan, threadwork, mac;
DeviceNotification g_DeviceNogification;
clock_t start, end;
AnsiString Buildday = "PingTool ver 1.0.0 B20171120";
AnsiString RUNTIME;		//程式開始執行時間(yymmddhhmmss)
AnsiString LOGFILE, INIFILE; //存檔路徑
FILE * fp;
AnsiString ERRMSG;
AnsiString GETMAC="";   //紀錄 系統讀取 MAC Address
AnsiString SETMAC="";   //紀錄 燒錄ASIX MAC Address
AnsiString HOSTIP="";   //紀錄 本機 IP Address
AnsiString DHCPIP="";   //紀錄 DHCP IP Address
bool PROGPass, ReadGood, isEC, CHKERR, GETDEVICE;
void SaveLogLing(AnsiString FileName,AnsiString FileLine);
AnsiString deviceVPID;
AnsiString lanname;
int windowsVer;
DWORD TestTime = 0;

#define IOCTL_EEPROM_READ_DATA			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EEPROM_WRITE_DATA			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EFUSE_READ_DATA			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EFUSE_WRITE_DATA			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_EEPROM_CHANGE_DELAY       CTL_CODE (FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_DEVICE				CTL_CODE (FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPEED_CHANGE				CTL_CODE (FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CHECK_LINK				CTL_CODE (FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_TO_MAC_MODE			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_TO_PHY_MODE			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x822, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MAC_REGISTER		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x830, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_MAC_REGISTER			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x831, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_PHY_REGISTER		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_PHY_REGISTER			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x841, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_USBDC_REGISTER		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x850, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_READ_USBDC_REGISTER		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x851, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_WAKEUP_FRAME			CTL_CODE (FILE_DEVICE_UNKNOWN, 0x860, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DEVICE_CYCLE_PORT_179		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x861, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DEVICE_CYCLE_PORT_772		CTL_CODE (FILE_DEVICE_UNKNOWN, 0x8A0, METHOD_BUFFERED, FILE_ANY_ACCESS)

//MAC 燒錄區段 開始~結束 字元
unsigned char MA5=0x00;
unsigned char MA4=0x01;
unsigned char MA3=0x8E;
unsigned char MA2=0x04;
unsigned char MA1=0x00;
unsigned char MA0=0x00;
unsigned char ME2=0x04;
unsigned char ME1=0x00;
unsigned char ME0=0x00;

unsigned char MR5,MR4,MR3,MR2,MR1,MR0;
//AnsiString deviceVPID = "VID_0B95&PID_1790";
//AnsiString CHIPSET="AX88179";
//AnsiString CHIPSET="AX88772C";
BYTE WriteBuf[512+8];
BYTE ReadBuf[512+8];

BYTE AX88772BROM[512]=
{
//                                                 8     9    10    11    12    13
//                                                00    01    8E    AE    04    4F
0x15, 0x5A, 0xEC, 0x75, 0x20, 0x12, 0x29, 0x27, 0x00, 0x01, 0x8E, 0xAE, 0x40, 0x4F, 0x09, 0x04,
0x60, 0x22, 0x71, 0x12, 0x19, 0x0E, 0x3D, 0x04, 0x3D, 0x04, 0x3D, 0x04, 0x3D, 0x04, 0x80, 0x05,
0x00, 0x06, 0x10, 0xE0, 0x42, 0x24, 0x40, 0x12, 0x49, 0x27, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
//48                      52          54          56          58          60          62
//                         A           E           4           0           4           F
0xC0, 0x09, 0x0E, 0x03, 0x41, 0x00, 0x45, 0x00, 0x34, 0x00, 0x30, 0x00, 0x34, 0x00, 0x46, 0x00,
0x12, 0x01, 0x00, 0x02, 0xFF, 0xFF, 0x00, 0x40, 0x95, 0x0B, 0x2B, 0x77, 0x01, 0x00, 0x01, 0x02,
0x03, 0x01, 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x04, 0xA0, 0x64, 0x09, 0x04, 0x00, 0x00, 0x03,
0xFF, 0xFF, 0x00, 0x07, 0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0B, 0x07, 0x05, 0x82, 0x02, 0x00,
0x02, 0x00, 0x07, 0x05, 0x03, 0x02, 0x00, 0x02, 0x00, 0xFF, 0x04, 0x03, 0x30, 0x00, 0xFF, 0xFF,
0x12, 0x01, 0x00, 0x02, 0xFF, 0xFF, 0x00, 0x08, 0x95, 0x0B, 0x2B, 0x77, 0x01, 0x00, 0x01, 0x02,
0x03, 0x01, 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x04, 0xA0, 0x64, 0x09, 0x04, 0x00, 0x00, 0x03,
0xFF, 0xFF, 0x00, 0x07, 0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0xA0, 0x07, 0x05, 0x82, 0x02, 0x40,
0x00, 0x00, 0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00, 0xDD, 0xFF, 0xFF, 0xAA, 0xAA, 0xBB, 0xBB,
0x22, 0x03, 0x41, 0x00, 0x53, 0x00, 0x49, 0x00, 0x58, 0x00, 0x20, 0x00, 0x45, 0x00, 0x6C, 0x00,
0x65, 0x00, 0x63, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x70, 0x00,
0x2E, 0x00, 0x12, 0x03, 0x41, 0x00, 0x58, 0x00, 0x38, 0x00, 0x38, 0x00, 0x37, 0x00, 0x37, 0x00,
0x32, 0x00, 0x42, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


BYTE AX88772CROM[512]=
{
//                                                 8     9    10    11    12    13
//                                                00    01    8E    AE    04    4F
0x15, 0x5A, 0xEC, 0x75, 0x20, 0x12, 0x29, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x09, 0x04,
0x60, 0x22, 0x71, 0x12, 0x19, 0x0E, 0x3D, 0x04, 0x3D, 0x04, 0x3D, 0x04, 0x3D, 0x04, 0x80, 0x05,
0x00, 0x06, 0x10, 0xE0, 0x42, 0x24, 0x40, 0x12, 0x49, 0x27, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
0xC0, 0x09, 0x0E, 0x03, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x32, 0x00,
0x12, 0x01, 0x00, 0x02, 0xFF, 0xFF, 0x00, 0x40, 0x95, 0x0B, 0x2B, 0x77, 0x02, 0x00, 0x01, 0x02,
0x03, 0x01, 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x04, 0xA0, 0x64, 0x09, 0x04, 0x00, 0x00, 0x03,
0xFF, 0xFF, 0x00, 0x07, 0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0B, 0x07, 0x05, 0x82, 0x02, 0x00,
0x02, 0x00, 0x07, 0x05, 0x03, 0x02, 0x00, 0x02, 0x00, 0xFF, 0x04, 0x03, 0x30, 0x00, 0xFF, 0xFF,
0x12, 0x01, 0x00, 0x02, 0xFF, 0xFF, 0x00, 0x08, 0x95, 0x0B, 0x2B, 0x77, 0x02, 0x00, 0x01, 0x02,
0x03, 0x01, 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x04, 0xA0, 0x64, 0x09, 0x04, 0x00, 0x00, 0x03,
0xFF, 0xFF, 0x00, 0x07, 0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0xA0, 0x07, 0x05, 0x82, 0x02, 0x40,
0x00, 0x00, 0x07, 0x05, 0x03, 0x02, 0x40, 0x00, 0x00, 0xDD, 0xFF, 0xFF, 0xAA, 0xAA, 0xBB, 0xBB,
0x22, 0x03, 0x41, 0x00, 0x53, 0x00, 0x49, 0x00, 0x58, 0x00, 0x20, 0x00, 0x45, 0x00, 0x6C, 0x00,
0x65, 0x00, 0x63, 0x00, 0x2E, 0x00, 0x20, 0x00, 0x43, 0x00, 0x6F, 0x00, 0x72, 0x00, 0x70, 0x00,
0x2E, 0x00, 0x12, 0x03, 0x41, 0x00, 0x58, 0x00, 0x38, 0x00, 0x38, 0x00, 0x37, 0x00, 0x37, 0x00,
0x32, 0x00, 0x42, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


BYTE AX88179ROM[512+8]=				//AX88179  [4TAG+2Len+2Offset+512Flash]
{//0x58495341
//                                                 8     9    10    11    12    13
//                                                00    01    8E    AB    B0    2B
0x41, 0x53, 0x49, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x01, 0x8E, 0xAB, 0xB0, 0x2B, 0x90, 0x17,
0x95, 0x0B, 0xB7, 0x73, 0x00, 0xE0, 0x3E, 0x01, 0x80, 0x0B, 0x09, 0x04, 0x0E, 0x07, 0x1A, 0x10,
0x26, 0x0E, 0x2D, 0x16, 0x41, 0x58, 0x38, 0x38, 0x31, 0x37, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x53, 0x49, 0x58,
0x20, 0x45, 0x6C, 0x65, 0x63, 0x2E, 0x20, 0x43, 0x6F, 0x72, 0x70, 0x2E, 0x00, 0x00, 0x00, 0x00,
//80                      84    85    86    87    88    89    90    91    92    93    94    95
//                         X     X     0     0     0     1     8     E     A     B     B     0
0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x38, 0x45, 0x41, 0x42, 0x42, 0x30,
//96    97
// 2     B
0x32, 0x42, 0x05, 0x0F, 0x16, 0x00, 0x02, 0x07, 0x10, 0x02, 0x02, 0x00, 0x00, 0x00, 0x0A, 0x10,
0x03, 0x00, 0x0E, 0x00, 0x01, 0x01, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF3, 0xFF, 0x40, 0x4A, 0x40, 0x00, 0x40, 0x30, 0x0D, 0x49, 0x90, 0x41, 0xC1, 0xBD, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
GUID guid;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
	changedeviceClick(NULL);
	OldFormWndProc = this->WindowProc;
	this->WindowProc = FormWndProc;
	// 註冊USB裝置
	guid =StringToGUID("{A5DCBF10-6530-11D2-901F-00C04FB951ED}");
	g_DeviceNogification.RegisterWindowsDeviceInterfaceNotification(
		Handle,guid);//	GUID_CLASS_USB_DEVICE_USB);
}

void __fastcall TfrmMain::FormWndProc(TMessage& Message) // 攔截設備訊息
{
	PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
	OldFormWndProc(Message);
	if (Message.Msg == WM_DEVICECHANGE) { // 有設備拔插
		if (!TestWorking) {
			pDevInf = PDEV_BROADCAST_DEVICEINTERFACE(Message.LParam);
			if (Message.WParam == DBT_DEVICEARRIVAL) // 新插入了一項裝置!
			{
				if(pDevInf->dbcc_classguid == guid){
					AnsiString dbccname = pDevInf->dbcc_name;
					bool deviceout;
					lanname = "NULL";
					int timeout = 0;
					while(lanname == "NULL"){
						if(timeout == 50)
						break;
						lanname = PORT->GetSlayerName();
						timeout++;
						PORT->Delay(100);
					}
					if(lanname == "NULL"){
						lab_devicename->Caption = "";
						current_mac->Caption = "未插入設備";
					}
					else{
						changedeviceClick(NULL);
						deviceVPID = PORT->GetLanPVID(lanname);
						if(dbccname.Pos(deviceVPID)){
							deviceout = PORT->FindDevice(deviceVPID);
						}
					}
				}
			}
			if (Message.WParam == DBT_DEVICEREMOVECOMPLETE) // 新拔出了一項裝置!
			{
				if(pDevInf->dbcc_classguid == guid){
					AnsiString dbccname = pDevInf->dbcc_name;
					bool deviceout;
					deviceout = PORT->FindDevice(deviceVPID);
					if(!deviceout){
						Panel_mac->Color = clWhite;
						Panel_lan->Color = clWhite;
						Panel_lan->Caption = "Wait";
						pan_10->Color = clWhite;
						pan_100->Color = clWhite;
						pan_1000->Color = clWhite;
						frmMain->Height = 255;
						plErrMsg->Caption = "";
						lab_devicename->Caption = "";
						frmMain->lab_time->Caption = "0";
						current_mac->Caption = "未插入設備";
						HaveDevice = false;
					}
				}
			}
		}
	}
}


DWORD WINAPI ThreadWorkExecute(LPVOID Param){
	int Step = 0, costTime = 0;
	while (frmMain->TestWorking){
		switch (Step) {
		case 0 :
			lanname = "NULL";
			lanname = PORT->GetSlayerName();
			if(lanname != "NULL"){
				frmMain->changedeviceClick(NULL);
				deviceVPID = PORT->GetLanPVID(lanname);

				frmMain->TestResult = true;
				frmMain->HaveDevice = true;
				frmMain->current_mac->Caption = PORT->getLanMAC(lanname); //顯示當前設備號
				Step = 1;

			}else{
				frmMain->lab_devicename->Caption = "";
				frmMain->current_mac->Caption = "未插入設備";
			}
		break;
		case 1:
			frmMain->Panel_lan->Caption = "Testing";
			if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt",
						"TEST ITEM", 2)=="LAN_true" && frmMain->TestResult){  //補燒不測網路取消   20160505   jim
				if(frmMain->CHIPSET == "AX88179"){
					//偵測AX88179 是否為USB3.0線路
					if(PORT->Analyze_USBView("0B95", "1790")){
						//網路測試
						strcpy(zTestResult[1].name, "LAN");
						lan = ::CreateThread(0, 1024, LANExecute, (LPVOID) & zTestResult[1], 0,
							NULL);
							WaitForSingleObject(lan, INFINITE);
							CloseHandle(lan);
					}else{
						frmMain->pan_1000->Color = clRed;
						frmMain->TestResult = false;
						frmMain->Panel_lan->Color = clRed;
						frmMain->Panel_lan->Caption = "FAIL";
						frmMain->Height = 289;
						frmMain->plErrMsg->Caption = "USB3.0 FAIL";
					}
				}else if(frmMain->CHIPSET == "Realtek"){
					if(PORT->Analyze_USBView("0BDA", "8153")){
						//網路測試
						strcpy(zTestResult[1].name, "LAN");
						lan = ::CreateThread(0, 1024, LANExecute, (LPVOID) & zTestResult[1], 0,
							NULL);
							WaitForSingleObject(lan, INFINITE);
							CloseHandle(lan);
					}else{
						frmMain->pan_1000->Color = clRed;
						frmMain->TestResult = false;
						frmMain->Panel_lan->Color = clRed;
						frmMain->Panel_lan->Caption = "FAIL";
						frmMain->Height = 289;
						frmMain->plErrMsg->Caption = "USB3.0 FAIL";
					}
				}else{
					//網路測試
					strcpy(zTestResult[1].name, "LAN");
					lan = ::CreateThread(0, 1024, LANExecute, (LPVOID) & zTestResult[1], 0,
						NULL);
						WaitForSingleObject(lan, INFINITE);
						CloseHandle(lan);
				}
			}
			//end = clock();
			//costTime = (int)(end - start)/1000;
			//frmMain->lab_time->Caption = PORT->Str2Unicode((AnsiString)costTime);
			TestTime = 0;

			frmMain->Refresh();
			Step = 2;
		break;
		case 2:
			if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt","TEST ITEM", 3) != "AUTORUN_true"){
				frmMain->btn_set->Enabled = true;
				frmMain->TestWorking = false;
				//frmMain->btn_start->Enabled = false;
			}
			if(!PORT->FindDevice(deviceVPID)){
				frmMain->Panel_mac->Color = clWhite;
				frmMain->Panel_lan->Color = clWhite;
				frmMain->Panel_lan->Caption = "Wait";
				frmMain->pan_10->Color = clWhite;
				frmMain->pan_100->Color = clWhite;
				frmMain->pan_1000->Color = clWhite;
				frmMain->Height = 255;
				frmMain->plErrMsg->Caption = "";
				frmMain->lab_devicename->Caption = "";
				frmMain->current_mac->Caption = "未插入設備";
				frmMain->HaveDevice = false;
				frmMain->lab_time->Caption = 0;
				if(frmMain->ckbNoMAC->Checked) frmMain->N1Click(NULL);
				Step = 0;
			}
		break;
		}
		frmMain->Refresh();
		Sleep(1000);
	}
	CloseHandle(threadwork);
	return 1;
}

DWORD WINAPI MACExecute(LPVOID Param){
	st_ThreadParam *pParam;
	pParam = (st_ThreadParam*)Param;
	frmMain->LOADClick(NULL);
	frmMain->CHECKClick(NULL);
	frmMain->WRITEClick(NULL);
	//檢查MAC號
	frmMain->READClick(NULL);
	if(frmMain->current_mac->Caption == frmMain->new_mac->Caption){
		pParam->result = 1;
		return 1;
	}else{
		pParam->result = -1;
		return -1;
	}
}

DWORD WINAPI LANExecute(LPVOID Param){
	st_ThreadParam *pParam;
	pParam = (st_ThreadParam*)Param;
	// 改用fping測速
	AnsiString PVID = PORT->GetLanPVID(lanname);
	//AnsiString lanname = PORT->GetLanName(PVID);
	if(frmMain->CHIPSET == "AX88179"||frmMain->CHIPSET == "Realtek"){
		if (PORT->ChangeLanSpeed(PVID, "{4d36e972-e325-11ce-bfc1-08002be10318}", lanname)){
			frmMain->Lan_test_result(1);
			pParam->result = 1;
			return 1;
		}
		else{
			frmMain->Lan_test_result(-1);
			pParam->result = -1;
			return -1;
		}
	}else{
		if (PORT->ChangeLanSpeed2(PVID, "{4d36e972-e325-11ce-bfc1-08002be10318}", lanname)){
			frmMain->Lan_test_result(1);
			pParam->result = 1;
			return 1;
		}
		else{
			frmMain->Lan_test_result(-1);
			pParam->result = -1;
			return -1;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Lan_test_result(int result){
	switch(result) {
	case 1:
		Panel_lan->Color = clGreen;
		frmMain->Panel_lan->Caption = "PASS";
		break;
	case -1:
		frmMain->TestResult = false;
		Panel_lan->Color = clRed;
		frmMain->Panel_lan->Caption = "FAIL";
		break;
	default:
		frmMain->TestResult = false;
		Panel_lan->Color = clRed;
		frmMain->Panel_lan->Caption = "FAIL";
		break;
	}
}
void __fastcall TfrmMain::btn_startClick(TObject *Sender)
{
	Memo2->SetFocus();
	lab_time->Caption = "0";
	if(!TestWorking && !AutoTest){
		if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt","TEST ITEM", 3) == "AUTORUN_true")
			AutoTest = true;
		else
			AutoTest = false;
		TestResult = true; //預設true  網路切速緊急停止會需要
		TestWorking = true;
		btn_set->Enabled = false;
		//初始元件
		Panel_mac->Color = clWhite;
		Panel_lan->Color = clWhite;
		frmMain->Panel_lan->Caption = "Wait";
		pan_10->Color = clWhite;
		pan_100->Color = clWhite;
		pan_1000->Color = clWhite;
		frmMain->Height = 255;
		if(AutoTest){
			btn_start->Caption = L"停止";
		}else{
			btn_start->Enabled = false;
		}
		// 呼叫LAN執行檔
		strcpy(zTestResult[0].name, "ThreadWork");
		threadwork = ::CreateThread(0, 1024, ThreadWorkExecute, (LPVOID) & zTestResult[0], 0,
			NULL);
	}else{
		AutoTest = false;
		TestWorking = false;
		TestResult = false;
		btn_set->Enabled = true;
		btn_start->Caption = L"開始";
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btn_setClick(TObject *Sender)
{
	Memo2->SetFocus();
	int MSBOXnum;
	SETBOX = new Tmessagebox_form(Application);
	SETBOX->Caption = L"請輸入設定金鑰";
	SETBOX->changeform(5, "");
	MSBOXnum = SETBOX->ShowModal();
	if (MSBOXnum == 3){
		frmInfo = new TfrmInfo(Application);
		if (frmInfo->ShowModal() == mrYes) // 輸入工號後主視窗顯現
		{
        	SETBOX->Caption = L"";
			SETBOX->changeform(4, "設定完成");
			MSBOXnum = SETBOX->ShowModal();
			LOADClick(NULL);
		}
	}else{
        SETBOX->Caption = L"";
		SETBOX->changeform(4, "金鑰錯誤");
		MSBOXnum = SETBOX->ShowModal();
	}
	delete SETBOX;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::WRITEClick(TObject *Sender)
{
	HANDLE	hControlDevice;
	//unsigned char MR5,MR4,MR3,MR2,MR1,MR0;
	unsigned int bytes;
	unsigned int romSize = 512;
	PROGPass=false;
	char szBuf[12];
	unsigned int DelayCount=1;
	AnsiString SS;
	bool Verify;
	int j;

	if (CHIPSET=="AX88772C") {
	hControlDevice = CreateFile (
							TEXT("\\\\.\\AX88772"),
							GENERIC_READ | GENERIC_WRITE,//FILE_READ_ATTRIBUTES | SYNCHRONIZE,
							FILE_SHARE_READ,
							NULL, // no SECURITY_ATTRIBUTES structure
							OPEN_EXISTING, // No special create flags
							FILE_ATTRIBUTE_NORMAL, // No special attributes
							NULL);
		}
	else if (CHIPSET=="AX88179") {
	hControlDevice = CreateFile (
							TEXT("\\\\.\\AX88179"),
							GENERIC_READ | GENERIC_WRITE,//FILE_READ_ATTRIBUTES | SYNCHRONIZE,
							FILE_SHARE_READ,
							NULL, // no SECURITY_ATTRIBUTES structure
							OPEN_EXISTING, // No special create flags
							FILE_ATTRIBUTE_NORMAL, // No special attributes
							NULL);
		}

	if (INVALID_HANDLE_VALUE == hControlDevice) {
		Memo2->Lines->Add("Create ASIX Fail");
		hControlDevice=NULL;
		}
	else
		{
	//	Memo2->Lines->Add("Create ASIX OK");
		if (CHIPSET=="AX88772C") {
			memcpy(WriteBuf,AX88772CROM,512);
			WriteBuf[8] =MA5;
			WriteBuf[9] =MA4;
			WriteBuf[10]=MA3;
			WriteBuf[11]=MA2;
			WriteBuf[12]=MA1;
			WriteBuf[13]=MA0;
			sprintf(szBuf,"%02X%02X%02X",MA2,MA1,MA0);
			WriteBuf[52]=szBuf[0];
			WriteBuf[54]=szBuf[1];
			WriteBuf[56]=szBuf[2];
			WriteBuf[58]=szBuf[3];
			WriteBuf[60]=szBuf[4];
			WriteBuf[62]=szBuf[5];
			}
		else if (CHIPSET=="AX88179") {
			romSize = 520;
			memcpy(WriteBuf,AX88179ROM,520);
			WriteBuf[8] =MA5;
			WriteBuf[9] =MA4;
			WriteBuf[10]=MA3;
			WriteBuf[11]=MA2;
			WriteBuf[12]=MA1;
			WriteBuf[13]=MA0;
			sprintf(szBuf,"%02X%02X%02X%02X%02X%02X",MA5,MA4,MA3,MA2,MA1,MA0);
			WriteBuf[86]=szBuf[0];
			WriteBuf[87]=szBuf[1];
			WriteBuf[88]=szBuf[2];
			WriteBuf[89]=szBuf[3];
			WriteBuf[90]=szBuf[4];
			WriteBuf[91]=szBuf[5];
			WriteBuf[92]=szBuf[6];
			WriteBuf[93]=szBuf[7];
			WriteBuf[94]=szBuf[8];
			WriteBuf[95]=szBuf[9];
			WriteBuf[96]=szBuf[10];
			WriteBuf[97]=szBuf[11];
			}
		}

	/*
	if (hControlDevice) {
			for(j=0;j<512;j+=16)
				{
				SS.printf("%3d %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X ",
				j,
				WriteBuf[0+j],WriteBuf[1+j],WriteBuf[2+j],WriteBuf[3+j],WriteBuf[4+j],WriteBuf[5+j],WriteBuf[6+j],WriteBuf[7+j],
				WriteBuf[8+j],WriteBuf[9+j],WriteBuf[10+j],WriteBuf[11+j],WriteBuf[12+j],WriteBuf[13+j],WriteBuf[14+j],WriteBuf[15+j]
				);
				Memo2->Lines->Add(SS);
				}
			SS.printf("%2d %02X %02X %02X %02X %02X %02X %02X %02X ",
			j,
			WriteBuf[0+j],WriteBuf[1+j],WriteBuf[2+j],WriteBuf[3+j],WriteBuf[4+j],WriteBuf[5+j],WriteBuf[6+j],WriteBuf[7+j]
			);
			Memo2->Lines->Add(SS);
		}
	*/

	if (hControlDevice) 	//IOCTL_EEPROM_CHANGE_DELAY
		{
		if(!DeviceIoControl (hControlDevice,
			  IOCTL_EEPROM_CHANGE_DELAY,
			  &DelayCount, sizeof(unsigned int),
			  NULL, 0,
			  (unsigned long *)&bytes, NULL))
			{
			hControlDevice=NULL;
			Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY failed!");
			}
		else
			Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY PASS!");
		}

	if(hControlDevice)	// IOCTL_EEPROM_WRITE_DATA
		{
		unsigned int WRITE_SIZE=512;
		if (CHIPSET=="AX88179") WRITE_SIZE=520;
		if (!DeviceIoControl(hControlDevice,
							IOCTL_EEPROM_WRITE_DATA,
							&WriteBuf[0], WRITE_SIZE,
							NULL, 0, (unsigned long *)&bytes, NULL))
			{
			Memo2->Lines->Add("IOCTL_EEPROM_WRITE_DATA failed!");
			CloseHandle(hControlDevice);
			hControlDevice=NULL;
			}
		else
			{
			Memo2->Lines->Add("IOCTL_EEPROM_WRITE_DATA PASS!");
			}
		//紀錄本次燒錄的MAC ADDRESS
		//SETMAC.printf("%02X:%02X:%02X:%02X:%02X:%02X",MA5,MA4,MA3,MA2,MA1,MA0);
		}

	if(hControlDevice)	// Verify
		{
		Sleep(1000);
		READClick(NULL);
		Verify=true;
		for(j=0;j<512;j++)
			{
			if (WriteBuf[j]!=ReadBuf[j])
				{
				Verify=false;
				}
			}
		if (!Verify)
			{
			Memo2->Lines->Add("EEPROM Verify failed!");
			//ERRMSG="EEPROM Verify failed!";
			//GETERRClick(NULL);
			CloseHandle(hControlDevice);
			hControlDevice=NULL;
			}
		}

	if (hControlDevice) 	//IOCTL_EEPROM_CHANGE_DELAY
		{
		if(!DeviceIoControl (hControlDevice,
			  IOCTL_EEPROM_CHANGE_DELAY,
			  &DelayCount, sizeof(unsigned int),
			  NULL, 0,
			  (unsigned long *)&bytes, NULL))
			{
			hControlDevice=NULL;
			Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY failed!");
			}
		else
			Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY PASS!");
		}
	//20170817 發現這段會重啟裝置，會導致燒入失敗，測網路失敗，也可能藍頻
	/*
	if(PORT->Findfilemsg("TEST_TOOL\\infomation.txt",
					"TEST ITEM", 2)=="LAN_true"){
		if(hControlDevice)
		{
			if (CHIPSET=="AX88772C") {
				if(!DeviceIoControl (hControlDevice,
					  IOCTL_DEVICE_CYCLE_PORT_772,
					  NULL, 0,
					  NULL, 0,
					  (unsigned long *)&bytes, NULL))
					{
					Memo2->Lines->Add("IOCTL_DEVICE_CYCLE_PORT failed!");
					CloseHandle(hControlDevice);
					hControlDevice=NULL;
					}
				else
					{
					Memo2->Lines->Add("IOCTL_DEVICE_CYCLE_PORT OK!");
					}
				}
			else if (CHIPSET=="AX88179") {
				if(!DeviceIoControl (hControlDevice,
					  IOCTL_DEVICE_CYCLE_PORT_179,
					  NULL, 0,
					  NULL, 0,
					  (unsigned long *)&bytes, NULL))
					{
					Memo2->Lines->Add("IOCTL_DEVICE_CYCLE_PORT failed!");
					CloseHandle(hControlDevice);
					hControlDevice=NULL;
					}
				else
					{
					Memo2->Lines->Add("IOCTL_DEVICE_CYCLE_PORT OK!");
					}
			}
		}
	}  */
	if(hControlDevice)
		{
		CloseHandle(hControlDevice);
		hControlDevice=NULL;
		PROGPass=true;
		SS.printf("%02X:%02X:%02X:%02X:%02X:%02X",
		MA5,MA4,MA3,MA2,MA1,MA0);
		}
}
//---------------------------------------------------------------------------
//for 88179
typedef struct _EEPROM_DATA
{
	ULONG		Tag;
	USHORT		Offset;
	USHORT		Length;
	USHORT		Data[2];
} EEPROM_DATA, *PEEPROM_DATA;
//---------------------------------------------------------------------------
void __fastcall TfrmMain::READClick(TObject *Sender)
{
	HANDLE	hReadDevice;
unsigned int bytes;
unsigned int romSize = 512;
//BYTE pbyBuf[512+8];
unsigned int DelayCount=1;
EEPROM_DATA	temp;
AnsiString SS;
unsigned int offset;
BYTE	inBuf[3];
BYTE	outBuf;
//CString	temp;
int i,j;

ReadGood=false;
isEC=false;

//Create Handle for Reader
if (CHIPSET=="AX88772C") {
	hReadDevice = CreateFile (
						TEXT("\\\\.\\AX88772"),
						GENERIC_READ | GENERIC_WRITE,//FILE_READ_ATTRIBUTES | SYNCHRONIZE,
						FILE_SHARE_READ,
						NULL, // no SECURITY_ATTRIBUTES structure
						OPEN_EXISTING, // No special create flags
						FILE_ATTRIBUTE_NORMAL, // No special attributes
						NULL);
	}
else if (CHIPSET=="AX88179") {
	hReadDevice = CreateFile (
						TEXT("\\\\.\\AX88179"),
						GENERIC_READ | GENERIC_WRITE,//FILE_READ_ATTRIBUTES | SYNCHRONIZE,
						FILE_SHARE_READ,
						NULL, // no SECURITY_ATTRIBUTES structure
						OPEN_EXISTING, // No special create flags
						FILE_ATTRIBUTE_NORMAL, // No special attributes
						NULL);
	}

if (hReadDevice) 	//IOCTL_EEPROM_CHANGE_DELAY
	{
	if(!DeviceIoControl (hReadDevice,
		  IOCTL_EEPROM_CHANGE_DELAY,
		  &DelayCount, sizeof(unsigned int),
		  NULL, 0,
		  (unsigned long *)&bytes, NULL))
		{
		hReadDevice=NULL;
		Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY failed!");
		}
	else
		Memo2->Lines->Add("IOCTL_EEPROM_CHANGE_DELAY PASS!");
	}

//Read
if (INVALID_HANDLE_VALUE == hReadDevice) {
	Memo2->Lines->Add("Create ASIX Fail");
	hReadDevice=NULL;
	}
else
	{
	if (CHIPSET=="AX88772C") {
		if(!DeviceIoControl (hReadDevice,
			  IOCTL_EEPROM_READ_DATA,
			  NULL, 0,
			  &ReadBuf[0], 512,
			  (unsigned long *)&bytes, NULL))
			{
			Memo2->Lines->Add("IOCTL_EEPROM_READ_DATA failed!");
			CloseHandle(hReadDevice);
			hReadDevice=NULL;
			current_mac->Caption = PORT->getLanMAC(lanname); //顯示當前設備號
			}
		}
	else if (CHIPSET=="AX88179") {
			for (i = 0; i < 512/2; i++)
				{
				inBuf[0] = 0x07;
				inBuf[1] = 1;
				inBuf[2] = i;
				if(!DeviceIoControl (hReadDevice,
					  IOCTL_WRITE_MAC_REGISTER,
					  &inBuf, sizeof(inBuf),
					  NULL, 0,
					  (unsigned long *)&bytes, NULL))
				{
					Memo2->Lines->Add("IOCTL_WRITE_MAC_REGISTER 0x07 failed!");
					CloseHandle(hReadDevice);
					hReadDevice=NULL;
					break;
				}

				inBuf[0] = 0x0a;
				inBuf[1] = 1;
				inBuf[2] = 0x04;
				if(!DeviceIoControl (hReadDevice,
					  IOCTL_WRITE_MAC_REGISTER,
					  &inBuf, sizeof(inBuf),
					  NULL, 0,
					  (unsigned long *)&bytes, NULL))
				{
					Memo2->Lines->Add("IOCTL_WRITE_MAC_REGISTER 0x0a failed!");
					CloseHandle(hReadDevice);
					hReadDevice=NULL;
					break;
				}

				inBuf[0] = 0x0a;
				inBuf[1] = 1;
				inBuf[2] = 0x10;
				for ( j = 0; i < 1000; j++)
				{
					if(!DeviceIoControl (hReadDevice,
						  IOCTL_READ_MAC_REGISTER,
						  &inBuf, sizeof(inBuf),
						  &outBuf, 1,
						  (unsigned long *)&bytes, NULL))
					{
						Memo2->Lines->Add("IOCTL_READ_MAC_REGISTER 0x0a failed 1!");
						CloseHandle(hReadDevice);
						hReadDevice=NULL;
						break;
					}
					if (outBuf & 0x10 && j < 999)
						continue;
					else if (outBuf & 0x10 && j == 999)
					{
						Memo2->Lines->Add("IOCTL_READ_MAC_REGISTER 0x0a failed 2!");
						CloseHandle(hReadDevice);
						hReadDevice=NULL;
						break;
					}
					else
					{
					break;
					}
				}
				if (INVALID_HANDLE_VALUE == hReadDevice) break;

				inBuf[0] = 0x08;
				inBuf[1] = 1;
				inBuf[2] = 0x00;
				if(!DeviceIoControl (hReadDevice,
					  IOCTL_READ_MAC_REGISTER,
					  &inBuf, sizeof(inBuf),
					  &outBuf, 1,
					  (unsigned long *)&bytes, NULL))
				{
						Memo2->Lines->Add("IOCTL_READ_MAC_REGISTER 0x08 failed!");
						CloseHandle(hReadDevice);
						hReadDevice=NULL;
						break;
				}
				ReadBuf[i * 2 + 8] = outBuf;
				inBuf[1] = 1;

				inBuf[0] = 0x09;
				inBuf[2] = 0x00;
				if(!DeviceIoControl (hReadDevice,
					  IOCTL_READ_MAC_REGISTER,
					  &inBuf, sizeof(inBuf),
					  &outBuf, 1,
					  (unsigned long *)&bytes, NULL))
				{
						Memo2->Lines->Add("IOCTL_READ_MAC_REGISTER 0x09 failed!");
						CloseHandle(hReadDevice);
						hReadDevice=NULL;
						break;
				}
				ReadBuf[i * 2 + 9] = outBuf;
			}
		if (hReadDevice)   //AX88179的TAG並沒有存在EEPROM
			{
			ReadBuf[0]=AX88179ROM[0];
			ReadBuf[1]=AX88179ROM[1];
			ReadBuf[2]=AX88179ROM[2];
			ReadBuf[3]=AX88179ROM[3];
			ReadBuf[4]=AX88179ROM[4];
			ReadBuf[5]=AX88179ROM[5];
			ReadBuf[6]=AX88179ROM[6];
			ReadBuf[7]=AX88179ROM[7];
			}
		}//Read AX88179 END
	}
if (hReadDevice)
	{
	//AX88772C 與 AX88179 MAC位址存放相同
	Memo2->Lines->Add("IOCTL_EEPROM_READ_DATA OK!");
	MR5=ReadBuf[8];
	MR4=ReadBuf[9];
	MR3=ReadBuf[10];
	MR2=ReadBuf[11];
	MR1=ReadBuf[12];
	MR0=ReadBuf[13];
	SS.printf("%02X:%02X:%02X:%02X:%02X:%02X",
	MR5,MR4,MR3,MR2,MR1,MR0);
	current_mac->Caption = SS; //顯示當前設備號
	Memo2->Lines->Add(SS);

if (ReadBuf[2]==0xEC)
	{
	isEC = true;
	Memo2->Lines->Add("is EC do Test");
	}
else
	{
	isEC = false;
	Memo2->Lines->Add("not EC do Program");
	}

	if (LoopCheck->Checked)
		{
		isEC = false;
		Memo2->Lines->Add("not EC do Program (LoopCheck)");
		}

	ReadGood=true;
	CloseHandle(hReadDevice);
	hReadDevice=NULL;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::CHECKClick(TObject *Sender)
{
	AnsiString SS;
unsigned int SV,EV;

CHKERR=false;
//XMLDocument1->FileName=Edit11->Text;
MA5 = ("0x"+Edit1->Text).ToInt();
MA4 = ("0x"+Edit2->Text).ToInt();
MA3 = ("0x"+Edit3->Text).ToInt();
MA2 = ("0x"+Edit4->Text).ToInt();
MA1 = ("0x"+Edit5->Text).ToInt();
MA0 = ("0x"+Edit6->Text).ToInt();
ME2 = ("0x"+Edit7->Text).ToInt();
ME1 = ("0x"+Edit8->Text).ToInt();
ME0 = ("0x"+Edit9->Text).ToInt();
SS.printf("%02X",MA5); Edit1->Text=SS;
SS.printf("%02X",MA4); Edit2->Text=SS;
SS.printf("%02X",MA3); Edit3->Text=SS;
SS.printf("%02X",MA2); Edit4->Text=SS;
SS.printf("%02X",MA1); Edit5->Text=SS;
SS.printf("%02X",MA0); Edit6->Text=SS;
SS.printf("%02X",ME2); Edit7->Text=SS;
SS.printf("%02X",ME1); Edit8->Text=SS;
SS.printf("%02X",ME0); Edit9->Text=SS;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
/////////////////////////////////////////// 限制只能執行一次
	mx = CreateMutex( NULL, true, L"AbacusPosAP" );
	if(GetLastError()){
		Application->Terminate(); //Finsh Application
	}
	frmMain->N1Click(NULL);
///////////////////////////////////////////////////////////////////
	RUNTIME = FormatDateTime("yymmddhhmmss",Now());
	lanname = "NULL";
	int timeout = 0;
	while(lanname == "NULL"){
		if(timeout == 3)
		break;
		lanname = PORT->GetSlayerName();
		timeout++;
		PORT->Delay(100);
	}

	deviceVPID = PORT->GetLanPVID(lanname);
	bool FINDDEVICE1 = PORT->FindDevice(deviceVPID);
	if (!FINDDEVICE1 || lanname == "NULL") {
		lab_devicename->Caption = "";
		current_mac->Caption = "未插入設備";
	}else{
		changedeviceClick(NULL);
		btn_start->Enabled = true;
		HaveDevice = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::MACPClick(TObject *Sender)
{
	AnsiString SS;
	if (MA0==0xFF) {
		MA0=0x00;
		if (MA1==0xFF) {
			MA1=0x00;
			if (MA2==0xFF)
				{
				MA2=0x00;
				} else MA2++;
			} else MA1++;
		} else MA0++;
	SS.printf("%02X",MA2); Edit4->Text=SS;
	SS.printf("%02X",MA1); Edit5->Text=SS;
	SS.printf("%02X",MA0); Edit6->Text=SS;
	SS.printf("%02X:%02X:%02X:%02X:%02X:%02X",MA5,MA4,MA3,MA2,MA1,MA0);
	Memo2->Lines->Add("Next MAC Address is "+SS);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::LOADClick(TObject *Sender)
{
	AnsiString filemac;
	if(ResetMAC){
		filemac = new_mac->Caption;
	}else{
		filemac = PORT->Findfilemsg("TEST_TOOL\\infomation.txt",
						"MAC SITE", 1);
		new_mac->Caption = filemac;
	}
	Edit1->Text = filemac.SubString(0,2);
	Edit2->Text = filemac.SubString(4,2);
	Edit3->Text = filemac.SubString(7,2);
	Edit4->Text = filemac.SubString(10,2);
	Edit5->Text = filemac.SubString(13,2);
	Edit6->Text = filemac.SubString(16,2);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::SAVEClick(TObject *Sender)
{
	AnsiString SS;
	SS.printf("%02X:%02X:%02X:%02X:%02X:%02X",MA5,MA4,MA3,MA2,MA1,MA0);
    Memo2->Lines->Add("Next MAC Address is "+SS);
	AnsiString totalmsg = ""; // 檔案內容
	ifstream imsgfile("TEST_TOOL\\infomation.txt");
	std::string CFGmsg;
	if (imsgfile.is_open()) {
		while (!imsgfile.eof()) {
			getline(imsgfile, CFGmsg);
			if (strstr(CFGmsg.c_str(), "MAC SITE")) {
				totalmsg = totalmsg + AnsiString(CFGmsg.c_str()) + "\n";
				getline(imsgfile, CFGmsg);
				totalmsg = totalmsg + SS + "\n";
			}else
				totalmsg = totalmsg + AnsiString(CFGmsg.c_str()) + "\n";
		}
		imsgfile.close();
	}
	// 寫入檔案
	ofstream omsgfile("TEST_TOOL\\infomation.txt");
	omsgfile << totalmsg.c_str();
	omsgfile.close();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::Memo2KeyPress(TObject *Sender, wchar_t &Key)
{
	if(btn_start->Enabled && Key == VK_RETURN && !TestWorking)
		btn_start->Click();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::changedeviceClick(TObject *Sender)
{
	_mkdir( "C:\\ASMP\\" );
	_mkdir( "C:\\ASMP\\log\\" );
	_mkdir( "C:\\ASMP\\bin\\" );
	if(PORT->lan_find_inf(lanname.c_str(), "AX88179")){
		CHIPSET = "AX88179";
		_mkdir( "C:\\ASMP\\log\\AX88179\\" );
		_mkdir( "C:\\ASMP\\bin\\AX88179\\" );
		_mkdir( "C:\\ASMP\\bin\\AX88179\\config\\" );
		SpeedTest->Checked = true;
		LOGFILE="C:\\ASMP\\log\\AX88179\\"+RUNTIME+".log";
		INIFILE="C:\\ASMP\\bin\\AX88179\\config\\AX88179.ini";
		frmMain->Caption = "AX881790 "+Buildday;
	}else if(PORT->lan_find_inf(lanname.c_str(), "Realtek USB GBE Family Controller")){
		CHIPSET = "Realtek";
		_mkdir( "C:\\ASMP\\log\\Realtek\\" );
		_mkdir( "C:\\ASMP\\bin\\Realtek\\" );
		_mkdir( "C:\\ASMP\\bin\\Realtek\\config\\" );
		SpeedTest->Checked = true;
		LOGFILE="C:\\ASMP\\log\\Realtek\\"+RUNTIME+".log";
		INIFILE="C:\\ASMP\\bin\\Realtek\\config\\Realtek.ini";
		frmMain->Caption = "Realtek "+Buildday;
	}else if(PORT->lan_find_inf(lanname.c_str(), "Realtek USB GbE Family Controller")){
		CHIPSET = "Realtek";
		_mkdir( "C:\\ASMP\\log\\Realtek\\" );
		_mkdir( "C:\\ASMP\\bin\\Realtek\\" );
		_mkdir( "C:\\ASMP\\bin\\Realtek\\config\\" );
		SpeedTest->Checked = true;
		LOGFILE="C:\\ASMP\\log\\Realtek\\"+RUNTIME+".log";
		INIFILE="C:\\ASMP\\bin\\Realtek\\config\\Realtek.ini";
		frmMain->Caption = "Realtek LAN Test"+Buildday;
	}else if(PORT->lan_find_inf(lanname.c_str(), "AX88772")){
		CHIPSET = "AX88772C";
		_mkdir( "C:\\ASMP\\log\\AX88772C\\" );
		_mkdir( "C:\\ASMP\\bin\\AX88772C\\" );
		_mkdir( "C:\\ASMP\\bin\\AX88772C\\config\\" );
		SpeedTest->Checked = false;
		LOGFILE="C:\\ASMP\\log\\AX88772C\\"+RUNTIME+".log";
		INIFILE="C:\\ASMP\\bin\\AX88772C\\config\\AX88772C.ini";
		frmMain->Caption = "AX88772C "+Buildday;
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::SaveLogClick(TObject *Sender)
{
	AnsiString SS;
	SS=FormatDateTime("hhmmss",Now());
	SS=SS+"\t";
	/*if (TestOnly->Checked)
		{
		SS=SS+"*** TEST ONLY ***\t";
		}
	else
		{
		SS=SS+SETMAC+"\t";
		} */

	SS=SS+GETMAC+"\t";

	SS=SS+HOSTIP+"\t";

	SS=SS+DHCPIP+"\t";

	if (SpeedTest->Checked && PORT->Findfilemsg("TEST_TOOL\\infomation.txt",
					"TEST ITEM", 2)=="LAN_true") SS=SS+"1G\t";
	//if (DoReadXML->Checked) SS=SS+"XML\t";

	if (isEC)
		SS=SS+"isEC\t";
	else
		SS=SS+"noEC\t";

	//Memo1->Lines->Add(SS);

	SS=SS+"\n";
	SaveLogLing(LOGFILE,SS);

	TIniFile *ini;
	ini = new TIniFile(INIFILE.c_str());
	//ini->WriteString("SYSTEM","XML File Web",Edit11->Text);
	ini->WriteString("MAC Address","MAC1",Edit1->Text);
	ini->WriteString("MAC Address","MAC2",Edit2->Text);
	ini->WriteString("MAC Address","MAC3",Edit3->Text);
	ini->WriteString("MAC Address","MAC4",Edit4->Text);
	ini->WriteString("MAC Address","MAC5",Edit5->Text);
	ini->WriteString("MAC Address","MAC6",Edit6->Text);
	ini->WriteString("MAC Address","MAC7",Edit7->Text);
	ini->WriteString("MAC Address","MAC8",Edit8->Text);
	ini->WriteString("MAC Address","MAC9",Edit9->Text);
	delete ini;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::CHKDEVICEClick(TObject *Sender)
{
	AnsiString SS;
	GETDEVICE=false;
	HOSTIP="";
	GETMAC="";
	DHCPIP="";
	//TransmitLinkSpeed=0;
	//ReceiveLinkSpeed=0;

	IP_ADAPTER_INFO AdapterInfo[16];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo( AdapterInfo,&dwBufLen);
	if (dwStatus == ERROR_SUCCESS)
		{
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		if (((AnsiString)pAdapterInfo->Description).c_str(), "ASIX")
			{
			SS=pAdapterInfo->IpAddressList.IpAddress.String;
			if ((pAdapterInfo->Address[0]==0x00)&&
				(pAdapterInfo->Address[1]==0x00)&&
				(pAdapterInfo->Address[2]==0x00))
				{
				// 系統尚未取得MAC Address
				}
			else if (strncmp(SS.c_str(),"0.0.",4)==0)
				{
				// 系統尚未取得 IP Address
				}
			else if (strncmp(pAdapterInfo->DhcpServer.IpAddress.String,"0.0.",4)==0)
				{
				// 系統尚未取得 DHCP Address
				}
			else if (strncmp(SS.c_str(),"127.",4)!=0)	//取得IP
				{
	//			if(TestTimer->Tag!=8)
					Memo2->Lines->Add("HOST IP:"+SS);
				HOSTIP=SS;		//Save New TCP/IP

				SS=pAdapterInfo->DhcpServer.IpAddress.String;
	//			if(TestTimer->Tag!=8)
					Memo2->Lines->Add("DHCP IP:"+SS);
				DHCPIP=SS;

				SS.printf("%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0],pAdapterInfo->Address[1],pAdapterInfo->Address[2],
				pAdapterInfo->Address[3],pAdapterInfo->Address[4],pAdapterInfo->Address[5]);
	//			if(TestTimer->Tag!=8)
					Memo2->Lines->Add("MAC Address:"+SS);
				GETMAC=SS;

				GETDEVICE=true;
				}
			}
		}
}
//---------------------------------------------------------------------------

void SaveLogLing(AnsiString FileName,AnsiString FileLine)
{
	fp = fopen(FileName.c_str(),"a+");
	fseek( fp, 0, SEEK_END);
	fwrite(FileLine.c_str(),FileLine.Length(),1,fp); //寫入一筆資料
	fclose(fp);
}

void __fastcall TfrmMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	ReleaseMutex(mx);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::TestTimeTimerTimer(TObject *Sender)
{
	if(frmMain->Panel_lan->Caption == "Testing")
	{
		if(TestTime == 0)
			TestTime = GetTickCount();
		lab_time->Caption = AnsiString((GetTickCount()-TestTime)/1000);
	}
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::N1Click(TObject *Sender)
{
	AnsiString temppath, temp, LANnum;
	AnsiString lanGUID = "{4d36e972-e325-11ce-bfc1-08002be10318}";
	TRegistry *reg = new TRegistry();
	reg->RootKey = HKEY_LOCAL_MACHINE;
	for (int i = 0; i <= 9999; i++) {
	//路徑名稱補0
		if (i < 10)
			LANnum = "000" + (AnsiString)i;
		if (i >= 10 && i < 100)
			LANnum = "00" + (AnsiString)i;
		if (i >= 100 && i < 1000)
			LANnum = "0" + (AnsiString)i;
		if (i >= 1000 && i < 10000)
			LANnum = (AnsiString)i;
		temppath = "SYSTEM\\CurrentControlSet\\Control\\Class\\" + lanGUID +
			"\\" + LANnum;
		reg->CloseKey();
		reg->OpenKey(temppath.c_str(), false);
		long tollnum = reg->InstanceSize();
		AnsiString tempDeviceID = (AnsiString)reg->ReadString("DriverDesc");

		if(tempDeviceID.Pos("Realtek USB")){
			if(reg->ValueExists ("NetworkAddress"))
				reg->DeleteValue("NetworkAddress");
		}
	}
	reg->CloseKey();
	delete reg;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ckbNoMACClick(TObject *Sender)
{
	N1Click(NULL);
}
//---------------------------------------------------------------------------


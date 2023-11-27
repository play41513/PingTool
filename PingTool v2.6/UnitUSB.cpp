//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitUSB.h"
#include "Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
int DeviceU3Amount = 0;
int DeviceU2Amount = 0;
HANDLE ConnectedHubHandle;

const char usbClassName[] [20] = {
"Reserved", "Audio", "Communications", "Human Interface",
"Monitor", "Physical Interface", "Power", "Printer",
"Storage", "Hub", "Vendor Specific", "*ILLEGAL VALUE*"
};

const char ConnectionStatus[] [30] = {
"No device connected", "Device connected", "Device FAILED enumeration",
"Device general FAILURE", "Device caused overcurrent", "Not enough power for device"
};

#define BUFSIZE 2048   //For buf buffer size

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormResize(TObject *Sender)
{
  GroupBox1->Height = Form1->Height - 80;
  GroupBox1->Width = Form1->Width - 10;
  //cmdSave->Top = GroupBox1->Height + 5;
  //cmdGetUSB->Top = GroupBox1->Height + 5;
  //cmdAbout->Top= GroupBox1->Height + 5;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cmdGetUSBClick(TObject *Sender)
{
  HANDLE HostControllerHandle;
  char HostControllerName[] = "\\\\.\\HCD0";
  int i;
  PCHAR driverKeyName, deviceDesc;
  AnsiString str1;   //Tree node name

  treeBack=false;
  usbCount=0;
  treeCount=0;
  treePn=0;
  treeRoot=0;
  DWORD ErrorCode = 0;
  DEBUG = !true;
  SA.nLength = sizeof(SECURITY_ATTRIBUTES);
  SA.lpSecurityDescriptor = NULL;
  SA.bInheritHandle = false;

  //Clean ListBox and TreeView
  ListBox1->Items->Clear();
  TreeView1->Items->Clear();
  USB_Device_Information.devicePath=NULL;    //Device Path Clean at begining

  for (i=0; i<10; i++)
  {
	HostControllerName[7] = i + '0';
	HostControllerHandle = CreateFile(HostControllerName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);

    if (HostControllerHandle != INVALID_HANDLE_VALUE)
    {
      ListBox1->Items->Add("******************************************************");

	  driverKeyName = GetHCDDriverKeyName(HostControllerHandle);
	  if (driverKeyName)
      {
		 deviceDesc = DriverNameToDeviceDesc(driverKeyName);
         str1=deviceDesc;
		 if (deviceDesc)
         {
		   TreeLeaf("root",str1,treeCount);
           treeLevel=treeCount;
           treeCount++;
           ListBox1->Items->Add(str1);
		 }
		 //delete(driverKeyName);
      }

	  ErrorCode = EnumerateHostController(HostControllerHandle);
	  //CloseHandle(HostControllerHandle);
	}
	CloseHandle(HostControllerHandle);
  }
  //delete(deviceDesc);
  //delete(driverKeyName);
  TreeView1->ShowButtons=true;
  TreeView1->FullExpand();
  //GetDeviceInfo->Click();
  GetAmount->Click();
}
//---------------------------------------------------------------------------
//Save the ListBox data (USB device information)
void __fastcall TForm1::cmdSaveClick(TObject *Sender)
{
   FILE *stream;
   char msg[1000];
   AnsiString str; 
   AnsiString fFileName;

   SaveDialog1->Title = "Save Text";
   SaveDialog1->Filter = "Text files (*.txt)|*.txt";

  if (SaveDialog1->Execute())
  {
    fFileName=SaveDialog1->FileName + ".txt";

    if (FileExists(fFileName))
      DeleteFile(fFileName);

    if ((stream = fopen(fFileName.c_str(), "w+")) == NULL)
    {
      ShowMessage("Cannot open output file.");
    }

    for (int i=0; i <= ListBox1->Items->Count -1; i++)
    {
      str=ListBox1->Items->Strings[i] + "\n";
      strcpy(msg,str.c_str());
      fwrite(msg, strlen(msg), 1, stream);
    }
    fclose(stream);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::TreeView1Click(TObject *Sender)
{ 
  TTreeNodes *pItems = TreeView1->Items;
  int index = 0;

  ListBox1->Items->Clear();

  while ( index < pItems->Count )
  {
	if ( pItems->Item[index]->Selected )
	{
	  for(int i=0; i < 19; i++)
	  {
		//if(index == USB_Device_Handle_Array[i].treeNode
		//    && pItems->Item[index]->Text == USB_Device_Handle_Array[i].USBName)
		if(pItems->Item[index]->Text == USB_Device_Handle_Array[i].USBName)
		{
          ListBox1->Items->Add(USB_Device_Handle_Array[i].USBName);
		  ListBox1->Items->Add(USB_Device_Handle_Array[i].devicePath);

		  GetUSBDeviceAlong(USB_Device_Handle_Array[i].hWnd, USB_Device_Handle_Array[i].port);
		}
	  }
	}
	index++;
  }
}
//---------------------------------------------------------------------------

DWORD TForm1::EnumerateHostController(HANDLE HostControllerHandle)
{
  DWORD BytesReturned;
  bool Success;
  char RootHubName[256] = "\\\\.\\";
  HANDLE RootHubHandle;
  PUSBDEVICEINFO  info;

  info = NULL;
  info = (PUSBDEVICEINFO) alloca(sizeof(USBDEVICEINFO));
  info->HubInfo = (PUSB_NODE_INFORMATION)alloca(sizeof(USB_NODE_INFORMATION));


  // First get the system name of the host controller for display
  Success = DeviceIoControl(HostControllerHandle, IOCTL_GET_HCD_DRIVERKEY_NAME,
           &UnicodeName,sizeof(UnicodeName),&UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL);

  if(Success)
  {
    // Now get the system name of it's root hub for interrogation
    Success = DeviceIoControl(HostControllerHandle, IOCTL_USB_GET_ROOT_HUB_NAME, &UnicodeName,
            sizeof(UnicodeName),&UnicodeName, sizeof(UnicodeName), &BytesReturned, NULL);

	if(Success)
    {
	  // Now open the root hub. Need to construct a char name from "\\.\" + UnicodeName
	  WideCharToMultiByte(CP_ACP, 0, &UnicodeName.Name[0], (UnicodeName.Length)/2, &RootHubName[4], 252, NULL,NULL);

	  RootHubHandle = CreateFile(RootHubName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);
	  if(RootHubHandle != INVALID_HANDLE_VALUE)
      {
        // Root hub is open. Collect the node information
		Success = DeviceIoControl(RootHubHandle, IOCTL_USB_GET_NODE_INFORMATION, info->HubInfo,
                  sizeof(USB_NODE_INFORMATION), info->HubInfo, sizeof(USB_NODE_INFORMATION), &BytesReturned, NULL);

		if (!Success)
          return GetLastError();
        else
        {
          //Add Hub to TreeView
		  TreeLeaf("child","RootHub",treeLevel);
          treeLevel=treeCount;
          treeCount++;
		  treePn++;

          //Hub Informations
          info->HubName="RootHub :USB Root Hub";
          ListBox1->Items->Add(info->HubName);

          ListBox1->Items->Add(info->HubInfo->u.HubInformation.HubIsBusPowered ? "Hub Power :Bus Power" : "Hub Power :Self Power");
          ListBox1->Items->Add("Number of Ports :" + IntToStr(info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts));

		  OutPutHubInfomation(info->HubInfo->u.HubInformation.HubDescriptor.wHubCharacteristics);

		  // Get the ports and take the data
		  GetPortData(RootHubHandle, info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts, 0);
		}
      }
	  CloseHandle(RootHubHandle);
	}
  }
  return 0;
}
//---------------------------------------------------------------------------
void TForm1::OutPutHubInfomation(USHORT wHubChar)
{
  switch (wHubChar & 0x0003)
  {
    case 0x0000:
      ListBox1->Items->Add("Power switching :Ganged");
      break;
    case 0x0001:
      ListBox1->Items->Add("Power switching :Individual");
      break;
    case 0x0002:
    case 0x0003:
      ListBox1->Items->Add("Power switching :None");
      break;
  }

  switch (wHubChar & 0x0004)
  {
    case 0x0000:
      ListBox1->Items->Add("Compound device :No");
      break;
    case 0x0004:
      ListBox1->Items->Add("Compound device :Yes");
      break;
  }

  switch (wHubChar & 0x0018)
  {
    case 0x0000:
      ListBox1->Items->Add("Over-current Protection :Global");
      break;
    case 0x0008:
      ListBox1->Items->Add("Over-current Protection :Individual");
      break;
    case 0x0010:
    case 0x0018:
      ListBox1->Items->Add("No Over-current Protection (Bus Power Only)");
      break;
  }
}
//---------------------------------------------------------------------------
void TForm1::GetPortData(HANDLE HubHandle, UCHAR PortCount, int HubDepth)
{
  DWORD BytesReturned;
  bool Success;
  int i;
  int PortIndex;
  USHORT LanguageID;
  UCHAR ThisDevice, PortStatus;
  char ConnectedHubName[256] = "\\\\.\\";
  //HANDLE ConnectedHubHandle;
  NODE_INFORMATION NodeInformation;
  NODE_CONNECTION_INFORMATION ConnectionInformation;
  AnsiString str1;
  PCHAR driverKeyName, deviceDesc;
  PUSBDEVICEINFO  info;

  info = NULL;
  info = (PUSBDEVICEINFO) alloca(sizeof(USBDEVICEINFO));
  info->HubInfo = (PUSB_NODE_INFORMATION)alloca(sizeof(USB_NODE_INFORMATION));

  // Iterate over the ports to discover what is connected to each one
  for (PortIndex = 1; PortIndex < (ULONG)PortCount + 1; PortIndex++)
  {
	LanguageID = 0; // Reset for each port
	ConnectionInformation.ConnectionIndex = PortIndex;

    //Get Connection's Node Informations
	Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION, &ConnectionInformation,
         sizeof(ConnectionInformation), &ConnectionInformation, sizeof(ConnectionInformation), &BytesReturned, NULL);

    if (!Success)
      ListBox1->Items->Add(" *** ERROR *** Node connection information not returned.");

    PortStatus = ConnectionInformation.ConnectionStatus[0]; // Save some typing!
    ThisDevice = (PortStatus == DeviceConnected) ? ConnectionInformation.DeviceAddress[0] : 0;

    ListBox1->Items->Add("======================================================");
    str1= IntToStr(PortIndex);

    //Count branch forware or backware
    if (((ULONG)PortCount==PortIndex) && (treeBack))
      str1="Port:[" + IntToStr(treeRoot) + "." + IntToStr(treePn--) + "." + str1 + "]";
    else
      str1="Port:[" + IntToStr(treeRoot) + "." + IntToStr(treePn) + "." + str1 + "]";

    // Now the connection specific information
    if (PortStatus != DeviceConnected)
    {
      str1=str1 + ConnectionStatus[PortStatus];

      ListBox1->Items->Add(str1);
      TreeLeaf("child",str1,treeLevel);
      treeCount++;

      if (((ULONG)PortCount==PortIndex) && (treeBack))
      {
        treeLevel--;
        treeBack=false;
      }
    }
    else
    {
      //Have a device or a hub connected to this port
      if (!ConnectionInformation.DeviceIsHub)
      {
        USB_Device_Information.devicePath="";

		// There is an I/O device connected. Print out it's descriptors
		// Note that many current devices do not respond correctly if ConfigID != 0.
		// So only request the first configuration
		driverKeyName = GetDriverKeyName(HubHandle, PortIndex);
        if (driverKeyName)
        {
		   DriverNameToDeviceDescInfo(driverKeyName);

           if (USB_Device_Information.deviceDesc != NULL)
           {
             str1=str1 + USB_Device_Information.deviceDesc +
                 " (" + LookingString(USB_Device_Information.DeviceID,"\\") + ")";
           }
        }

        //Save USB Device Handle to Array
        USB_Device_Handle_Array[usbCount].hWnd=HubHandle;
        USB_Device_Handle_Array[usbCount].USBName=str1;
        USB_Device_Handle_Array[usbCount].treeNode=treeCount;
        USB_Device_Handle_Array[usbCount].port=PortIndex;
        USB_Device_Handle_Array[usbCount].devicePath=USB_Device_Information.devicePath;
        USB_Device_Handle_Array[usbCount].deviceSN=LookingString(USB_Device_Information.DeviceID,"\\");
        USB_Device_Handle_Array[usbCount].deviceDesc=USB_Device_Information.deviceDesc;
        usbCount++;

        //Add USB device to TreeView
        ListBox1->Items->Add(str1);
        TreeLeaf("child",str1,treeLevel);
        treeCount++;

        str1="ConnectionStatus :";
        str1=str1 + ConnectionStatus[PortStatus];
		ListBox1->Items->Add(str1);

		ListBox1->Items->Add("Current Config Value :0x" + IntToHex(ConnectionInformation.CurrentConfigurationValue,2));

        if(&ConnectionInformation.LowSpeed)
		  ListBox1->Items->Add("Device Bus Speed :Low");
		else
		  ListBox1->Items->Add("Device Bus Speed :Full");

		ListBox1->Items->Add("Device Address :0x" + IntToHex(*ConnectionInformation.DeviceAddress,2));
		ListBox1->Items->Add("Open Pipes :" + IntToHex(*ConnectionInformation.NumberOfOpenPipes,2));

        if(*ConnectionInformation.NumberOfOpenPipes)
          DisplayPipeInfo(*ConnectionInformation.NumberOfOpenPipes, ConnectionInformation.PipeList);


		LanguageID = DisplayDeviceDescriptor(HubHandle, PortIndex, LanguageID,
					 &ConnectionInformation.DeviceDescriptor.bLength);

		LanguageID = DisplayConfigurationDescriptor(HubHandle, PortIndex, LanguageID);
      }
      else
      {
        //Have no a device or a hub connected to this port
        // There is a hub connected and we need to iterate over it's ports
        str1=str1 + "Hub connected : ";

        // Get the system name of the connected hub so that we can make a connection to it
		ConnectedHub.ConnectionIndex = PortIndex;

        driverKeyName = GetDriverKeyName(HubHandle, PortIndex);
        if (driverKeyName)
        {
           DriverNameToDeviceDescInfo(driverKeyName);

           if (USB_Device_Information.deviceDesc != NULL)
             str1=str1 + USB_Device_Information.deviceDesc +
                 " (" + LookingString(USB_Device_Information.DeviceID,"\\") + ")";
        }

        //Save USB Device Handle to Array
        USB_Device_Handle_Array[usbCount].hWnd=HubHandle;
        USB_Device_Handle_Array[usbCount].USBName=str1;
        USB_Device_Handle_Array[usbCount].treeNode=treeCount;
        USB_Device_Handle_Array[usbCount].port=PortIndex;
        USB_Device_Handle_Array[usbCount].devicePath=USB_Device_Information.devicePath;
        USB_Device_Handle_Array[usbCount].deviceSN=LookingString(USB_Device_Information.DeviceID,"\\");
		USB_Device_Handle_Array[usbCount].deviceDesc=USB_Device_Information.deviceDesc;
        usbCount++;

        //try to connect nest node 
		Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_NAME, &ConnectedHub,
				sizeof(ConnectedHub), &ConnectedHub, sizeof(ConnectedHub), &BytesReturned, NULL);

        if (Success)
        {
          ListBox1->Items->Add(str1);
		  TreeLeaf("child",str1,treeLevel);
          treeCount++;
          treePn++;
          treeBack=true;
          treeLevel++;

		  WideCharToMultiByte(CP_ACP, 0, &ConnectedHub.Name[0], (ConnectedHub.ActualLength)/2, &ConnectedHubName[4], 252, NULL, NULL);

		  ConnectedHubHandle = CreateFile(ConnectedHubName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);

          if(ConnectedHubHandle != INVALID_HANDLE_VALUE)
          {
            // Root hub is open. Collect the node information
			Success = DeviceIoControl(ConnectedHubHandle, IOCTL_USB_GET_NODE_INFORMATION, info->HubInfo,
                    sizeof(USB_NODE_INFORMATION), info->HubInfo, sizeof(USB_NODE_INFORMATION), &BytesReturned, NULL);

            if (Success)
            {
              //Hub Informations
              info->HubName="RootHub :External Hub";
			  ListBox1->Items->Add(info->HubName);

			  ListBox1->Items->Add(info->HubInfo->u.HubInformation.HubIsBusPowered ? "Hub Power :Bus Power" : "Hub Power :Self Power");
			  ListBox1->Items->Add("Number of Ports :" + IntToStr(info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts));

			  OutPutHubInfomation(info->HubInfo->u.HubInformation.HubDescriptor.wHubCharacteristics);

			  // Get the ports and take the data
			  GetPortData(ConnectedHubHandle, info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts, HubDepth+1);
			}
            else
            {
              str1=str1 + " *** ERROR *** Node information not returned";
              ListBox1->Items->Add(str1);
              TreeLeaf("child",str1,treeLevel);
              treeCount++;
            }
		  }
		  //CloseHandle(ConnectedHubHandle);
		}
        else
        {
          str1=str1 + " *** ERROR *** Node connection name not returned";
          ListBox1->Items->Add(str1);
          TreeLeaf("child",str1,treeLevel);
          treeCount++;
		}
      };
	};
  };
}
//---------------------------------------------------------------------------
//For select one TreeView branch, display it to ListBox
void TForm1::GetUSBDeviceAlong(HANDLE HubHandle, int PortIndex)
{
  DWORD BytesReturned;
  bool Success;
  int i;
  USHORT LanguageID;
  UCHAR ThisDevice, PortStatus;
  char ConnectedHubName[256] = "\\\\.\\";
  //HANDLE ConnectedHubHandle;
  NODE_INFORMATION NodeInformation;
  NODE_CONNECTION_INFORMATION ConnectionInformation;
  AnsiString str1;
  PCHAR driverKeyName, deviceDesc;
  PUSBDEVICEINFO  info;

  info = NULL;
  info = (PUSBDEVICEINFO) alloca(sizeof(USBDEVICEINFO));
  info->HubInfo = (PUSB_NODE_INFORMATION)alloca(sizeof(USB_NODE_INFORMATION));    

  LanguageID = 0; // Reset for each port
  ConnectionInformation.ConnectionIndex = PortIndex;

  Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_INFORMATION, &ConnectionInformation,
         sizeof(ConnectionInformation), &ConnectionInformation, sizeof(ConnectionInformation), &BytesReturned, NULL);

  if (!Success)
      ListBox1->Items->Add(" *** ERROR *** Node connection information not returned.");

  PortStatus = ConnectionInformation.ConnectionStatus[0]; // Save some typing!
  ThisDevice = (PortStatus == DeviceConnected) ? ConnectionInformation.DeviceAddress[0] : 0;

  ListBox1->Items->Add("======================================================");
  str1=IntToStr(PortIndex);
  str1="Port:[" + IntToStr(treePn) + "." + str1 + "]";
  
  // Now the connection specific information
  if (PortStatus != DeviceConnected)
  {
    str1=str1 + ConnectionStatus[PortStatus];
  }
  else
  { // have a device or a hub connected to this port
    if (!ConnectionInformation.DeviceIsHub)
    {
      // There is an I/O device connected. Print out it's descriptors
      // Note that many current devices do not respond correctly if ConfigID != 0.
      // So only request the first configuration
      str1=str1 + "I/O device connected : ";

      driverKeyName = GetDriverKeyName(HubHandle, PortIndex);
      if (driverKeyName)
        DriverNameToDeviceDescInfo(driverKeyName);

      str1="ConnectionStatus :";
      str1=str1 + ConnectionStatus[PortStatus];
	  ListBox1->Items->Add(str1);

	  ListBox1->Items->Add("Current Config Value :0x" + IntToHex(ConnectionInformation.CurrentConfigurationValue,2));

      if(&ConnectionInformation.LowSpeed)
		ListBox1->Items->Add("Device Bus Speed :Low");
	  else
		ListBox1->Items->Add("Device Bus Speed :Full");


	  ListBox1->Items->Add("Device Address :0x" + IntToHex(*ConnectionInformation.DeviceAddress,2));
	  ListBox1->Items->Add("Open Pipes :" + IntToHex(*ConnectionInformation.NumberOfOpenPipes,2));

	  if(*ConnectionInformation.NumberOfOpenPipes)
        DisplayPipeInfo(*ConnectionInformation.NumberOfOpenPipes, ConnectionInformation.PipeList);

	  LanguageID = DisplayDeviceDescriptor(HubHandle, PortIndex, LanguageID,
				 &ConnectionInformation.DeviceDescriptor.bLength);

	  LanguageID = DisplayConfigurationDescriptor(HubHandle, PortIndex, LanguageID);
    }
    else
	{
	  // There is a hub connected and we need to iterate over it's ports
      str1=str1 + "Hub connected : ";
      // Get the system name of the connected hub so that we can make a connection to it
      ConnectedHub.ConnectionIndex = PortIndex;

      Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_NODE_CONNECTION_NAME, &ConnectedHub,
              sizeof(ConnectedHub), &ConnectedHub, sizeof(ConnectedHub), &BytesReturned, NULL);

      if (Success)
      {
        WideCharToMultiByte(CP_ACP, 0, &ConnectedHub.Name[0], (ConnectedHub.ActualLength)/2, &ConnectedHubName[4], 252, NULL, NULL);

        ConnectedHubHandle = CreateFile(ConnectedHubName, GENERIC_WRITE, FILE_SHARE_WRITE, &SA, OPEN_EXISTING, 0, NULL);

        if(ConnectedHubHandle != INVALID_HANDLE_VALUE)
        {
		  // Root hub is open. Collect the node information
		  Success = DeviceIoControl(ConnectedHubHandle, IOCTL_USB_GET_NODE_INFORMATION, info->HubInfo,
				  sizeof(USB_NODE_INFORMATION), info->HubInfo, sizeof(USB_NODE_INFORMATION), &BytesReturned, NULL);

          if (Success)
          {
            //Hub Informations
            info->HubName="RootHub :External Hub";
			ListBox1->Items->Add(info->HubName);

			ListBox1->Items->Add(info->HubInfo->u.HubInformation.HubIsBusPowered ? "Hub Power :Bus Power" : "Hub Power :Self Power");
			ListBox1->Items->Add("Number of Ports :" + IntToStr(info->HubInfo->u.HubInformation.HubDescriptor.bNumberOfPorts));

			OutPutHubInfomation(info->HubInfo->u.HubInformation.HubDescriptor.wHubCharacteristics);
          }
          else
          {
            str1=str1 + " *** ERROR *** Node information not returned";

          }
		}
		CloseHandle(HubHandle);     //����˸m
		//CloseHandle(ConnectedHubHandle);
	  }
      else
      {
        str1=str1 + " *** ERROR *** Node connection name not returned";
      }
    };
  }
}
//---------------------------------------------------------------------------
USHORT TForm1::DisplayDeviceDescriptor (HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID, PUCHAR BufferPtr)
{
  AnsiString strVendor,str;
  USHORT tmp;
  UCHAR LowByte;
  char *buf;

  ListBox1->Items->Add("------------------------------------------");
  ListBox1->Items->Add("Device Descriptor");

  BufferPtr--; // Backup pointer to prepare for pre-increment
  ListBox1->Items->Add(" bLength :0x" + IntToHex(*++BufferPtr,4));

  ListBox1->Items->Add(" bDescriptorType :0x" + IntToHex(*++BufferPtr,4));

  LowByte = *++BufferPtr;
  ListBox1->Items->Add(" bcdUSB :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
  ListBox1->Items->Add(" bDeviceClass :0x" + IntToHex(*++BufferPtr,2));
  ListBox1->Items->Add(" bDeviceSubClass :0x" + IntToHex(*++BufferPtr,2));
  ListBox1->Items->Add(" bDeviceProtocol :0x" + IntToHex(*++BufferPtr,2));
  ListBox1->Items->Add(" bMaxEP0Size :0x" + IntToHex(*++BufferPtr,2));

  LowByte = *++BufferPtr;
  
  tmp=LowByte + (*++BufferPtr << 8);
  str=IntToHex(tmp,4);
  
  strVendor=GetVendorString(tmp);

  ListBox1->Items->Add(" wVendorID :0x" + str + "  (" + strVendor + ")");
  //ListBox1->Items->Add(" wVendorID :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
  
  LowByte = *++BufferPtr;
  ListBox1->Items->Add(" wProductID :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4) + "  (" + USB_Device_Information.localInfo + ")");

  //ListBox1->Items->Add(" wProductID :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
  LowByte = *++BufferPtr;
  ListBox1->Items->Add(" wDeviceID :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
  ListBox1->Items->Add(" iManufacturer :0x" + IntToHex(*++BufferPtr,2));
  
  if (*BufferPtr != 0)
    LanguageID = DisplayStringDescriptor(HubHandle, PortIndex, LanguageID, *BufferPtr);

  ListBox1->Items->Add(" iProduct :0x" + IntToHex(*++BufferPtr,2));

  if (*BufferPtr != 0)
    LanguageID = DisplayStringDescriptor(HubHandle, PortIndex, LanguageID, *BufferPtr);

  ListBox1->Items->Add(" iSerialNumber :0x" + IntToHex(*++BufferPtr,2));

  if (*BufferPtr != 0)
    LanguageID = DisplayStringDescriptor(HubHandle, PortIndex, LanguageID, *BufferPtr);

  ListBox1->Items->Add(" bNumConfigurations :0x" + IntToHex(*++BufferPtr,2));

  return LanguageID;
}
//---------------------------------------------------------------------------
//*****************************************************************************
//
// GetVendorString()
//
// idVendor - USB Vendor ID
//
// Return Value - Vendor name string associated with idVendor, or NULL if
// no vendor name string is found which is associated with idVendor.
//
//*****************************************************************************
PCHAR TForm1::GetVendorString(USHORT idVendor)
{
    PUSBVENDORID vendorID;

    if (idVendor != 0x0000)
    {
        vendorID = USBVendorIDs;

        while (vendorID->usVendorID != 0x0000)
        {
            if (vendorID->usVendorID == idVendor)
            {
                return (vendorID->szVendor);
            }
            vendorID++;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
USHORT TForm1::DisplayConfigurationDescriptor(HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID)
{
  DWORD BytesReturned;
  bool Success;
  UCHAR LowByte;
  DESCRIPTOR_REQUEST Packet;
  int i;

  ListBox1->Items->Add("------------------------------------------");
  ListBox1->Items->Add("Configuration Descriptor");

  //First need to get the configuration descriptor
  memset(&Packet, 0, sizeof(Packet));
  Packet.ConnectionIndex = PortIndex;
  Packet.SetupPacket.bmRequest = 0x80;
  Packet.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
  Packet.SetupPacket.wValue[1] = USB_CONFIGURATION_DESCRIPTOR_TYPE;
  Packet.SetupPacket.wLength[1] = 1; // Using a 2K buffer

  Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION, &Packet,
            sizeof(Packet), &Packet, sizeof(Packet), &BytesReturned, NULL);

  if (!Success)
    ListBox1->Items->Add(" *** ERROR *** Configuration Descriptor not returned. ErrorCode = " + GetLastError());

  PUCHAR BufferPtr = &Packet.Data[0];
  UCHAR Length = *BufferPtr;
  while (Length != 0)
  {
    UCHAR Type = *++BufferPtr;
    switch (Type)
    {
      case 2:
        ListBox1->Items->Add(" bLength :0x" + IntToHex(Length,4));
        ListBox1->Items->Add(" bDescriptorType 0x" + IntToHex(Type,4) + " =Configuration Header");
        LowByte = *++BufferPtr;
        ListBox1->Items->Add(" wTotalLength :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
        ListBox1->Items->Add(" bNumInterfaces :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bConfigValue :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" iConfiguration :0x" + IntToHex(*++BufferPtr,4));
        if (*BufferPtr != 0)
		  LanguageID = DisplayStringDescriptor(HubHandle, PortIndex, LanguageID,*BufferPtr);

        ListBox1->Items->Add(" bmAttributes :0x" + IntToHex(*++BufferPtr,4));
        LowByte = *++BufferPtr;
        ListBox1->Items->Add(" bMaxPower :0x" + IntToHex(LowByte,4) + "=0x" + IntToHex(LowByte << 1,4) + "mA");
        break;
      case 4:
        ListBox1->Items->Add(" bLength :0x" + IntToHex(Length,4));
        ListBox1->Items->Add(" bDescriptorType 0x" + IntToHex(Type,4) + " Interface Descriptor");
        ListBox1->Items->Add(" bInterfaceNum :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bAlternateSetting :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bNumEndpoints :0x" + IntToHex(*++BufferPtr,4));
        LowByte = *++BufferPtr;
        if ((LowByte > 9) & (LowByte < 255))
          LowByte = 11;

        if (LowByte == 255)
          LowByte = 10;

        ListBox1->Items->Add(" bInterfaceClass :0x" + IntToHex(*BufferPtr,4) + "=" + usbClassName[LowByte]);
        ListBox1->Items->Add(" bSubClass :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bProtocol :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" iInterface :0x" + IntToHex(*++BufferPtr,4));

        if (*BufferPtr != 0)
          LanguageID = DisplayStringDescriptor(HubHandle, PortIndex, LanguageID,*BufferPtr);

        break;
      case 5:
		ListBox1->Items->Add(" bLength :0x" + IntToHex(Length,4));
		ListBox1->Items->Add(" bDescriptorType 0x" + IntToHex(Type,4) + " Endpoint Descriptor");
		ListBox1->Items->Add(" bEndpointAddress :0x" + IntToHex(*++BufferPtr,4));
		ListBox1->Items->Add(" bmAttributes :0x" + IntToHex(*++BufferPtr,4));
        LowByte = *++BufferPtr;
        ListBox1->Items->Add(" wMaxPacketSize :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
		ListBox1->Items->Add(" bInterval :0x" + IntToHex(*++BufferPtr,4));
        break;
      case 0x21:
        ListBox1->Items->Add(" bLength :0x" + IntToHex(Length,4));
        ListBox1->Items->Add(" bDescriptorType 0x" + IntToHex(Type,4) + " =HID Descriptor");
        LowByte = *++BufferPtr;
        ListBox1->Items->Add(" wHIDversion :0x" + IntToHex(LowByte + (*++BufferPtr << 8),4));
        ListBox1->Items->Add(" bCountryCode :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bHIDDescriptorCount :0x" + IntToHex(*++BufferPtr,4));
        ListBox1->Items->Add(" bHIDReportType :0x" + IntToHex(*++BufferPtr,4));
        LowByte = *++BufferPtr;
        ListBox1->Items->Add(" wHIDReportLength :0x" + IntToHex( LowByte + (*++BufferPtr << 8),4));
        break;
      default:
        ListBox1->Items->Add("Unknown descriptor with Length = 0x" + IntToHex(Length,4) + " and Type = 0x" + IntToHex(Type,4));
        BufferPtr-=2; // Back up to start of descriptor

        for (i = 0; i < Length; i++)
        {
          if ((i % 16) == 0) ListBox1->Items->Add("                     ");

          ListBox1->Items->Add("UnKnown :0x" + IntToHex(*++BufferPtr,4));
        }
        break;
    }

    Length = *++BufferPtr;
    ListBox1->Items->Add("                           ");
  }

  return LanguageID;
}
//---------------------------------------------------------------------------
USHORT TForm1::DisplayStringDescriptor (HANDLE HubHandle, ULONG PortIndex, USHORT LanguageID, UCHAR Index)
{
  DESCRIPTOR_REQUEST Packet;
  DWORD BytesReturned;
  bool Success;

  if (LanguageID == 0) 
  { // Get the language ID
    memset(&Packet, 0, sizeof(Packet));
    Packet.ConnectionIndex = PortIndex;
    Packet.SetupPacket.bmRequest = 0x80;
    Packet.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
    Packet.SetupPacket.wValue[1] = USB_STRING_DESCRIPTOR_TYPE;
    Packet.SetupPacket.wLength[0] = 4;

    Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION, &Packet,
                    sizeof(Packet), &Packet, sizeof(Packet), &BytesReturned, NULL);

    if (!Success)
      ListBox1->Items->Add(" *** ERROR *** String Descriptor 0 not returned, ErrorCode =" + GetLastError());

    LanguageID = Packet.Data[2] + (Packet.Data[3] << 8);
  }

  memset(&Packet, 0, sizeof(Packet));
  Packet.ConnectionIndex = PortIndex;
  Packet.SetupPacket.bmRequest = 0x80;
  Packet.SetupPacket.bRequest = USB_REQUEST_GET_DESCRIPTOR;
  Packet.SetupPacket.wValue[1] = USB_STRING_DESCRIPTOR_TYPE;
  Packet.SetupPacket.wValue[0] = Index;
  Packet.SetupPacket.wIndex[0] = LanguageID & 0xFF;
  Packet.SetupPacket.wIndex[1] = (LanguageID >> 8) & 0xFF;
  Packet.SetupPacket.wLength[0] = 255;
  
  Success = DeviceIoControl(HubHandle, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION, &Packet,
                       sizeof(Packet), &Packet, sizeof(Packet), &BytesReturned, NULL);

  if (!Success)
    ListBox1->Items->Add(" *** ERROR *** String Descriptor  not returned. ErrorCode = " + IntToHex(Index,4) + GetLastError());
  
  //ListBox1->Items->Add(" Packet Data :0x" + &Packet.Data[2]);
  
  return LanguageID;
}
//---------------------------------------------------------------------------
PCHAR TForm1::GetHCDDriverKeyName (HANDLE  HCD)
{
    BOOL                    success;
    ULONG                   nBytes;
    USB_HCD_DRIVERKEY_NAME  driverKeyName;
    PUSB_HCD_DRIVERKEY_NAME driverKeyNameW;
    PCHAR                   driverKeyNameA;

    driverKeyNameW = NULL;
    driverKeyNameA = NULL;

    // Get the length of the name of the driver key of the HCD
    success = DeviceIoControl(HCD, IOCTL_GET_HCD_DRIVERKEY_NAME, &driverKeyName,
              sizeof(driverKeyName), &driverKeyName, sizeof(driverKeyName), &nBytes, NULL);

    if (!success)
       return NULL;

    // Allocate space to hold the driver key name
    nBytes = driverKeyName.ActualLength;
    if (nBytes <= sizeof(driverKeyName))
      return NULL;

    driverKeyNameW =(PUSB_HCD_DRIVERKEY_NAME) alloca(nBytes);
    if (driverKeyNameW == NULL)
       return NULL;

    // Get the name of the driver key of the device attached to
    // the specified port.
    success = DeviceIoControl(HCD, IOCTL_GET_HCD_DRIVERKEY_NAME, driverKeyNameW, nBytes,
              driverKeyNameW, nBytes, &nBytes, NULL);

    if (!success)
      return NULL;
    else
    {
      return WideStrToMultiStr(driverKeyNameW->DriverKeyName);
    }  
}
//---------------------------------------------------------------------------
PCHAR TForm1::DriverNameToDeviceDesc (PCHAR DriverName)
{
    CHAR buf[BUFSIZE]; // XXXXX How big does this have to be? Dynamically size it?

    DEVINST     devInst;
    DEVINST     devInstNext;
    CONFIGRET   cr;
    ULONG       walkDone = 0;
    ULONG       len;

    // Get Root DevNode
    cr = CM_Locate_DevNode(&devInst, NULL, 0);

    if (cr != CR_SUCCESS)
      return NULL;

    // Do a depth first search for the DevNode with a matching
    // DriverName value
    while (!walkDone)
    {
        // Get the DriverName value
        len = sizeof(buf);
        cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DRIVER, NULL, buf, &len, 0);

        // If the DriverName value matches, return the DeviceDescription
        if (cr == CR_SUCCESS && strcmp(DriverName, buf) == 0)
        {
            len = sizeof(buf);
            cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DEVICEDESC, NULL, buf, &len, 0);

            if (cr == CR_SUCCESS)
            {
              return buf;
            }
            else
              return NULL;
        }

        // This DevNode didn't match, go down a level to the first child.
        cr = CM_Get_Child(&devInstNext, devInst, 0);

        if (cr == CR_SUCCESS)
        {
          devInst = devInstNext;
          continue;
        }

        // Can't go down any further, go across to the next sibling.  If
        // there are no more siblings, go back up until there is a sibling.
        // If we can't go up any further, we're back at the root and we're
        // done.
        for (;;)
        {
            cr = CM_Get_Sibling(&devInstNext, devInst, 0);

            if (cr == CR_SUCCESS)
            {
              devInst = devInstNext;
              break;
            }

            cr = CM_Get_Parent(&devInstNext, devInst, 0);


            if (cr == CR_SUCCESS)
                devInst = devInstNext;
            else
            {
                walkDone = 1;
                break;
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void TForm1::DriverNameToDeviceDescInfo(PCHAR DriverName)
{
    CHAR buf[BUFSIZE]; // XXXXX How big does this have to be? Dynamically size it?

    DEVINST     devInst;
    DEVINSTID   pDeviceID;
    DEVINST     devInstNext;
    CONFIGRET   cr;
    ULONG       walkDone = 0;
    ULONG       len;
    GUID        GuidPtr;

    //HP900 GUID2k
    /*
    GuidPtr.Data1=685215661;
    GuidPtr.Data2=23058;
    GuidPtr.Data3 = 4561;
    GuidPtr.Data4[0] = 174;
    GuidPtr.Data4[1] = 91;
    GuidPtr.Data4[2] = 0;
    GuidPtr.Data4[3] = 0;
    GuidPtr.Data4[4] = 248;
    GuidPtr.Data4[5] = 3;
    GuidPtr.Data4[6] = 168;
    GuidPtr.Data4[7] = 194;
    */
    //SymbolicName="\??\USB#Vid_03f0&Pid_1112#SUB-SS-003#{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
    //Symbolic Name for all USB device interface
    GuidPtr.Data1=2782707472;
    GuidPtr.Data2=25904;
    GuidPtr.Data3 = 4562;
    GuidPtr.Data4[0] = 144;
    GuidPtr.Data4[1] = 31;
    GuidPtr.Data4[2] = 0;
    GuidPtr.Data4[3] = 192;
    GuidPtr.Data4[4] = 79;
    GuidPtr.Data4[5] = 185;
    GuidPtr.Data4[6] = 81;
    GuidPtr.Data4[7] = 237;

    USB_Device_Information.deviceDesc=NULL;
    USB_Device_Information.localInfo=NULL;
    USB_Device_Information.DeviceID=NULL;
    
    // Get Root DevNode
    cr = CM_Locate_DevNode(&devInst, NULL, 0);

    if (cr != CR_SUCCESS)
      USB_Device_Information.deviceDesc=NULL;

    // Do a depth first search for the DevNode with a matching
    // DriverName value
    while (!walkDone)
    {
        // Get the DriverName value
        len = sizeof(buf);
        cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DRIVER, NULL, buf, &len, 0);

        // If the DriverName value matches, return the DeviceDescription
        if (cr == CR_SUCCESS && strcmp(DriverName, buf) == 0)
        {
            //Location Information
            len = sizeof(buf);
            cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_LOCATION_INFORMATION, NULL, buf, &len, 0);

            if (cr == CR_SUCCESS)
            {
              USB_Device_Information.localInfo=buf;

              //Get GUID from INI file
              //GuidPtr=OpenINIFiles(buf);
            }

            //Device Decsription
            len = sizeof(buf);
            cr = CM_Get_DevNode_Registry_Property(devInst, CM_DRP_DEVICEDESC, NULL, buf, &len, 0);

            if (cr == CR_SUCCESS)
              USB_Device_Information.deviceDesc=buf;

            //Device ID
            len = sizeof(buf);
            cr= CM_Get_Device_ID(devInst, buf, len, 0);

            if (cr == CR_SUCCESS)
            {
              USB_Device_Information.DeviceID=buf;
              pDeviceID=buf;
            }
            
            //Device Interface Path
            len = sizeof(buf);
            cr =  CM_Get_Device_Interface_List(&GuidPtr, pDeviceID, buf, len, 0);

            if (cr == CR_SUCCESS)
              USB_Device_Information.devicePath=buf;

        }

        // This DevNode didn't match, go down a level to the first child.
        cr = CM_Get_Child(&devInstNext, devInst, 0);

        if (cr == CR_SUCCESS)
        {
          devInst = devInstNext;
          continue;
        }

        // Can't go down any further, go across to the next sibling.  If
        // there are no more siblings, go back up until there is a sibling.
        // If we can't go up any further, we're back at the root and we're
        // done.
        for (;;)
        {
            cr = CM_Get_Sibling(&devInstNext, devInst, 0);

            if (cr == CR_SUCCESS)
            {
              devInst = devInstNext;
              break;
            }

            cr = CM_Get_Parent(&devInstNext, devInst, 0);


            if (cr == CR_SUCCESS)
                devInst = devInstNext;
            else
            {
                walkDone = 1;
                break;
            }
        }
    }

}
//---------------------------------------------------------------------------
PCHAR TForm1::WideStrToMultiStr (PWCHAR WideStr)
{
    ULONG nBytes;
    PCHAR MultiStr;

    // Get the length of the converted string
    //
    nBytes = WideCharToMultiByte(CP_ACP, 0, WideStr, -1, NULL, 0, NULL, NULL);

    if (nBytes == 0)
      return NULL;

    // Allocate space to hold the converted string
    MultiStr =(PCHAR )alloca(nBytes);

    if (MultiStr == NULL)
      return NULL;

    // Convert the string
    //
    nBytes = WideCharToMultiByte(CP_ACP, 0, WideStr, -1, MultiStr, nBytes, NULL, NULL);

    if (nBytes == 0)
    {
        delete(MultiStr);
        return NULL;
    }

    return MultiStr;
}
//---------------------------------------------------------------------------
PCHAR TForm1::GetDriverKeyName (HANDLE Hub, ULONG ConnectionIndex)
{
    BOOL                                success;
    ULONG                               nBytes;
    USB_NODE_CONNECTION_DRIVERKEY_NAME  driverKeyName;
    PUSB_NODE_CONNECTION_DRIVERKEY_NAME driverKeyNameW;
    PCHAR                               driverKeyNameA;

    driverKeyNameW = NULL;
    driverKeyNameA = NULL;

    // Get the length of the name of the driver key of the device attached to
    // the specified port.
    driverKeyName.ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME, &driverKeyName,
         sizeof(driverKeyName), &driverKeyName, sizeof(driverKeyName), &nBytes, NULL);

    if (!success)
      return NULL;

    // Allocate space to hold the driver key name
    nBytes = driverKeyName.ActualLength;

    if (nBytes <= sizeof(driverKeyName))
      return NULL;

    driverKeyNameW = (PUSB_NODE_CONNECTION_DRIVERKEY_NAME) alloca(nBytes);

    if (driverKeyNameW == NULL)
      return NULL;

    // Get the name of the driver key of the device attached to
    // the specified port.
    driverKeyNameW->ConnectionIndex = ConnectionIndex;

    success = DeviceIoControl(Hub, IOCTL_USB_GET_NODE_CONNECTION_DRIVERKEY_NAME,
              driverKeyNameW, nBytes, driverKeyNameW, nBytes, &nBytes, NULL);

    if (success)
    {
      // Convert the driver key name
      driverKeyNameA = WideStrToMultiStr(driverKeyNameW->DriverKeyName);

      // All done, free the uncoverted driver key name and return the
      // converted driver key name
	  //delete(driverKeyNameW);

      //return driverKeyNameA;
      return WideStrToMultiStr(driverKeyNameW->DriverKeyName);
    }
    else
      return NULL;
}
//---------------------------------------------------------------------------
void TForm1::TreeLeaf(AnsiString Branch, AnsiString BranchName, int blevel)
{
  TTreeNode *Node1;

  if(Branch == "root")
  {
    Node1 = NULL; //Add a root node
    TreeView1->Items->Add(Node1, BranchName);
    treeRoot++;
    treePn=0;
  }
  else
  {
    Node1 = TreeView1->Items->Item[blevel]; //node in tree view
    TreeView1->Items->AddChild(Node1,BranchName);
  }
}
//---------------------------------------------------------------------------
void TForm1::DisplayPipeInfo(ULONG NumPipes, USB_PIPE_INFO *PipeInfo)
{
	int i;

	for (i=0; i < NumPipes; i++)
    {
	  ListBox1->Items->Add(" ");
	  ListBox1->Items->Add("Endpoint Descriptor: [" + IntToStr(i) + "]");
      DisplayEndpointDescriptor(&PipeInfo[i].EndpointDescriptor);
    }
}
//---------------------------------------------------------------------------
void TForm1::DisplayEndpointDescriptor(PUSB_ENDPOINT_DESCRIPTOR EndpointDesc)
{
    ListBox1->Items->Add(" bEndpointAddress :0x" + IntToHex(EndpointDesc->bEndpointAddress,2));

    switch (EndpointDesc->bmAttributes & 0x03)
    {
        case 0x00:
            ListBox1->Items->Add(" Transfer Type :Control");
            break;

        case 0x01:
            ListBox1->Items->Add(" Transfer Type :Isochronous");
            break;

        case 0x02:
            ListBox1->Items->Add(" Transfer Type :Bulk");
            break;

        case 0x03:
            ListBox1->Items->Add(" Transfer Type :Interrupt");
            break;
    }

    ListBox1->Items->Add(" wMaxPacketSize :0x" + IntToHex(EndpointDesc->wMaxPacketSize,4) +
                         "(" + IntToHex(EndpointDesc->wMaxPacketSize,4) + ")");

    if (EndpointDesc->bLength == sizeof(USB_ENDPOINT_DESCRIPTOR))
    {
        ListBox1->Items->Add(" bInterval :0x" + IntToHex(EndpointDesc->bInterval,2));
    }
    else
    {
        PUSB_ENDPOINT_DESCRIPTOR2 endpointDesc2;

        endpointDesc2 = (PUSB_ENDPOINT_DESCRIPTOR2)EndpointDesc;

        ListBox1->Items->Add(" wInterval :0x" + IntToHex(endpointDesc2->wInterval,2));

        ListBox1->Items->Add(" bSyncAddress :0x" + IntToHex(endpointDesc2->bSyncAddress,2));
    }
}
//---------------------------------------------------------------------------
AnsiString TForm1::LookingString(AnsiString Sour, AnsiString Dest)
{
  for(int i=0; i < Sour.Length() - Dest.Length(); i++)
  {
    if(Sour.SubString(i+1, Dest.Length())==Dest)
      Sour=MidStr(Sour,i+2,100);
  }
  return Sour;
}
//---------------------------------------------------------------------------
/*
GUID TForm1::OpenINIFiles(AnsiString SectionName)
{
   GUID    GuidPtr;
   AnsiString szPath=ExtractFilePath(Application->ExeName);
   TIniFile *ini=new TIniFile(szPath+"GUID.ini");

   ListBox2->Items->Clear();
   ListBox3->Items->Clear();

   ini->ReadSections(ListBox2->Items);
   for(int i=0; i <= ListBox2->Items->Count-1 ; i++)
   {
     if(ListBox2->Items->Strings[i]==SectionName)
     {
       ini->ReadSectionValues(SectionName,ListBox3->Items);
     }
   }

   if(ListBox3->Items->Count != 0)
   {
     GuidPtr.Data1=StrToInt64(ListBox3->Items->Strings[0].Delete(1,ListBox3->Items->Strings[0].Pos("=")));
     GuidPtr.Data2=StrToInt64(ListBox3->Items->Strings[1].Delete(1,ListBox3->Items->Strings[1].Pos("=")));
     GuidPtr.Data3=StrToInt64(ListBox3->Items->Strings[2].Delete(1,ListBox3->Items->Strings[2].Pos("=")));
     GuidPtr.Data4[0]=StrToInt(ListBox3->Items->Strings[3].Delete(1,ListBox3->Items->Strings[3].Pos("=")));
     GuidPtr.Data4[1]=StrToInt(ListBox3->Items->Strings[4].Delete(1,ListBox3->Items->Strings[4].Pos("=")));
     GuidPtr.Data4[2]=StrToInt(ListBox3->Items->Strings[5].Delete(1,ListBox3->Items->Strings[5].Pos("=")));
     GuidPtr.Data4[3]=StrToInt(ListBox3->Items->Strings[6].Delete(1,ListBox3->Items->Strings[6].Pos("=")));
     GuidPtr.Data4[4]=StrToInt(ListBox3->Items->Strings[7].Delete(1,ListBox3->Items->Strings[7].Pos("=")));
     GuidPtr.Data4[5]=StrToInt(ListBox3->Items->Strings[8].Delete(1,ListBox3->Items->Strings[8].Pos("=")));
     GuidPtr.Data4[6]=StrToInt(ListBox3->Items->Strings[9].Delete(1,ListBox3->Items->Strings[9].Pos("=")));
     GuidPtr.Data4[7]=StrToInt(ListBox3->Items->Strings[10].Delete(1,ListBox3->Items->Strings[10].Pos("=")));
   }
   delete ini;

   return GuidPtr;
}
//---------------------------------------------------------------------------
*/


void __fastcall TForm1::GetDeviceInfoClick(TObject *Sender)
{
	ListBox1->Items->Clear();
	for(int i=0; i < 19; i++) //�̦h20�Ӹ˸m
	{
		AnsiString tempN = USB_Device_Handle_Array[i].USBName;
		tempN = tempN.LowerCase();
		if(tempN.Pos("realtek usb gbe family controller"))
		{
			ListBox1->Items->Add("////////////////////////////////////////////////////////////");
			ListBox1->Items->Add("////////////////////////////////////////////////////////////");
			ListBox1->Items->Add(USB_Device_Handle_Array[i].USBName);
			ListBox1->Items->Add(USB_Device_Handle_Array[i].devicePath);

			GetUSBDeviceAlong(USB_Device_Handle_Array[i].hWnd, USB_Device_Handle_Array[i].port);
		}
		//CloseHandle(USB_Device_Handle_Array[i].hWnd);
	}

	GetAmount->Click();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::clearClick(TObject *Sender)
{
	ListBox1->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GetAmountClick(TObject *Sender)
{
	/*Memo1->Clear();
	int U2 = 0;
	int U3 = 0;
	for(int i=0;i<ListBox1->Count;i++){
		AnsiString tempS = ListBox1->Items->Strings[i];
		if(tempS.Pos("wMaxPacketSize :0x0200(0200)")){
			U2++;
		}
		if(tempS.Pos("wMaxPacketSize :0x0400(0400)")){
			U3++;
		}
	}
	AnsiString TempMsg = "U2 �˸m = "+IntToStr(U2/2);
	Memo1->Lines->Add(TempMsg);
	TempMsg = "U3 �˸m = "+IntToStr(U3/2);
	Memo1->Lines->Add(TempMsg);
	DeviceU3Amount = U3/2;
	DeviceU2Amount = U2/2;
	if(DeviceU3Amount!=0)
		PORTTESTUI->LANU3GET = true;
	else
		PORTTESTUI->LANU3GET = false;
	//ModalResult = DeviceU3Amount;
	CloseHandle(ConnectedHubHandle);
	Form1->Close();*/
	int U2 = 0;
	int U3 = 0;
	for(int i=0;i<ListBox1->Count;i++){
		AnsiString tempS = ListBox1->Items->Strings[i];
		tempS = tempS.LowerCase();
		if(tempS.Pos("asix ax88179 usb 3.0 to gigabit ethernet adapter")){
			tempS = ListBox1->Items->Strings[i+16];
			if(tempS.Pos("wMaxPacketSize :0x0400(0400)"))
				frmMain->LANU3GET = true;
			else
				frmMain->LANU3GET = false;
		}
	}
	CloseHandle(ConnectedHubHandle);
	Form1->Close();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormShow(TObject *Sender)
{
	Form1->Top = 15000;
	Form1->Caption="Display USB  Version 1.08";

  //Get USB when the form created
	cmdGetUSBClick(Sender);
  //Form1->Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Action = caFree;
}
//---------------------------------------------------------------------------

object frmMain: TfrmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'PingTool ver1.0.0'
  ClientHeight = 222
  ClientWidth = 582
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 237
    Top = 112
    Width = 341
    Height = 0
    Caption = #26412#27425#29138#20837#20839#23481
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    object new_mac: TLabel
      Left = 38
      Top = 37
      Width = 60
      Height = 33
      Caption = 'MAC'
    end
  end
  object WRITE: TButton
    Left = 8
    Top = 366
    Width = 75
    Height = 25
    Caption = 'WRITE'
    TabOrder = 1
    OnClick = WRITEClick
  end
  object Memo2: TMemo
    Left = 196
    Top = 389
    Width = 326
    Height = 131
    Lines.Strings = (
      'Memo2')
    ScrollBars = ssVertical
    TabOrder = 2
    OnKeyPress = Memo2KeyPress
  end
  object READ: TButton
    Left = 8
    Top = 397
    Width = 75
    Height = 25
    Caption = 'READ'
    TabOrder = 3
    OnClick = READClick
  end
  object LoopCheck: TCheckBox
    Left = 93
    Top = 401
    Width = 97
    Height = 17
    Caption = 'LoopCheck'
    TabOrder = 4
  end
  object CHECK: TButton
    Left = 8
    Top = 335
    Width = 75
    Height = 25
    Caption = 'CHECK'
    TabOrder = 5
    OnClick = CHECKClick
  end
  object Edit1: TEdit
    Left = 224
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 6
  end
  object Edit2: TEdit
    Left = 264
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 7
  end
  object Edit3: TEdit
    Left = 304
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 8
  end
  object Edit4: TEdit
    Left = 360
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 9
  end
  object Edit5: TEdit
    Left = 400
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 10
  end
  object Edit6: TEdit
    Left = 440
    Top = 335
    Width = 34
    Height = 21
    TabOrder = 11
  end
  object Edit7: TEdit
    Left = 360
    Top = 362
    Width = 34
    Height = 21
    TabOrder = 12
    Text = '00'
  end
  object Edit8: TEdit
    Left = 400
    Top = 362
    Width = 34
    Height = 21
    TabOrder = 13
    Text = '00'
  end
  object Edit9: TEdit
    Left = 440
    Top = 362
    Width = 34
    Height = 21
    TabOrder = 14
    Text = '01'
  end
  object MACP: TButton
    Left = 8
    Top = 428
    Width = 75
    Height = 25
    Caption = 'MAC ++'
    TabOrder = 15
    OnClick = MACPClick
  end
  object LOAD: TButton
    Left = 97
    Top = 333
    Width = 75
    Height = 25
    Caption = 'LOAD'
    TabOrder = 16
    OnClick = LOADClick
  end
  object SAVE: TButton
    Left = 97
    Top = 364
    Width = 75
    Height = 25
    Caption = 'SAVE'
    TabOrder = 17
    OnClick = SAVEClick
  end
  object Panel_mac: TPanel
    Left = 81
    Top = 8
    Width = 150
    Height = 1
    BevelKind = bkSoft
    BevelOuter = bvLowered
    BevelWidth = 5
    BiDiMode = bdRightToLeftNoAlign
    Caption = #29138#20837
    Color = clWhite
    Ctl3D = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -29
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentBiDiMode = False
    ParentBackground = False
    ParentCtl3D = False
    ParentFont = False
    TabOrder = 18
    Visible = False
  end
  object changedevice: TButton
    Left = 8
    Top = 493
    Width = 75
    Height = 25
    Caption = #35373#20633#20999#25563
    TabOrder = 19
    OnClick = changedeviceClick
  end
  object SaveLog: TButton
    Left = 97
    Top = 493
    Width = 75
    Height = 25
    Caption = 'LOG'
    TabOrder = 20
    OnClick = SaveLogClick
  end
  object SpeedTest: TCheckBox
    Left = 93
    Top = 424
    Width = 97
    Height = 17
    Caption = 'SpeedTest'
    TabOrder = 21
  end
  object DoReadXML: TCheckBox
    Left = 93
    Top = 447
    Width = 97
    Height = 17
    Caption = 'DoReadXML'
    TabOrder = 22
  end
  object CHKDEVICE: TButton
    Left = 8
    Top = 459
    Width = 75
    Height = 25
    Caption = 'CHKDEVICE'
    TabOrder = 23
    OnClick = CHKDEVICEClick
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 582
    Height = 225
    Align = alTop
    TabOrder = 24
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 430
      Height = 223
      Align = alClient
      Color = clWhite
      ParentBackground = False
      TabOrder = 0
      object Label1: TLabel
        Left = 87
        Top = 3
        Width = 89
        Height = 22
        Caption = #32178#21345#21517#31281':'
        Font.Charset = ANSI_CHARSET
        Font.Color = clWindowText
        Font.Height = -19
        Font.Name = 'Times New Roman'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lab_devicename: TLabel
        Left = 93
        Top = 28
        Width = 5
        Height = 22
        Font.Charset = ANSI_CHARSET
        Font.Color = clBlue
        Font.Height = -19
        Font.Name = 'Times New Roman'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Panel3: TPanel
        Left = 1
        Top = 181
        Width = 428
        Height = 41
        Align = alBottom
        TabOrder = 0
        object pan_1000: TPanel
          Left = 1
          Top = 1
          Width = 144
          Height = 39
          Align = alLeft
          Caption = '1000 M'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
        end
        object pan_100: TPanel
          Left = 145
          Top = 1
          Width = 144
          Height = 39
          Align = alLeft
          Caption = '100 M'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 1
        end
        object pan_10: TPanel
          Left = 289
          Top = 1
          Width = 144
          Height = 39
          Align = alLeft
          Caption = '10 M'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 2
        end
      end
      object Panel4: TPanel
        Left = 1
        Top = 1
        Width = 80
        Height = 180
        Align = alLeft
        BevelOuter = bvNone
        Caption = 'Panel3'
        TabOrder = 1
        object btn_start: TButton
          Left = 0
          Top = 0
          Width = 80
          Height = 153
          Align = alTop
          Caption = #38283#22987
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -29
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
          WordWrap = True
          OnClick = btn_startClick
        end
        object btn_set: TButton
          Left = 0
          Top = 180
          Width = 80
          Height = 0
          Align = alBottom
          Caption = #35373#23450
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -27
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = btn_setClick
        end
        object Panel5: TPanel
          Left = 0
          Top = 153
          Width = 80
          Height = 27
          Align = alClient
          TabOrder = 2
          object ckbNoMAC: TCheckBox
            Left = 6
            Top = 1
            Width = 73
            Height = 25
            Align = alRight
            Caption = 'ZeroMAC'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Times New Roman'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            OnClick = ckbNoMACClick
          end
        end
      end
      object current_mac_group: TGroupBox
        Left = 87
        Top = 55
        Width = 341
        Height = 73
        Caption = #30070#21069#35373#20633#34399
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -27
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 2
        object current_mac: TLabel
          Left = 39
          Top = 37
          Width = 60
          Height = 33
          Caption = 'MAC'
        end
      end
      object GroupBox2: TGroupBox
        Left = 87
        Top = 127
        Width = 339
        Height = 51
        Caption = #28204#35430#26178#38291
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -19
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        TabOrder = 3
        object lab_time: TLabel
          Left = 150
          Top = 21
          Width = 12
          Height = 23
          Caption = '0'
        end
        object Label2: TLabel
          Left = 254
          Top = 21
          Width = 37
          Height = 23
          Caption = 'SEC'
        end
      end
    end
    object Panel_lan: TPanel
      Left = 431
      Top = 1
      Width = 150
      Height = 223
      Align = alRight
      BevelKind = bkSoft
      BevelOuter = bvLowered
      BevelWidth = 5
      BiDiMode = bdRightToLeftNoAlign
      Caption = 'Wait'
      Color = clWhite
      Ctl3D = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -29
      Font.Name = 'Times New Roman'
      Font.Style = [fsBold]
      ParentBiDiMode = False
      ParentBackground = False
      ParentCtl3D = False
      ParentFont = False
      PopupMenu = PopupMenu1
      TabOrder = 1
    end
  end
  object plErrMsg: TPanel
    Left = 0
    Top = 225
    Width = 582
    Height = 41
    Align = alTop
    Caption = 'Test'
    Color = clCream
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -27
    Font.Name = #27161#26999#39636
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 25
  end
  object TestTimeTimer: TTimer
    Interval = 500
    OnTimer = TestTimeTimerTimer
    Left = 384
    Top = 8
  end
  object PopupMenu1: TPopupMenu
    Left = 336
    Top = 8
    object N1: TMenuItem
      Caption = #24489#21407#35387#20874#34920
      OnClick = N1Click
    end
  end
end
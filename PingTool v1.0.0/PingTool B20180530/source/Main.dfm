object frmMain: TfrmMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'PingTool ver1.0.0  B20180530'
  ClientHeight = 223
  ClientWidth = 585
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 585
    Height = 225
    Align = alTop
    TabOrder = 0
    ExplicitWidth = 582
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 433
      Height = 223
      Align = alClient
      Color = clWhite
      ParentBackground = False
      TabOrder = 0
      ExplicitWidth = 430
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
        Width = 431
        Height = 41
        Align = alBottom
        TabOrder = 0
        ExplicitWidth = 428
        object pan_1000: TPanel
          Left = 1
          Top = 1
          Width = 430
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
          Left = 431
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
          Left = 575
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
        object Panel5: TPanel
          Left = 0
          Top = 153
          Width = 80
          Height = 27
          Align = alClient
          TabOrder = 1
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
      Left = 434
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
      ExplicitLeft = 431
    end
  end
  object plErrMsg: TPanel
    Left = 0
    Top = 225
    Width = 585
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
    TabOrder = 1
    ExplicitWidth = 582
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

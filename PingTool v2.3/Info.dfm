object frmInfo: TfrmInfo
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = #35373#23450#36039#35338
  ClientHeight = 199
  ClientWidth = 557
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object btn_set: TButton
    Left = 479
    Top = 8
    Width = 74
    Height = 99
    Caption = #35373#23450
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -29
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = btn_setClick
  end
  object GroupBox1: TGroupBox
    Left = 4
    Top = 113
    Width = 397
    Height = 80
    Caption = #32178#36335#28204#35430#36984#38917
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 40
      Width = 102
      Height = 29
      Caption = 'PING'#22320#22336
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -24
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object LAN_Ping: TEdit
      Left = 116
      Top = 40
      Width = 274
      Height = 33
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
  end
  object GroupBox2: TGroupBox
    Left = 319
    Top = 0
    Width = 154
    Height = 121
    Caption = #28204#35430#36984#38917
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    object mac_check: TCheckBox
      Left = 16
      Top = 43
      Width = 153
      Height = 29
      Caption = 'MAC'#29138#20837
      TabOrder = 0
    end
    object lan_check: TCheckBox
      Left = 16
      Top = 78
      Width = 153
      Height = 29
      Caption = #32178#36335#28204#35430
      TabOrder = 1
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 8
    Width = 305
    Height = 99
    Caption = 'MAC'#29138#20837#36215#22987#35373#23450
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -27
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    object macset_edit: TMaskEdit
      Left = 16
      Top = 40
      Width = 278
      Height = 41
      EditMask = '!>AA:AA:AA:AA:AA:AA;1;_'
      MaxLength = 17
      TabOrder = 0
      Text = '  :  :  :  :  :  '
    end
  end
  object autoRun_check: TCheckBox
    Left = 451
    Top = 135
    Width = 98
    Height = 34
    Caption = #33258#21205#28204#35430
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
  end
end
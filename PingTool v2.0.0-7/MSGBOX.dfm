object frmMsg: TfrmMsg
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'frmMsg'
  ClientHeight = 134
  ClientWidth = 457
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Panel8: TPanel
    Left = 0
    Top = 0
    Width = 457
    Height = 48
    Align = alTop
    Caption = #25475#25551'Barcode(scan barcode)'
    Color = clTeal
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -24
    Font.Name = #27161#26999#39636
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    TabOrder = 0
  end
  object edtBarcode: TEdit
    Left = 0
    Top = 48
    Width = 457
    Height = 45
    Align = alTop
    Alignment = taCenter
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -33
    Font.Name = 'Times New Roman'
    Font.Style = []
    ImeMode = imSAlpha
    ParentFont = False
    TabOrder = 1
    Text = 'FFFFFFFFFFFF'
    OnChange = edtBarcodeChange
    OnKeyDown = edtBarcodeKeyDown
  end
  object Panel1: TPanel
    Left = 0
    Top = 93
    Width = 457
    Height = 41
    Align = alTop
    Caption = 'Panel1'
    TabOrder = 2
    object BitBtn1: TBitBtn
      Left = 1
      Top = 1
      Width = 224
      Height = 39
      Align = alLeft
      Caption = #28165#38500' Clear'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = #27161#26999#39636
      Font.Style = []
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      OnClick = BitBtn1Click
    end
    object BitBtn2: TBitBtn
      Left = 225
      Top = 1
      Width = 231
      Height = 39
      Align = alClient
      Caption = #21462#28040' Cancel'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = #27161#26999#39636
      Font.Style = []
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 1
      OnClick = BitBtn2Click
    end
  end
end

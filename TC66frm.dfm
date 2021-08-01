object TC66F: TTC66F
  Left = 250
  Top = 147
  Width = 1072
  Height = 523
  Anchors = [akLeft]
  Caption = 'TC66'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  PopupMenu = PMCtl
  Scaled = False
  OnCanResize = FormCanResize
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnKeyDown = FormKeyDown
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object MTest: TMemo
    Left = 0
    Top = 400
    Width = 1064
    Height = 90
    TabStop = False
    Align = alBottom
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Courier'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object Chart: TChart
    Left = 0
    Top = 80
    Width = 1064
    Height = 320
    AnimatedZoom = True
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    MarginBottom = 0
    MarginLeft = 1
    MarginRight = 1
    MarginTop = 0
    Title.Text.Strings = (
      '')
    OnClickAxis = ChartClickAxis
    OnClickSeries = ChartClickSeries
    BottomAxis.Title.Caption = 'Time, s'
    LeftAxis.AxisValuesFormat = '#,##0.######'
    LeftAxis.Title.Caption = 'Voltage, V'
    Legend.HorizMargin = 1
    Legend.ShadowSize = 1
    RightAxis.AxisValuesFormat = '#,##0.######'
    RightAxis.Title.Caption = 'Current, A'
    View3D = False
    Align = alBottom
    Color = clWhite
    ParentShowHint = False
    PopupMenu = PMChart
    ShowHint = True
    TabOrder = 0
    TabStop = True
    Anchors = [akLeft, akTop, akRight, akBottom]
    OnMouseWheelDown = ChartMouseWheelDown
    OnMouseWheelUp = ChartMouseWheelUp
    object Voltage: TFastLineSeries
      Marks.ArrowLength = 8
      Marks.Style = smsValue
      Marks.Visible = False
      SeriesColor = clRed
      Title = 'Voltage'
      LinePen.Color = clRed
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
    object Current: TFastLineSeries
      Marks.ArrowLength = 8
      Marks.Visible = False
      SeriesColor = clGreen
      Title = 'Current'
      VertAxis = aRightAxis
      LinePen.Color = clGreen
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1
      YValues.Order = loNone
    end
  end
  object PControl: TPanel
    Left = 0
    Top = 0
    Width = 1064
    Height = 80
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    DesignSize = (
      1064
      80)
    object Lms: TLabel
      Left = 202
      Top = 8
      Width = 18
      Height = 16
      Caption = 'ms'
    end
    object LPort: TLabel
      Left = 14
      Top = 8
      Width = 27
      Height = 16
      Caption = 'Port:'
    end
    object BData: TBevel
      Left = 2
      Top = 32
      Width = 620
      Height = 46
      Shape = bsFrame
    end
    object ind_rx: TShape
      Left = 274
      Top = 8
      Width = 33
      Height = 16
      Hint = 'Blink if receive'
      HelpContext = 1000
      Brush.Color = clBlack
      Pen.Width = 2
      Shape = stCircle
    end
    object Bini: TButton
      Left = 1000
      Top = 4
      Width = 41
      Height = 25
      Hint = 'Edit .ini file'
      Anchors = [akTop, akRight]
      Caption = '.ini'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = BiniClick
    end
    object BStart: TButton
      Left = 224
      Top = 4
      Width = 44
      Height = 25
      Hint = 'Start/Stop polling'
      Caption = 'Start'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = BStartClick
    end
    object CBCom: TComboBox
      Left = 50
      Top = 4
      Width = 85
      Height = 24
      Hint = 'Select COM port'
      ItemHeight = 16
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnChange = CBComChange
      OnDropDown = CBComDropDown
    end
    object EdDelay: TEdit
      Left = 144
      Top = 4
      Width = 53
      Height = 24
      Hint = 'Polling period, ms'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnEnter = EdDelayEnter
    end
    object STAh0: TStaticText
      Left = 200
      Top = 36
      Width = 42
      Height = 20
      Hint = 'Measured charge 0'
      Caption = '.. Ah0'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
    end
    object STCurrent: TStaticText
      Left = 12
      Top = 56
      Width = 26
      Height = 20
      Hint = 'Measured current'
      Caption = '.. A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
    end
    object STPower: TStaticText
      Left = 100
      Top = 36
      Width = 30
      Height = 20
      Hint = 'Measured power'
      Caption = '.. W'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 6
    end
    object STRes: TStaticText
      Left = 100
      Top = 56
      Width = 47
      Height = 20
      Hint = 'Measured resistance'
      Caption = '.. Ohm'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
    end
    object STVoltage: TStaticText
      Left = 12
      Top = 36
      Width = 26
      Height = 20
      Hint = 'Measured voltage'
      Caption = '.. V'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 8
    end
    object STWh0: TStaticText
      Left = 200
      Top = 56
      Width = 46
      Height = 20
      Hint = 'Measured energy 0'
      Caption = '.. Wh0'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 9
    end
    object STLocalPower: TStaticText
      Left = 400
      Top = 36
      Width = 30
      Height = 20
      Hint = 'Calculated power'
      Caption = '.. W'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 10
    end
    object STLocalAh: TStaticText
      Left = 500
      Top = 36
      Width = 34
      Height = 20
      Hint = 'Calculated charge'
      Caption = '.. Ah'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 11
    end
    object STLocalWh: TStaticText
      Left = 500
      Top = 56
      Width = 38
      Height = 20
      Hint = 'Calculated energy'
      Caption = '.. Wh'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 12
    end
    object STWh1: TStaticText
      Left = 300
      Top = 56
      Width = 46
      Height = 20
      Hint = 'Measured energy 1'
      Caption = '.. Wh1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 13
    end
    object STAh1: TStaticText
      Left = 300
      Top = 36
      Width = 42
      Height = 20
      Hint = 'Measured charge 1'
      Caption = '.. Ah1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 14
    end
    object STTmp: TStaticText
      Left = 400
      Top = 56
      Width = 31
      Height = 20
      Hint = 'Measured temperature'
      Caption = '.. '#176'C'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 15
    end
  end
  object Timer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = TimerTimer
    Left = 280
  end
  object UTimer: TTimer
    Interval = 20
    OnTimer = UTimerTimer
    Left = 440
  end
  object PMChart: TPopupMenu
    Left = 312
    object Pan: TMenuItem
      Caption = 'Pan'
      OnClick = PanClick
    end
    object CopytoBMP: TMenuItem
      Caption = 'Copy to BMP'
      OnClick = CopytoBMPClick
    end
    object CopytoWMF: TMenuItem
      Caption = 'Copy to WMF'
      OnClick = CopytoWMFClick
    end
    object Clear: TMenuItem
      Caption = 'Clear'
      OnClick = ClearClick
    end
    object Save: TMenuItem
      Caption = 'Save as'
      ShortCut = 16467
      OnClick = SaveClick
    end
    object Loadcsv: TMenuItem
      Caption = 'Load .csv'
      ShortCut = 16463
      OnClick = LoadcsvClick
    end
    object Appendcsv: TMenuItem
      Caption = 'Append .csv'
      ShortCut = 16449
      OnClick = AppendcsvClick
    end
  end
  object OpenDialog: TOpenDialog
    Left = 376
  end
  object SaveDialog: TSaveDialog
    Left = 344
  end
  object PMCtl: TPopupMenu
    Left = 408
    object Rotatescreen: TMenuItem
      Caption = 'Rotate screen'
      OnClick = RotatescreenClick
    end
    object Nextpage: TMenuItem
      Caption = 'Next page'
      OnClick = NextpageClick
    end
    object Previouspage: TMenuItem
      Caption = 'Previous page'
      OnClick = PreviouspageClick
    end
  end
end

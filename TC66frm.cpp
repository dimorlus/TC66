//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vcl.h>
#include <IniFiles.hpp>
#pragma hdrstop

#include "sys_utils.h"
#include "StrUtils.h"
#include "strnum.h"
#include "Serial.h"
#include "ports.h"
#include "TC66data.h"
#include "TC66frm.h"
//---------------------------------------------------------------------------
#pragma warn -8004
#pragma package(smart_init)
#pragma link "PERFGRAP"
#pragma resource "*.dfm"
TTC66F *TC66F;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//Forward declarations
void AES_decrypt(const uint8_t* key, uint8_t* data, uint32_t size);
void CALLBACK TimerProc(unsigned int uID, unsigned int uMsg, DWORD dwUser,
    DWORD dw1, DWORD dw2);
//---------------------------------------------------------------------------
void cb_fn(void *owner, const char *str)
{
 ((TTC66F *)owner)->CBCom->Items->Add(str);
 Application->ProcessMessages();
 Sleep(0);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TTC66F::TTC66F(TComponent* Owner)
        : TForm(Owner)
{
 TimerID = 0;
 MMTimer = false;
 AbortFlag = false;
 AppendTime = 0;
 TC66localCounter.W = 0;
 TC66localCounter.As = 0;
 TC66localCounter.Ws = 0;
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::FormCreate(TObject *Sender)
{
//  char str[128];

//  dt2str(str,100.001, 0, 0);

  CBCom->Text = CBCom->Items->Strings[0];
  TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
  CBCom->Text = ini->ReadString("UART", "PORT", "COM1");
  EdDelay->Text = ini->ReadInteger("UART", "DELAY", 1000);
  PortDescription = ini->ReadBool("UART", "DESCRIPTION", 1);
  LogName = ini->ReadString("FILES", "LOG", "");
  if (ExtractFilePath(LogName) == "")
   LogName = ExtractFilePath(Application->ExeName)+ LogName;
  ls = ini->ReadString("FILES", "LS", ListSeparator)[1];
  LastName = ini->ReadString("FILES", "LASTNAME", "");
  kCurr = ini->ReadFloat("TC66", "KCURR", 1.0);
  kVolt = ini->ReadFloat("TC66", "KVOLT", 1.0);
  Imin = ini->ReadFloat("RECORD", "IMIN", 0.0);
  Timin = ini->ReadFloat("RECORD", "TIMIN", 60.0);
  Blink = ini->ReadBool("INTERFACE", "BLINK", 1);
  Memo = ini->ReadBool("INTERFACE", "MEMO", 1);
  delete ini;
  if (PortDescription) GetPortListUsingSetupAPI(this, cb_fn);
  else GetPortList(this, cb_fn);

  lastms = GetTickCount();
  CBComChange(Sender);
  ClearPort(Port);
  Timer->Interval = atoi(EdDelay->Text.c_str());
  if (Timer->Interval < 100) Timer->Interval = 100;
  lprintf(MTest->Lines,"Polling period %dms\n", Timer->Interval);
  PControl->Color = clWhite;
  Color = clWhite;
  ind_rx->Visible = Blink;
  Application->HintPause=500;
  Application->HintShortPause=1500;

  //TC66F->PixelsPerInch = 96;
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::LoadCal(int sn)
{
  AnsiString TC66 = asprintf("TC66_%d", sn);
  TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
  kCurr = ini->ReadFloat(TC66, "KCURR", 1.0);
  kVolt = ini->ReadFloat(TC66, "KVOLT", 1.0);
  delete ini;
}

//---------------------------------------------------------------------------
void __fastcall TTC66F::FormClose(TObject *Sender, TCloseAction &Action)
{
 if (TimerID) KillMMTimer(TimerID);
 TIniFile *ini = new TIniFile(ChangeFileExt(Application->ExeName, ".ini"));
 if (Port != INVALID_HANDLE_VALUE)
  {
   ini->WriteString("UART", "PORT", CBCom->Text);
   lprintf(MTest->Lines, "Close COM port\n");
   CloseComPort(Port);
  }
 ini->WriteBool("UART", "DESCRIPTION", PortDescription);
 ini->WriteInteger("UART", "DELAY", atoi(EdDelay->Text.c_str()));
 ini->WriteString("FILES", "LS", ls);
 ini->WriteString("FILES", "LASTNAME", LastName);
 ini->WriteBool("INTERFACE", "BLINK", Blink);
 ini->WriteBool("INTERFACE", "MEMO", Memo);
 delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::TimerTimer(TObject *Sender)
{
 char cmd[] = "getva";
 if (Blink) ind_rx->Brush->Color = clYellow;
 PutChars(Port, cmd, strlen(cmd));
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::CBComChange(TObject *Sender)
{
  int port_num;
  CloseComPort(Port);
  sscanf(CBCom->Text.c_str(), "COM%d", &port_num);
  int baud = 115200;
  Port = OpenComPort(port_num, baud, 8, 'N', 1);
  if (Port != INVALID_HANDLE_VALUE)
   {
    lprintf(MTest->Lines,"Open COM%d@%d, 8, N, 1 Ok\n", port_num, baud);
    Start();
   }
  else
   {
    lprintf(MTest->Lines,"Invalid COM%d\n", port_num);
    Stop();
   }
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::CBComDropDown(TObject *Sender)
{
 CBCom->Items->Clear();
 if (PortDescription) GetPortListUsingSetupAPI(this, cb_fn);
 else GetPortList(this, cb_fn);
 CBCom->Text = CBCom->Items->Strings[0];
}
//---------------------------------------------------------------------------
// multimedia timer wrapper functions
//---------------------------------------------------------------------------
int __fastcall TTC66F::GetResolution(int target)
{
 TIMECAPS tc;
 unsigned int uiTimerRes;
 int tmp;

 timeGetDevCaps(&tc, sizeof(TIMECAPS));

 if ((int)tc.wPeriodMin > target) tmp = (int)tc.wPeriodMin;
 else tmp = target;

 if (tmp < (int)tc.wPeriodMax) uiTimerRes = tmp;
 else uiTimerRes = (int)tc.wPeriodMax;

 //uiTimerRes = min(max((int)tc.wPeriodMin, target), (int)tc.wPeriodMax);
 timeBeginPeriod(uiTimerRes);
 return uiTimerRes;
}
//---------------------------------------------------------------------------
int __fastcall TTC66F::StartMMTimer(int Interval, int Resolution, DWORD data)
{
 return timeSetEvent(Interval, Resolution, TimerProc, data, TIME_PERIODIC);
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::KillMMTimer(int FTimerID)
{
 timeKillEvent(FTimerID);
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::StartMMT(int Interval)
 {
  if (!TimerID)
   {
    timeBeginPeriod(1);
    int Resolution = GetResolution(1);
    TimerID = StartMMTimer(Interval, Resolution, 0);
    MMTimer = true;
   }
 }
//---------------------------------------------------------------------------
void __fastcall TTC66F::StopMMT(void)
{
  if (TimerID)
   {
    timeKillEvent(TimerID);
    timeEndPeriod(1);
    TimerID = 0;
    MMTimer = false;
   }
}
//---------------------------------------------------------------------------
void CALLBACK TimerProc(unsigned int uID, unsigned int uMsg, DWORD dwUser,
    DWORD dw1, DWORD dw2)
{
 TC66F->Counter++;
 if (TC66F->TimerID)
  {
//   TC66F->TimerHandle();
//   Sleep(1);
  }
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::Log(TTC66res *TC66res)
{
 if (ExtractFileName(LogName) != "")
  {
   bool Title = (!FileExists(LogName));
   FILE * LogFile = fopen(LogName.c_str(), "at");
   if (LogFile)
    {
     if (Title)
      fprintf(LogFile, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s\n",
       "Time, s", ls,
       "Volts, V", ls,
       "Amps, A", ls,
       "Ah0", ls,
       "Wh0", ls,
       "Ah1", ls,
       "Wh1", ls,
       "W", ls,
       "Tc"
       );
     fprintf(LogFile, "%0.3f%c%2.5f%c%1.6f%c%0.4f%c%0.4f%c%0.4f%c%0.4f%c%0.4f%c%0.1f\n",
      TC66res->t, ls,
      TC66res->V, ls,
      TC66res->I, ls,
      TC66res->Ah0, ls,
      TC66res->Wh0, ls,
      TC66res->Ah1, ls,
      TC66res->Wh1, ls,
      TC66res->W, ls,
      TC66res->Tc
      );
    }
   fclose(LogFile);
  }
}

//---------------------------------------------------------------------------
#pragma warn -8075
AnsiString ad2scistrup(double d, const char* units, int prec, int adj)
{
 char str[64];
 d2scistrup(str,d, units, prec, adj);
 return str;
}
//---------------------------------------------------------------------------

AnsiString at2str(double d, int adj)
{
 char str[64];
 dt2str(str, d, adj, false);
 return str;
}
//---------------------------------------------------------------------------
// Compute the MODBUS RTU CRC
uint16_t ModRTU_CRC(const uint8_t *buf, int len)
{
  uint16_t crc = 0xFFFF;

  for (int pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];          // XOR byte into least sig. byte of crc

    for (int i = 8; i != 0; i--)      // Loop over each bit
     {
      if ((crc & 0x0001) != 0)        // If the LSB is set
       {
        crc >>= 1;                    // Shift right and XOR 0xA001
        crc ^= 0xA001;
       }
      else                            // Else LSB is not set
        crc >>= 1;                    // Just shift right
    }
  }
  // Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
  return crc;
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::TimerHandle(void)
{
 static int ms1 = 0;
 uint32_t currms;
 tTC66Data TC66Data;
 int i = 0;
 if (Avail(Port) >= DATASIZE)
  {
   while(Avail(Port))
    {
     int b = GetChar(Port);
     if (b < 0) break;
     else
     if (i < DATASIZE) TC66Data.raw[i++] = b&0xff;
    }
   if (i)
    {
     const uint8_t key[] =
     {0x58, 0x21, 0xfa, 0x56, 0x01, 0xb2, 0xf0, 0x26,
      0x87, 0xff, 0x12, 0x04, 0x62, 0x2a, 0x4f, 0xb0,
      0x86, 0xf4, 0x02, 0x60, 0x81, 0x6f, 0x9a, 0x0b,
      0xa7, 0xf1, 0x06, 0x61, 0x9a, 0xb8, 0x72, 0x88};

     currms = GetTickCount();
     AES_decrypt(key, TC66Data.raw, DATASIZE);
     if ((*(uint32_t*)&TC66Data.pac1 == 0x31636170) && //"pac1"
         (*(uint32_t*)&TC66Data.pac2 == 0x32636170) && //"pac2"
         (*(uint32_t*)&TC66Data.pac3 == 0x33636170) && //"pac3"
         (*(uint32_t*)&TC66Data.model == 0x36364354) &&  //"TC66"
         (TC66Data.chk1 == (uint32_t)ModRTU_CRC(&TC66Data.raw[0], 60)) &&
         (TC66Data.chk2 == (uint32_t)ModRTU_CRC(&TC66Data.raw[64], 60)) &&
         (TC66Data.chk3 == (uint32_t)ModRTU_CRC(&TC66Data.raw[128], 60)))
       {
        if (Caption == "TC66")
         {
          Caption = asprintf("%c%c%c%c v%c%c%c%c sn: %d",
           TC66Data.model[0],  TC66Data.model[1],TC66Data.model[2],TC66Data.model[3],
           TC66Data.ver[0], TC66Data.ver[1], TC66Data.ver[2], TC66Data.ver[3],
           TC66Data.sn);
           LoadCal(TC66Data.sn);
         }

        TC66res.V = kVolt*TC66Data.Volts/10000.0;
        TC66res.I = kCurr*TC66Data.Amps/100000.0;
        TC66res.W = kVolt*kCurr*TC66Data.Watts/10000.0;
        TC66res.R = kVolt*(TC66Data.Ohms/10.0)/kCurr;
        TC66res.Ah0 = kCurr*TC66Data.mAh0/1000.0;
        TC66res.Wh0 = kVolt*kCurr*TC66Data.mWh0/1000.0;
        TC66res.Ah1 = kCurr*TC66Data.mAh1/1000.0;
        TC66res.Wh1 = kVolt*kCurr*TC66Data.mWh1/1000.0;
        TC66res.Tc = TC66Data.Tmp*(TC66Data.Tsign?-1.0:1.0);

        double dt = (currms-lastms)/1000.0;
        lastms = currms;

        if (TC66res.I < Imin) Timeout += dt;
        else Timeout = 0;

        TC66localCounter.W = TC66res.V*TC66res.I;
        TC66localCounter.As += TC66res.I*dt;
        TC66localCounter.Ws += TC66localCounter.W*dt;

        STVoltage->Caption = ad2scistrup(TC66res.V, "V", 4, 0);
        STCurrent->Caption = ad2scistrup(TC66res.I, "A", 4, 0);
        STPower->Caption = ad2scistrup(TC66res.W, "W", 4, 0);
        STRes->Caption = ad2scistrup(TC66res.R, "Ohm", 3, 0);
        STAh0->Caption = ad2scistrup(TC66res.Ah0, "Ah0", 4, 0);
        STWh0->Caption = ad2scistrup(TC66res.Wh0, "Wh0", 4, 0);
        STAh1->Caption = ad2scistrup(TC66res.Ah1, "Ah1", 4, 0);
        STWh1->Caption = ad2scistrup(TC66res.Wh1, "Wh1", 4, 0);
        STTmp->Caption = ad2scistrup(TC66res.Tc, "°C", 3, 0);
        STLocalPower->Caption = ad2scistrup(TC66localCounter.W, "W", 3, 0);
        STLocalAh->Caption = ad2scistrup(TC66localCounter.As/3600, "Ah", 3, 0);
        STLocalWh->Caption = ad2scistrup(TC66localCounter.Ws/3600, "Wh", 3, 0);

        if (Timeout < Timin)
         {
          TC66res.t += dt;
          Chart->Series[0]->AddXY(TC66res.t, TC66res.V);
          Chart->Series[1]->AddXY(TC66res.t, TC66res.I);
         }
        Log(&TC66res);

        BStart->Enabled = true;

        int ms2 = GetTickCount();
        if ((ms2-ms1) > 990)
         {
          STTime->Caption = at2str(TC66res.t, 0);
          if (Memo) lprintf(MTest->Lines,"%0.2fs %2.5fV %1.6fA\n",
                            TC66res.t, TC66res.V, TC66res.I);
          ms1 = ms2;
         }
        if (Blink) ind_rx->Brush->Color = clBlack;
       }

    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::Stop(void)
 {
   if (Timer->Enabled)
    {
     BStart->Caption = "Start";
     Timer->Enabled = false;
     UTimer->Enabled = false;
     lprintf(MTest->Lines,"Stop pooling at %fs\n", TC66res.t);
    }
 }
//---------------------------------------------------------------------------

void __fastcall TTC66F::Start(void)
 {
   if (!Timer->Enabled)
    {
     BStart->Caption = "Stop";
     Timer->Interval = atoi(EdDelay->Text.c_str());
     if (Timer->Interval < 100) Timer->Interval = 100;
     lprintf(MTest->Lines,"Polling period %dms\n", Timer->Interval);
     lprintf(MTest->Lines,"Start pooling from %fs\n", TC66res.t);
     lastms = GetTickCount();
     UTimer->Enabled = true;
     Timer->Enabled = true;
    }
 }
//---------------------------------------------------------------------------

void __fastcall TTC66F::BStartClick(TObject *Sender)
{
// if (BStart->Enabled)
  {
   if (Timer->Enabled) Stop();
   else Start();
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::UTimerTimer(TObject *Sender)
{
 TimerHandle();
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::BiniClick(TObject *Sender)
{
 AnsiString IniFile = ChangeFileExt(Application->ExeName, ".ini");
 AnsiString AStr = "notepad.exe \"" + IniFile + "\"";
 int res = WinExec(AStr.c_str(), SW_SHOWNORMAL);
 if (res < 32) MessageDlg("Error on attempt to execute " + IniFile,
                             mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::EdDelayEnter(TObject *Sender)
{
  Timer->Interval = atoi(EdDelay->Text.c_str());
  if (Timer->Interval < 100) Timer->Interval = 100;
  lprintf(MTest->Lines,"Polling period %dms\n", Timer->Interval);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::FormCloseQuery(TObject *Sender, bool &CanClose)
{
 lprintf(MTest->Lines,"Close\n");
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
//ssShift
//ssCtrl
//ssAlt
/*
VK_LBUTTON	Left mouse button
VK_RBUTTON	Right mouse button
VK_CANCEL	Control+Break
VK_MBUTTON	Middle mouse button
VK_BACK	        Backspace key
VK_TAB	        Tab key
VK_CLEAR	Clear key
VK_RETURN	Enter key
VK_SHIFT	Shift key
VK_CONTROL	Ctrl key
VK_MENU 	Alt key
VK_PAUSE	Pause key
VK_CAPITAL	Caps Lock key
VK_KANA	        Used with IME
VK_HANGUL	Used with IME
VK_JUNJA	Used with IME
VK_FINAL	Used with IME
VK_HANJA	Used with IME
VK_KANJI	Used with IME
VK_CONVERT	Used with IME

VK_NONCONVERT	Used with IME
VK_ACCEPT	Used with IME
VK_MODECHANGE	Used with IME
VK_ESCAPE	Esc key
VK_SPACE	Space bar
VK_PRIOR	Page Up key
VK_NEXT	        Page Down key
VK_END	        End key
VK_HOME	        Home key
VK_LEFT	        Left Arrow key
VK_UP	        Up Arrow key
VK_RIGHT	Right Arrow key
VK_DOWN	        Down Arrow key
VK_SELECT	Select key
VK_PRINT	Print key (keyboard-specific)
VK_EXECUTE	Execute key
VK_SNAPSHOT	Print Screen key
VK_INSERT	Insert key
VK_DELETE	Delete key
VK_HELP	        Help key

VK_LWIN	        Left Windows key (Microsoft keyboard)
VK_RWIN	        Right Windows key (Microsoft keyboard)
VK_APPS	        Applications key (Microsoft keyboard)

VK_NUMPAD0	0 key (numeric keypad)
..
VK_NUMPAD9	9 key (numeric keypad)

VK_MULTIPLY	Multiply key (numeric keypad)
VK_ADD	        Add key (numeric keypad)
VK_SEPARATOR	Separator key (numeric keypad)
VK_SUBTRACT	Subtract key (numeric keypad)
VK_DECIMAL	Decimal key (numeric keypad)
VK_DIVIDE	Divide key (numeric keypad)

VK_F1	F1 key
..
VK_F24	F24 key

VK_NUMLOCK	Num Lock key
VK_SCROLL	Scroll Lock key
VK_LSHIFT	Left Shift key (only used with GetAsyncKeyState and GetKeyState)
VK_RSHIFT	Right Shift key (only used with GetAsyncKeyState and GetKeyState)
VK_LCONTROL	Left Ctrl key (only used with GetAsyncKeyState and GetKeyState)
VK_RCONTROL	Right Ctrl key (only used with GetAsyncKeyState and GetKeyState)

VK_LMENU	Left Alt key (only used with GetAsyncKeyState and GetKeyState)
VK_RMENU	Right Alt key (only used with GetAsyncKeyState and GetKeyState)
VK_PROCESSKEY	Process key
VK_ATTN	Attn key
VK_CRSEL	CrSel key
VK_EXSEL	ExSel key
VK_EREOF	Erase EOF key
VK_PLAY	Play key
VK_ZOOM	Zoom key
VK_NONAME	Reserved for future use
VK_PA1	PA1 key
VK_OEM_CLEAR	Clear key
*/
 if (Key == VK_ESCAPE)
  {
   AbortFlag = true;
   Key = 0;
  }
 else
 if (Key == VK_LEFT)
  {
   double sb = (Chart->BottomAxis->Maximum-Chart->BottomAxis->Minimum)/100;
   Chart->BottomAxis->Scroll(sb, false);
   Key = 0;
  }
 else
 if (Key == VK_RIGHT)
  {
   double sb = (Chart->BottomAxis->Maximum-Chart->BottomAxis->Minimum)/100;
   Chart->BottomAxis->Scroll(-sb, false);
   Key = 0;
  }
 else
 if (Key == VK_UP)
  {
   double sl = (Chart->LeftAxis->Maximum-Chart->LeftAxis->Minimum)/100;
   double sr = (Chart->RightAxis->Maximum-Chart->RightAxis->Minimum)/100;
   Chart->LeftAxis->Scroll(sl, false);
   Chart->RightAxis->Scroll(sr, false);
   Key = 0;
  }
 else
 if (Key == VK_DOWN)
  {
   double sl = (Chart->LeftAxis->Maximum-Chart->LeftAxis->Minimum)/100;
   double sr = (Chart->RightAxis->Maximum-Chart->RightAxis->Minimum)/100;
   Chart->LeftAxis->Scroll(-sl, false);
   Chart->RightAxis->Scroll(-sr, false);
   Key = 0;
  }
 else
 if (Key == VK_PRIOR)
  {
   Chart->ZoomPercent(101);
   Key = 0;
  }
 else
 if (Key == VK_NEXT)
  {
   Chart->ZoomPercent(99);
   Key = 0;
  }
 else
 if (Key == VK_HOME)
  {
   Chart->UndoZoom();
   Key = 0;
  }
 else
 if (Key == VK_DELETE)
  {
   ClearClick(Sender);
   Key = 0;
  }
 else
 if (Shift.Contains(ssAlt) && (Key == 'C'))
  {
   Key = 0;
   Chart->CopyToClipboardMetafile(true);
  }
 else
 if (Shift.Contains(ssCtrl) && (Key == 'C'))
  {
   Key = 0;
   Chart->CopyToClipboardBitmap();
  }
 else
 if (Shift.Contains(ssCtrl) && (Key == 'X'))
  {
   Key = 0;
   Chart->CopyToClipboardMetafile(true);
   Chart->Series[0]->Clear();
   Chart->Series[1]->Clear();
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::FormResize(TObject *Sender)
{
/*
 Chart->Anchors >> akTop;
 Chart->Anchors >> akBottom;
 Chart->Top = STCurrent->Top + STCurrent->Height+4;
 Chart->Height = MTest->Top - Chart->Top;
 Chart->Repaint();
 Chart->Anchors << akTop;
 Chart->Anchors << akBottom;
*/
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::CopytoBMPClick(TObject *Sender)
{
 Chart->CopyToClipboardBitmap();
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::CopytoWMFClick(TObject *Sender)
{
 Chart->CopyToClipboardMetafile(true);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::ClearClick(TObject *Sender)
{
  lprintf(MTest->Lines,"Clear graph\n");
  Chart->Series[0]->Clear();
  Chart->Series[1]->Clear();
  TC66res.t = 0;
  TC66localCounter.W = 0;
  TC66localCounter.As = 0;
  TC66localCounter.Ws = 0;
}
//---------------------------------------------------------------------------


void __fastcall TTC66F::ChartMouseWheelUp(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
 Chart->ZoomPercent(101);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::ChartMouseWheelDown(TObject *Sender,
      TShiftState Shift, TPoint &MousePos, bool &Handled)
{
 Chart->ZoomPercent(99);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::ChartClickSeries(TCustomChart *Sender,
      TChartSeries *Series, int ValueIndex, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
 double t, I, V;
 int i = 0;
 char str[64];
 str[0] = '\0';
 if (ValueIndex > 0)
  {
   t = Series->XValue[ValueIndex];
   V = Chart->Series[0]->YValue[ValueIndex];
   I = Chart->Series[1]->YValue[ValueIndex];

   i += dt2str(&str[i], t, 0, false);
   i += add(&str[i], '\n');
   i += d2scistrup(&str[i], V, "V", 5, 0);
   i += add(&str[i], '\n');
   i += d2scistrup(&str[i], I, "A", 5, 0);
   i += add(&str[i], '\n');
   i += d2scistrup(&str[i], I*V, "W", 5, 0);
   Chart->Hint = str;
  }
 else Chart->Hint = "";
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::ChartClickAxis(TCustomChart *Sender,
      TChartAxis *Axis, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
 char str[64];
 str[0] = '\0';
 if (Axis == Chart->BottomAxis)
  {
   double t = Axis->CalcPosPoint(X);
   dt2str(str, t, 0, false);
  }
 else
 if (Axis == Chart->LeftAxis)
  {
   double y = Axis->CalcPosPoint(Y);
   d2scistrup(str, y, "V", 5, 0);
  }
 else
 if (Axis == Chart->RightAxis)
  {
   double y = Axis->CalcPosPoint(Y);
   d2scistrup(str, y, "A", 5, 0);
  }
 Chart->Hint = str;
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::PanClick(TObject *Sender)
{
 Chart->UndoZoom();
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::LoadcsvClick(TObject *Sender)
{
 OpenDialog->InitialDir = ExtractFileDir(LastName);
 OpenDialog->Filter = "CSV log(*.csv)|*.csv";
 OpenDialog->FileName = LastName;
 OpenDialog->Title = "Open log file";
 if (OpenDialog->Execute())
  {
   LastName = OpenDialog->FileName;
   Stop();
   AppendTime = 0;
   ClearClick(Sender);
   Load_csv(OpenDialog->FileName);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::AppendcsvClick(TObject *Sender)
{
 OpenDialog->InitialDir = ExtractFileDir(LastName);
 OpenDialog->Filter = "CSV log(*.csv)|*.csv";
 OpenDialog->FileName = LastName;
 OpenDialog->Title = "Open log file";
 if (OpenDialog->Execute())
  {
   LastName = OpenDialog->FileName;
   Stop();
   TC66res.t = AppendTime = Chart->BottomAxis->Maximum;
   Load_csv(OpenDialog->FileName);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::Load_csv(AnsiString fName)
{
 FILE * File;
 // t, V, I, Ah0, Wh0, Ah1, Wh1
 File = fopen(fName.c_str(), "rt");
 if (File)
  {
   TTC66res rec;
   double pt = 0;
   char str[256];
   int ms1 = GetTickCount();
   int ms2;
   int n=0;
   TC66localCounter.W = 0;
   TC66localCounter.As = 0;
   TC66localCounter.Ws = 0;
   AbortFlag = false;
   while (fgets(str, 0x256, File))
    {
     n++;
     char *pstr = str;
     char *endptr;
     for(int i=0; i < 7; i++)
      {
       rec.dim[i] = 0;
       rec.dim[i] = strtod(pstr, &endptr);
       if (endptr && ((endptr[0] == ls)||
                      (endptr[0] == ',')||
                      (endptr[0] == ';')||
                      (endptr[0] == '\n'))) pstr = endptr+1;
      }
     TC66res.t = AppendTime+rec.t;
     Chart->Series[0]->AddXY(TC66res.t, rec.V);
     Chart->Series[1]->AddXY(TC66res.t, rec.I);
     double dt = rec.t - pt;
     pt = rec.t;
     TC66localCounter.W = rec.V*rec.I;
     TC66localCounter.Ws += TC66localCounter.W*dt;
     TC66localCounter.As += rec.I*dt;
     ms2 = GetTickCount();
     if ((ms2 - ms1) > 500)
      {
       ms1 = ms2;
       lprintf(MTest->Lines,"Load %d lines\n", n);
       Application->ProcessMessages();
       if (AbortFlag)
        {
         lprintf(MTest->Lines,"Abort loading\n", n);
         AbortFlag = false;
         break;
        }
      }
    }
   fclose(File);
   lprintf(MTest->Lines,"%d points loaded\n", n);
   STLocalPower->Caption = ad2scistrup(TC66localCounter.W, "W", 3, 0);
   STLocalAh->Caption = ad2scistrup(TC66localCounter.As/3600, "Ah", 3, 0);
   STLocalWh->Caption = ad2scistrup(TC66localCounter.Ws/3600, "Wh", 3, 0);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::Save_csv(AnsiString fName)
{
 char str[256];
 FILE * File;
 File = fopen(fName.c_str(), "wt");
 if (File)
  {
   int n=0;
   int CountV = Chart->Series[0]->Count();
   int CountI = Chart->Series[1]->Count();
   int Count = (CountV < CountI)?CountV:CountI;
   int X1 = Chart->ChartRect.Left;
   int X2 = Chart->ChartRect.Right;
   for(int ValueIndex=0; ValueIndex<Count; ValueIndex++)
    {
     int x = Chart->Series[0]->CalcXPosValue(Chart->Series[0]->XValue[ValueIndex]);
     if ((x >= X1) && (x <= X2))
      {
       double t = Chart->Series[0]->XValue[ValueIndex];
       double V = Chart->Series[0]->YValue[ValueIndex];
       double I = Chart->Series[1]->YValue[ValueIndex];
       fprintf(File, "%0.2f%c%2.5f%c%1.6f\n", t, ls, V, ls, I);
       n++;
       if (n%1000==0)
        {
         lprintf(MTest->Lines,"Save %d lines\n", n);
         Application->ProcessMessages();
         if (AbortFlag)
          {
           lprintf(MTest->Lines,"Abort saving\n", n);
           AbortFlag = false;
           break;
          }
        }
      }
    }
   fclose(File);
   lprintf(MTest->Lines,"%d points saved\n", n);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::SaveClick(TObject *Sender)
{
 SaveDialog->DefaultExt = "bmp";
 SaveDialog->Filter =
  "BMP (*.bmp)|*.bmp|WMF (*.wmf)|*.wmf|CSV (*.csv)|*.csv";
 SaveDialog->InitialDir = ExtractFileDir(LastName);
 SaveDialog->Title = "Save graph";
 SaveDialog->FileName = LastName;
 SaveDialog->FilterIndex = 3;
 if (SaveDialog->Execute())
  {
   LastName = SaveDialog->FileName;
   switch (SaveDialog->FilterIndex)
    {
     case 1: Chart->SaveToBitmapFile(SaveDialog->FileName); break;
     case 2: Chart->SaveToMetafileEnh(SaveDialog->FileName); break;
     case 3: Save_csv(SaveDialog->FileName); break;
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::FormCanResize(TObject *Sender, int &NewWidth,
      int &NewHeight, bool &Resize)
{
 Resize = ((NewWidth > Width) || (PControl->Width > (BData->Left+BData->Width+4)))&&
          ((NewHeight > Height) || (Chart->Height > 150));
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::FormShow(TObject *Sender)
{
 FormResize(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TTC66F::WMChangeDev(TMessage &Message)
{
 CBComChange(this);
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::RotatescreenClick(TObject *Sender)
{
 char cmd[] = "rotat";
 PutChars(Port, cmd, strlen(cmd));
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::NextpageClick(TObject *Sender)
{
 char cmd[] = "nextp";
 PutChars(Port, cmd, strlen(cmd));
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::PreviouspageClick(TObject *Sender)
{
 char cmd[] = "lastp";
 PutChars(Port, cmd, strlen(cmd));
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::SavereportClick(TObject *Sender)
{
 SaveDialog->DefaultExt = "txt";
 SaveDialog->Filter = "TXT (*.txt)|*.txt";
 SaveDialog->InitialDir = ExtractFileDir(LogName);
 SaveDialog->Title = "Save report";
 SaveDialog->FileName = ChangeFileExt(LogName, ".txt");
 SaveDialog->FilterIndex = 1;
 if (SaveDialog->Execute())
  {
   FILE * File = fopen(SaveDialog->FileName.c_str(), "at");
   if (File)
    {
     fprintf(File, "%s\n", Caption.c_str());
     fprintf(File, "%s\n", FormatDateTime("c", Now()).c_str());
     fprintf(File, "Vm=%s\n", STVoltage->Caption.c_str());
     fprintf(File, "Im=%s\n", STCurrent->Caption.c_str());
     fprintf(File, "Pm=%s\n", STPower->Caption.c_str());
     fprintf(File, "Rm=%s\n", STRes->Caption.c_str());
     fprintf(File, "Ah0=%s\n", STAh0->Caption.c_str());
     fprintf(File, "Wh0=%s\n", STWh0->Caption.c_str());
     fprintf(File, "Ah1=%s\n", STAh1->Caption.c_str());
     fprintf(File, "Wh1=%s\n", STWh1->Caption.c_str());
     fprintf(File, "Tc=%s\n", STTmp->Caption.c_str());
     fprintf(File, "Pc=%s\n", STLocalPower->Caption.c_str());
     fprintf(File, "Ahc=%s\n", STLocalAh->Caption.c_str());
     fprintf(File, "Whc=%s\n", STLocalWh->Caption.c_str());
    }
   fclose(File);
  }
}
//---------------------------------------------------------------------------

void __fastcall TTC66F::ChartClickLegend(TCustomChart *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 int i = 0;
 char str[64];
 str[0] = '\0';
 i += dt2str(str, TC66res.t, 0, false);
 i += add(&str[i], '\n');
 i += d2scistrup(&str[i], TC66res.V, "V", 5, 0);
 i += add(&str[i], '\n');
 i += d2scistrup(&str[i], TC66res.I, "A", 5, 0);
 Chart->Hint = str;
}

//---------------------------------------------------------------------------

void __fastcall TTC66F::ChartMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Chart->Hint = "";
}
//---------------------------------------------------------------------------


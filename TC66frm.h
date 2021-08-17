//---------------------------------------------------------------------------

#ifndef TC66frmH
#define TC66frmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "PERFGRAP.h"
#include <Chart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Series.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
#pragma warn -8027
typedef union
{
 struct
  {
   double t, V, I, Ah0, Wh0, Ah1, Wh1, W, R, Tc;
  };
 double dim[10];
} TTC66res;

typedef struct
{
 double W, As, Ws;
} TTC66localCounter;


class TTC66F : public TForm
{
__published:	// IDE-managed Components
        TComboBox *CBCom;
        TLabel *LPort;
        TMemo *MTest;
        TTimer *Timer;
        TTimer *UTimer;
        TButton *BStart;
        TEdit *EdDelay;
        TStaticText *STVoltage;
        TStaticText *STCurrent;
        TStaticText *STPower;
        TStaticText *STRes;
        TStaticText *STAh0;
        TStaticText *STWh0;
        TButton *Bini;
        TChart *Chart;
        TFastLineSeries *Voltage;
        TFastLineSeries *Current;
        TLabel *Lms;
        TPopupMenu *PMChart;
        TMenuItem *CopytoBMP;
        TMenuItem *CopytoWMF;
        TMenuItem *Clear;
        TMenuItem *Pan;
        TOpenDialog *OpenDialog;
        TMenuItem *Loadcsv;
        TSaveDialog *SaveDialog;
        TMenuItem *Save;
        TPanel *PControl;
        TPopupMenu *PMCtl;
        TMenuItem *Rotatescreen;
        TMenuItem *Nextpage;
        TMenuItem *Previouspage;
        TStaticText *STLocalPower;
        TStaticText *STLocalAh;
        TStaticText *STLocalWh;
        TStaticText *STWh1;
        TStaticText *STAh1;
        TMenuItem *Appendcsv;
        TStaticText *STTmp;
        TBevel *BData;
        TShape *ind_rx;
        TMenuItem *Savereport;
        TStaticText *STTime;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall CBComChange(TObject *Sender);
        void __fastcall CBComDropDown(TObject *Sender);
        void __fastcall BStartClick(TObject *Sender);
        void __fastcall UTimerTimer(TObject *Sender);
        void __fastcall BiniClick(TObject *Sender);
        void __fastcall EdDelayEnter(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall CopytoBMPClick(TObject *Sender);
        void __fastcall CopytoWMFClick(TObject *Sender);
        void __fastcall ClearClick(TObject *Sender);
        void __fastcall ChartMouseWheelUp(TObject *Sender,
          TShiftState Shift, TPoint &MousePos, bool &Handled);
        void __fastcall ChartMouseWheelDown(TObject *Sender,
          TShiftState Shift, TPoint &MousePos, bool &Handled);
        void __fastcall ChartClickSeries(TCustomChart *Sender,
          TChartSeries *Series, int ValueIndex, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall PanClick(TObject *Sender);
        void __fastcall LoadcsvClick(TObject *Sender);
        void __fastcall SaveClick(TObject *Sender);
        void __fastcall FormCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ChartClickAxis(TCustomChart *Sender,
          TChartAxis *Axis, TMouseButton Button, TShiftState Shift, int X,
          int Y);
        void __fastcall RotatescreenClick(TObject *Sender);
        void __fastcall NextpageClick(TObject *Sender);
        void __fastcall PreviouspageClick(TObject *Sender);
        void __fastcall AppendcsvClick(TObject *Sender);
        void __fastcall SavereportClick(TObject *Sender);
        void __fastcall ChartClickLegend(TCustomChart *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ChartMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
        void *Port;
        AnsiString LogName;
        AnsiString LastName;

        double kCurr;
        char ls;
        TTC66res TC66res;
        TTC66localCounter TC66localCounter;
        bool AbortFlag;
        double AppendTime;
        uint32_t lastms;
        bool Blink;
        bool Memo;
        bool PortDescription;

        void __fastcall Log(TTC66res *TC66res);
        void __fastcall Load_csv(AnsiString fName);
        void __fastcall Save_csv(AnsiString fName);
        void __fastcall Stop(void);
        void __fastcall Start(void);

public:		// User declarations
        bool MMTimer;
        int TimerID;
        int Counter;

        int __fastcall GetResolution(int target);
        int __fastcall StartMMTimer(int Interval, int Resolution, DWORD data);
        void __fastcall KillMMTimer(int FTimerID);
        void __fastcall StartMMT(int Interval);
        void __fastcall StopMMT(void);

        void __fastcall TimerHandle(void);

        __fastcall TTC66F(TComponent* Owner);
protected:
        void __fastcall WMChangeDev(TMessage &Message);
BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_DEVICECHANGE, TMessage, WMChangeDev)
END_MESSAGE_MAP(TControl)
        
};
//---------------------------------------------------------------------------
extern PACKAGE TTC66F *TC66F;
//---------------------------------------------------------------------------
#endif

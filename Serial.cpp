//---------------------------------------------------------------------------
#include <windows.h>
#include <setupapi.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#pragma hdrstop

#include "Serial.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -8004
#pragma warn -8060

//---------------------------------------------------------------------------

void* OpenComPort(int port, int baud, int bits,
                   char parity, int stopbits)
{
  DCB     dcb;
  HANDLE  hLoc;
  char    pName[] = "\\\\.\\COM\0\0\0\0\0\0\0";

  itoa(port, &pName[7], 10);

  hLoc = CreateFile(
      pName,
      GENERIC_READ | GENERIC_WRITE,
      0,               // comm devices must be opened w/exclusive-access
      NULL,            // no security attributes
      OPEN_EXISTING,   // comm devices must use OPEN_EXISTING
      0,
      NULL             // hTemplate must be NULL for comm devices
  );


  if (hLoc == INVALID_HANDLE_VALUE)
   {
    return hLoc;
   }

  if (!GetCommState(hLoc, &dcb))
   {
    CloseHandle(hLoc);
    return hLoc;
   }

  // Fill in the DCB

  dcb.BaudRate = baud;
  dcb.ByteSize = (BYTE)bits;
  switch(stopbits)
   {
    case 1: dcb.StopBits = ONESTOPBIT; break;
    case 2: dcb.StopBits = ONE5STOPBITS; break;
    case 3: dcb.StopBits = TWOSTOPBITS; break;
    default: dcb.StopBits = ONESTOPBIT; break;
   }
  switch (parity)
   {
    case 'n':
    case 'N': dcb.Parity = NOPARITY; break;
    case 'e':
    case 'E': dcb.Parity = EVENPARITY; break;
    case 'o':
    case 'O': dcb.Parity = ODDPARITY; break;
    default: dcb.Parity = NOPARITY; break;
   }

  dcb.fParity           = 0;                    // enable parity checking
  dcb.fOutxCtsFlow      = 0;                    // CTS output flow control
  dcb.fOutxDsrFlow      = 0;                    // DSR output flow control
  dcb.fDtrControl       = DTR_CONTROL_DISABLE;  // DTR flow control type
  dcb.fDsrSensitivity   = 0;                    // DSR sensitivity
  dcb.fTXContinueOnXoff = 0;                    // XOFF continues Tx
  dcb.fOutX             = 0;                    // XON/XOFF out flow control
  dcb.fInX              = 0;                    // XON/XOFF in flow control
  dcb.fErrorChar        = 0;                    // enable error replacement
  dcb.fNull             = 0;                    // enable null stripping
  dcb.fRtsControl       = RTS_CONTROL_DISABLE;  // RTS flow control
  dcb.fAbortOnError     = 0;                    // abort on error

  if (!SetCommState(hLoc, &dcb))
   {
    CloseHandle(hLoc);
    return hLoc;
   }

  return hLoc;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CloseComPort(void* &port)
{
  if (port != INVALID_HANDLE_VALUE) CloseHandle(port);
  port = INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void msDelay(int ms)
{
    HANDLE hTimer = NULL;
    LARGE_INTEGER liDueTime;

    liDueTime.QuadPart = -10000LL*ms;
    timeBeginPeriod(1);
    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer)
     {
     // Set a timer to wait for 10 seconds.
     if (SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
      WaitForSingleObject(hTimer, INFINITE);
     CloseHandle(hTimer);
     }
    timeEndPeriod(1);
}
//---------------------------------------------------------------------------
uint32_t mills(void)
{
  return GetTickCount();
}
//---------------------------------------------------------------------------
void idle(void)
{
 Sleep(0);
}
//---------------------------------------------------------------------------
void Delay(int ms, int SLEEP)
{
  DWORD time, time1, time2;
  timeBeginPeriod(1);
  time1 = GetTickCount();
  do
   {
    if (SLEEP < 0) Sleep(ms);
    else Sleep(SLEEP);
    time2 = GetTickCount();
    time = time2 - time1;
   }
  while (time < (unsigned)ms);
  timeEndPeriod(1);
}
//---------------------------------------------------------------------------
void GetPortList(void* owner, tprint print_fn)
{
 char physical[256];
 char com[16];
 for(int i=1; i<256; i++)
  {
   sprintf(com, "COM%d", i);
   physical[0] = 0;
   QueryDosDevice(com, physical, 256);
   if (owner && print_fn)
    {
     if (physical[0])
     {
      print_fn(owner, com);
     }
    }
  }
}
//---------------------------------------------------------------------------

int GetChar(void* port)
{
 DWORD lpErr;
 COMSTAT lpStat;
 DWORD Actual;
 BYTE ch;

 if (!port) return -1;
 if (port == INVALID_HANDLE_VALUE) return -1;

 if (ClearCommError(port, &lpErr, &lpStat) &&
     (lpStat.cbInQue >= 1) &&
     ReadFile(port, &ch, 1, &Actual, NULL) &&
     (Actual == 1)) return ch;
 else return -1;
}

//---------------------------------------------------------------------------

bool PutChar(void* port, uint8_t ch)
{
 DWORD Actual;
 if (!port) return false;
 if (port == INVALID_HANDLE_VALUE) return false;
 if (WriteFile(port, &ch, 1, &Actual, NULL) && (Actual == 1)) return true;
 else return false;
}
//---------------------------------------------------------------------------

bool PutChars(void* port, uint8_t *chs, int num)
{
 DWORD Actual;
 if (!port) return false;
 if (port == INVALID_HANDLE_VALUE) return false;
 if (WriteFile(port, chs, num, &Actual, NULL) && (Actual == num)) return true;
 else return false;
}

//---------------------------------------------------------------------------
void ClearPort(void* port)
{
 bool res;
 if (!port) return;
 if (port == INVALID_HANDLE_VALUE) return;
 while(Avail(port)) GetChar(port);
}
//---------------------------------------------------------------------------
int Avail(void* port)
{
 DWORD lpErr;
 COMSTAT lpStat;
 if (!port) return -1;
 if (port == INVALID_HANDLE_VALUE) return -1;
 if (ClearCommError(port, &lpErr, &lpStat)) return lpStat.cbInQue;
 else return -1;
}
//---------------------------------------------------------------------------


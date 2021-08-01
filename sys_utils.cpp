//---------------------------------------------------------------------------

#include <windows.h>

#pragma hdrstop

#include "sys_utils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------
int GetMainMenuHeight(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iMenuHeight;
  else return -1;
}
//---------------------------------------------------------------------------
int GetScroolBarHeight(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iScrollHeight;
  else return -1;
}
//---------------------------------------------------------------------------
int GetScroolBarWidth(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iCaptionWidth;
  else return -1;
}
//---------------------------------------------------------------------------
int GetCaptionHeight(void)
{
  NONCLIENTMETRICS Rec;

  Rec.cbSize = sizeof(Rec);
  if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, Rec.cbSize, &Rec.cbSize, 0))
	return Rec.iCaptionHeight;
  else return -1;
}
//---------------------------------------------------------------------------

/*

- In TcxGridDBTableView you can set the scrollbars property to say what
scrollbars you want
- How do you determine if a scrollbar is actually visible? I cannot
seem to find a property/method that will tell me this. I also tried:

  function ScrollBar_Exists(const AWindowHandle : HWnd) : TScrollStyle;
  var
    dwStyleFlags : DWORD;
  begin
    dwStyleFlags := GetWindowLong(AWindowHandle, GWL_STYLE) and
      (WS_VSCROLL or WS_HSCROLL);
    case dwStyleFlags of
      0 : result := ssNone;
      WS_VSCROLL : result := ssVertical;
      WS_HSCROLL : result := ssHorizontal;
    else
      result := ssBoth;
    end;
  end;

passing the handle of the grid, but even when the scrollbar was visible
the above function returned ssNone.
*/

//TScrollStyle ScrollBar_Exists(HWnd AWindowHandle)
bool VScrollBar_Exists(HWND AWindowHandle)
{
 return GetWindowLong(AWindowHandle, GWL_STYLE) & WS_VSCROLL;
}

bool HScrollBar_Exists(HWND AWindowHandle)
{
 return GetWindowLong(AWindowHandle, GWL_STYLE) & WS_HSCROLL;
}

//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <stdlib.h>


#pragma hdrstop

#include "StrUtils.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -8060
//---------------------------------------------------------------------------
char *stpcpyi(char *dst, char *src, int cnt)
{
 if (src) while(((*dst = *src)!=0) && (cnt > 0)){dst++; src++; cnt--;}
 *dst = '\0';
 return dst;
}
//---------------------------------------------------------------------------
int icPos(const AnsiString Str, const char c, const int p)
{
 for(int i = p; i <= Str.Length(); i++)
  if (Str[i] == c) return i;
 return 0;
}
//---------------------------------------------------------------------------
int isPos(const AnsiString Str, const AnsiString SStr, const int p)
{
 int i, j;
 bool b;
 for(i = p; i <= Str.Length(); i++)
  {
   for(j = 1; j <= SStr.Length(); j++)
    if (b = (((i+j-1)>Str.Length()) || (Str[i+j-1] != SStr[j]))) break;
   if (!b) return i;
  }
 return 0;
}
//---------------------------------------------------------------------------
int icPosIC(const AnsiString Str, const char c, const int p)
{
 for(int i = p; i <= Str.Length(); i++)
  if (toupper(Str[i]) == toupper(c)) return i;
 return 0;
}
//---------------------------------------------------------------------------
int isPosIC(const AnsiString Str, const AnsiString SStr, const int p)
{
 int i, j;
 bool b;
 for(i = p; i <= Str.Length(); i++)
  {
   for(j = 1; j <= SStr.Length(); j++)
    if (b = (((i+j-1)>Str.Length()) ||
             (toupper(Str[i+j-1]) != toupper(SStr[j])))) break;
   if (!b) return i;
  }
 return 0;
}
//---------------------------------------------------------------------------

void DoMacro(const AnsiString MStr, const AnsiString MVal, AnsiString &Str)
{
 int p;

 while ((p = Str.Pos(MStr)) > 0)
  {
   Str.Delete(p, MStr.Length());
   Str.Insert(MVal, p);
  }
}
//---------------------------------------------------------------------------

void DoMacroIC(const AnsiString MStr, const AnsiString MVal, AnsiString &Str)
{
 int p;

 while ((p = Str.UpperCase().Pos(MStr.UpperCase())) > 0)
  {
   Str.Delete(p, MStr.Length());
   Str.Insert(MVal, p);
  }
}

//---------------------------------------------------------------------------
void ExtractCSV(char *str, TStrings *Strings)
{
 char field[0x4000];
 enum {stChar, stLiteral} State = stChar;
 int i = 0;
 int j = 0;
 char c;

 Strings->Clear();
 while (((c = str[i++]) != '\0') && (j < 0x4000))
  {
   switch (State)
    {
     case stChar:
       if ((c == ';') || (c == '\n'))
        {
         field[j] = '\0';
         j = 0;
         Strings->Add(field);
        }
       else
       if (c == '"') State = stLiteral;
       else field[j++] = c;
     break;
     case stLiteral:
      if (c == '"') State = stChar;
      else field[j++] = c;
     break;
    }
  }
 if (c == '\0') field[j] = c;
 Strings->Add(field);
}

//---------------------------------------------------------------------------
void getCSVfield(char *str, char *field, int size, int &i, int &j)
{
 enum {stChar, stLiteral} State = stChar;
 char c;

 while (((c = str[i++]) != '\0') && (j < size))
  {
   switch (State)
    {
     case stChar:
       if ((c == ';') || (c == '\n'))
        {
         field[j] = '\0';
         j = 0;
         return;
        }
       else
       if (c == '"') State = stLiteral;
       else field[j++] = c;
     break;
     case stLiteral:
      if (c == '"') State = stChar;
      else field[j++] = c;
     break;
    }
  }
 if (c == '\0') field[j] = c;
}

//---------------------------------------------------------------------------
AnsiString GetPNDBVal(AnsiString fName, AnsiString PartNum, AnsiString Key)
{
 char str[4096];
 FILE * fMacro;
 int i, key;
 char c;
 AnsiString Res = "";
 TStringList *Keys = new TStringList();
 TStringList *Vals = new TStringList();

 fMacro = fopen(fName.c_str(), "rt");
 if (fMacro)
  {
   if (fgets(str, 256, fMacro))
    {
     ExtractCSV(str, Keys);
     for(key = 0; key < Keys->Count; key++)
       if (Key.AnsiCompareIC(Keys->Strings[key]) == 0) break;
     if (key < Keys->Count)
      {
       while (fgets(str, 256, fMacro))
        {
         ExtractCSV(str, Vals);
         if (PartNum.AnsiCompareIC(Vals->Strings[0])==0)
          {
           Res = Vals->Strings[key];
           break;
          }
        }
      }
    }
   fclose(fMacro);
  }
  delete Vals;
  delete Keys;
  return Res;
}
//---------------------------------------------------------------------------
// Split 'sch' divided string to <num> fields. Reurn fieds count.
int ncsplit(int num, char sch, char* str, ...)
{
 char c, cc;
 int i = 0;
 int n = 0;
 char **ostr = &str; //ostr[1] point to first <...> argument
 do
  {
   c = str[i++];
   if (c) cc = str[i];  // next character
   if ((c == '\n') || (c == '\r')) // ignore
    {
     if (cc) i++;
     continue;
    }
   if (c == '\t') c = ' '; // change
   if (c == sch)
    {
     if (cc == sch) i++; // screaning
     else
      {
       n++;
       continue;
      }
    }
   if (n && ostr[n])
    {
     *ostr[n]++ = c;
     *ostr[n] = '\0';
    }
  }
 while ((c)&&(n<=num));
 for(i=n; i <= num; i++) if (ostr[n]) *ostr[n] = '\0';
 return n;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Split 'sch' divided string to <num> fields. Stop after comment char. Reurn fieds count.
int nccsplit(int num, char cch, char sch, char* str, ...)
{
 char c, cc;
 int i = 0;
 int n = 0;
 char **ostr = &str; //ostr[1] point to first <...> argument
 do
  {
   c = str[i++];
   if (c == cch) c = 0;
   if (c) cc = str[i];  // next character
   if ((c == '\n') || (c == '\r')) // ignore
    {
     if (cc) i++;
     continue;
    }
   if (c == '\t') c = ' '; // change
   if (c == sch)
    {
     if (cc == sch) i++; // screaning
     else
      {
       n++;
       continue;
      }
    }
   if (n && ostr[n])
    {
     *ostr[n]++ = c;
     *ostr[n] = '\0';
    }
  }
 while ((c)&&(n<=num));
 for(i=n; i <= num; i++) if (ostr[n]) *ostr[n] = '\0';
 return n;
}
//---------------------------------------------------------------------------
#pragma warn -8075
AnsiString asprintf(const char* fmt, ...)
{
 char str[4096];

 va_list args;// = (void**)&fmt;
 va_start(args, fmt);
 vsprintf(str, fmt, args);
 va_end(args);
 return str;
 /*
 void **params = (void**)&fmt;
 sprintf(str, fmt, params[1], params[2], params[3], params[4],
                   params[5], params[6], params[7], params[8],
                   params[9], params[10], params[11], params[12],
                   params[13], params[14], params[15], params[16]);
 return str;
 */
}
//---------------------------------------------------------------------------
void lprintf(TStrings *List, const char* fmt, ...)
{
 if (List)
  {
   char str[4096];
   AnsiString aStr = "";
   va_list args;
   va_start(args, fmt);
   vsprintf(str, fmt, args);
   va_end(args);
   int i = 0;
   while (List->Count > LOGLINES) List->Delete(0);
   do
    {
     int Index = List->Count;
     if (Index)
     {
      Index--;
      aStr = List->Strings[Index];
      List->Delete(Index);
     }
     while(str[i]&&str[i] != '\n')
      {
       if (str[i] == '\r') {i++; aStr="";}
       else aStr = aStr+str[i++];
      }
     List->Insert(Index, aStr);
     aStr = "";
     if (str[i] == '\n')
      {
       List->Add("");
       i++;
      }
    }
   while(str[i]);
  }
}
//---------------------------------------------------------------------------
void ldump(TStrings *List, const char* data, int size)
{
 for(int i=0; i<size; i++)
  {
   if (i%16 == 0)
    {
     if (i==0) lprintf(List, "%04x", i);
     else lprintf(List, "\n%04x", i);
    }
   lprintf(List, " %02x", (unsigned char)data[i]);
  }
}
//---------------------------------------------------------------------------


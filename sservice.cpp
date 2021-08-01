//---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>




#pragma hdrstop

#include "sservice.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#pragma warn -8060 // W8060 Possibly incorrect assignment

//---------------------------------------------------------------------------
//get trancuted upper case value, so "   abc de  ;" -> "ABC DE"
void getCSVfieldUC(char *str, char *field, int size, int &i, int &j)
{
 enum {stChar, stLiteral, stSpace} State = stChar;
 char c;
 while ((str[i] == ' ')||str[i] == '\t') i++;
 while (((c = str[i++]) != '\0') && (j < size))
  {
   switch (State)
    {
     case stChar:
       if ((c == ';') || (c == '\n'))
        {
         while (j&&((field[j-1] == ' ')||(field[j-1] == '\t'))) j--;
         field[j] = '\0';
         j = 0;
         return;
        }
       else
       if (c == '"') State = stLiteral;
       else field[j++] = toupper(c);
     break;
     case stLiteral:
      if (c == '"') State = stChar;
      else field[j++] = toupper(c);
     break;
    }
  }
 if (c == '\0') field[j] = c;
}
//---------------------------------------------------------------------------
void getCsvKey(char *str, char *key, char *val)
{
  char field[64];
  char *endptr;
  int i = 0;
  int j = 0;
  val[0] = '\0';
  do
   {
    getCSVfieldUC(str, field, 64, i, j);
    if (field[0] && (endptr=strstr(field, key)))
     {
      strcpy(val, endptr+strlen(key));
      break;
     } 
   }
  while (field[0]);
}
//---------------------------------------------------------------------------
unsigned long tstrtoi(char *s, char **endptr)
{
 const int dms[] =
   {(60.0*60.0*60.0*24.0*365.25*100.0),(60.0*60.0*24.0*365.25),
    (60.0*60.0*24.0), (60.0*60.0), 60.0, 1.0};
 const char cdt[] =  {'c', 'y', 'd', 'h', 'm', 's'};
 double res = 0;
 double d;
 char* end = s;

 for(int i = 0; i < 6; i++)
  {
   d = strtod(end, &end);
   do
    {
     if ((*end == ':') && (*(end+1) == cdt[i]))
      {
       res += d * dms[i];
       end += 2;
       break;
      }
     else i++;
    }
   while (i < 6);
  }
 *endptr = end;
 return res;
}

//---------------------------------------------------------------------------

double sci(const char *end)
{
 while(*end == ' ') end++;
 switch(*end)
  {
   case 'y': return 1e-24;
   case 'z': return 1e-21;
   case 'a': return 1e-18;
   case 'f': return 1e-15;
   case 'p': return 1e-1;
   case 'n': return 1e-9;
   case 'u': return 1e-6;
   case 'm': return 1e-3;
   case 'c': return 1e-2;
   case 'd': return 1e-1;
   case 'R': return 1.0;
   case 'D': return 1e1;
   case 'h': return 1e2;
   case 'k': return 1e3;
   case 'M': return 1e6;
   case 'G': return 1e9;
   case 'T': return 1e12;
   case 'P': return 1e15;
   case 'E': return 1e18;
   case 'Z': return 1e21;
   case 'Y': return 1e24;
  }
 return 1.0;
}
//---------------------------------------------------------------------------

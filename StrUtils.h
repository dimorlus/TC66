//---------------------------------------------------------------------------

#ifndef StrUtilsH
#define StrUtilsH
//---------------------------------------------------------------------------
#define LOGLINES        1023

extern char *stpcpyi(char *dst, char *src, int cnt);
extern void ExtractCSV(char *str, TStrings *Strings);
extern void getCSVfield(char *str, char *field, int size, int &i, int &j);
extern int icPos(const AnsiString Str, const char c, const int p);
extern int isPos(const AnsiString Str, const AnsiString SStr, const int p);
extern int icPosIC(const AnsiString Str, const char c, const int p);
extern int isPosIC(const AnsiString Str, const AnsiString SStr, const int p);
extern void DoMacro(const AnsiString MStr, const AnsiString MVal, AnsiString &Str);
extern void DoMacroIC(const AnsiString MStr, const AnsiString MVal, AnsiString &Str);
extern AnsiString GetPNDBVal(AnsiString fName, AnsiString PartNum, AnsiString Key);
// Split slashes ('/') divided string to fields. Reurn fieds count.
extern int nsplit(int num, char* str, ...);
extern int ncsplit(int num, char sch, char* str, ...);


extern AnsiString asprintf(const char* fmt, ...);
extern void lprintf(TStrings *List, const char* fmt, ...);
extern void ldump(TStrings *List, const char* data, int zise);



#endif

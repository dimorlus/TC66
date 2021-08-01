//---------------------------------------------------------------------------

#ifndef sserviceH
#define sserviceH
//---------------------------------------------------------------------------
//get trancuted upper case value, so "   abc de  ;" -> "ABC DE"
extern void getCSVfieldUC(char *str, char *field, int size, int &i, int &j);
extern void getCsvKey(char *str, char *key, char *val);

#endif

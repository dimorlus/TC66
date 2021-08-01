//---------------------------------------------------------------------------

#ifndef datastrH
#define datastrH
//---------------------------------------------------------------------------

//The base64 ratio of output bytes to input bytes is 4:3 (33% overhead).
typedef unsigned char (tdatafn)(int idx);


extern int dat2hex(char *str, unsigned char *data, int size);
extern bool hex2dat(const char *xstr, int data_len, unsigned char *data);
extern int dat2b64(char *str, unsigned char *data, int size);
extern int dat2b64fn(char *str, tdatafn datafn, int size);

extern int b642data(char *src, unsigned char *dec);
#endif

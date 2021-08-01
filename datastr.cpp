//---------------------------------------------------------------------------
#include <ctype.h>


#pragma hdrstop

#include "datastr.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

int dat2hex(char *str, unsigned char *data, int size)
{
 int i;
 for(i=0; i < size; i++)
  {
   unsigned char d = (data[i]&0xf0)>>4;
   str[2*i] = (d<10)?'0'+d:'A'+d-10;
   d = data[i]&0x0f;
   str[2*i+1] = (d<10)?'0'+d:'A'+d-10;
  }
 str[2*i] = '\0';
 return 2*i;
}
//---------------------------------------------------------------------------
static const char b64_table[] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3',
  '4', '5', '6', '7', '8', '9', '+', '/'
};
//---------------------------------------------------------------------------
int dat2b64(char *str, unsigned char *data, int size)
{
 int i = 0;
 int j = 0;
 int enc_size = 0;
 unsigned char buf[4];
 unsigned char tmp[3];

 // parse until end of source
 while (size--)
  {
   // read up to 3 bytes at a time into `tmp'
   tmp[i++] = *(data++);

   // if 3 bytes read then encode into `buf'
   if (i == 3)
    {
     buf[0] = (tmp[0] & 0xfc) >> 2;
     buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
     buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
     buf[3] = tmp[2] & 0x3f;

     for (i = 0; i < 4; ++i) str[enc_size++] = b64_table[buf[i]];
     // reset index
     i = 0;
    }
   }
  // remainder
  if (i > 0)
   {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) tmp[j] = '\0';

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) str[enc_size++] = b64_table[buf[j]];

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) str[enc_size++] = '=';
   }
  str[enc_size] = '\0';

  return enc_size;
}
//---------------------------------------------------------------------------
int dat2b64fn(char *str, tdatafn datafn, int size)
{
 int i = 0;
 int j = 0;
 int idx = 0;
 int enc_size = 0;
 unsigned char buf[4];
 unsigned char tmp[3];

 // parse until end of source
 while (size--)
  {
   // read up to 3 bytes at a time into `tmp'
   tmp[i++] = datafn(idx++);

   // if 3 bytes read then encode into `buf'
   if (i == 3)
    {
     buf[0] = (tmp[0] & 0xfc) >> 2;
     buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
     buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
     buf[3] = tmp[2] & 0x3f;

     for (i = 0; i < 4; ++i) str[enc_size++] = b64_table[buf[i]];
     // reset index
     i = 0;
    }
   }
  // remainder
  if (i > 0)
   {
    // fill `tmp' with `\0' at most 3 times
    for (j = i; j < 3; ++j) tmp[j] = '\0';

    // perform same codec as above
    buf[0] = (tmp[0] & 0xfc) >> 2;
    buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
    buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
    buf[3] = tmp[2] & 0x3f;

    // perform same write to `enc` with new allocation
    for (j = 0; (j < i + 1); ++j) str[enc_size++] = b64_table[buf[j]];

    // while there is still a remainder
    // append `=' to `enc'
    while ((i++ < 3)) str[enc_size++] = '=';
   }
  str[enc_size] = '\0';

  return enc_size;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int b642data(char *src, unsigned char *dec)
{
  int i = 0;
  int j = 0;
  int l = 0;
  int size = 0;
  unsigned char buf[3];
  unsigned char tmp[4];

  // parse until end of source
  while (src[j]) {
    // break if char is `=' or not base64 char
    if ('=' == src[j]) { break; }
    if (!(isalnum(src[j]) || '+' == src[j] || '/' == src[j])) { break; }

    // read up to 4 bytes at a time into `tmp'
    tmp[i++] = src[j++];

    // if 4 bytes read then decode into `buf'
    if (4 == i) {
      // translate values in `tmp' from table
      for (i = 0; i < 4; ++i) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[i] == b64_table[l]) {
            tmp[i] = l;
            break;
          }
        }
      }

      // decode
      buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
      buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
      buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

      // write decoded buffer to `dec'
      if (dec != NULL){
        for (i = 0; i < 3; ++i) {
          dec[size++] = buf[i];
        }
      } else {
        return 0;
      }

      // reset
      i = 0;
    }
  }

  // remainder
  if (i > 0) {
    // fill `tmp' with `\0' at most 4 times
    for (j = i; j < 4; ++j) {
      tmp[j] = '\0';
    }

    // translate remainder
    for (j = 0; j < 4; ++j) {
        // find translation char in `b64_table'
        for (l = 0; l < 64; ++l) {
          if (tmp[j] == b64_table[l]) {
            tmp[j] = l;
            break;
          }
        }
    }

    // decode remainder
    buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
    buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
    buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];

    // write remainer decoded buffer to `dec'
    if (dec != NULL){
      for (j = 0; (j < i - 1); ++j) {
        dec[size++] = buf[j];
      }
    } else {
      return 0;
    }
  }
  return size;

}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool hex2dat(const char* xstr, int data_len, unsigned char* data)
{
 int i;
 unsigned char d = 0;
 for(i = 0; i < data_len; i++)
  {
   char c = xstr[i];
   if ((c >= '0') && (c <= '9')) d += c-'0';
   else
   if ((c >= 'A') && (c <= 'F')) d += 10 + c-'A';
   else
   if ((c >= 'a') && (c <= 'f')) d += 10 + c-'a';
   else break;

   if (i%2)
    {
     *data++ = d;
     d = 0;
    }
   else d *= 16;
  }
 if (i < data_len) return false;
 return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#ifndef SerialH
#define SerialH
//---------------------------------------------------------------------------
#pragma warn -8059
#pragma pack(1)
//#include "types.h"


typedef void (tprint)(void *owner, const char *str);


extern void* OpenComPort(int port, int baud, int bits,
                   char parity, int stopbits);
extern void CloseComPort(void* &port);

extern void msDelay(int ms);
extern void Delay(int ms, int SLEEP);

extern void GetPortList(void* owner, tprint print_fn);

extern uint32_t mills(void);
extern void idle(void);

extern int GetChar(void* port);
extern bool PutChar(void* port, uint8_t ch);
extern bool PutChars(void* port, uint8_t *chs, int num);
extern void ClearPort(void* port);
extern int Avail(void* port);

#endif


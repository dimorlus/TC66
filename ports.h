//---------------------------------------------------------------------------

#ifndef portsH
#define portsH
//---------------------------------------------------------------------------
typedef void (tprint)(void *owner, const char *str);

extern void GetPortListUsingSetupAPI(void* owner, tprint print_fn);
#endif

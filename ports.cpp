//---------------------------------------------------------------------------
#include <windows.h>
#include <setupapi.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#pragma hdrstop


#pragma hdrstop

#include "ports.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
#pragma warn -8004
//---------------------------------------------------------------------------
bool QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, char *str)
{
  DWORD dwType = 0;
  DWORD dwSize = 0;
  //Query initially to get the buffer size required
  if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, NULL, 0, &dwSize))
  {
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      return false;
  }
  //check dwSize here
  if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, str, dwSize, &dwSize))
    return false;
  if (dwType != REG_SZ)
  {
    SetLastError(ERROR_INVALID_DATA);
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
bool RegQueryValueString(HKEY& key, LPCTSTR lpValueName, char *sValue, DWORD sSize)
{
  DWORD dwType;
  //Reset the output parameter
  sValue[0] = '\0';

  //First query for the size of the registry value
  ULONG nChars = 0;
  ULONG nStatus = RegQueryValueEx(key, lpValueName, NULL, &dwType, sValue, &nChars);
  if (nChars < sSize)
   {
    DWORD dwType = 0;
    nStatus = RegQueryValueEx(key, lpValueName, NULL, &dwType, sValue, &sSize);
    if (nStatus != ERROR_SUCCESS)
    {
      SetLastError(nStatus);
      return false;
    }
    if ((dwType != REG_SZ) && (dwType != REG_EXPAND_SZ))
    {
      SetLastError(ERROR_INVALID_DATA);
      return false;
    }
    if (sValue[sSize - 1] != '\0')
    {
      //Forcibly null terminate the data ourselves
      sValue[sSize] = '\0';
    }

    return true;
   }
  else return false;
}
//---------------------------------------------------------------------------

bool QueryRegistryPortName(HKEY& deviceKey, int& nPort)
{
  //What will be the return value from the method (assume the worst)
  bool bAdded = false;

  //Read in the name of the port
  char sPortName[256];
  if (RegQueryValueString(deviceKey, "PortName", sPortName, sizeof(sPortName)))
  {
    //If it looks like "COMX" then
    //add it to the array which will be returned
    const size_t nLen = strlen(sPortName);
    if (nLen > 3)
    {
     if (strnicmp(sPortName, "COM", 3) == 0)
      {
       nPort = atoi(&sPortName[3]);
       bAdded = true;
      }
    }
  }

  return bAdded;
}

//---------------------------------------------------------------------------
void GetPortListUsingSetupAPI(void* owner, tprint print_fn)
{
 char com[256];
 char str[256];
 DWORD dwFlags = DIGCF_PRESENT;
 int nIndex = 0;
 bool bMoreItems = true;
 SP_DEVINFO_DATA devInfo;
 GUID guid = GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR;
 HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, dwFlags);
 if (hDevInfoSet != INVALID_HANDLE_VALUE)
  {
   while (bMoreItems)
    {
     str[0] = '\0';
     devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
     bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
     if (bMoreItems)
      {
       HKEY hKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
       if (hKey != INVALID_HANDLE_VALUE)
        {
         int nPort = 0;
         if (QueryRegistryPortName(hKey, nPort))
          {
           sprintf(com, "COM%d", nPort);
           if (QueryDeviceDescription(hDevInfoSet, devInfo, str))
             sprintf(com, "COM%d %s", nPort, str);
           print_fn(owner, com);
          }
        }
      }
     nIndex++;
    }
  }
}
//---------------------------------------------------------------------------

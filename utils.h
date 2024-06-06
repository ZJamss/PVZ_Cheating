#ifndef UTILS_H
#define UTILS_H
#include<Windows.h>

int Readmeo(HANDLE hgame,DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,DWORD AdressThird);
int Readmeo(HANDLE hgame, DWORD Adress, DWORD AdressFirst, DWORD AdressSecond);
bool writeMemo(HANDLE handle,DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,int value);
HANDLE getGameHandle(LPCSTR className,LPCWSTR windowName);
#endif // UTILS_H

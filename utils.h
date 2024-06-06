#ifndef UTILS_H
#define UTILS_H
#include<Windows.h>
#define NOP 0x90

int Readmeo(HANDLE hgame,DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,DWORD AdressThird);
int Readmeo(HANDLE hgame, DWORD Adress, DWORD AdressFirst, DWORD AdressSecond);
HANDLE getGameHandle(LPCSTR className,LPCWSTR windowName);
#endif // UTILS_H

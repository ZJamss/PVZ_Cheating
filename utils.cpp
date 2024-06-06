#include "utils.h"

int Readmeo(HANDLE hgame,DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,DWORD AdressThird)
{
    DWORD AdressValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)Adress, &AdressValue, sizeof(DWORD), 0);
    DWORD AdressFirstValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)(AdressValue + AdressFirst), &AdressFirstValue, sizeof(DWORD), 0);
    DWORD AdressSecondValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)(AdressFirstValue + AdressSecond), &AdressSecondValue, sizeof(DWORD), 0);
    DWORD AdressThirdValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)(AdressSecondValue + AdressThird), &AdressThirdValue, sizeof(DWORD), 0);
    return AdressSecondValue + AdressThird;
}
int Readmeo(HANDLE hgame, DWORD Adress, DWORD AdressFirst, DWORD AdressSecond)
{
    DWORD AdressValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)Adress, &AdressValue, sizeof(DWORD), 0);
    DWORD AdressFirstValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)(AdressValue + AdressFirst), &AdressFirstValue, sizeof(DWORD), 0);
    DWORD AdressSecondValue = 0;
    ReadProcessMemory(hgame, (LPDWORD)(AdressFirstValue + AdressSecond), &AdressSecondValue, sizeof(DWORD), 0);
    return AdressFirstValue + AdressSecond;
}

bool writeMemo(HANDLE handle,DWORD Adress,DWORD AdressFirst,DWORD AdressSecond,int value)
{
    DWORD Final_Adress = Readmeo(handle, Adress, AdressFirst, AdressSecond);
    return WriteProcessMemory(handle, (LPVOID)(Final_Adress), &value, sizeof(DWORD), 0);
}

HANDLE getGameHandle(LPCSTR className,LPCWSTR windowName){
    HWND game = FindWindow(nullptr, windowName);
    if (game == nullptr){
        return nullptr;
    }
    DWORD PID;
    GetWindowThreadProcessId(game, &PID);
    HANDLE hgame = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
    return hgame;
}

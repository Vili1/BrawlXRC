#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h> 
#include <array>
#include <thread>

HWND hGameWindow = FindWindow(NULL, "Brawlhalla");
DWORD pID = NULL;
HANDLE processHandle = NULL;
DWORD XtoScaleAddressCPY = NULL;
int XtoScaleValDef, YtoScaleValDef, XtoScaleVal, YtoScaleVal = 0;

DWORD dwGetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID)
{
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do
        {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));

    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

void findGameWindowToHook()
{
    if (hGameWindow != NULL)
    {
        std::cout << "Brawlhalla found successfully!" << std::endl;
    }
    else
    {
        std::cout << "Unable to find Brawlhalla, Please make sure that you are in a game!" << std::endl;
        Sleep(3000);
        exit(EXIT_FAILURE);
    }
}

void checkProcessHandle()
{
    GetWindowThreadProcessId(hGameWindow, &pID);
    processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
    {
        std::cout << "Try to run the application as administrator." << std::endl;
        Sleep(3000);
        exit(EXIT_FAILURE);
    }
}

void checkGameToExit()
{
    HWND hGameWindowToExit = FindWindow(NULL, "Brawlhalla");
    if (hGameWindowToExit == NULL)
    {
        CloseHandle(processHandle);
        exit(EXIT_FAILURE);
    }
}

void iniPRT()
{
    char moduleName[] = "Adobe AIR.dll";
    DWORD gameBaseAddress = dwGetModuleBaseAddress(_T(moduleName), pID);
    DWORD offsetGameToBaseAddress = 0x012DAACC;
    std::array<DWORD, 6> XtoScaleOffsets{ 0x170, 0x268, 0x78, 0x50, 0x34, 0x2B8 };
    DWORD baseAddress = NULL;

    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);
    DWORD XtoScaleAddress = baseAddress;
    for (int i = 0; i < XtoScaleOffsets.size() - 1; i++)
    {
        ReadProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + XtoScaleOffsets.at(i)), &XtoScaleAddress, sizeof(XtoScaleAddress), NULL);
    }
    XtoScaleAddress += XtoScaleOffsets.at(XtoScaleOffsets.size() - 1);
    XtoScaleAddressCPY = XtoScaleAddress;

    ReadProcessMemory(processHandle, (LPCVOID)(XtoScaleAddressCPY), &XtoScaleValDef, sizeof(int), NULL);
    ReadProcessMemory(processHandle, (LPCVOID)(XtoScaleAddressCPY + 4), &YtoScaleValDef, sizeof(int), NULL);

    XtoScaleVal = XtoScaleValDef;
    YtoScaleVal = YtoScaleValDef;
}

void menu()
{
    system("cls");
    std::cout << "Brawlhalla external resolution changer by Vili." << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Press delete to enter a custom resolution." << std::endl;
    std::cout << "Press insert to reset the resolution to the default one." << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Default resolution:" << std::dec << XtoScaleValDef << "x" << YtoScaleValDef << std::endl;
    std::cout << "Current resolution:" << std::dec << XtoScaleVal << "x" << YtoScaleVal << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
}

int main()
{
    findGameWindowToHook();

    checkProcessHandle();

    iniPRT();
    
    menu();

    while (true)
    {
        Sleep(100);

        if (GetAsyncKeyState(VK_DELETE)) // delete
        {
            
            std::cout << "Enter width:"<< std::endl;
            std::cin >> XtoScaleVal;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY), &XtoScaleVal, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 132), &XtoScaleVal, sizeof(int), 0);
            std::cout << "Enter height:" << std::endl;
            std::cin >> YtoScaleVal;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY + 4), &YtoScaleVal, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 128), &YtoScaleVal, sizeof(int), 0);
            menu();
        }

        if (GetAsyncKeyState(VK_INSERT)) // insert
        {
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY), &XtoScaleValDef, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 132), &XtoScaleValDef, sizeof(int), 0);
            XtoScaleVal = XtoScaleValDef;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY + 4), &YtoScaleValDef, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 128), &YtoScaleValDef, sizeof(int), 0);
            YtoScaleVal = YtoScaleValDef;
            menu();
        }
    }
    
    return 0;
}
//XtoScaleAddress= XtoScaleAddressCPY
//Xinternal = XtoScaleAddressCPY - 132;
//YtoScale = XtoScaleAddressCPY + 4;
//Yinternal = XtoScaleAddressCPY - 128;
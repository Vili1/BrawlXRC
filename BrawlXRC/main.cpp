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
int XtoScaleValDef, YtoScaleValDef, XtoScaleVal, YtoScaleVal, widthCPY, heightCPY = 0;
int scale = 100;

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

void findGameWindow()
{
    if (hGameWindow != NULL)
    {
        std::cout << "Brawlhalla found successfully!" << std::endl;
    }
    else
    {
        std::cout << "Unable to find Brawlhalla, Please make sure that the game is opened!" << std::endl;
        Sleep(3000);
        exit(EXIT_FAILURE);
    }
}

void openProcessHandle()
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
    DWORD baseAddress = NULL;

    int ptrSelect = 0;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Select a version" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "1. Normal version" << std::endl;
    std::cout << "2. Tech-test version" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cin >> ptrSelect;

    if (ptrSelect == 1)
    {
        SetConsoleTitle("Normal version");
        DWORD offsetGameToBaseAddress = 0x01331740;
        std::array<DWORD, 8> XtoScaleOffsets{ 0x28, 0x14, 0x154, 0x14, 0x78, 0x50, 0x34, 0x2A4 };

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
    else if (ptrSelect == 2)
    {
        SetConsoleTitle("Tech-test version");
        DWORD offsetGameToBaseAddress = 0x01331740;
        std::array<DWORD, 8> XtoScaleOffsets{ 0x28, 0x14, 0x154, 0x14, 0x78, 0x50, 0x34, 0x2A4 };

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
    else
    {
        std::cout << "Invalid value" << std::endl;
        Sleep(3000);
        exit(EXIT_FAILURE);
    }
}

void resScale()
{
    float Fwidth = XtoScaleValDef;
    float Fheight = YtoScaleValDef;
    int pixel = Fwidth * Fheight / 100 * scale;
    float Wratio = Fheight / Fwidth;
    float Hratio = Fwidth / Fheight;
    int width = sqrt(pixel / Wratio);
    int height = sqrt(pixel / Hratio);

    widthCPY = width;
    heightCPY = height;
}

void menu()
{
    system("cls");
    std::cout << "Brawlhalla external resolution changer by Vili" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Press delete to set a custom resolution" << std::endl;
    std::cout << "Press insert to reset the resolution to the default one" << std::endl;
    std::cout << "Press right shift to set a custom resolution scale%" << std::endl;
    std::cout << "Press numpad + increase the resolution scale by 5%" << std::endl;
    std::cout << "Press numpad - decrease the resolution scale by 5%" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
    std::cout << "Default resolution:" << std::dec << XtoScaleValDef << "x" << YtoScaleValDef << std::endl;
    std::cout << "Current resolution:" << std::dec << XtoScaleVal << "x" << YtoScaleVal << std::endl;
    std::cout << "Current resolution scale in %:" << std::dec << scale << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;
}

int main()
{
    findGameWindow();

    openProcessHandle();
    
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
            scale = 100;
            menu();
        }

        if (GetAsyncKeyState(VK_RSHIFT)) //right shift
        {
            std::cout << "Enter custom %:" << std::endl;
            std::cin >> scale;
            resScale();
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 132), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY + 4), &heightCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 128), &heightCPY, sizeof(int), 0);
            XtoScaleVal = widthCPY;
            YtoScaleVal = heightCPY;
            menu();
        }

        if (GetAsyncKeyState(VK_ADD)) //numpad +
        {
            scale += 5;
            resScale();
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 132), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY + 4), &heightCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 128), &heightCPY, sizeof(int), 0);
            XtoScaleVal = widthCPY;
            YtoScaleVal = heightCPY;
            menu();
        }

        if (GetAsyncKeyState(VK_SUBTRACT)) // numpad -
        {
            scale -= 5;
            resScale();
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 132), &widthCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY + 4), &heightCPY, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddressCPY - 128), &heightCPY, sizeof(int), 0);
            XtoScaleVal = widthCPY;
            YtoScaleVal = heightCPY;
            menu();
        }

        checkGameToExit();
    }
    
    return 0;
}
//XtoScaleAddress= XtoScaleAddressCPY
//Xinternal = XtoScaleAddressCPY - 132;
//YtoScale = XtoScaleAddressCPY + 4;
//Yinternal = XtoScaleAddressCPY - 128;

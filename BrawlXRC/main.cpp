#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <tchar.h> 
#include <vector>

//some global vars
HWND hGameWindow = FindWindow(NULL, "Brawlhalla");
char moduleName[] = "Adobe AIR.dll";
DWORD pID = NULL;
HANDLE processHandle = NULL;
uintptr_t XtoScaleAddress = NULL;
int XtoScaleValDef, YtoScaleValDef, XtoScaleVal, YtoScaleVal;
int scale = 100;

//gets the base addr of the module
uintptr_t dwGetModuleBaseAddress(TCHAR* lpszModuleName, uintptr_t pID)
{
    uintptr_t dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &ModuleEntry32))
    {
        do
        {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0)
            {
                dwModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));

    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

//finds the game window and spits out errors if its not fond
void findGameWindow()
{
    if (hGameWindow != NULL)
    {
        std::cout << "Brawlhalla found successfully!" << std::endl;
        GetWindowThreadProcessId(hGameWindow, &pID);
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
        if (processHandle == INVALID_HANDLE_VALUE || processHandle == NULL)
        {
            std::cout << "Try to run the application as administrator." << std::endl;
            Sleep(3000);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cout << "Unable to find Brawlhalla, Please make sure that the game is opened!" << std::endl;
        Sleep(3000);
        exit(EXIT_FAILURE);
    }
}

//closes the pogram when the game isn't open
void checkGameToExit()
{
    HWND hGameWindowToExit = FindWindow(NULL, "Brawlhalla");
    if (hGameWindowToExit == NULL)
    {
        CloseHandle(processHandle);
        exit(EXIT_FAILURE);
    }
}

//my crappy function that reads a multilevel pointer
uintptr_t iniPRT(char moduleName[], uintptr_t offsetGameToBaseAddress, std::vector<uintptr_t> Offsets)
{
    uintptr_t gameBaseAddress = dwGetModuleBaseAddress(_T(moduleName), pID);
    uintptr_t baseAddress = NULL;
    ReadProcessMemory(processHandle, (LPVOID)(gameBaseAddress + offsetGameToBaseAddress), &baseAddress, sizeof(baseAddress), NULL);
    uintptr_t Address = baseAddress;
    for (int i = 0; i < Offsets.size() - 1; i++)
    {
        ReadProcessMemory(processHandle, (LPVOID)(Address + Offsets.at(i)), &Address, sizeof(Address), NULL);
    }
    Address += Offsets.at(Offsets.size() - 1);
    return Address;
}

//my crappy res sacle function that calculates the resolution based on a % scale
std::pair<int, int> resScale(float Fwidth, float Fheight)
{
    int pixel = Fwidth * Fheight / 100 * scale;
    float Wratio = Fheight / Fwidth;
    float Hratio = Fwidth / Fheight;
    int width = sqrt(pixel / Wratio);
    int height = sqrt(pixel / Hratio);
    return std::make_pair(width, height);
}

//the menu function, just a crappy cout stuff 
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

    //select game version
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
        XtoScaleAddress = iniPRT(moduleName, 0x01331740, { 0x28, 0x14, 0x154, 0x14, 0x78, 0x50, 0x34, 0x2A4 });

    }
    else if (ptrSelect == 2)
    {
        SetConsoleTitle("Tech-test version");
        XtoScaleAddress = iniPRT(moduleName, 0x012DAACC, { 0x170, 0x268, 0x78, 0x50, 0x34, 0x2B8 });

    }
    else
    {
        std::cout << "Invalid value" << std::endl;
        Sleep(3000);
        return 1;
    }

    //reading the game resolution and storing it to X and Y ScaleValDef so we can reset back to default res
    ReadProcessMemory(processHandle, (LPCVOID)(XtoScaleAddress), &XtoScaleValDef, sizeof(int), NULL);
    ReadProcessMemory(processHandle, (LPCVOID)(XtoScaleAddress + 4), &YtoScaleValDef, sizeof(int), NULL);
    //copying the def res to new variables that can be modified throughout the code
    XtoScaleVal = XtoScaleValDef;
    YtoScaleVal = YtoScaleValDef;
    menu();

    //main loop
    while (true)
    {
        Sleep(100);

        //set custom res
        if (GetAsyncKeyState(VK_DELETE))
        {
            std::cout << "Enter width:"<< std::endl;
            std::cin >> XtoScaleVal;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress), &XtoScaleVal, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 132), &XtoScaleVal, sizeof(int), 0);
            std::cout << "Enter height:" << std::endl;
            std::cin >> YtoScaleVal;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + 4), &YtoScaleVal, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 128), &YtoScaleVal, sizeof(int), 0);
            menu();
        }

        //reset defaults
        if (GetAsyncKeyState(VK_INSERT))
        {
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress), &XtoScaleValDef, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 132), &XtoScaleValDef, sizeof(int), 0);
            XtoScaleVal = XtoScaleValDef;
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + 4), &YtoScaleValDef, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 128), &YtoScaleValDef, sizeof(int), 0);
            YtoScaleVal = YtoScaleValDef;
            scale = 100;
            menu();
        }

        //custom % scale
        if (GetAsyncKeyState(VK_RSHIFT))
        {
            std::cout << "Enter custom %:" << std::endl;
            std::cin >> scale;
            std::pair<int, int> Scaleresult = resScale(XtoScaleValDef, YtoScaleValDef);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 132), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + 4), &Scaleresult.second, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 128), &Scaleresult.second, sizeof(int), 0);
            XtoScaleVal = Scaleresult.first;
            YtoScaleVal = Scaleresult.second;
            menu();
        }

        //scale up res
        if (GetAsyncKeyState(VK_ADD))
        {
            scale += 5;
            std::pair<int, int> Scaleresult = resScale(XtoScaleValDef, YtoScaleValDef);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 132), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + 4), &Scaleresult.second, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 128), &Scaleresult.second, sizeof(int), 0);
            XtoScaleVal = Scaleresult.first;
            YtoScaleVal = Scaleresult.second;
            menu();
        }

        //scale down rse
        if (GetAsyncKeyState(VK_SUBTRACT))
        {
            scale -= 5;
            std::pair<int, int> Scaleresult = resScale(XtoScaleValDef, YtoScaleValDef);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 132), &Scaleresult.first, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress + 4), &Scaleresult.second, sizeof(int), 0);
            WriteProcessMemory(processHandle, (LPVOID)(XtoScaleAddress - 128), &Scaleresult.second, sizeof(int), 0);
            XtoScaleVal = Scaleresult.first;
            YtoScaleVal = Scaleresult.second;
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

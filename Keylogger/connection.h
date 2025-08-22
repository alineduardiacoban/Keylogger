#ifndef CONNECTION_H
#define CONNECTION_H

#include <winsock2.h>

extern SOCKET ConnectSocket;

bool clientConnection();
BOOL WINAPI ConsoleHandler(DWORD signal);
void cleanupConnection();

#endif

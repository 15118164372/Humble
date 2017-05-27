
#ifndef H_CMD_H_
#define H_CMD_H_

#include "../LTask.h"

H_BNAMSP

bool cmdMode(const int &argc, char *argv[]);
void onCommand(H_SOCK &cmdSock);
bool sendCmd(H_SOCK sock, const char *pszCommand, const char *pszTask, const char *pszMsg);

H_ENAMSP

#endif//H_CMD_H_

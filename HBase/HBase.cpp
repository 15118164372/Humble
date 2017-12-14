
#include "HBase.h"

std::string g_strProPath;
std::string g_strScriptPath;

pthread_cond_t g_ExitCond;
pthread_mutex_t g_objExitMu;

int g_iSVId;
int g_iSVType;

std::string g_strRPCKey;

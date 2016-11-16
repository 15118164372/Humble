
#ifndef H_BASE_H_
#define H_BASE_H_

#include "Atomic.h"
#include "RWLock.h"
#include "LockThis.h"
#include "Thread.h"

#include "AES.h"
#include "Base64.h"
#include "CRC.h"
#include "md5.h"
#include "RSAKey.h"
#include "HRSA.h"
#include "ZEncrypt.h"
#include "UEncrypt.h"
#include "SHA1.h"

#include "Ini.h"
#include "TableFile.h"
#include "Library.h"
#include "Binary.h"
#include "Funcs.h"
#include "Singleton.h"
#include "SnowFlake.h"
#include "Clock.h"
#include "Charset.h"
#include "NETAddr.h"
#include "CoreDump.h"
#include "Chan.h"

#include "EvBuffer.h"
#include "NetWorker.h"
#include "Sender.h"
#include "Log.h"
#include "Linker.h"
#include "WorkerDisp.h"
#include "Tick.h"
#include "DefParser.h"
#include "Tcp1.h"
#include "Tcp2.h"
#include "Httpd.h"
#include "WebSock.h"
#include "mqtt.h"
#include "NetParser.h"

extern std::string g_strProPath;
extern std::string g_strScriptPath;

extern pthread_cond_t g_ExitCond;
extern pthread_mutex_t g_objExitMu;

#endif//H_BASE_H_

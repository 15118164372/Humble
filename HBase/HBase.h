
#ifndef H_BASE_H_
#define H_BASE_H_

#include "Atomic.h"
#include "RWLock.h"
#include "LockThis.h"
#include "Thread.h"

#include "AOI.h"
#include "AStar.h"
#include "ConsistentHash.h"

#include "AES.h"
#include "HDES.h"
#include "Base64.h"
#include "CRC.h"
#include "md5/md5.h"
#include "RSAKey.h"
#include "HRSA.h"
#include "ZEncrypt.h"
#include "UEncrypt.h"
#include "SHA1.h"

#include "Ini.h"
#include "TableFile.h"
#include "Library.h"
#include "Funcs.h"
#include "Singleton.h"
#include "SnowFlake.h"
#include "Clock.h"
#include "NETAddr.h"
#include "CoreDump.h"
#include "HMySQL.h"
#include "MMap.h"

#include "Httpd.h"
#include "Tcp1.h"
#include "Tcp2.h"
#include "WebSocket.h"
#include "MQTT.h"

#include "EvBuffer.h"
#include "NetParser.h"
#include "NetListener.h"
#include "NetWorkerMgr.h"
#include "Tick.h"
#include "Log.h"
#include "Linker.h"
#include "Sender.h"
#include "TaskGlobleQu.h"
#include "TaskMgr.h"
#include "MSGDispatch.h"
#include "RPCLink.h"
#include "HMailer.h"

extern std::string g_strProPath;
extern std::string g_strScriptPath;

extern pthread_cond_t g_ExitCond;
extern pthread_mutex_t g_objExitMu;

extern int g_iSVId;
extern int g_iSVType;

#endif//H_BASE_H_

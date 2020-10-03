#ifndef _WIN_LINUX_H_
#define _WIN_LINUX_H_

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "fcntl.h"
#include "sys/stat.h"

#ifndef ANDROID
#include "sys/ipc.h"
#include <sys/sem.h>
#include "sys/shm.h"
#endif

#include "dirent.h"
#include "sys/stat.h"
#include "unistd.h"
#include <dlfcn.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <dlfcn.h>
#include <pthread.h>

#define NOERROR             0

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif 

#ifndef SOCKET
#define SOCKET int
#endif

#define TRUE 1
#define FALSE 0
#define MAX_PATH  260

typedef char TFName [64];
#define WINCHAR_POINT void

#define closesocket close
#define FreeLibrary dlclose
#define ThreadType void*
#else

#pragma warning(disable: 4996)
#include <WinSock2.h>
#include <windows.h>
#include <process.h>

#define usleep(a)  Sleep(a/1000)

#define strncasecmp strnicmp        


typedef int pid_t;
typedef int key_t;
typedef HANDLE pthread_t;
typedef int pthread_attr_t;
typedef int pthread_cond_t;
typedef int socklen_t;


typedef CRITICAL_SECTION pthread_mutex_t;
static void pthread_mutex_init(pthread_mutex_t* phMutex, void* pData)
{
	InitializeCriticalSection(phMutex);
}
static int pthread_mutex_lock(pthread_mutex_t* phMutex)
{
	EnterCriticalSection(phMutex);  
	return 0;
};

static int pthread_mutex_unlock(pthread_mutex_t* phMutex)
{
	LeaveCriticalSection(phMutex); 
	return 0;
}
static void pthread_mutex_destroy(pthread_mutex_t* phMutex)
{
	DeleteCriticalSection(phMutex);
}

#define ThreadType     UINT WINAPI
typedef unsigned int (__stdcall *thread_func)(void*);

static HANDLE pthread_create(pthread_t *pid, pthread_attr_t *pstAttr, 
						  thread_func ThreadProc, void* arglist)
{
	HANDLE hThread;
	hThread = (HANDLE)_beginthreadex(0, 0, ThreadProc, (void *)arglist, 0, nullptr);
	return hThread;
}
static int pthread_self()
{
    return GetCurrentThreadId();
}

#endif


#ifndef WIN32

typedef char WCHAR;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned int UINT;

#define nullptr NULL
typedef long long __int64;
typedef int INT;


#endif
#endif


// libevent_http.cpp : 定义控制台应用程序的入口点。
//

#include <SDKDDKVer.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <evhttp.h>
#include "../codequeue.h"

class CHttpServerHandle 
{
public:
	CHttpServerHandle();
	~CHttpServerHandle();

	bool IsToBeBlocked();
	int PrepareToRun();
	int Run();

	int GetHeadCode(unsigned char *pOutCode, unsigned int nOutMaxBufLen)
	{
		if (mHttpCallBackQueue == NULL)
		{
			return 0;
		}
		return mHttpCallBackQueue->GetHeadCode(pOutCode, nOutMaxBufLen);
	}

private:
	static void root_handler(struct evhttp_request *req, void *arg);
	static void generic_handler(struct evhttp_request *req, void *arg);

private:
	static CCodeQueue* mHttpCallBackQueue;
	struct evhttp *httpd;
};

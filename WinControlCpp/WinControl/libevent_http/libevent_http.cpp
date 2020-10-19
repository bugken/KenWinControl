#include "stdafx.h"
#include "libevent_http.h"
#include "../LogFile.h"
#include "json/reader.h"

CHttpServerHandle::CHttpServerHandle()
{
	httpd = NULL;
}

CHttpServerHandle::~CHttpServerHandle()
{
}

void CHttpServerHandle::RootHandler(struct evhttp_request *req, void *arg)
{
	struct evbuffer *buf = evbuffer_new();
	if (!buf)
	{
		//puts("failed to create response buffer");
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}

	char* decodeuri = evhttp_decode_uri(req->uri);

	/*处理数据*/

	const char* requri = evhttp_request_uri(req);
	evbuffer_add_printf(buf, "Hello: %s\n", requri);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);

	evbuffer_free(buf);
}

void CHttpServerHandle::GenericHandler(struct evhttp_request *req, void *arg)
{
	struct evbuffer *buf = evbuffer_new();
	if (!buf)
	{
		//puts("failed to create response buffer");
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}
	const char* requri = evhttp_request_uri(req);
	//LOG_TOFILE("paycallback.log", "%s callback is \n%s\nrequesturi:%s\n", __FUNCTION__, req->uri, requri);

	evbuffer_add_printf(buf, "Requested: %s\n", requri);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);

	evbuffer_free(buf);
}

int CHttpServerHandle::ServerInit()
{
	WSADATA wsaData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		GetLogFileHandle().ErrorLog("%s %d WSAStartup failed with error %d\n", __FUNCTION__, __LINE__, Ret);
		return -1;
	}
	event_init();
	httpd = evhttp_start("0.0.0.0", 9002);
	if (!httpd)
	{
		printf("httpd start error\n");
		GetLogFileHandle().ErrorLog("%s %d httpd start error \n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int CHttpServerHandle::Run()
{
	evhttp_set_cb(httpd, "/", RootHandler, NULL);
	evhttp_set_gencb(httpd, GenericHandler, NULL);
	printf("httpd(%d) server start OK!\n", GetCurrentThreadId());
	GetLogFileHandle().ErrorLog("%s %d httpd server start OK!\n", __FUNCTION__, __LINE__);
	event_dispatch();
	evhttp_free(httpd);
	WSACleanup();
	return 0;
}

void CHttpServerHandle::ServerStart()
{
	ServerInit();
	Run();
}


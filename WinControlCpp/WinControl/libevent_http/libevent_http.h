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

class CHttpServerHandle 
{
public:
	CHttpServerHandle();
	~CHttpServerHandle();

	void ServerStart();

private:
	static void RootHandler(struct evhttp_request *req, void *arg);
	static void GenericHandler(struct evhttp_request *req, void *arg);
	int ServerInit();
	int Run();

	struct evhttp *httpd;
};

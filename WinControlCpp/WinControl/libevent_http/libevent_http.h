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
#include <string>

using namespace std;

class CHttpServerHandle 
{
public:
	CHttpServerHandle();
	~CHttpServerHandle();
	
	void ServerStart();

private:
	//初始化Http
	int ServerInit();
	//启动Http线程
	int Run();


	//Http其他方式使用HttpDefaultHandler处理
	static void HttpDefaultHandler(string &strDataBuffer, struct evhttp_request *pEvHttpRequest, void *pArg);
	//Get方法处理函数
	static void HttpGetHandler(string& strDataBuffer, struct evhttp_request *pEvHttpRequest, void *pArg);
	//将不同的Http request请求分发到不同的函数处理
	static void WebRequestDispatch(UINT32 uiID, Json::Value& jsonJarg, string& strReplyBuffer);
	//处理Web的request请求并得到返回结果
	static void WebRequestParser(string& strRequestBuffer, string& strReplyBuffer);
	//获取post的json数据
	static void GetHttpPostData(string& strDataBuffer, struct evhttp_request *pEvHttpRequest, void *pArg);
	//Post方法处理函数
	static void HttpPostHandler(string& strDataBuffer, struct evhttp_request *pEvHttpRequest, void *pArg);
	//特定URI回调函数
	static void HttpRootHandler(struct evhttp_request *pEvHttpRequest, void *pArg);
	//没有被特定URI回调则调用此函数 
	static void HttpGenericHandler(struct evhttp_request *pEvHttpRequest, void *pArg);

private:
	struct evhttp *m_pEvHttpd;
};

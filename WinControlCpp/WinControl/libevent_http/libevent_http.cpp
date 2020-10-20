#include "stdafx.h"
#include "libevent_http.h"
#include "../LogFile.h"
#include "json/reader.h"
#include "../HttpRequestProcess.h"
#include <string>

using namespace std;

CHttpServerHandle::CHttpServerHandle()
{
	m_pEvHttpd = NULL;
}

CHttpServerHandle::~CHttpServerHandle()
{
}

//Http其他方式使用HttpDefaultHandler处理
void CHttpServerHandle::HttpDefaultHandler(string& strReplyBuffer, struct evhttp_request *pEvHttpRequest, void *pArg)
{
	Json::Value jsonReply;
	jsonReply["ret"] = -1;
	jsonReply["error_message"] = string_To_UTF8("http post method is only supported").c_str();

	strReplyBuffer = jsonReply.toStyledString();
}

//Get方法处理函数
void CHttpServerHandle::HttpGetHandler(string& strReplyBuffer, struct evhttp_request *pEvHttpRequest, void *pArg)
{
	Json::Value jsonReply;
	jsonReply["ret"] = -1;
	jsonReply["error_message"] = string_To_UTF8("http get method is not supported").c_str();

	strReplyBuffer = jsonReply.toStyledString();
}

//将不同的Http request请求分发到不同的函数处理
void CHttpServerHandle::WebRequestDispatch(UINT32 uiID, Json::Value& jsonJarg, string& strReplyBuffer)
{
	switch (uiID)
	{
	case HTTP_USERSNUMONLINE:
		CHttpProcessor::Proc_GetUsersNumOnline(jsonJarg, strReplyBuffer);
		break;
	default:
		CHttpProcessor::Proc_HttpDefaultProcessor(jsonJarg, strReplyBuffer);
		break;
	}
}

//处理Web的request请求并得到返回结果
void CHttpServerHandle::WebRequestParser(string& strRequestBuffer, string& strReplyBuffer)
{
	std::string strError;
	Json::Value jsonReply;
	try
	{
		Json::Reader jsonReader;
		Json::Value jsonDataValue;
		if (jsonReader.parse(strRequestBuffer, jsonDataValue))
		{
			UINT32 uiID = jsonDataValue["id"].asInt();
			Json::Value jsonJarg = jsonDataValue["arg"];
			WebRequestDispatch(uiID, jsonJarg, strReplyBuffer);
		}
		else
		{
			jsonReply["ret"] = -1;
			jsonReply["error_message"] = string_To_UTF8("wrong json data.").c_str();
			strReplyBuffer = jsonReply.toStyledString();
		}
	}
	catch (const char* e)
	{
		strError = e;
		jsonReply["ret"] = -1;
		jsonReply["error_message"] = string_To_UTF8(strError).c_str();
		strReplyBuffer = jsonReply.toStyledString();
	}
}

//获取post的json数据
void CHttpServerHandle::GetHttpPostData(string& strDataBuffer, struct evhttp_request *pEvHttpRequest, void *pArg)
{
	struct evbuffer* pEvBuffer = evhttp_request_get_input_buffer(pEvHttpRequest);
	UINT32 uiLeft = evbuffer_get_length(pEvBuffer);
	while (left > 0)
	{
		char szBuffer[512] = { 0 };
		int hasRead = evbuffer_remove(pEvBuffer, szBuffer, 512);
		if (hasRead > 0)
		{
			uiLeft -= static_cast<UINT32>(hasRead);
			strDataBuffer.append(szBuffer);
		}
		else
		{
			break;
		}
	}
}

//Post方法处理函数
void CHttpServerHandle::HttpPostHandler(string& strReplyBuffer, struct evhttp_request *pEvHttpRequest, void *pArg)
{
	string strRequestBuffer;
	GetHttpPostData(strRequestBuffer, pEvHttpRequest, pArg);
	if (strRequestBuffer.length() <= 0)
	{
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}
	GetLogFileHandle().InfoLog("%s %d get request from web:%s\n", __FUNCTION__, __LINE__, strRequestBuffer.c_str());

	WebRequestParser(strRequestBuffer, strReplyBuffer);
}

//特定URI回调函数
void CHttpServerHandle::HttpRootHandler(struct evhttp_request *pEvHttpRequest, void *pArg)
{
	struct evbuffer *pEvBuffer = evbuffer_new();
	if (!pEvBuffer)
	{
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}

	string strReplyBuffer;
	switch (evhttp_request_get_command(pEvHttpRequest))
	{
	case EVHTTP_REQ_GET:
		HttpGetHandler(strReplyBuffer, pEvHttpRequest, pArg);
		break;
	case EVHTTP_REQ_POST:
		HttpPostHandler(strReplyBuffer, pEvHttpRequest, pArg);
		break;
	default:
		HttpDefaultHandler(strReplyBuffer, pEvHttpRequest, pArg);
		break;
	}
	GetLogFileHandle().InfoLog("%s %d reply to web:%s\n", __FUNCTION__, __LINE__, strReplyBuffer.c_str());

	evbuffer_add_printf(pEvBuffer, strReplyBuffer.c_str());
	evhttp_send_reply(pEvHttpRequest, HTTP_OK, "OK", pEvBuffer);
	evbuffer_free(pEvBuffer);
}

//没有被特定URI回调则调用此函数 
void CHttpServerHandle::HttpGenericHandler(struct evhttp_request *pEvHttpRequest, void *pArg)
{
	struct evbuffer *buf = evbuffer_new();
	if (!buf)
	{
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}

	const char* requri = evhttp_request_uri(pEvHttpRequest);
	evbuffer_add_printf(buf, "Requested: %s\n", requri);
	evhttp_send_reply(pEvHttpRequest, HTTP_OK, "OK", buf);

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
	m_pEvHttpd = evhttp_start("0.0.0.0", 9002);
	if (!m_pEvHttpd)
	{
		printf("httpd start error\n");
		GetLogFileHandle().ErrorLog("%s %d httpd start error \n", __FUNCTION__, __LINE__);
		return -1;
	}

	return 0;
}

int CHttpServerHandle::Run()
{
	evhttp_set_cb(m_pEvHttpd, "/", HttpRootHandler, NULL);
	evhttp_set_gencb(m_pEvHttpd, HttpGenericHandler, NULL);
	printf("http server %d start OK!\n", GetCurrentThreadId());
	GetLogFileHandle().InfoLog("%s %d httpd server start OK!\n", __FUNCTION__, __LINE__);
	event_dispatch();
	evhttp_free(m_pEvHttpd);
	WSACleanup();
	return 0;
}

void CHttpServerHandle::ServerStart()
{
	ServerInit();
	Run();
}


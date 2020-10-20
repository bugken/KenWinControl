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
		GetLogFileHandle().ErrorLog("%s %d failed to create response buffer\n", __FUNCTION__, __LINE__);
		return;
	}

	Json::Value jsonReply;
	jsonReply["ret"] = -1;
	jsonReply["error_message"] = string_To_UTF8("unknown error").c_str();
	std::string strError;
	switch(evhttp_request_get_command(req))
	{
	case EVHTTP_REQ_GET:
		jsonReply["ret"] = -1;
		jsonReply["error_message"] = string_To_UTF8("This request method is not supported").c_str();
		evbuffer_add_printf(buf, jsonReply.toStyledString().c_str());
		break;
	case EVHTTP_REQ_POST:
		struct evbuffer* postBuffer = evhttp_request_get_input_buffer(req);
		size_t left = evbuffer_get_length(postBuffer);
		string buffer;
		while (left > 0)
		{
			char cbuffer[512] = { 0 };
			int hasRead = evbuffer_remove(postBuffer, cbuffer, 512);
			if (hasRead != -1)
			{
				left -= static_cast<size_t>(hasRead);
				buffer.append(cbuffer);
			}
			else
			{
				break;
			}
		}
		try
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(buffer, root))
			{
				int id = root["id"].asInt();
				Json::Value jarg = root["arg"];
				jsonReply["ret"] = 0;
				jsonReply["error_message"] = string_To_UTF8("").c_str();;
			}
			else
			{
				jsonReply["ret"] = -1;
				jsonReply["error_message"] = string_To_UTF8("wrong data").c_str();
			}
		}
		catch (const char* e)
		{
			strError = e;
			jsonReply["ret"] = -1;
			jsonReply["error_message"] = string_To_UTF8(strError).c_str();
		}
		evbuffer_add_printf(buf, jsonReply.toStyledString().c_str());
		break;
	}

	const char* requri = evhttp_request_uri(req);
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


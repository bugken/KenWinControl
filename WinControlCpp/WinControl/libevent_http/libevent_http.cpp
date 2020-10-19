#include "stdafx.h"
#include "libevent_http.h"
#include "../LogFile.h"
#include "json/reader.h"

CCodeQueue* CHttpServerHandle::mHttpCallBackQueue = NULL;

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

#ifdef _DEBUG
	//char* tempuri = "type=alipay&ret=%7B%22gmt_create%22%3A%222018-04-02+20%3A13%3A05%22%2C%22charset%22%3A%22UTF-8%22%2C%22seller_email%22%3A%22zjzlkj%40189.cn%22%2C%22subject%22%3A%2226300.6jinbin%22%2C%22sign%22%3A%22f9NtRRMHByhOJgCVsWXKrWFzKAE6qZAMzBl7o8b%5C%2FCnso%5C%2FSmeCE5ukTgbEKHUn5zqiApzeASxkXm8Y8oOZfT15u%5C%2Fj0MND69RJZkMH4ddEL%5C%2FTaAux0LkXOomte3X8lFGcAfo7f993myflwV3s5oJB4xDiLxGiV3YuNt8qB4Vhhma3Obu33zwXLq%2BWlXa0J0qd6Zm9tROwvr39AlOBI4bMh2VHBsyzRcRCMroMD%2BfJWvt6VQetdIkVi%5C%2FZm182yndEwrLIzzphU6EEHGl28VDNmhO8PJcafb6ooTpYr4%2BD1pmVpgM9QmxIVTY6vuxzPWIsZgW%2Bsh2nM6GbJvK82xAPuKVg%3D%3D%22%2C%22body%22%3A%2226300+alipay+test%22%2C%22buyer_id%22%3A%222088032547577583%22%2C%22invoice_amount%22%3A%220.01%22%2C%22notify_id%22%3A%22fc5fb976c327bcfcb511ac1688cbedbkh9%22%2C%22fund_bill_list%22%3A%22%5B%7B%5C%22amount%5C%22%3A%5C%220.01%5C%22%2C%5C%22fundChannel%5C%22%3A%5C%22ALIPAYACCOUNT%5C%22%7D%5D%22%2C%22notify_type%22%3A%22trade_status_sync%22%2C%22trade_status%22%3A%22TRADE_SUCCESS%22%2C%22receipt_amount%22%3A%220.01%22%2C%22app_id%22%3A%222018033002475139%22%2C%22buyer_pay_amount%22%3A%220.01%22%2C%22sign_type%22%3A%22RSA2%22%2C%22seller_id%22%3A%222088821502772314%22%2C%22gmt_payment%22%3A%222018-04-02+20%3A13%3A06%22%2C%22notify_time%22%3A%222018-04-02+20%3A13%3A06%22%2C%22version%22%3A%221.0%22%2C%22out_trade_no%22%3A%22trade_1522671179%22%2C%22total_amount%22%3A%220.01%22%2C%22trade_no%22%3A%222018040221001004580505532993%22%2C%22auth_app_id%22%3A%222018033002475139%22%2C%22buyer_logon_id%22%3A%22153%2A%2A%2A%2A6063%22%2C%22point_amount%22%3A%220.00%22%7D";
	//tempuri = "/?type=wxpay&ret=%7B%22appid%22%3A%22wx852abc76c37554e5%22%2C%22attach%22%3A%22test%22%2C%22bank_type%22%3A%22CFT%22%2C%22cash_fee%22%3A%221%22%2C%22fee_type%22%3A%22CNY%22%2C%22is_subscribe%22%3A%22N%22%2C%22mch_id%22%3A%221501371651%22%2C%22nonce_str%22%3A%22R9mQ8EZGThzwE2Wn%22%2C%22openid%22%3A%22osm0q1tTUkOwpSu3-7epcrjqY1_0%22%2C%22out_trade_no%22%3A%22150137165120180409141439%22%2C%22result_code%22%3A%22SUCCESS%22%2C%22return_code%22%3A%22SUCCESS%22%2C%22return_msg%22%3A%22OK%22%2C%22sign%22%3A%227C6A3A6FE7EB171B0826B85383847A7D%22%2C%22time_end%22%3A%2220180409141453%22%2C%22total_fee%22%3A%221%22%2C%22trade_state%22%3A%22SUCCESS%22%2C%22trade_state_desc%22%3A%22%5Cu652f%5Cu4ed8%5Cu6210%5Cu529f%22%2C%22trade_type%22%3A%22NATIVE%22%2C%22transaction_id%22%3A%224200000087201804095199561116%22%7D";
	//decodeuri = evhttp_decode_uri(tempuri);
#endif

	int ret = mHttpCallBackQueue->AppendOneCode((const BYTE*)decodeuri, strlen(decodeuri) + 1);
	if (ret < 0)
	{
		return;
	}
	
	//LOG_TOFILE("paycallback.log", "%s callback is \n%s\nrequesturi:%s\n", __FUNCTION__, req->uri, decodeuri);
	
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
	mHttpCallBackQueue = new CCodeQueue(4 * 1024);
	return 0;
}

int CHttpServerHandle::Run()
{
	evhttp_set_cb(httpd, "/", RootHandler, NULL);
	evhttp_set_gencb(httpd, GenericHandler, NULL);
	printf("httpd server start OK!\n");
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


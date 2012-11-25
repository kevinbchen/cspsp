#ifndef _HTTPMANAGER_H_
#define _HTTPMANAGER_H_

#include "Wlan.h"
#include <deque>
#include <string>
#include <string.h>

#define CONTENT_UNKNOWN 0
#define CONTENT_CHUNKED 1
#define CONTENT_NORMAL 2

#define REQUEST_GET 0
#define REQUEST_POST 1
struct Request 
{
	int type;
	char page[4096];
	char data[4096];
};

struct Response 
{
	char buffer[8192];
	int size;
};

//------------------------------------------------------------------------------------------------
class HttpManager
{
private:
	char mHostText[128];
	char mBuffer[8192];

	std::deque<Request> mRequests;
	std::deque<Response> mResponses;
	Socket* mSocket;
	float mTimer;
	float mReconnectTimer;
	char mHost[128];
	int mPort;
	int mContentSize;
	int mBufferSize;
	int mContentType;
	bool ReadHTTP(char* string, int n);

public:
	HttpManager();
	~HttpManager();

	void Connect(char* host, char* hosttext, int port);
	void Reconnect();
	void Disconnect();
	void SendRequest(char* page, char* data="", int type=REQUEST_GET);
	int GetResponse(char* buffer);
	int GetContentSize();
	bool ResponseReady();
	void ClearRequests();

	void Update(float dt);
};

#endif

#ifndef _WLAN_H_
#define _WLAN_H_

#ifdef WIN32
#include "JGE.h"
#include <stdio.h> // For perror() call
#include <stdlib.h> // For exit() call
#include <windows.h>
#include <winsock.h> // Include Winsock Library
#include <iostream>

#define bzero(p, l) memset(p, 0, l)
#define bcopy(s, t, l) memmove(t, s, l)
#else
#include <stdlib.h>
#include <pspsdk.h>
#include <psputility.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspwlan.h>
#include <netinet/in.h>
#include <pspnet_resolver.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h> 
#endif

#include <vector>

#define MAX_PICK 100

struct ConnectionConfig {
	char name[64];
	int index;
};

class Socket
{
public:
	int sock;
	struct sockaddr_in addrTo;
	struct sockaddr_in addr;
	bool serverSocket;
	//Socket():  {};
};


static const char* wlanNotInitialized = "WLAN not initialized.";
static bool wlanInitialized = false;
static char resolverBuffer[1024];
static int resolverId;

int WlanInit();
int WlanTerm();
std::vector<ConnectionConfig> GetConnectionConfigs();
int UseConnectionConfig(int config, int state);
char* GetIPAddress();
int SocketFree(Socket* socket);
int SetSockNoBlock(int s, u32 val);
int SocketConnect(Socket* socket, char* host, int port);
int SocketConnectUdp(Socket* socket, char* host, int port);
int SocketRecv(Socket* socket, char* buf, int size);
//sockaddr SocketRecvfrom(Socket* socket, char* buf, int size);
int SocketSend(Socket* socket, char* buf, int size);
int SocketSendUdp(Socket* socket, char* buf, int size);
int SocketClose(Socket* socket);

#endif

#include "Wlan.h"

#ifdef WIN32
int WlanInit()
{
	//if (wlanInitialized) return 0;
	WSADATA wsaData;	// Windows socket

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2,2), &wsaData) == SOCKET_ERROR) {
	}

	wlanInitialized = true;
	return 0;
}

int WlanTerm()
{
	if (!wlanInitialized) return 0;
	WSACleanup();
	wlanInitialized = false;
	return 0;
}

int SocketFree(Socket* socket)
{
	closesocket(socket->sock);
	delete socket;
	return 0;
}

int SetSockNoBlock(int s, u32 val)
{ 
	unsigned long nonblocking = 1;
	ioctlsocket(s, FIONBIO, &nonblocking);

    return 1;
}

int SocketConnect(Socket* socket1, char* host, int port)
{
	
	//if (!wlanInitialized) return 0;
	
	socket1->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (socket1->sock < 0) ;//error("socket");

	struct hostent *hp;
	hp = gethostbyname(host);

	socket1->addrTo.sin_family = AF_INET;
	socket1->addrTo.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)hp->h_addr));
	socket1->addrTo.sin_port = htons(port);

	SetSockNoBlock(socket1->sock, 1);

	connect(socket1->sock, (struct sockaddr *)&socket1->addrTo, sizeof socket1->addrTo);

	return 1;
}

int SocketConnectUdp(Socket* socket1, char* host, int port)
{
	
	//if (!wlanInitialized) return 0;
	
	socket1->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket1->sock < 0) ;//error("socket");

	socket1->addrTo.sin_family = AF_INET;

	struct hostent *hp;
	hp = gethostbyname(host);

	bcopy((char *)hp->h_addr, (char *)&socket1->addrTo.sin_addr, hp->h_length);

	socket1->addrTo.sin_port = htons(port);

	SetSockNoBlock(socket1->sock, 1);
	//unsigned long nonblocking = 1;
	//ioctlsocket(socket1->sock, FIONBIO, &nonblocking);

	return 1;
}

int SocketRecv(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) {} //return 0;

	//if (socket->serverSocket) {}//return luaL_error(L, "recv not allowed for server sockets.");

	struct sockaddr_in addrFrom;
	unsigned int len = sizeof (addrFrom); 

	int count = recv(socket->sock, buf, size, 0);
	return count;
	/*if (count > 0) {
		return count;
	} else {
		return 0;
	}*/
}

int SocketSend(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) return 0;

	int n = send(socket->sock,buf,size,0);

	return n;
}


int SocketSendUdp(Socket* socket, char* buf, int size)
{
	//if (!wlanInitialized) return 0;

    int total = 0;        // how many bytes we've sent
    int bytesleft = size; // how many we have left to send
    int n;

    while(total < size) {
        n = sendto(socket->sock, buf+total, bytesleft, 0, (sockaddr*)&(socket->addrTo),sizeof(socket->addrTo));
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

	return 1;
}

int SocketClose(Socket* socket)
{
	//if (!wlanInitialized) return 0;

	closesocket(socket->sock);
	return 1;
}

#else
int WlanInit()
{
	if (wlanInitialized) return 0;
	//pspSdkInetInit();
	int err = sceNetResolverCreate(&resolverId, resolverBuffer, sizeof(resolverBuffer));
	wlanInitialized = true;
	return 0;
}

int WlanTerm()
{
	// TODO: doesn't term; another call to pspSdkInetInit fails
	if (!wlanInitialized) return 0;
	if (resolverId >= 0) {
		sceNetResolverDelete(resolverId); 
	}
	sceNetApctlDisconnect();
	//pspSdkInetTerm();
	wlanInitialized = false;
	return 0;
}

std::vector<ConnectionConfig> GetConnectionConfigs()
{
	std::vector<ConnectionConfig> connections;
	//if (!wlanInitialized) return;

	struct
	{
		int index;
		char name[64];
	} picks[MAX_PICK];
	int pick_count = 0;
	
	int iNetIndex;
	for (iNetIndex = 1; iNetIndex < 100; iNetIndex++) // skip the 0th connection
	{
		if (sceUtilityCheckNetParam(iNetIndex) != 0) continue;  // no more
		sceUtilityGetNetParam(iNetIndex, 0, (netData*) picks[pick_count].name);
		picks[pick_count].index = iNetIndex;
		
		ConnectionConfig config = ConnectionConfig();
		config.index = iNetIndex;
		strcpy(config.name,picks[pick_count].name);
		
		connections.push_back(config);

		//connections.push_back(picks[pick_count].name);


		pick_count++;
		if (pick_count >= MAX_PICK) break;  // no more room
	}
	return connections;
}

int UseConnectionConfig(int config, int state)
{
	if (state == 1 || state == 0  || state == -1) {
		//if (!wlanInitialized) return 0;
		
		if (sceWlanGetSwitchState() != 1) return -1;

		if (state == 0) return 0;

		int result = sceNetApctlConnect(config); 

		return 2;
	}
	else {
		
		//int state = 0; 
		
		//while (1) { 
			sceKernelDelayThread(50*1000); // 200ms 
			
			int err = sceNetApctlGetState(&state); 
			if (err != 0 || state == 0) {
				// conncection failed
				return 0; 
			}
			
			return state;
			/*if (state == 4) {
				//connection succeeded 
				result = 1; 
				break; 
			} */
		//} 
		
		//return 1; 
	}
}

/*char* GetIPAddress()
{
	//if (!wlanInitialized) return;

	char szMyIPAddr[32];
	if (sceNetApctlGetInfo(8, szMyIPAddr) != 0) //return;
	return szMyIPAddr;
}*/

int SocketFree(Socket* socket)
{
	sceNetInetClose(socket->sock);
	delete socket;
	return 0;
}

int SetSockNoBlock(int s, u32 val)
{ 
    return sceNetInetSetsockopt(s, SOL_SOCKET, 0x1009, (const char*)&val, sizeof(u32));
}


int SocketConnect(Socket* socket, char* host, int port)
{
	//FILE *file = fopen("error.log", "r+"); 
	//fseek (file,0,SEEK_END);

	if (!wlanInitialized) return 0;
	
	sceKernelDelayThread(50*1000); // without this delay it doesn't work sometime

	//Socket** luaSocket = pushSocket(L);
	//socket = (Socket*)malloc(sizeof(Socket));

	//socket->serverSocket = false;

	// resolve host
	socket->addrTo.sin_family = AF_INET;
	socket->addrTo.sin_port = htons(port);


	int err = sceNetInetInetAton(host, &socket->addrTo.sin_addr);

	/*struct hostent *hp; 
	hp = gethostbyname(host);      
	socket->addrTo.sin_addr = *(struct in_addr *)hp->h_addr; */


	/*if (err == 0) {
		err = sceNetResolverStartNtoA(resolverId, host, &socket->addrTo.sin_addr, 2, 3);
		//fputs(inet_ntoa(socket->addrTo.sin_addr),file);
		//if (err < 0) return 0;
	}*/

	// create non-blocking socket	
	socket->sock = sceNetInetSocket(AF_INET, SOCK_STREAM, 0);
	if (socket->sock & 0x80000000) {
		return 0; 
	}
	
	SetSockNoBlock(socket->sock, 1);

    err = sceNetInetConnect(socket->sock, (struct sockaddr*)&socket->addrTo, sizeof(socket->addrTo));

	//fclose(file);

	return 1;
}


int SocketConnectUdp(Socket* socket, char* host, int port)
{
	//FILE *file = fopen("error.log", "r+"); 
	//fseek (file,0,SEEK_END);

	//if (file == NULL) return 0;



	if (!wlanInitialized) return 0;
	
	sceKernelDelayThread(50*1000); // without this delay it doesn't work sometime

	//Socket** luaSocket = pushSocket(L);
	//socket = (Socket*)malloc(sizeof(Socket));

	//socket->serverSocket = false;

	// resolve host
	socket->addrTo.sin_family = AF_INET;
	socket->addrTo.sin_port = htons(port);
	
	int err = sceNetInetInetAton(host, &socket->addrTo.sin_addr);

	//fputs("1\r\n",file);

	if (err == 0) {
		err = sceNetResolverStartNtoA(resolverId, host, &socket->addrTo.sin_addr, 2, 3);
		if (err < 0) return 0;
	}

    //fputs("2\r\n",file);

	// create non-blocking socket	
	socket->sock = sceNetInetSocket(AF_INET, SOCK_DGRAM, 0);
	if (socket->sock & 0x80000000) {
		return 0; 
	}
	
	SetSockNoBlock(socket->sock, 1);
	//fputs("3\r\n",file);

    err = sceNetInetBind(socket->sock, (struct sockaddr*)&socket->addrTo, sizeof(socket->addrTo));   
    


	//fputs("4\r\n",file);
	
	//fclose(file);

	return 1;
}


/*static int Socket_isConnected(lua_State *L)
{
	if (!wlanInitialized) return luaL_error(L, wlanNotInitialized);
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "no argument expected.");

	Socket* socket = *toSocket(L, 1);
	if (socket->serverSocket) {
		lua_pushboolean(L, 1);
		return 1;
	}

	// try connect again, which should always fail
	// look at why it failed to figure out if it is connected
	//REVIEW: a conceptually cleaner way to poll this?? (accept?)
	int err = sceNetInetConnect(socket->sock, &socket->addrTo, sizeof(socket->addrTo));
	if (err == 0 || (err == -1 && sceNetInetGetErrno() == 0x7F)) {
		// now connected - I hope
		lua_pushboolean(L, 1);
		return 1;
	}
	lua_pushboolean(L, 0);
	return 1;
}

static int Socket_createServerSocket(lua_State *L)
{
	if (!wlanInitialized) return luaL_error(L, wlanNotInitialized);
	int argc = lua_gettop(L); 
	if (argc != 1) return luaL_error(L, "port expected."); 
	
	int port = luaL_checkint(L, 1);

	Socket** luaSocket = pushSocket(L);
	Socket* socket = (Socket*) malloc(sizeof(Socket));
	*luaSocket = socket;
	socket->serverSocket = true;
	
        socket->sock = sceNetInetSocket(AF_INET, SOCK_STREAM, 0);
        if (socket->sock <= 0) {
		return luaL_error(L, "invalid socket."); 
        }

        socket->addrTo.sin_family = AF_INET;
        socket->addrTo.sin_port = htons(port);
        socket->addrTo.sin_addr = 0;

        int err = sceNetInetBind(socket->sock, &socket->addrTo, sizeof(socket->addrTo));
        if (err != 0) {
		return luaL_error(L, "bind error."); 
        }

	setSockNoBlock(socket->sock, 1);

        err = sceNetInetListen(socket->sock, 1);
        if (err != 0) {
		return luaL_error(L, "listen error."); 
        }
        
        return 1;
}

static int Socket_accept(lua_State *L)
{
	if (!wlanInitialized) return luaL_error(L, wlanNotInitialized);
	int argc = lua_gettop(L);
	if (argc != 1) return luaL_error(L, "no argument expected.");

	Socket* socket = *toSocket(L, 1);

	if (!socket->serverSocket) return luaL_error(L, "accept allowed for server sockets only.");

	// check for waiting incoming connections
        struct sockaddr_in addrAccept;
        int cbAddrAccept = sizeof(addrAccept);
        SOCKET sockClient = sceNetInetAccept(socket->sock, &addrAccept, &cbAddrAccept);
        if (sockClient <= 0) {
        	return 0;
        }

	// create new lua socket
	Socket** luaSocket = pushSocket(L);
	Socket* incomingSocket = (Socket*) malloc(sizeof(Socket));
	*luaSocket = incomingSocket;
	incomingSocket->serverSocket = false;
	incomingSocket->sock = sockClient;
	incomingSocket->addrTo = addrAccept;

	return 1;
}
*/

int SocketRecv(Socket* socket, char* buf, int size)
{
	if (!wlanInitialized) {} //return 0;

	if (socket->serverSocket) {}//return luaL_error(L, "recv not allowed for server sockets.");

	struct sockaddr_in addrFrom;
	unsigned int len = sizeof (addrFrom); 

	int count = sceNetInetRecv(socket->sock, buf, size, 0);
	return count;
	/*if (count > 0) {
		return count;
	} else {
		return 0;
	}*/
}

int SocketSend(Socket* socket, char* buf, int size)
{
	if (!wlanInitialized) return 0;

	int n = sceNetInetSend(socket->sock, buf, size, 0);

	return n;
}

int SocketSendUdp(Socket* socket, char* buf, int size)
{
	if (!wlanInitialized) return 0;

    int total = 0;        // how many bytes we've sent
    int bytesleft = size; // how many we have left to send
    int n;

    while(total < size) {
        n = sceNetInetSendto(socket->sock, buf+total, bytesleft, 0, (struct sockaddr*)&(socket->addrTo),sizeof(socket->addrTo));
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

	//sceNetInetSendto(socket->sock, buf, size, 0, (struct sockaddr*)&(socket->addrTo),sizeof(socket->addrTo));

	return 1;
}

int SocketClose(Socket* socket)
{
	if (!wlanInitialized) return 0;

	sceNetInetClose(socket->sock);
	return 1;
}
/*
static int Socket_tostring(lua_State *L)
{
	Socket* socket = *toSocket(L, 1);
	char buf[128];
	sprintf(buf, "%i.%i.%i.%i",
		socket->addrTo.sin_addr & 255,
		(socket->addrTo.sin_addr >> 8) & 255,
		(socket->addrTo.sin_addr >> 16) & 255,
		(socket->addrTo.sin_addr >> 24) & 255);
	lua_pushstring(L, buf);  // pushfstring doesn't work, returns userdata object
	return 1;
}

static const luaL_reg Socket_methods[] = {
	{"isConnected", Socket_isConnected},
	{"accept", Socket_accept},
	{"send", Socket_send},
	{"udpSend", Socket_send_udp},
	{"recv", Socket_recv},
	{"close", Socket_close},
	{0,0}
};

static const luaL_reg Socket_meta[] = {
	{"__gc", Socket_free},
	{"__tostring", Socket_tostring},
	{0,0}
};

UserdataRegister(Socket, Socket_methods, Socket_meta)

static const luaL_reg Wlan_functions[] = {
	{"init", Wlan_init},
	{"term", Wlan_term},
	{"getConnectionConfigs", Wlan_getConnectionConfigs},
	{"useConnectionConfig", Wlan_useConnectionConfig},
	{"getIPAddress", Wlan_getIPAddress},
	{0, 0}
};

static const luaL_reg Socket_functions[] = {
	{"connect", Socket_connect},
	{"createServerSocket", Socket_createServerSocket},
	{"udpConnect", Socket_connect_udp},
	{0, 0}
};

void luaWlan_init(lua_State *L)
{
	luaL_openlib(L, "Wlan", Wlan_functions, 0);
	Socket_register(L);
	luaL_openlib(L, "Socket", Socket_functions, 0);
}
*/
#endif
#ifndef _GAME_STATE_CONNECT_H_
#define _GAME_STATE_CONNECT_H_

#include "JGui.h"
#include "GameState.h"
#include <vector>
#include "Wlan.h"
#include "ListBox.h"

#ifdef WIN32
#else
#include <malloc.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <dirent.h>
#include <unistd.h>
#include <pspidstorage.h>
#include <pspopenpsid.h>

#include <pspnet.h> 
#include <pspnet_inet.h> 
#include <pspnet_apctl.h> 
#include <pspnet_resolver.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 

extern "C" {
	int ReadKey(int key, char *buffer);
	int CheckTempAR();
}
#endif

#define STAGE_SELECT 0
#define STAGE_CONNECTING 1
#define STAGE_LOGIN 2
#define STAGE_NEWACCOUNT 3
#define STAGE_NEWACCOUNTSUBMIT 4
#define STAGE_SUSPENDED 5
#define STAGE_MAINTENANCE 6
#define STAGE_ERROR 7
/*#define STAGE_CONNECTING 1
#define STAGE_TEXT 2
#define STAGE_UPDATESTART 3
#define STAGE_UPDATEFINISH 4
#define STAGE_UPDATEFAIL 5
#define STAGE_RETURN 6*/
static char instructions[] =
"If you already have an account, please enter your name and password \
to sign in.\n\n\
Otherwise, you can create an account. Make sure to remember your password\
though (also, *DO NOT* use a password that you use for anything else)!";

class ConnectionItem : public ListItem 
{
public:
	ConnectionConfig config;
	ConnectionItem(ConnectionConfig config) {
		this->config = config;
	}
	~ConnectionItem() {}

	void Render(float x, float y, bool selected) {
		gFont->DrawShadowedString(config.name,SCREEN_WIDTH_2,y+3,JGETEXT_CENTER);
	}
};


class GameStateConnect:	public GameState
{
private:
	//Socket* socket;
	char id[2000];
	char psid[128];
	char encodedKey[256];
	char name[16];
	char password[16];
	char* tempname;
	char* temppassword;
	int index;

	ListBox *mConnectionsListBox;

	int mStage;
	int mConnectState;
	int mConnectId;

	int mLoginStatus;
	std::vector<char*> mInstructions;

	int mSuspendHours;
	char mSuspendReason[64];

	char mConfigs[4][256];
	char mConfigInfo[4][256];
	float mInfoX;

	bool mArePluginsModified;
	bool mArePluginsLoaded;
	char mErrorString[64];



public:
	GameStateConnect(GameApp* parent);
	~GameStateConnect();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();

	int CheckLogin(char* buffer);
	int CheckNewAccount(char* buffer);

	void KillSwitch(char *dir);
};


#endif


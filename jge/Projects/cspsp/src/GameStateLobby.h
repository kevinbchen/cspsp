#ifndef _GAME_STATE_LOBBY_H_
#define _GAME_STATE_LOBBY_H_

#include <string>
#include <vector>
#include "JGui.h"
#include "GameState.h"
#include "MenuItem.h"
#include "Wlan.h"
#include "UdpManager.h"
#include "ListBox.h"

#ifdef WIN32
#else
#include <malloc.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <dirent.h>
#include <unistd.h>
#endif

#define STAGE_MENU 0
#define STAGE_NEWS 1
#define STAGE_SERVERS 2
#define STAGE_FRIENDS 3
#define STAGE_HELP 4
#define STAGE_PLAYERINFO 5
#define ERROR 6

#define SERVERSSTAGE_SERVERS 0
#define SERVERSSTAGE_FAVORITES 1

#define FRIENDSSTAGE_FRIENDS 0
#define FRIENDSSTAGE_PLAYERS 1

#define MAXPING 1000
#define MAXSOCKETS 1

#define UPDATETIME 3*60*1000

static char helptext[] =
"Welcome to CSPSP online!\n\
\n\
If this is your first time playing CSPSP, you will want to first learn the controls \
(check the -Controls- tab within the -settings- section of the main menu). Also, playing \
singleplayer with the bots can help you get familiar with the game.\n\
\n\
Here's a quick rundown of the sections in the online lobby:\n\
\n\
-servers-\n\
To begin playing CSPSP online, join any server in the list. Try to connect to servers with low ping \
- this usually means less lag. If you find a server you like, you can also add it to your \
favorites list.\n\
\n\
-friends-\n\
This is your friends list. You can add/remove friends and view their profiles. The -Players- tab \
lists everyone who is currently online.\n\
\n\
-news-\n\
This simply lists the 10 most recent news updates.\n\
\n\
-help-\n\
You are here :)\n\
\n\
\n\
Overall, try to be civil when playing; keep in mind that you can be banned from servers or suspended \
from multiplayer completely. Just keep it chill, and everything will be fine. Remember, it's just a \
game - don't get too angry or frustrated! Anyways, I hope you have a great time playing CSPSP online!\n\
\n\
For any questions, comments, suggestions, etc., check out the official forums at \
http://z4.invisionfree.com/CSPSP. \n\
Here are answers to a few frequently asked questions though:\n\
\n\
-Someone is cheating! What should I do?-\n\
Sometimes, perceived 'hacking' can really just be caused by lag. Even if you see a bullet hit, it \
doesn't necessarily mean it really did from the server's perspective. However, if you're positive \
that someone is indeed cheating, report it on the forums, along with an explanation of exactly what \
you saw.\n\
\n\
-How do I edit my profile or change my icon?-\n\
You can do that by visiting the CSPSP site at http://cspsp.appspot.com. Simply login and click the \
'edit profile' link at the top of the page.\n\
\n\
-How do I get a clan tag?-\n\
First, you have to join a clan. Visit the forums to find out how to do so. Once you are invited to \
one, you can select your clan tag by editing your profile on the CSPSP site.\n\
";

struct ServerInfo {
	int id;
	char name[128];
	char map[32];
	char ip[16];
	int port;
	int numPlayers;
	int numMaxPlayers;
	int ping;
};


struct PlayerInfo {
	char key[64];
	char name[32];
	bool online;
};

class PlayerItem : public ListItem 
{
public:
	PlayerInfo info;

	PlayerItem(PlayerInfo info) {
		this->info = info;
	}
	~PlayerItem() {}

	void Render(float x, float y, bool selected) {
		char buffer[10];
		if (info.online) {
			gFont->SetColor(ARGB(255,255,255,255));
			strcpy(buffer,"Online");
		}
		else {
			gFont->SetColor(ARGB(255,175,175,175));
			strcpy(buffer,"Offline");
		}
		
		if (selected) gFont->SetColor(ARGB(255,0,128,255));

		gFont->DrawShadowedString(info.name,x-10,y+3);
		gFont->DrawShadowedString(buffer,x+SCREEN_WIDTH-60,y+3,JGETEXT_RIGHT);
	}

	static bool Compare(ListItem* first, ListItem* second) {
		PlayerItem *a = (PlayerItem*)first;
		PlayerItem *b = (PlayerItem*)second;
		if (a->info.online == b->info.online) {
			return (stricmp(a->info.name,b->info.name) < 0);
		}
		else if (a->info.online) {
			return true;
		}
		return false;
	}
};

class ServerItem : public ListItem 
{
public:
	ServerInfo info;

	ServerItem(ServerInfo info) {
		this->info = info;
	}
	~ServerItem() {}

	void Render(float x, float y, bool selected) {
		if (selected) gFont->SetColor(ARGB(255,0,128,255));

		char buffer[256];
		FormatText(buffer,info.name,200,0.75f);
		gFont->DrawShadowedString(buffer,x-10,y+3);
		FormatText(buffer,info.map,120,0.75f);
		gFont->DrawShadowedString(buffer,225,y+3);

		sprintf(buffer,"%i/%i",info.numPlayers,info.numMaxPlayers);
		gFont->DrawShadowedString(buffer,350,y+3);

		if (info.ping == MAXPING) {
			sprintf(buffer,"%s","???");
		}
		else {
			sprintf(buffer,"%i",info.ping);
		}
		gFont->DrawShadowedString(buffer,SCREEN_WIDTH-30,y+3,JGETEXT_RIGHT);
	}

	static bool Compare(ListItem* first, ListItem* second) {
		ServerItem *a = (ServerItem*)first;
		ServerItem *b = (ServerItem*)second;
		return a->info.ping < b->info.ping;
	}
};


struct PingSocket {
	Socket* socket;
	bool isFree;
	float timer;
};

struct SortByPing
{
	bool operator()(ServerInfo first, ServerInfo second)
	{ 
		return (first.ping < second.ping);
	}
};

class GameStateLobby:	public GameState,
						public JGuiListener
{

private:
	JGuiController* mGuiController;

	int mStage;
	int mServersStage;
	int mFriendsStage;
	std::vector<char*> mLatestNewsLines;
	std::vector<ServerInfo> mServers;
	std::vector<ServerInfo> mFavorites;

	std::vector<char*> mPlayerInfoLines;
	
	ListBox *mServersListBox;
	ListBox *mFavoritesListBox;
	ListBox *mFriendsListBox;
	ListBox *mPlayersListBox;
	ListBox *mNewsTextBox;
	ListBox *mHelpTextBox;
	ListBox *mPlayerInfoTextBox;

	bool mHasReceivedServers;
	bool mHasReceivedFriends;
	bool mHasReceivedPlayers;

	PlayerInfo* mCurrentPlayerInfo;
	bool mHasReceivedInfo;

	int mFavoritesCounter;
	std::vector<ServerInfo>* mPingList;
	ListBox *mPingListBox;
	Socket* mPingSocket;
	bool mIsPinging;
	bool mHasRequestedPing;
	int mPingIndex;
	int mMaxPing;
	float mPingTimer;

	float mUpdateTimer;

	float mInfoX;

	char mPlayerInfoJoinDate[16];
	char mPlayerInfoLoginDate[16];
	char mPlayerInfoServerIp[16];
	int mPlayerInfoServerPort;
	int mPlayerInfoKills;
	int mPlayerInfoDeaths;
	char mPlayerInfoServerName[128];
	char mPlayerInfoServerMap[32];
	JTexture* mPlayerInfoIconTexture;
	JQuad* mPlayerInfoIconQuad;


public:
	GameStateLobby(GameApp* parent);
	virtual ~GameStateLobby();

	void Create();
	void Destroy();
	void Start();
	void End();
	void Update(float dt);
	void Render();
	void ButtonPressed(int controllerId, int controlId);

	bool LoadLatestNews(char* buffer);
	bool LoadServers(char* buffer);
	bool LoadNews(char* buffer);
	bool LoadFavorites();
	bool LoadFriends(char* buffer);
	bool LoadPlayers(char* buffer);
	bool LoadPlayerInfo(char* buffer);
	bool CheckVersion(char* buffer);
	void PingServers(int serversStage);

};


#endif


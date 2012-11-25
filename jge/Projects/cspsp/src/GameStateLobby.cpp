#include "GameStateLobby.h"

GameStateLobby::GameStateLobby(GameApp* parent): GameState(parent)
{

}

GameStateLobby::~GameStateLobby()
{

}


void GameStateLobby::Create()
{
	gLogout = false;

	mGuiController = new JGuiController(100,this);
	if (mGuiController) {
		mGuiController->Add(new MenuItem(STAGE_SERVERS, gFont, "servers", SCREEN_WIDTH-20, 130, TYPE_MAIN, JGETEXT_RIGHT, true));
		mGuiController->Add(new MenuItem(STAGE_FRIENDS, gFont, "friends", SCREEN_WIDTH-20, 155, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(STAGE_NEWS, gFont, "news", SCREEN_WIDTH-20, 180, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(STAGE_HELP, gFont, "help", SCREEN_WIDTH-20, 205, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(-1, gFont, "logout", SCREEN_WIDTH-20, 230, TYPE_MAIN, JGETEXT_RIGHT));
	}

	mPingSocket = new Socket();

	mIsPinging = false;
	mPingTimer = 0.0f;
	mPingIndex = 0;
	mMaxPing = 200;
	mHasRequestedPing = false;

	mStage = STAGE_MENU;
	mServersStage = SERVERSSTAGE_SERVERS;
	mFriendsStage = FRIENDSSTAGE_FRIENDS;

	mServersListBox = new ListBox(0,51,SCREEN_WIDTH,171,20,8);
	mFavoritesListBox = new ListBox(0,51,SCREEN_WIDTH,171,20,8);
	mPingListBox = mServersListBox;

	mFriendsListBox = new ListBox(0,51,SCREEN_WIDTH,171,20,8);
	mPlayersListBox = new ListBox(0,51,SCREEN_WIDTH,171,20,8);
	mNewsTextBox = new ListBox(0,35,SCREEN_WIDTH,205,15,13,TYPE_TEXT);
	mHelpTextBox = new ListBox(0,35,SCREEN_WIDTH,205,15,13,TYPE_TEXT);
	mHelpTextBox->AddText(helptext);

	mPlayerInfoTextBox = new ListBox(260,55,220,185,15,12,TYPE_TEXT);

	mHasReceivedServers = false;
	mHasReceivedFriends = false;
	mHasReceivedPlayers = false;

	mCurrentPlayerInfo = NULL;
	mHasReceivedInfo = false;

	mUpdateTimer = 0.0f;

	mFavoritesCounter = 0;
	LoadFavorites();
}


void GameStateLobby::Destroy()
{
	if (mGuiController)
		delete mGuiController;

}


void GameStateLobby::Start()
{
	mRenderer->EnableVSync(true);

	//gHttpManager->Connect("127.0.0.1","localhost",8080);
	//gHttpManager->Connect("74.125.53.141","cspsp.appspot.com",80);

	if (mLatestNewsLines.size() == 0) {
		char decoding[2000];
		gHttpManager->SendRequest(DecodeText(decoding,"148210202219216146205216210208164208206209206216162149"));
			// /news.html?limit=1
		strcpy(decoding,"");
	}

	gKills = gKills2/7;
	gDeaths = gDeaths2/7;
	/*char decoding[2000];
	char data[256];
	sprintf(data,DecodeText(decoding,"208201222161138215139207206208209215162137201138201201198216205215162137201138216201216215206211211161138215"),gKey,gKills,gDeaths,gSessionKey);
		// key=%s&kills=%d&deaths=%d&session=%s
	strcpy(decoding,"");
	gHttpManager->SendRequest("/accounts/update.html",data,REQUEST_POST);*/

	//mUpdateTimer = 0.0f;

	mInfoX = -400.0f;

	mPlayerInfoIconTexture = mRenderer->CreateTexture(10,10);
	mPlayerInfoIconQuad = new JQuad(mPlayerInfoIconTexture,0,0,10,10);
}


void GameStateLobby::End()
{
	mRenderer->EnableVSync(false);

	gHttpManager->ClearRequests();
	SocketClose(mPingSocket);

	//mIsPinging = false;

	if (mStage == STAGE_MENU || gLogout) {
		WlanTerm();
		mStage = STAGE_MENU;
		mServersStage = SERVERSSTAGE_SERVERS;
		mServers.clear();
		
		for (int i=0; i<mLatestNewsLines.size(); i++) {
			delete mLatestNewsLines[i];
		}
		mLatestNewsLines.clear();

		mServersListBox->Clear();
		//mFavoritesListBox->Clear();
		mNewsTextBox->Clear();
		mFriendsListBox->Clear();
		mPlayersListBox->Clear();

		mCurrentPlayerInfo = NULL;

		mNewsTextBox->SetIndices(0,0);

		mGuiController->SetCurr(0);
	}
}


void GameStateLobby::Update(float dt)
{
	char decoding[2000];
	if (strcmp(gName,DecodeText(decoding,gEncodedName)) != 0 || strcmp(gDisplayName,DecodeText(decoding,gEncodedDisplayName)) != 0) {
		gLogout = true;
	}
	strcpy(decoding,"");

	if (gReconnect) {
		mParent->SetNextState(GAME_STATE_ONLINE);
		return;
	}
	if (gLogout) {
		mParent->SetNextState(GAME_STATE_MENU);
		return;
	}

	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
		if (mStage == STAGE_MENU) {
			//mParent->SetNextState(GAME_STATE_MENU);
			//return;
		}
		else if (mStage == STAGE_PLAYERINFO) {
			mStage = STAGE_FRIENDS;
		}
		else {
			mStage = STAGE_MENU;
		}
	}

	mUpdateTimer += dt;
	if (mUpdateTimer > UPDATETIME) {
		mUpdateTimer = 0.0f;
		gKills = gKills2/7;
		gDeaths = gDeaths2/7;
		char decoding[2000];
		char data[2000];
		sprintf(data,DecodeText(decoding,"208201222161138215139207206208209215162137201138201201198216205215162137201138216201216215206211211161138215"),gKey,gKills,gDeaths,gSessionKey);
			// key=%s&kills=%d&deaths=%d&session=%s
		strcpy(decoding,"");
		gHttpManager->SendRequest("/accounts/update.html",data,REQUEST_POST);
	}

	if (mStage == STAGE_MENU) {
		mInfoX *= 0.75f;
		int i = mGuiController->GetCurr();
		mGuiController->Update(dt);
		if (i != mGuiController->GetCurr()) {
			mInfoX = -400.0f;
		}
	}
	else if (mStage == STAGE_NEWS) {
		mNewsTextBox->Update(dt);
	}
	else if (mStage == STAGE_SERVERS) {
		if (mEngine->GetButtonClick(PSP_CTRL_LTRIGGER)) {
			mServersStage--;
			if (mServersStage < 0) {
				mServersStage = SERVERSSTAGE_FAVORITES;
			}
		}
		if (mEngine->GetButtonClick(PSP_CTRL_RTRIGGER)) {
			mServersStage++;
			if (mServersStage > SERVERSSTAGE_FAVORITES) {
				mServersStage = 0;
			}
		}

		ListBox *mListBox = NULL;
		if (mServersStage == SERVERSSTAGE_SERVERS) {
			mListBox = mServersListBox;
		}
		else if (mServersStage == SERVERSSTAGE_FAVORITES) {
			mListBox = mFavoritesListBox;
		}

		if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE)) {
			//if (!mIsPinging) {
				PingServers(mServersStage);
			//}
		}

		if (mServersStage == SERVERSSTAGE_SERVERS) {
			if (mEngine->GetButtonClick(PSP_CTRL_SELECT)) {
				mServers.clear();
				mServersListBox->Clear();
				mHasReceivedServers = false;
				mIsPinging = false;
				char decoding[2000];
				char request[2000];
				sprintf(request,DecodeText(decoding,"148215202214219201215215148208206215217215202214219201215215147204217209209"));
					// /servers/listservers.html
				strcpy(decoding,"");
				gHttpManager->SendRequest(request);
			}
			if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) {
				ServerItem *item = (ServerItem*)mServersListBox->GetItem();
				if (item) {
					bool exists = false;
					for (int i=0; i<mFavorites.size(); i++) {
						if (strcmp(item->info.ip,mFavorites[i].ip) == 0 && item->info.port == mFavorites[i].port) {
							exists = true;
						}
					}
					if (!exists) {
						mFavorites.push_back(item->info);
						mFavorites.back().id = mFavoritesCounter++;
						mFavoritesListBox->AddItem(new ServerItem(item->info));
						FILE* file = fopen("data/favorites.txt","a+");
						if (file != NULL) {
							fseek(file,-3,SEEK_END);
							int c1 = fgetc(file);
							int c2 = fgetc(file);
							if (c1 != '\r' || c2 != '\n') {
								fputs("\r\n",file);
							}
							fseek(file,0,SEEK_END);
							fprintf(file,"%s:%i\r\n",item->info.ip,item->info.port);
							fclose(file);
						}
					}
				}
			}
		}
		else if (mServersStage == SERVERSSTAGE_FAVORITES) {
			if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) {
				ServerItem *item = (ServerItem*)mFavoritesListBox->GetItem();
				if (item) {
					for (int i=0; i<mFavorites.size(); i++) {
						if (strcmp(item->info.ip,mFavorites[i].ip) == 0 && item->info.port == mFavorites[i].port) {
							mFavorites.erase(mFavorites.begin()+i);
							break;
						}
					}
					mFavoritesListBox->RemoveItem(item);

					FILE* file = fopen("data/favorites.txt","w");
					if (file != NULL) {
						for (int i=0; i<mFavorites.size(); i++) {
							fprintf(file,"%s:%i\r\n",mFavorites[i].ip,mFavorites[i].port);
						}
						fclose(file);
					}
				}
			}
		}

		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			ServerItem *item = (ServerItem*)mListBox->GetItem();
			if (item) {
				char decoding[2000];
				if (strcmp(gName,DecodeText(decoding,gEncodedName)) != 0 || strcmp(gDisplayName,DecodeText(decoding,gEncodedDisplayName)) != 0) {
					mParent->SetNextState(GAME_STATE_MENU);
					return;
				}
				strcpy(decoding,"");

				gServerName = item->info.name;
				gServerIP = item->info.ip;
				gServerPort = item->info.port;
				mParent->SetNextState(GAME_STATE_ONLINE);
				return;
			}
		}

		mListBox->Update(dt);

		if (mIsPinging) {
			mPingTimer += dt;
			if (mPingList->size() > mPingIndex) {
				if ((*mPingList)[mPingIndex].ping != MAXPING) { // already pinged this server
					mPingIndex++;
				}
				else {
					if (!mHasRequestedPing) {
						SocketConnectUdp(mPingSocket,(*mPingList)[mPingIndex].ip,(*mPingList)[mPingIndex].port);
						Packet packet = Packet();
						packet.WriteInt8(NETVERSION);
						packet.WriteInt8(SERVERINFO);
						packet.WriteInt16((*mPingList)[mPingIndex].id);
						SocketSendUdp(mPingSocket,packet.Data(),packet.Length());
						mHasRequestedPing = true;
					}

					char buffer[4096];
					int n;
					n = SocketRecv(mPingSocket,buffer,4096);
					if (n > 0) {
						Packet recvpacket = Packet(buffer,n);
						if (recvpacket.ReadInt8() == NETVERSION) {
							if (recvpacket.ReadInt8() == SERVERINFO) {
								int id = recvpacket.ReadInt16();
								ServerInfo* server = NULL;
								for (int i=0; i<mPingList->size(); i++) {
									if (id == (*mPingList)[i].id) {
										server = &(*mPingList)[i];
										break;
									}
								}

								if (server != NULL && server->ping == MAXPING) {
									recvpacket.ReadChar(server->name,127);
									recvpacket.ReadChar(server->map,32);
									server->numPlayers = recvpacket.ReadInt8();
									server->numMaxPlayers = recvpacket.ReadInt8();
									server->ping = (int)mPingTimer;
									
									mPingListBox->AddItem(new ServerItem(*server));
									
									SocketClose(mPingSocket);

									mPingIndex++;

									mHasRequestedPing = false;
									mPingTimer = 0.0f;
								}
							}	
						}
					}
					else {
						if (mPingTimer > mMaxPing) {
							SocketClose(mPingSocket);
							(*mPingList)[mPingIndex].ping = MAXPING;
						
							mPingIndex++;

							mHasRequestedPing = false;
							mPingTimer = 0.0f;
						}
					}
				}
			}
			else {
				mPingIndex = 0;
				if (mMaxPing >= MAXPING) {
					mIsPinging = false;
					std::sort(mPingList->begin(),mPingList->end(),SortByPing());
					mPingListBox->Sort(ServerItem::Compare);
				}
				else {
					mMaxPing = MAXPING;
				}
			}
		}
	}
	else if (mStage == STAGE_FRIENDS) {
		if (mEngine->GetButtonClick(PSP_CTRL_LTRIGGER)) {
			mFriendsStage--;
			if (mFriendsStage < 0) {
				mFriendsStage = FRIENDSSTAGE_PLAYERS;
			}
		}
		if (mEngine->GetButtonClick(PSP_CTRL_RTRIGGER)) {
			mFriendsStage++;
			if (mFriendsStage > FRIENDSSTAGE_PLAYERS) {
				mFriendsStage = 0;
			}
		}

		if (mFriendsStage == FRIENDSSTAGE_FRIENDS) {
			if (mEngine->GetButtonClick(PSP_CTRL_SELECT)) {
				mFriendsListBox->Clear();
				mHasReceivedFriends = false;
				char decoding[2000];
				char request[2000];
				sprintf(request,DecodeText(decoding,"148197200199212217211216216147209205216216203214206201211200216146205216210208164207202221162137216"),gKey);
					// /accounts/listfriends.html?key=%s
				strcpy(decoding,"");
				gHttpManager->SendRequest(request);
			}
			if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) {
				PlayerItem *item = (PlayerItem*)mFriendsListBox->GetItem();
				if (item != NULL) {
					char decoding[2000];
					char data[2000];
					sprintf(data,DecodeText(decoding,
						"208201222161138215139202215205202210201207202221162137216138216201216215206211211161138215"),
						gKey,item->info.key,gSessionKey);
						// key=%s&friendkey=%s

					strcpy(decoding,"");
					gHttpManager->SendRequest("/accounts/deletefriend.html",data,REQUEST_POST);
					
					if (mCurrentPlayerInfo == &item->info) {
						mCurrentPlayerInfo = NULL;
					}
					mFriendsListBox->RemoveItem(item);
				}
			}
		}
		else if (mFriendsStage == FRIENDSSTAGE_PLAYERS) {
			if (mEngine->GetButtonClick(PSP_CTRL_SELECT)) {
				mPlayersListBox->Clear();
				mHasReceivedPlayers = false;
				char decoding[2000];
				char request[2000];
				sprintf(request,DecodeText(decoding,"148197200199212217211216216147209205216216213208198221202214216146205216210208"));
					// /accounts/listplayers.html
				strcpy(decoding,"");
				gHttpManager->SendRequest(request);
			}
			if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) {
				PlayerItem *item = (PlayerItem*)mPlayersListBox->GetItem();
				if (item != NULL) {
					char *key = item->info.key;
					bool exists = false;
					for (int i=0; i<mFriendsListBox->mItems.size(); i++) {
						char *key2 = ((PlayerItem*)mFriendsListBox->mItems[i])->info.key;
						if (strcmp(key,key2) == 0) {
							exists = true;
							break;
						}
					}
					if (!exists) {
						mFriendsListBox->AddItem(new PlayerItem(item->info));
						mFriendsListBox->Sort(PlayerItem::Compare);
						
						char decoding[2000];
						char data[2000];
						sprintf(data,DecodeText(decoding,
							"208201222161138215139202215205202210201207202221162137216138216201216215206211211161138215"),
							gKey,item->info.key,gSessionKey);
							// key=%s&friendkey=%s&session=%s
						strcpy(decoding,"");
						gHttpManager->SendRequest("/accounts/addfriend.html",data,REQUEST_POST);
					}
				}
			}
		}

		ListBox *listBox = NULL;
		if (mFriendsStage == FRIENDSSTAGE_FRIENDS) {
			listBox = mFriendsListBox;
		}
		else if (mFriendsStage == FRIENDSSTAGE_PLAYERS) {
			listBox = mPlayersListBox;
		}

		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			PlayerItem *item = (PlayerItem*)listBox->GetItem();
			if (item != NULL) {
				mCurrentPlayerInfo = &item->info;
				mHasReceivedInfo = false;
				char decoding[2000];
				char request[2000];
				sprintf(request,DecodeText(decoding,"148197200199212217211216216147204201217205211202212146205216210208164207202221162137216"),mCurrentPlayerInfo->key);
					// /accounts/getinfo.html?key=%s
				strcpy(decoding,"");
				gHttpManager->SendRequest(request);
				mStage = STAGE_PLAYERINFO;
			}
		}

		listBox->Update(dt);
	}
	else if (mStage == STAGE_HELP) {
		mHelpTextBox->Update(dt);
	}
	else if (mStage == STAGE_PLAYERINFO) {
		if (mCurrentPlayerInfo != NULL) {
			mPlayerInfoTextBox->Update(dt);
		
			if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
				if (mHasReceivedInfo && mPlayerInfoServerPort != -1) {
					if (strcmp(gName,DecodeText(decoding,gEncodedName)) != 0 || strcmp(gDisplayName,DecodeText(decoding,gEncodedDisplayName)) != 0) {
						mParent->SetNextState(GAME_STATE_MENU);
						return;
					}
					strcpy(decoding,"");
					gServerName = "";//(*mList)[*index].name;
					gServerIP = mPlayerInfoServerIp;
					gServerPort = mPlayerInfoServerPort;
					mParent->SetNextState(GAME_STATE_ONLINE);
					return;
				}
			}
		}
	}

	gHttpManager->Update(dt);
	char buffer[8192];
	int size = gHttpManager->GetResponse(buffer);
	if (size > 0) {
		if (strstr(buffer,"LATESTNEWS")) {
			if (LoadLatestNews(buffer)) {
			}
		}
		else if (strstr(buffer,"NEWS")) {
			if (LoadNews(buffer)) {
			}
		}
		else if (strstr(buffer,"SERVERS")) {
			if (LoadServers(buffer)) {
			}
		}
		else if (strstr(buffer,"FRIENDS")) {
			if (LoadFriends(buffer)) {
			}
		}
		else if (strstr(buffer,"PLAYERS")) {
			if (LoadPlayers(buffer)) {
			}
		}
		else if (strstr(buffer,"GETINFO")) {
			if (LoadPlayerInfo(buffer)) {
			}
		}
		else if (strstr(buffer,"UPDATE")) {
			char* s;
			s = strstr(buffer,"UPDATE");
			s += strlen("UPDATE") + 2;
			if (*s == '1') { //suspended
				mParent->SetNextState(GAME_STATE_MENU);
				return;
			}
		}
		//delete buffer;
	}
}


void GameStateLobby::Render()
{
	mRenderer->RenderQuad(gBgQuad, 0.0f, 0.0f);
	gFont->SetBase(0);
	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(0.75f);

	if (mStage == STAGE_MENU) {
		gFont->SetColor(ARGB(255,200,200,200));
		gFont->SetScale(0.6f);
		char buffer[128];
		sprintf(buffer,"%.2f",VERSION);
		gFont->DrawString(buffer,SCREEN_WIDTH-4,SCREEN_HEIGHT-12,JGETEXT_RIGHT);

		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.75f);

		//gFont->DrawString("[X] Select     [O] Return to Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);

		//char buffer[128];
		sprintf(buffer,"%s!",gDisplayName);
		gFont->DrawShadowedString("Welcome to CSPSP online,",15,15);
		mRenderer->RenderQuad(gIconQuad,15,32);
		gFont->DrawShadowedString(buffer,30,30);

		mRenderer->DrawLine(85,60,85,115,ARGB(255,255,255,255));
		gFont->DrawShadowedString("Kills",75,65,JGETEXT_RIGHT);
		gFont->DrawShadowedString("Deaths",75,80,JGETEXT_RIGHT);
		gFont->DrawShadowedString("Ratio",75,95,JGETEXT_RIGHT);

		if (gKills != -1 || gDeaths != -1) {
			sprintf(buffer,"%d",gKills);
			gFont->DrawShadowedString(buffer,95,65);
			sprintf(buffer,"%d",gDeaths);
			gFont->DrawShadowedString(buffer,95,80);
			float ratio = 1.0f;
			if (gDeaths != 0) {
				ratio = (float)gKills/gDeaths;
			}
			sprintf(buffer,"%.2f",ratio);
			gFont->DrawShadowedString(buffer,95,95);
		}
		else {
			gFont->SetColor(ARGB(255,175,175,175));
			gFont->DrawShadowedString("-",95,65);
			gFont->DrawShadowedString("-",95,80);
			gFont->DrawShadowedString("-",95,95);
			gFont->SetColor(ARGB(255,255,255,255));
		}


		mRenderer->FillRect(220,15,260-15,100,ARGB(150,0,0,0));

		sprintf(buffer,"%s","Latest News");
		for (int i=0; i<strlen(buffer); i++) {
			char letter[2];
			sprintf(letter,"%c",buffer[i]);
			gFont->DrawString(letter,SCREEN_WIDTH-20-3,18+8*i,JGETEXT_CENTER);
		}
		//gFont->DrawString("Latest News",SCREEN_WIDTH-20,22,JGETEXT_RIGHT);
	
		if (mLatestNewsLines.size() > 0) {
			for (int i=0; i<mLatestNewsLines.size(); i++) {
				if (i == 6) break;
				gFont->DrawShadowedString(mLatestNewsLines[i],220+5,22+15*i);
			}
		}
		else {
			gFont->DrawShadowedString("Loading...",220+5,22);
		}

		mRenderer->FillRect(0,128,SCREEN_WIDTH,125,ARGB(100,0,0,0));
		int i = mGuiController->GetCurr();
		mRenderer->FillRect(0,128+i*25,SCREEN_WIDTH,25,ARGB(255,0,0,0));
		mGuiController->Render();

		gFont->SetScale(0.75f);
		gFont->SetColor(ARGB(255,255,255,255));

		char info[256];
		if (i == 0) {
			strcpy(info,"Join a server to start playing!");
		}
		else if (i == 1) {
			strcpy(info,"Add other players to your friends list");
		}
		else if (i == 2) {
			strcpy(info,"View recent news about CSPSP");
		}
		else if (i == 3) {
			strcpy(info,"Need help?");
		}
		else if (i == 4) {
			strcpy(info,"Logout and return to the main menu");
		}
		gFont->DrawString(info,10+mInfoX,133+i*25);

		//gFont->SetScale(0.7f);
		gFont->SetColor(ARGB(255,255,255,255));
		gFont->DrawShadowedString("Official Website: http://cspsp.appspot.com", 5, SCREEN_HEIGHT-15);
	}
	else if (mStage == STAGE_NEWS) {
		mRenderer->FillRect(0,35,SCREEN_WIDTH,205,ARGB(100,0,0,0));

		gFont->DrawShadowedString("[O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

		gFont->SetScale(1.0f);
		//gFont->SetColor(ARGB(255,255,255,0));
		gFont->DrawShadowedString("News",20,10);

		gFont->SetScale(0.75f);
		if (!mNewsTextBox->IsEmpty()) {
			mNewsTextBox->Render();
		}
		else {
			gFont->DrawShadowedString("Retrieving news updates...", 30, 40);
		}
	}
	else if (mStage == STAGE_SERVERS) {
		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("< [L]",10,10,JGETEXT_LEFT);
		gFont->DrawShadowedString("[R] >",SCREEN_WIDTH-10,10,JGETEXT_RIGHT);

		mRenderer->FillRect(0,30,SCREEN_WIDTH,5,ARGB(100,0,0,0));
		mRenderer->FillRect(0,35,SCREEN_WIDTH,187,ARGB(100,0,0,0));
		//mRenderer->FillRect(0,35+175,SCREEN_WIDTH,30,ARGB(175,0,0,0));

		gFont->DrawShadowedString("name",20,35);
		gFont->DrawShadowedString("map",225,35);
		gFont->DrawShadowedString("players",350,35);
		gFont->DrawShadowedString("ping",SCREEN_WIDTH-30,35,JGETEXT_RIGHT);
		mRenderer->DrawLine(0,50,SCREEN_WIDTH,50,ARGB(255,255,255,255));

		if (mServersStage == SERVERSSTAGE_SERVERS) {
			mRenderer->FillRect(155-62,5,125,25,ARGB(100,0,0,0));
			mRenderer->FillRect(325-62,5,125,25,ARGB(175,0,0,0));

			gFont->SetScale(1.0f);
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawShadowedString("Servers",155,10,JGETEXT_CENTER);
			gFont->SetColor(ARGB(255,175,175,175));
			gFont->DrawShadowedString("Favorites",325,10,JGETEXT_CENTER);

			gFont->SetScale(0.75f);
			gFont->SetColor(ARGB(255,255,255,255));
			if (!mHasReceivedServers) {
				gFont->DrawShadowedString("Retrieving server list...",20,55);
			}
			else if (mServers.size() == 0) {
				gFont->SetColor(ARGB(255,175,175,175));
				gFont->DrawShadowedString("No servers",20,55);
				gFont->SetColor(ARGB(255,255,255,255));
			}
			gFont->DrawShadowedString("[X] Connect     [[]] Add to Favorites",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20-20,JGETEXT_CENTER);
			gFont->DrawShadowedString("[SELECT] Request Servers     [^] Refresh     [O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

			mServersListBox->Render();
		}
		else if (mServersStage == SERVERSSTAGE_FAVORITES) {
			mRenderer->FillRect(155-62,5,125,25,ARGB(175,0,0,0));
			mRenderer->FillRect(325-62,5,125,25,ARGB(100,0,0,0));

			gFont->SetScale(1.0f);
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawShadowedString("Favorites",325,10,JGETEXT_CENTER);
			gFont->SetColor(ARGB(255,175,175,175));
			gFont->DrawShadowedString("Servers",155,10,JGETEXT_CENTER);

			gFont->SetScale(0.75f);
			if (mFavorites.size() == 0) {
				gFont->SetColor(ARGB(255,175,175,175));
				gFont->DrawShadowedString("No favorites",20,55);
				gFont->SetColor(ARGB(255,255,255,255));
			}
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawShadowedString("[X] Connect     [[]] Delete     [^] Refresh     [O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

			mFavoritesListBox->Render();
		}

		if (mPingList->size() > 0) {
			if (mIsPinging) {
				mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-27,200,4,ARGB(255,100,100,100));
				mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-27,200*((float)mPingIndex/(float)mPingList->size()),4,ARGB(255,255,255,255));
			}
		}
	}
	else if (mStage == STAGE_FRIENDS) {
		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("< [L]",10,10,JGETEXT_LEFT);
		gFont->DrawShadowedString("[R] >",SCREEN_WIDTH-10,10,JGETEXT_RIGHT);

		mRenderer->FillRect(0,30,SCREEN_WIDTH,5,ARGB(100,0,0,0));
		mRenderer->FillRect(0,35,SCREEN_WIDTH,187,ARGB(100,0,0,0));

		gFont->DrawShadowedString("name",20,35);
		gFont->DrawShadowedString("online status",SCREEN_WIDTH-30,35,JGETEXT_RIGHT);
		mRenderer->DrawLine(0,50,SCREEN_WIDTH,50,ARGB(255,255,255,255));

		if (mFriendsStage == FRIENDSSTAGE_FRIENDS) {
			mRenderer->FillRect(155-62,5,125,25,ARGB(100,0,0,0));
			mRenderer->FillRect(325-62,5,125,25,ARGB(175,0,0,0));

			gFont->SetScale(1.0f);
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawShadowedString("Friends",155,10,JGETEXT_CENTER);
			gFont->SetColor(ARGB(255,175,175,175));
			gFont->DrawShadowedString("Players",325,10,JGETEXT_CENTER);

			gFont->SetScale(0.75f);
			gFont->SetColor(ARGB(255,255,255,255));
			if (!mHasReceivedFriends) {
				gFont->DrawShadowedString("Retrieving friends list...",20,55);
			}
			else if (mFriendsListBox->IsEmpty()) {
				gFont->SetColor(ARGB(255,175,175,175));
				gFont->DrawShadowedString("No friends",20,55);
				gFont->SetColor(ARGB(255,255,255,255));
			}
			else {
				mFriendsListBox->Render();
			}

			gFont->DrawShadowedString("[X] View Info     [[]] Delete",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20-20,JGETEXT_CENTER);
			gFont->DrawShadowedString("[SELECT] Request Friends     [O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
		}
		else if (mFriendsStage == FRIENDSSTAGE_PLAYERS) {
			mRenderer->FillRect(155-62,5,125,25,ARGB(175,0,0,0));
			mRenderer->FillRect(325-62,5,125,25,ARGB(100,0,0,0));

			gFont->SetScale(1.0f);
			gFont->SetColor(ARGB(255,255,255,255));
			gFont->DrawShadowedString("Players",325,10,JGETEXT_CENTER);
			gFont->SetColor(ARGB(255,175,175,175));
			gFont->DrawShadowedString("Friends",155,10,JGETEXT_CENTER);

			gFont->SetScale(0.75f);
			gFont->SetColor(ARGB(255,255,255,255));
			if (!mHasReceivedPlayers) {
				gFont->DrawShadowedString("Retrieving players list...",20,55);
			}
			else if (mPlayersListBox->IsEmpty()) {
				gFont->SetColor(ARGB(255,175,175,175));
				gFont->DrawShadowedString("No players online",20,55);
				gFont->SetColor(ARGB(255,255,255,255));
			}
			else {
				mPlayersListBox->Render();
			}

			gFont->DrawShadowedString("[X] View Info     [[]] Add to Friends",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20-20,JGETEXT_CENTER);
			gFont->DrawShadowedString("[SELECT] Request Players     [O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
		}
	}
	else if (mStage == STAGE_HELP) {
		mRenderer->FillRect(0,35,SCREEN_WIDTH,205,ARGB(100,0,0,0));

		gFont->DrawShadowedString("[O] Return to Online Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

		gFont->SetScale(1.0f);
		//gFont->SetColor(ARGB(255,255,255,0));
		gFont->DrawShadowedString("Help",20,10);

		gFont->SetScale(0.75f);
		mHelpTextBox->Render();
	}
	else if (mStage == STAGE_PLAYERINFO) {
		mRenderer->FillRect(0,35,260,205,ARGB(100,0,0,0));
		mRenderer->FillRect(260,35,220,205,ARGB(175,0,0,0));
		gFont->DrawShadowedString("[O] Return to Friends", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
		gFont->SetScale(1.0f);

		if (mCurrentPlayerInfo != NULL) {
			gFont->DrawShadowedString(mCurrentPlayerInfo->name,35,10);

			if (mHasReceivedInfo) {
				mRenderer->RenderQuad(mPlayerInfoIconQuad,15,13);
				mRenderer->DrawLine(125,35,125,35+205,ARGB(255,255,255,255));

				gFont->SetScale(0.75f);

				gFont->DrawShadowedString("Join Date",115,40,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Last Login Date",115,55,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Kills",115,85,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Deaths",115,100,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Ratio",115,115,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Online Status",115,145,JGETEXT_RIGHT);
				gFont->DrawShadowedString("Current Server",115,160,JGETEXT_RIGHT);

				char buffer[128];

				gFont->DrawShadowedString(mPlayerInfoJoinDate,135,40);
				gFont->DrawShadowedString(mPlayerInfoLoginDate,135,55);

				if (mPlayerInfoKills != -1 || mPlayerInfoKills != -1) {
					sprintf(buffer,"%d",mPlayerInfoKills);
					gFont->DrawShadowedString(buffer,135,85);
					sprintf(buffer,"%d",mPlayerInfoDeaths);
					gFont->DrawShadowedString(buffer,135,100);

					float ratio = 1.0f;
					if (mPlayerInfoDeaths != 0) {
						ratio = (float)mPlayerInfoKills/mPlayerInfoDeaths;
					}
					sprintf(buffer,"%.2f",ratio);
					gFont->DrawShadowedString(buffer,135,115);
				}
				else {
					gFont->SetColor(ARGB(255,175,175,175));
					gFont->DrawShadowedString("-",135,85);
					gFont->DrawShadowedString("-",135,100);
					gFont->DrawShadowedString("-",135,115);
					gFont->SetColor(ARGB(255,255,255,255));
				}

				if (mCurrentPlayerInfo->online) {
					strcpy(buffer,"Online");
				}
				else {
					gFont->SetColor(ARGB(255,175,175,175));
					strcpy(buffer,"Offline");
				}
				gFont->DrawShadowedString(buffer,135,145);
				
				gFont->SetColor(ARGB(255,255,255,255));
				//gFont->SetScale(0.5f);

				if (mPlayerInfoServerPort != -1) {
					//sprintf(buffer,"%s:%d",mCurrentPlayerInfo->serverIp,mCurrentPlayerInfo->serverPort);
					FormatText(buffer,mPlayerInfoServerName,120,0.75f);
					gFont->DrawShadowedString(buffer,135,160);
					FormatText(buffer,mPlayerInfoServerMap,120,0.75f);
					gFont->DrawShadowedString(buffer,135,175);
					gFont->DrawShadowedString("([X] to Connect)",135,190);
				}
				else {
					gFont->SetColor(ARGB(255,175,175,175));
					gFont->DrawShadowedString("None",135,160);
					gFont->SetColor(ARGB(255,255,255,255));
				}

				gFont->DrawShadowedString("Info",275,40);
				mRenderer->DrawLine(260,54,SCREEN_WIDTH,54,ARGB(255,255,255,255));
				mPlayerInfoTextBox->Render();
			}
			else {
				gFont->DrawShadowedString("Retrieving info...",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			}
		}
	}
}

bool GameStateLobby::LoadLatestNews(char* buffer)
{
	char* s;

	s = strstr(buffer,"LATESTNEWS");
	if (s == NULL) {
		return false;		
	}

	s += strlen("LATESTNEWS") + 2;

	FormatText(mLatestNewsLines,s,225,0.75f);
	
	return true;
}

bool GameStateLobby::LoadNews(char* buffer)
{
	char* s;

	s = strstr(buffer,"NEWS");
	if (s == NULL) {
		return false;		
	}

	s += strlen("NEWS") + 2;

	mNewsTextBox->Clear();
	mNewsTextBox->AddText(s);
	
	return true;
}


bool GameStateLobby::LoadServers(char* buffer)
{
	char* s;

	s = strstr(buffer,"SERVERS");
	if (s == NULL) {
		return false;		
	}

	s += strlen("SERVERS") + 2;

	int i = 0;
	while (true) {
		PlayerInfo info = PlayerInfo();

		ServerInfo serverinfo = ServerInfo();

		int n = sscanf(s,"%127[^:]:%31[^:]:%i:%i:%15[^:]:%i",serverinfo.name,serverinfo.map,&serverinfo.numPlayers,&serverinfo.numMaxPlayers,serverinfo.ip,&serverinfo.port);

		serverinfo.id = i;
		serverinfo.ping = -1;

		if (n == 6) {
			mServers.push_back(serverinfo);
		}
		else {
			break;
		}
		i++;
		s = strstr(s,"\r\n");
		s += 2;
	}

	if (mServers.size() > 0) {
		PingServers(SERVERSSTAGE_SERVERS);
	}
	mHasReceivedServers = true;
	return true;
}

bool GameStateLobby::LoadFavorites()
{
	FILE* file = fopen("data/favorites.txt","r+");

	if (file == NULL) return false;

	char line[32]; 

	mFavoritesCounter = 0;
	while(fgets(line,32,file)) {
		ServerInfo serverinfo = ServerInfo();

		int n = sscanf(line,"%15[^:]:%i",serverinfo.ip,&serverinfo.port);

		if (n == 2) {
			serverinfo.id = mFavoritesCounter;
			strcpy(serverinfo.name,"???");
			strcpy(serverinfo.map,"???");
			serverinfo.numPlayers = 0;
			serverinfo.numMaxPlayers = 0;
			serverinfo.ping = MAXPING;

			mFavorites.push_back(serverinfo);
			mFavoritesListBox->AddItem(new ServerItem(serverinfo));
			mFavoritesCounter++;
		}
	}

	fclose(file);

	return true;
}

bool GameStateLobby::LoadFriends(char* buffer)
{
	char* s;

	s = strstr(buffer,"FRIENDS");
	if (s == NULL) {
		return false;		
	}

	s += strlen("FRIENDS") + 2;
	if (s == NULL) {
		return false;		
	}

	while (true) {
		PlayerInfo info = PlayerInfo();

		int online = 0;
		int n = sscanf(s,"%63[^:]:%31[^:]:%d",info.key,info.name,&online);
		info.online = (online == 1);

		if (n == 3) {
			mFriendsListBox->AddItem(new PlayerItem(info));
		}
		else {
			break;
		}

		s = strstr(s,"\r\n");
		s += 2;
	}
	mFriendsListBox->Sort(PlayerItem::Compare);
	mHasReceivedFriends = true;
	return true;
}

bool GameStateLobby::LoadPlayers(char* buffer)
{
	char* s;

	s = strstr(buffer,"PLAYERS");
	if (s == NULL) {
		return false;		
	}

	s += strlen("PLAYERS") + 2;

	while (true) {
		PlayerInfo info = PlayerInfo();

		int n = sscanf(s,"%63s %31s",info.key,info.name);
		info.online = true;

		if (n == 2) {
			mPlayersListBox->AddItem(new PlayerItem(info));
		}
		else {
			break;
		}

		s = strstr(s,"\r\n");
		s += 2;
	}
	mPlayersListBox->Sort(PlayerItem::Compare);
	mHasReceivedPlayers = true;
	return true;
}

bool GameStateLobby::LoadPlayerInfo(char* buffer)
{
	char* s;

	s = strstr(buffer,"GETINFO");
	if (s == NULL) {
		return false;		
	}

	s += strlen("GETINFO") + 2;

	if (mCurrentPlayerInfo != NULL) {
		UpdateIcon(mPlayerInfoIconTexture,(unsigned char*)&s[0]);
		s += 300;

		char infobuffer[512];
		int online = 0;
		int n = sscanf(s,"%15s %15s %d %15s %d %127[^\r\n] %31s %d %d",
											mPlayerInfoJoinDate,
											mPlayerInfoLoginDate,
											&online,
											mPlayerInfoServerIp,
											&mPlayerInfoServerPort,
											mPlayerInfoServerName,
											mPlayerInfoServerMap,
											&mPlayerInfoKills,
											&mPlayerInfoDeaths);
		for (int i=0; i<9; i++) {
			s = strstr(s,"\r\n") + 2;
		}
		strncpy(infobuffer,s,511);
		infobuffer[511] = '\0';

		mCurrentPlayerInfo->online = (online == 1);
		
		mPlayerInfoTextBox->Clear();
		
		if (n == 9) {
			//strcpy(infobuffer,s);
			mPlayerInfoTextBox->AddText(infobuffer);
		}
	}

	mHasReceivedInfo = true;
	return true;
}

void GameStateLobby::ButtonPressed(int controllerId, int controlId)
{
	switch (controlId)
	{
	case STAGE_NEWS:
		mStage = STAGE_NEWS;
		if (mNewsTextBox->IsEmpty()) {
			char decoding[2000];
			char request[2000];
			sprintf(request,DecodeText(decoding,"148210202219216146205216210208"));
				// /news.html
			strcpy(decoding,"");
			gHttpManager->SendRequest(request);
		}
		break;
	case STAGE_SERVERS:
		mStage = STAGE_SERVERS;
		//mServersStage = SERVERSSTAGE_SERVERS;
		PingServers(mServersStage);
		if (mServers.size() == 0) {
			mHasReceivedServers = false;
			char decoding[2000];
			char request[2000];
			sprintf(request,DecodeText(decoding,"148215202214219201215215148208206215217215202214219201215215147204217209209"));
				// /servers/listservers.html
			strcpy(decoding,"");
			gHttpManager->SendRequest(request);
		}
		break;
	case STAGE_FRIENDS:
		mStage = STAGE_FRIENDS;
		if (mFriendsListBox->IsEmpty()) {
			mHasReceivedFriends = false;
			char decoding[2000];
			char request[2000];
			sprintf(request,DecodeText(decoding,"148197200199212217211216216147209205216216203214206201211200216146205216210208164207202221162137216"),gKey);
				// /accounts/listfriends.html?key=%s
			strcpy(decoding,"");
			gHttpManager->SendRequest(request);
		}
		if (mPlayersListBox->IsEmpty()) {
			mHasReceivedPlayers = false;
			char decoding[2000];
			char request[2000];
			sprintf(request,DecodeText(decoding,"148197200199212217211216216147209205216216213208198221202214216146205216210208"));
				// /accounts/listplayers.html
			strcpy(decoding,"");
			gHttpManager->SendRequest(request);
		}
		break;
	case STAGE_HELP:
		mStage = STAGE_HELP;
		break;
	case -1:
		mParent->SetNextState(GAME_STATE_MENU);
		break;
	}
}

void GameStateLobby::PingServers(int serversStage) {
	SocketClose(mPingSocket);

	mIsPinging = true;
	mPingIndex = 0;
	mPingTimer = 0.0f;
	mHasRequestedPing = false;
	mMaxPing = 200;
	if (serversStage == SERVERSSTAGE_SERVERS) {
		mPingList = &mServers;
		mPingListBox = mServersListBox;
	}
	else if (serversStage == SERVERSSTAGE_FAVORITES) {
		mPingList = &mFavorites;
		mPingListBox = mFavoritesListBox;
	}
	mPingListBox->Clear();
	for (int i=0; i<mPingList->size(); i++) {
		(*mPingList)[i].ping = MAXPING;
	}
}
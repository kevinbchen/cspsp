#include "GameStateUpdate.h"

GameStateUpdate::GameStateUpdate(GameApp* parent): GameState(parent) 
{
	mStage = STAGE_GETVERSIONS;
	mTimer = 5000;
	gLastKey = 0;
	gKeyRepeatDelay = 0;
}

GameStateUpdate::~GameStateUpdate() {}

void GameStateUpdate::Create()
{
	/*mGuiController = new JGuiController(0, this);
	#ifdef WIN32
	mGuiController->Add(new MenuItem(1, gFont, "firstconnection", SCREEN_WIDTH_2, 20, TYPE_NORMAL, JGETEXT_CENTER, true));
	mGuiController->Add(new MenuItem(2, gFont, "second", SCREEN_WIDTH_2, 50, TYPE_NORMAL, JGETEXT_CENTER, false));

	#else

	std::vector<ConnectionConfig> connections = GetConnectionConfigs();

	for (int i=0; i<connections.size(); i++) {
		if (i == 0) {
			mGuiController->Add(new MenuItem(connections[i].index, gFont, connections[i].name, SCREEN_WIDTH_2, (i+1)*20, TYPE_NORMAL, JGETEXT_CENTER, true));
		}
		else {
			mGuiController->Add(new MenuItem(connections[i].index, gFont, connections[i].name, SCREEN_WIDTH_2, (i+1)*20, TYPE_NORMAL, JGETEXT_CENTER, false));
		}
	}
	#endif*/

	mTextBox = new ListBox(0,35,SCREEN_WIDTH,205,15,13,TYPE_TEXT);
}

void GameStateUpdate::Destroy() 
{
	/*if (mGuiController) {
		delete mGuiController;
	}*/
	#ifdef WIN32
	#else
	//WlanTerm();
	//sceNetApctlDisconnect();
	//pspSdkInetTerm();
	#endif
}

void GameStateUpdate::Start()
{
	mRenderer->EnableVSync(true);
	mStage = STAGE_GETVERSIONS;

	//SocketConnect(gSocket,"83.211.240.146",80); //free.7host07.com
	//75.101.142.97
	gHttpManager->Connect("75.101.142.97","dl.dropbox.com",80);

	mHasRequestedFile = false;
	mVersionIndex = 0;
	mFileIndex = 0;
	mFileSize = 1;
	mFileAmount = 0;
}


void GameStateUpdate::End()
{
	WlanTerm();
	mRenderer->EnableVSync(false);
	//delete mGuiController;
	/**for (int i=1; i<10; i++) {
		mGuiController->Remove(i);
	}**/
	for (int i=0; i<mVersions.size(); i++) {
		delete mVersions[i];
	}
	mVersions.clear();

	for (int i=0; i<mFiles.size(); i++) {
		delete mFiles[i];
	}
	mFiles.clear();

	mTextBox->Clear();

	//SocketClose(gSocket);

	/*#ifdef WIN32
	#else
	//sceIoRemove("update.txt");
	//WlanTerm();
	//sceNetApctlDisconnect();
	#endif*/
}


void GameStateUpdate::Update(float dt)
{
	if (mStage == STAGE_GETVERSIONS) {
		if (!mHasRequestedFile) {
			gHttpManager->SendRequest("/u/48168/cspsp/versionlist.txt");
			mHasRequestedFile = true;
		}
	}
	else if (mStage == STAGE_GETFILES) {
		if (!mHasRequestedFile) {
			char request[1024];
			sprintf(request,"/u/48168/cspsp/update-%s.txt",mVersions[mVersionIndex]);
			gHttpManager->SendRequest(request);
			mHasRequestedFile = true;
		}
	}
	else if (mStage == STAGE_GETTEXT) {
		if (!mHasRequestedFile) {
			char request[1024];
			gHttpManager->SendRequest("/u/48168/cspsp/update.txt");
			mHasRequestedFile = true;
		}
	}
	else if (mStage == STAGE_TEXT) {
		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			mStage = STAGE_UPDATE;
		}
		mTextBox->Update(dt);
	}
	else if (mStage == STAGE_UPDATE) {
		if (mFiles.size() == 0) {
			mStage = STAGE_UPDATEFINISH;
			return;
		}
		if (!mHasRequestedFile) {
			char request[1024];
			sprintf(request,"/u/48168/cspsp/%s",mFiles[mFileIndex]);
			gHttpManager->SendRequest(request);
			mHasRequestedFile = true;
		}
	}
	else if (mStage == STAGE_UPDATEFINISH) {
		mTimer -= dt;
		if (mTimer <= 0) {
			mEngine->End();
		}
		return;
	}

	if (mHasRequestedFile) {
		
		gHttpManager->Update(dt);
		char buffer[8192];
		int size = gHttpManager->GetResponse(buffer);
		if (size > 0) {
			//int n = gHttpManager->GetBufferSize(); //strlen(buffer);
			char* s;
			s = buffer;

			if (mStage == STAGE_GETVERSIONS) {
				s = strtok(s,"\r\n");
				while (s) {
					float version;
					sscanf(s,"%f",&version);
					if (version > VERSION) {
						char* buffer2 = new char[128];
						strcpy(buffer2,s);
						mVersions.push_back(buffer2);
					}
					s = strtok(NULL,"\r\n");
				}
				mHasRequestedFile = false;

				if (mVersions.size() > 0) {
					mStage = STAGE_GETFILES;
				}
				else {
					mStage = STAGE_RETURN;
				}
			}
			else if (mStage == STAGE_GETFILES) {
				s = strtok(s,"\r\n");
				while (s) {
					char* buffer2 = new char[128];
					strcpy(buffer2,s);

					bool exists = false;
					for (int i=0; i<mFiles.size(); i++) {
						if (stricmp(buffer2,mFiles[i]) == 0) {
							exists = true;
							break;
						}
					}
					if (!exists) {
						mFiles.push_back(buffer2);
					}
					s = strtok(NULL,"\r\n");
				}
				mHasRequestedFile = false;

				mVersionIndex++;
				if (mVersionIndex >= mVersions.size()) {
					mStage = STAGE_GETTEXT;
				}
			}
			else if (mStage == STAGE_GETTEXT) {
				//s = strtok(s,"\r\n");
				mTextBox->Clear();
				mTextBox->AddText(s);
				mHasRequestedFile = false;

				mStage = STAGE_TEXT;
			}
			else if (mStage == STAGE_UPDATE) {
				//mFileSize = gHttpManager->GetContentSize();

				FILE* file;
				int contentSize = gHttpManager->GetContentSize();
				if (contentSize > 0) {
					mFileSize = contentSize;

					file = fopen(mFiles[mFileIndex],"wb+");
					if (file == NULL) {
						mStage = STAGE_UPDATEFAIL;
						return;
					}
					fwrite(s,1,size,file);

					mFileAmount += size;
					fclose(file);
				}
				else {
					file = fopen(mFiles[mFileIndex],"rb+");
					if (file == NULL) {
						mStage = STAGE_UPDATEFAIL;
						return;
					}
					fseek(file,0,SEEK_END);

					//int size = n;

					fwrite(buffer,1,size,file);
					mFileAmount += size;
					fclose(file);
				}


				if (mFileAmount >= mFileSize) {
					mHasRequestedFile = false;
					mFileSize = 1;
					mFileAmount = 0;

					mFileIndex++;
					if (mFileIndex >= mFiles.size()) {
						mStage = STAGE_UPDATEFINISH;
					}
				}
			}
			//delete buffer;
		}
	}

	if (mStage != STAGE_UPDATE && mStage != STAGE_UPDATEFINISH) {
		if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
			mParent->SetNextState(GAME_STATE_MENU);
			return;
		}
	}
}


void GameStateUpdate::Render()
{
	mRenderer->ClearScreen(ARGB(0,0,0,0));
	mRenderer->RenderQuad(gBgQuad, 0.0f, 0.0f);
	mRenderer->FillRect(0,35,SCREEN_WIDTH,205,ARGB(100,0,0,0));

	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(1.0f);
	gFont->DrawShadowedString("Update",20,10);


	gFont->SetColor(ARGB(255,255,255,255));
	/*if (mStage == STAGE_SELECT) {
		mGuiController->Render();
		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.75f);
		gFont->DrawString("[X] Select Connection     [O] Return to Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}*/
	/**else if (mStage == STAGE_CONNECTING) {
		gFont->SetScale(1.0f);
		gFont->SetColor(ARGB(255,255,255,255));
		if (mConnectStage == -1) {
			gFont->DrawString("Wifi switch is not on", SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
		}
		else {
			char buffer[64];
			strcpy(buffer,"connection state ");
			char numbuffer[2];
			sprintf(numbuffer,"%i",mConnectStage);
			strcat(buffer,numbuffer);
			gFont->DrawString(buffer, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
		}

	}**/
	if (mStage == STAGE_GETVERSIONS || mStage == STAGE_GETFILES) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Checking for updates...", SCREEN_WIDTH_2, SCREEN_HEIGHT_2, JGETEXT_CENTER);

		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("[O] Return to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_GETTEXT) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Retrieving update info...", SCREEN_WIDTH_2, SCREEN_HEIGHT_2, JGETEXT_CENTER);

		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("[O] Return to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_TEXT) {
		gFont->SetScale(0.75f);
		mTextBox->Render();

		gFont->DrawShadowedString("[X] Start Update     [O] Return to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_UPDATE) {
		gFont->SetScale(1.0f);
		/*char buffer[128];
		sprintf(buffer,"%i",mFileSize);
		gFont->DrawString(buffer, SCREEN_WIDTH_2, SCREEN_HEIGHT_2-30, JGETEXT_CENTER);*/
		gFont->DrawShadowedString("Updating. Do not turn off psp.", SCREEN_WIDTH_2, SCREEN_HEIGHT_2, JGETEXT_CENTER);
		if (mFileIndex >= mFiles.size()) {
			return;
		}
		gFont->DrawShadowedString(mFiles[mFileIndex], SCREEN_WIDTH_2, SCREEN_HEIGHT_2+50, JGETEXT_CENTER);
		mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200,4,ARGB(255,100,100,100));
		mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200*((float)mFileAmount/(float)mFileSize),4,ARGB(255,255,255,255));
		//gFont->SetScale(0.75f);
		//gFont->DrawString("[X] Start Update     [O] Return to Menu", SCREEN_WIDTH/2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_UPDATEFINISH) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Update complete!", SCREEN_WIDTH_2, SCREEN_HEIGHT_2-10, JGETEXT_CENTER);
		char buffer[64];
		strcpy(buffer,"CSPSP will exit in ");
		char numbuffer[2];
		sprintf(numbuffer,"%i",(int)(mTimer/1000));
		strcat(buffer,numbuffer);
		strcat(buffer,"...");
		gFont->DrawShadowedString(buffer, SCREEN_WIDTH_2, SCREEN_HEIGHT_2+10, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_UPDATEFAIL) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Update failed. Please retry if any problems arise.", SCREEN_WIDTH_2, SCREEN_HEIGHT_2, JGETEXT_CENTER);
		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("[O] Return to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}
	else if (mStage == STAGE_RETURN) {
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Update not found. Check http://cspsp.appspot.com.", SCREEN_WIDTH_2, SCREEN_HEIGHT_2, JGETEXT_CENTER);
		gFont->SetScale(0.75f);
		gFont->DrawShadowedString("[O] Return to Online Menu", SCREEN_WIDTH_2, SCREEN_HEIGHT-20, JGETEXT_CENTER);
	}


	//gFont->DrawString("under construction", 10.0f, 10.0f);
}


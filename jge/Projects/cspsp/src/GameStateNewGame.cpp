#include "GameStateNewGame.h"

GameStateNewGame::GameStateNewGame(GameApp* parent): GameState(parent)
{
}

GameStateNewGame::~GameStateNewGame()
{

}


void GameStateNewGame::Create()
{
	mMapsListBox = new ListBox(0,65,SCREEN_WIDTH,175,25,7);

	strcpy(mSearchString,"");
	mMatch = true;
	mMatchIndex = -1;

	gFont->SetBase(0);	// using 2nd font

	int y = 20;
	#ifdef WIN32
	for (int i=0; i<10; i++) {
		char buffer[10];
		sprintf(buffer,"test%d",i);
		mMapsListBox->AddItem(new MapItem(buffer));
	}
	#else
	
	int index = 0;
	DIR *dip;
	struct dirent *dit;
	dip = opendir("maps");
	char fullname[512];

	while ((dit = readdir(dip)) != NULL)
	{
		static char * name; 
		name = (char*)memalign( 16, 300); 
		sprintf(name,"%s" , dit->d_name);  
	 
		if ((FIO_S_IFREG & (dit->d_stat.st_mode & FIO_S_IFMT)) == 0 && stricmp (name, ".") != 0 && stricmp (name, "..") != 0) 
		{
			mMapsListBox->AddItem(new MapItem(name));
		}
	}
	closedir(dip);


	/**int index = 0;
	int fd; 
	struct SceIoDirent dirent; 
	fd = sceIoDopen("ms0:/psp/game/cspsp/maps"); 

	memset( &dirent, 0, sizeof(SceIoDirent)); 
	 
	while (sceIoDread(fd, &dirent) > 0) 
	{ 
		static char * name; 
		name = (char*)memalign( 16, 300); 
		sprintf(name,"%s" , dirent.d_name); 
		//sprintf(fullname, "%s/%s", "ms0:/PSP", dirent.d_name); 
	 
		if ((FIO_S_IFREG & (dirent.d_stat.st_mode & FIO_S_IFMT)) == 0 && stricmp (name, ".") != 0 && stricmp (name, "..") != 0) 
		{ 
			if (index == 0) {
				mGuiController->Add(new MenuItem(index, gFont, name, 20, y, TYPE_NORMAL, JGETEXT_LEFT, true));
			}else{
				mGuiController->Add(new MenuItem(index, gFont, name, 20, y, TYPE_NORMAL, JGETEXT_LEFT, false));
			}
			maps.push_back(name);
			index++;
			y += 20;
		} 
	} 
	sceIoDclose(fd);**/

	#endif

	mMapsListBox->Sort(MapItem::Compare);
}


void GameStateNewGame::Destroy()
{
}


void GameStateNewGame::Start()
{
	mRenderer->EnableVSync(true);
}


void GameStateNewGame::End()
{
	mRenderer->EnableVSync(false);
}


void GameStateNewGame::Update(float dt)
{
	if (!gDanzeff->mIsActive) {
		if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
			mParent->SetNextState(GAME_STATE_MENU);
		}
		if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) {
			gDanzeff->Enable();
			gDanzeff->mString = mSearchString;
		}
		if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
			MapItem *item = (MapItem*)mMapsListBox->GetItem();
			if (item != NULL) {
				gMapName = item->name;
				mParent->SetNextState(GAME_STATE_PLAY);
				return;
			}
		}

		mMapsListBox->Update(dt);
	}
	else {
		gDanzeff->Update(dt);
		if (gDanzeff->mString.length() > 50) {
			gDanzeff->mString = gDanzeff->mString.substr(0,50);
		}
		bool search = false;
		if (stricmp(mSearchString,(char*)gDanzeff->mString.c_str()) != 0) {
			search = true;
			strcpy(mSearchString,(char*)gDanzeff->mString.c_str());
		}
		if (mEngine->GetButtonClick(PSP_CTRL_START)) {
			search = true;
			gDanzeff->Disable();
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_SELECT)) {
			gDanzeff->Disable();
		}

		if (search) {
			mMatch = false;
			mMatchIndex = -1;
			if (strlen(mSearchString) > 0) {
				for (int i=0; i<mMapsListBox->mItems.size(); i++) {
					MapItem *item = (MapItem*)mMapsListBox->mItems[i];
					//printf("%d\n",strnicmp(mSearchString,mMaps[i].name,strlen(mSearchString)));
					int s = strnicmp(mSearchString,item->name,strlen(mSearchString));
					if (s == 0) {
						mMatch = true;
						mMatchIndex = i;
						break;
					}
					else if (s < 0) {
						mMatch = false;
						mMatchIndex = -1;
						break;
					}
				}
			}
			for (int i=0; i<mMapsListBox->mItems.size(); i++) {
				MapItem *item = (MapItem*)mMapsListBox->mItems[i];
				if (i == mMatchIndex) {
					item->match = true;
					mMapsListBox->SetIndices(mMatchIndex,mMatchIndex);
				}
				else {
					item->match = false;
				}
			}
		}
	}
}


void GameStateNewGame::Render()
{
	mRenderer->ClearScreen(ARGB(255,255,255,255));
	//mRenderer->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,ARGB(255,255,255,255));
	mRenderer->RenderQuad(gBgQuad, 0.0f, 0.0f);
	//mEngine->FillRect(15,15,SCREEN_WIDTH-30,SCREEN_HEIGHT-30,ARGB(150,0,0,0));

	mRenderer->FillRect(0,65,SCREEN_WIDTH,175,ARGB(100,0,0,0));
	mRenderer->FillRect(0,35,SCREEN_WIDTH,30,ARGB(175,0,0,0));
	mRenderer->DrawLine(90,35,90,65,ARGB(255,255,255,255));

	gFont->SetColor(ARGB(255,255,255,255));


	gFont->SetScale(1.0f);
	gFont->DrawShadowedString("Map Selection",20,10);
	gFont->DrawShadowedString("search:",20,40);
	if (mMatch) {
		gFont->SetColor(ARGB(255,0,128,255));
	}
	else {
		gFont->SetColor(ARGB(255,255,0,0));
	}
	gFont->DrawShadowedString(mSearchString,100,40);

	gFont->SetColor(ARGB(255,255,255,255));
	if (gDanzeff->mIsActive) {
		gFont->DrawShadowedString("|",100+gFont->GetStringWidth(mSearchString),40);
	}

	mMapsListBox->Render();

	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(0.75f);
	if (gDanzeff->mIsActive) {
		gFont->DrawShadowedString("[START] Enter    [SELECT] Cancel",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
		gDanzeff->Render(SCREEN_WIDTH-175,SCREEN_HEIGHT-175);
	}
	else {
		gFont->DrawShadowedString("[X] Select     [[]] Search     [O] Return to Menu",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
	}
}
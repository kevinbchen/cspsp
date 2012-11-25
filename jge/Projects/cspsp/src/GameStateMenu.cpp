#include "GameStateMenu.h"

GameStateMenu::GameStateMenu(GameApp* parent): GameState(parent)
{
	mGuiController = NULL;
}

GameStateMenu::~GameStateMenu()
{

}


void GameStateMenu::Create()
{
	//gFont->SetBase(0);	// using 2nd font
	mGuiController = new JGuiController(100,this);
	//mGuiController->SetShadingBackground(10, 45, 80, 100, ARGB(255,0,0,0));
	if (mGuiController)
	{
		mGuiController->Add(new MenuItem(1, gFont, "singleplayer", SCREEN_WIDTH-20, 130, TYPE_MAIN, JGETEXT_RIGHT, true));
		mGuiController->Add(new MenuItem(2, gFont, "multiplayer", SCREEN_WIDTH-20, 155, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(3, gFont, "settings", SCREEN_WIDTH-20, 180, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(4, gFont, "credits", SCREEN_WIDTH-20, 205, TYPE_MAIN, JGETEXT_RIGHT));
		mGuiController->Add(new MenuItem(5, gFont, "quit", SCREEN_WIDTH-20, 230, TYPE_MAIN, JGETEXT_RIGHT));
	}
}


void GameStateMenu::Destroy()
{
	SAFE_DELETE(mGuiController);

	/**if (mMusic)
		mEngine->FreeMusic(mMusic);**/

}


void GameStateMenu::Start()
{
	gLogout = false;

	mStage = STAGE_MENU;
	mAlpha = 0.0f;
	mInfoX = -400.0f;
	//gLogoQuad->SetHotSpot(64.0f, 32.0f);
	mRenderer->EnableVSync(true);

	gFont->SetBase(0);
	/**mMusic = mEngine->LoadMusic("sfx/Expedite.mp3");
	if (mMusic)
	{
		mEngine->PlayMusic(mMusic);
		mEngine->SetVolume(64);
	}**/
}


void GameStateMenu::End()
{
	mRenderer->EnableVSync(false);

	/**if (mMusic)
	{
		mEngine->StopMusic();
		mEngine->FreeMusic(mMusic);		// need to free the music to stop it, bug in mikmodlib
		mMusic = NULL;
	}**/
}


void GameStateMenu::Update(float dt)
{
	if (mStage==STAGE_INIT)
	{
		mAlpha += FADING_SPEED*dt;
		if (mAlpha > 255.0f)
		{
			mAlpha = 255.0f;
			mStage = STAGE_MENU;

			//mEngine->EnableVSync(false);
		}
	}
	else
	{
		if (mStage==STAGE_MENU)
		{
			mInfoX *= 0.75f;
			int i = mGuiController->GetCurr();
			mGuiController->Update(dt);
			if (i != mGuiController->GetCurr()) {
				mInfoX = -400.0f;
			}

		}
		else
		{
			mCreditsYPos -= 0.03f*dt;
			if (mEngine->GetButtonState(PSP_CTRL_CROSS)) {
				mCreditsYPos -= 0.08f*dt;
			}
			if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
				mStage = STAGE_MENU;
			}
		}
	}
}


void GameStateMenu::Render()
{
	mRenderer->ClearScreen(ARGB(255,255,255,255));
	//mRenderer->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,ARGB(255,255,255,255));
	if (mStage == STAGE_INIT)
	{
		int alpha = (int)mAlpha;
		gBgQuad->SetColor(ARGB(alpha,255,255,255));
		gLogoQuad->SetColor(ARGB(alpha,255,255,255));
	}

	mRenderer->RenderQuad(gBgQuad, 0.0f, 0.0f);

	if (mStage == STAGE_MENU)
	{
		mRenderer->RenderQuad(gLogoQuad, 0.0f, 0.0f);
		mRenderer->FillRect(0,128,SCREEN_WIDTH,125,ARGB(100,0,0,0));
		int i = mGuiController->GetCurr();
		mRenderer->FillRect(0,128+i*25,SCREEN_WIDTH,25,ARGB(255,0,0,0));
		mGuiController->Render();

		gFont->SetScale(0.75f);
		gFont->SetColor(ARGB(255,255,255,255));

		char info[256];
		if (i == 0) {
			strcpy(info,"Play against bots");
		}
		else if (i == 1) {
			strcpy(info,"Play online with other people!");
		}
		else if (i == 2) {
			strcpy(info,"Change settings and view controls");
		}
		else if (i == 3) {
			strcpy(info,"View the credits roll");
		}
		else if (i == 4) {
			strcpy(info,"Quit the game ):");
		}
		gFont->DrawString(info,10+mInfoX,133+i*25);

		gFont->SetColor(ARGB(255,255,255,255));
		gFont->DrawShadowedString("Official Website: http://cspsp.appspot.com", 5, SCREEN_HEIGHT-15);

		gFont->SetBase(0);
		gFont->SetColor(ARGB(255,200,200,200));
		gFont->SetScale(0.6f);
		char buffer[10];
		sprintf(buffer,"%.2f",VERSION);
		gFont->DrawString(buffer,SCREEN_WIDTH-4,SCREEN_HEIGHT-12,JGETEXT_RIGHT);
	}
	else if (mStage == STAGE_CREDITS)
	{
		gFont->SetBase(0);
		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.75f);

		float y = mCreditsYPos;
		int count = sizeof(mCredits)/sizeof(char *);
		int alpha;
		//float scale;

		//gFont->SetBase(128);

		for (int i=0;i<count;i++)
		{
			if (y > -20.0f && y < SCREEN_HEIGHT_F)
			{
				//alpha = (int)((y/SCREEN_HEIGHT_F)*255.0f);
				alpha = (int)((SCREEN_HEIGHT-abs((int)(y-SCREEN_HEIGHT_2)))*255/(SCREEN_HEIGHT_2));
				if (alpha < 0)
					alpha = 0;
				//scale = 1.0f + (alpha/255.0f);//*0.5f;
				gFont->SetColor(ARGB(255, 255, 255, 255));
				//gFont->SetScale(0.5f);

				if (i < 2) {
					gFont->SetScale(1.0f);
					gFont->DrawShadowedString(mCategories[i], SCREEN_WIDTH_2, y,JGETEXT_CENTER);
				}
				/*else if (i == count-1) {
					gFont->SetScale(1.0f);
					gFont->DrawShadowedString(mCategories[i], SCREEN_WIDTH_2, y,JGETEXT_CENTER);
				}*/
				else {
					gFont->SetScale(0.75f);
					gFont->SetColor(ARGB(255,0,128,255));
					gFont->DrawShadowedString(mCategories[i], 100, y,JGETEXT_RIGHT);
					gFont->SetColor(ARGB(255,255,255,255));
					gFont->DrawShadowedString(mCredits[i], 120, y);
				}

			}
			y += 18.0f;
		}

		//gFont->SetBase(0);
		if (y < 0.0f)
			mStage = STAGE_MENU;
	}
}


void GameStateMenu::ButtonPressed(int controllerId, int controlId)
{
	switch (controlId)
	{
	case 1:
		mParent->SetNextState(GAME_STATE_NEW_GAME);
		break;
	case 2:
		mParent->SetNextState(GAME_STATE_CONNECT);
		break;
	case 3:
		mParent->SetNextState(GAME_STATE_OPTIONS);
		break;
	case 4:		// credits
		mStage = STAGE_CREDITS;
		mCreditsYPos = 270.0f;
		break;
	case 5:
		mEngine->End();
		break;
	}
}
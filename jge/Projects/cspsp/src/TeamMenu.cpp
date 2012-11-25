
#include "TeamMenu.h"
#include "Globals.h"

JGE* TeamMenu::mEngine = NULL;
JRenderer* TeamMenu::mRenderer = NULL;

//------------------------------------------------------------------------------------------------
TeamMenu::TeamMenu()
{
	mEngine = JGE::GetInstance();
	mRenderer = JRenderer::GetInstance();
	
	mTeam = NONE;
	mType = 0;

	mIsActive = false;
	mIsSelected = false;
	mCategoryIndex = MAIN1;
	mSelectedIndex = -1;

	mCategories[MAIN1].id = MAIN1;
	mCategories[MAIN1].buttons.push_back(Button(T,"terrorist forces"));
	mCategories[MAIN1].buttons.push_back(Button(CT,"CT forces"));
	mCategories[MAIN1].buttons.push_back(Button(NONE,"spectator"));

	mCategories[CT].id = CT;
	mCategories[CT].buttons.push_back(Button(0,"SEAL TEAM 6"));
	mCategories[CT].buttons.push_back(Button(1,"GSG-9"));
	mCategories[CT].buttons.push_back(Button(2,"SAS"));
	mCategories[CT].buttons.push_back(Button(3,"GIGN"));

	mCategories[T].id = T;
	mCategories[T].buttons.push_back(Button(0,"PHOENIX CONNEXION"));
	mCategories[T].buttons.push_back(Button(1,"ELITE CREW"));
	mCategories[T].buttons.push_back(Button(2,"ARCTIC AVENGERS"));
	mCategories[T].buttons.push_back(Button(3,"GUERILLA WARFARE"));

	mCT = new Person(gPlayersQuads[CT][0],gPlayersDeadQuads[0][0],NULL,NULL,CT,"test",0);
	mT = new Person(gPlayersQuads[T][0],gPlayersDeadQuads[0][0],NULL,NULL,CT,"test",0);
	mCT->mGuns[KNIFE] = new GunObject(&gGuns[0],0,0);
	mT->mGuns[KNIFE] = new GunObject(&gGuns[0],0,0);
	mCT->PickUp(new GunObject(&gGuns[17],0,0));
	mT->PickUp(new GunObject(&gGuns[18],0,0));
	mCT->mX = -SCREEN_WIDTH_2+330;
	mCT->mY = -SCREEN_HEIGHT_2+155;
	mT->mX = -SCREEN_WIDTH_2+330;
	mT->mY = -SCREEN_HEIGHT_2+155;

	mIsOldStyle = true;

	FormatText(mTeamLines,"Eliminate the other team. The first team to kill all of the other players on the other team wins.",150,0.6f);
	FormatText(mCTFLines,"Capture the enemy flag while defending your own. The team with the most captures by the end of the round wins.",150,0.6f);
	FormatText(mFFALines,"Eliminate everyone else. The player with the most kills by the end of the round wins.\n\nTeam selection only matters for your appearance and weapons.",150,0.6f);
}

//------------------------------------------------------------------------------------------------
TeamMenu::~TeamMenu()
{
	
}

//------------------------------------------------------------------------------------------------
void TeamMenu::Update(float dt)
{
	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
		Disable();
		//mIsActive = false;
		//mTeam = CT;
		return;
	}
	if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE) && mCategoryIndex != MAIN1) {
		mCategoryIndex = MAIN1;
	}

	if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
		if (mSelectedIndex != -1) {
			if (mCategoryIndex == MAIN1 && mSelectedIndex != 2) {
				mCategoryIndex = mCategories[mCategoryIndex].buttons[mSelectedIndex].id;
				if (mIsOldStyle) {
					mSelectedIndex = 0;
				}
			}
			else if (mCategoryIndex == MAIN1 && mSelectedIndex == 2) {
				mTeam = NONE;
				mType = 0;
				mIsSelected = true;
				Disable();
			}
			else {	
				mTeam = mCategoryIndex;
				mType = mCategories[mCategoryIndex].buttons[mSelectedIndex].id;
				mIsSelected = true;
				Disable();
			}
		}
	}

	if (!mIsOldStyle) {
		float aX = mEngine->GetAnalogX()-127.5f;
		float aY = mEngine->GetAnalogY()-127.5f;
		
		int size = mCategories[mCategoryIndex].buttons.size();
		if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
			angle = atan2f(aX,aY) + M_PI;
			if (mCategoryIndex == MAIN1) {
				angle += M_PI/3;
			}
			mSelectedIndex = (int)(size*angle/(2*M_PI)+0.5f);
			if (mSelectedIndex == size) {
				mSelectedIndex = 0;
			}
		}
		else {
			mSelectedIndex = -1;
		}
	}
	else {
		int size = mCategories[mCategoryIndex].buttons.size();
		if (mEngine->GetButtonClick(PSP_CTRL_UP)) {
			mSelectedIndex--;
			if (mSelectedIndex < 0) mSelectedIndex = size-1;
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_DOWN)) {
			mSelectedIndex++;
			if (mSelectedIndex >= size) mSelectedIndex = 0;
		}
	}

	mCT->Update(dt);
	mT->Update(dt);
}



//------------------------------------------------------------------------------------------------
void TeamMenu::Render()
{	
	if (!mIsActive) return;
	mRenderer->FillRect(300,25,SCREEN_WIDTH-300-10,SCREEN_HEIGHT-40,ARGB(200,45,50,50));
	mRenderer->DrawRect(300,25,SCREEN_WIDTH-300-10,SCREEN_HEIGHT-40,ARGB(255,255,255,255));
	//mRenderer->FillCircle(SCREEN_WIDTH_2,SCREEN_HEIGHT_2,SCREEN_HEIGHT_2,ARGB(200,50,50,50));
	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(1.0f);
	gFont->DrawString("Team Select", 310, 40);
	gFont->SetScale(0.75f);

	if (!mIsOldStyle) {
		gFont->DrawString("[ANALOG+X] Select",320,70);
	}
	else {
		gFont->DrawString("[DIR PAD+X] Select",320,70);
	}
	gFont->DrawString("[O] Cancel",320,90);
	if (mCategoryIndex != MAIN1) {
		gFont->DrawString("[^] Return",320,110);
	}

	if (mCategoryIndex == MAIN1) {
		if (*mGameType == TEAM) {
			gFont->SetScale(0.75f);
			gFont->DrawString("Team Deathmatch",310,140);
			gFont->SetScale(0.6f);
			for (int i=0; i<mTeamLines.size(); i++) {
				gFont->DrawString(mTeamLines[i],315,160+10*i);
			}
		}
		else if (*mGameType == FFA) {
			gFont->SetScale(0.75f);
			gFont->DrawString("Free for All",310,140);
			gFont->SetScale(0.6f);
			for (int i=0; i<mFFALines.size(); i++) {
				gFont->DrawString(mFFALines[i],315,160+10*i);
			}
		}
		else if (*mGameType == CTF) {
			gFont->SetScale(0.75f);
			gFont->DrawString("Capture the Flag",310,140);
			gFont->SetScale(0.6f);
			for (int i=0; i<mCTFLines.size(); i++) {
				gFont->DrawString(mCTFLines[i],315,160+10*i);
			}
		}
	}

	int size = mCategories[mCategoryIndex].buttons.size();
	float theta = -M_PI_2;
	if (mCategoryIndex == MAIN1) {
		theta += M_PI/3;
	}
	float step = 1.0f/size*(2*M_PI);
	for (int i=0; i<size; i++) {
		//float theta = (float)i/size*(2*M_PI);
		float x = 140+75*cosf(theta);
		float y = SCREEN_HEIGHT_2+75*sinf(theta);
		if (mIsOldStyle) {
			x = 50;
			y = 25+32*i;
		}

		if (i == mSelectedIndex) {
			gFont->SetColor(ARGB(255,255,255,255));
			if (mCategoryIndex == MAIN1) {
				if (i == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
				else if (i == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
			}
			else if (mCategoryIndex == T) {
				gFont->SetColor(ARGB(255,255,64,64));
			}
			else if (mCategoryIndex == CT) {
				gFont->SetColor(ARGB(255,153,204,255));
			}

			gFont->SetScale(1.0f);
			if (!mIsOldStyle) {
				mRenderer->FillCircle(x,y,30,ARGB(200,255,255,255));
				mRenderer->DrawCircle(x,y,30,ARGB(255,255,255,255));
				gFont->DrawShadowedString(mCategories[mCategoryIndex].buttons[i].name,x,y-8,JGETEXT_CENTER);
			}
			else {
				mRenderer->FillRect(x,y,200,25,ARGB(200,255,255,255));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				gFont->DrawShadowedString(mCategories[mCategoryIndex].buttons[i].name,x+10,y+3);
			}
			//mRenderer->FillPolygon(x,y,150,3,M_PI-theta,ARGB(200,255,255,255));
			//mRenderer->FillRect(x-75,y-25,150,50,ARGB(200,255,255,255));
		}
		else {
			gFont->SetScale(0.75f);

			gFont->SetColor(ARGB(255,255,255,255));
			if (mCategoryIndex == MAIN1) {
				if (i == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
				else if (i == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
			}
			else if (mCategoryIndex == T) {
				gFont->SetColor(ARGB(255,255,64,64));
			}
			else if (mCategoryIndex == CT) {
				gFont->SetColor(ARGB(255,153,204,255));
			}

			if (!mIsOldStyle) {
				mRenderer->FillCircle(x,y,20,ARGB(200,45,50,50));
				mRenderer->DrawCircle(x,y,20,ARGB(255,255,255,255));
				gFont->DrawShadowedString(mCategories[mCategoryIndex].buttons[i].name,x,y-5,JGETEXT_CENTER);
			}
			else {
				mRenderer->FillRect(x,y,200,25,ARGB(200,45,50,50));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				gFont->DrawShadowedString(mCategories[mCategoryIndex].buttons[i].name,x+10,y+6);
			}
			//mRenderer->FillPolygon(x,y,150,3,M_PI-theta,ARGB(200,50,50,50));
			//mRenderer->FillRect(x-75,y-25,150,50,ARGB(200,50,50,50));
		}
		//mRenderer->FillRect(x-50,y-10,100,20,ARGB(200,50,50,50));
		
		theta -= step;
	}
	if (mCategoryIndex != MAIN1 && mSelectedIndex != -1) {
		int id = mCategories[mCategoryIndex].buttons[mSelectedIndex].id;
		if (mCategoryIndex == CT) {
			mCT->SetQuads(gPlayersQuads[CT][id],gPlayersDeadQuads[0][0]);
			mCT->Render(0,0);
		}
		else if (mCategoryIndex == T) {
			mT->SetQuads(gPlayersQuads[T][id],gPlayersDeadQuads[0][0]);
			mT->Render(0,0);
		}
	}

	//gFont->SetScale(0.6f);
}

//------------------------------------------------------------------------------------------------
void TeamMenu::Enable()
{	
	mIsSelected = false;
	mIsActive = true;
	mCategoryIndex = MAIN1;
	if (!mIsOldStyle) {
		mSelectedIndex = -1;
	}
	else {
		mSelectedIndex = 0;
	}
}

//------------------------------------------------------------------------------------------------
void TeamMenu::Disable()
{	
	//mIsSelected = false;
	mIsActive = false;
	*cross = true;
	//mMainController->SetCurr(0);
	//mMainController->SetActive(true);
	//mMainController->mLastKey = PSP_CTRL_UP;
	//mMainController->mKeyRepeatDelay = JGUI_INITIAL_DELAY;
	/*for (int i=PISTOLS; i<=MACHINEGUNS; i++) {
		mControllers[i]->SetCurr(0);
		mControllers[i]->SetActive(false);
	}*/
}

//------------------------------------------------------------------------------------------------
int TeamMenu::GetTeam()
{
	return mTeam;
}

//------------------------------------------------------------------------------------------------
int TeamMenu::GetType()
{
	return mType;
}
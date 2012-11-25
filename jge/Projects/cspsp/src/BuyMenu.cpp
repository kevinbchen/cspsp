
#include "BuyMenu.h"
#include "Globals.h"

JGE* BuyMenu::mEngine = NULL;
JRenderer* BuyMenu::mRenderer = NULL;

//------------------------------------------------------------------------------------------------
BuyMenu::BuyMenu(Person* player, Gun guns[])
{
	for (int i=0; i<28; i++) {
		mGuns[i] = guns[i];
	}

	mEngine = JGE::GetInstance();
	mRenderer = JRenderer::GetInstance();
	
	mPlayer = player;

	mChoice = -3;

	mIsActive = false;
	mIsSelected = false;
	mCategoryIndex = MAIN;
	mSelectedIndex = -1;

	mCategories[T][MAIN].id = MAIN;
	mCategories[T][MAIN].buttons.push_back(Button(PISTOLS,"pistols"));
	mCategories[T][MAIN].buttons.push_back(Button(SHOTGUNS,"shotguns"));
	mCategories[T][MAIN].buttons.push_back(Button(SMG,"SMGs"));
	mCategories[T][MAIN].buttons.push_back(Button(RIFLES,"rifles"));
	mCategories[T][MAIN].buttons.push_back(Button(MACHINEGUNS,"machine guns"));
	mCategories[T][MAIN].buttons.push_back(Button(EQUIPMENT,"equipment"));
	mCategories[CT][MAIN] = mCategories[T][MAIN];

	for (int i=1; i<=6; i++) {
		if (i == 5) {
			mCategories[CT][PISTOLS].buttons.push_back(Button(i,guns[i].mName));
		}
		else if (i == 6) {
			mCategories[T][PISTOLS].buttons.push_back(Button(i,guns[i].mName));
		}
		else {
			mCategories[T][PISTOLS].buttons.push_back(Button(i,guns[i].mName));
			mCategories[CT][PISTOLS].buttons.push_back(Button(i,guns[i].mName));
		}
	}
	for (int i=7; i<=8; i++) {
		mCategories[T][SHOTGUNS].buttons.push_back(Button(i,guns[i].mName));
		mCategories[CT][SHOTGUNS].buttons.push_back(Button(i,guns[i].mName));
	}	
	for (int i=9; i<=13; i++) {
		if (i == 9) {
			mCategories[CT][SMG].buttons.push_back(Button(i,guns[i].mName));
		}
		else if (i == 10) {
			mCategories[T][SMG].buttons.push_back(Button(i,guns[i].mName));
		}
		else {
			mCategories[T][SMG].buttons.push_back(Button(i,guns[i].mName));
			mCategories[CT][SMG].buttons.push_back(Button(i,guns[i].mName));
		}
	}	
	for (int i=14; i<=23; i++) {
		if (i == 14 || i == 17 || i == 19 || i == 21) {
			mCategories[CT][RIFLES].buttons.push_back(Button(i,guns[i].mName));
		}
		else if (i == 15 || i == 18 || i == 20 || i == 22) {
			mCategories[T][RIFLES].buttons.push_back(Button(i,guns[i].mName));
		}
		else {
			mCategories[T][RIFLES].buttons.push_back(Button(i,guns[i].mName));
			mCategories[CT][RIFLES].buttons.push_back(Button(i,guns[i].mName));
		}
	}	
	mCategories[T][MACHINEGUNS].buttons.push_back(Button(24,guns[24].mName));
	mCategories[CT][MACHINEGUNS].buttons.push_back(Button(24,guns[24].mName));

	for (int i=25; i<=27; i++) {
		mCategories[T][EQUIPMENT].buttons.push_back(Button(i,guns[i].mName));
		mCategories[CT][EQUIPMENT].buttons.push_back(Button(i,guns[i].mName));
	}

	mIsOldStyle = true;
}

//------------------------------------------------------------------------------------------------
BuyMenu::~BuyMenu()
{
}

//------------------------------------------------------------------------------------------------
void BuyMenu::Update(float dt)
{
	int team = mPlayer->mTeam;
	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
		Disable();
		//mIsActive = false;
		return;
	}
	if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE) && mCategoryIndex != MAIN) {
		mCategoryIndex = MAIN;
	}

	if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
		if (mSelectedIndex != -1) {
			if (mCategoryIndex == MAIN) {
				mCategoryIndex = mCategories[team][mCategoryIndex].buttons[mSelectedIndex].id;
				if (mIsOldStyle) {
					mSelectedIndex = 0;
				}
			}
			else {	
				mChoice = mCategories[team][mCategoryIndex].buttons[mSelectedIndex].id;
				mIsSelected = true;
				Disable();
			}
		}
		else {
			if (mCategoryIndex == MAIN) {
				mChoice = -1;
				mIsSelected = true;
				Disable();
			}
		}
	}

	if (!mIsOldStyle) {
		float aX = mEngine->GetAnalogX()-127.5f;
		float aY = mEngine->GetAnalogY()-127.5f;
		
		int size = mCategories[team][mCategoryIndex].buttons.size();
		if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
			angle = atan2f(aX,aY) + M_PI;
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
		int size = mCategories[team][mCategoryIndex].buttons.size();
		if (mEngine->GetButtonClick(PSP_CTRL_UP)) {
			mSelectedIndex--;
			if (mCategoryIndex == MAIN) {
				if (mSelectedIndex < -1) mSelectedIndex = size-1;
			}
			else {
				if (mSelectedIndex < 0) mSelectedIndex = size-1;
			}
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_DOWN)) {
			mSelectedIndex++;
			if (mCategoryIndex == MAIN) {
				if (mSelectedIndex >= size) mSelectedIndex = -1;
			}
			else {
				if (mSelectedIndex >= size) mSelectedIndex = 0;
			}
		}
	}
}



//------------------------------------------------------------------------------------------------
void BuyMenu::Render()
{	
	if (!mIsActive) return;
	mRenderer->FillRect(300,25,SCREEN_WIDTH-300-10,SCREEN_HEIGHT-40,ARGB(200,45,50,50));
	mRenderer->DrawRect(300,25,SCREEN_WIDTH-300-10,SCREEN_HEIGHT-40,ARGB(255,255,255,255));
	//mRenderer->FillCircle(SCREEN_WIDTH_2,SCREEN_HEIGHT_2,SCREEN_HEIGHT_2,ARGB(200,50,50,50));
	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(1.0f);
	gFont->DrawString("Buy Menu", 310, 40);
	gFont->SetScale(0.75f);

	if (!mIsOldStyle) {
		gFont->DrawString("[ANALOG+X] Select",320,70);
	}
	else {
		gFont->DrawString("[DIR PAD+X] Select",320,70);
	}
	gFont->DrawString("[O] Cancel",320,90);
	if (mCategoryIndex != MAIN) {
		gFont->DrawString("[^] Return",320,110);
	}

	int team = mPlayer->mTeam;
	int size = mCategories[team][mCategoryIndex].buttons.size();
	float theta = -M_PI_2;
	float step = 1.0f/size*(2*M_PI);
	for (int i=0; i<size; i++) {
		//float theta = (float)i/size*(2*M_PI);
		float x = 140+100*cosf(theta);
		float y = SCREEN_HEIGHT_2+100*sinf(theta);

		if (mIsOldStyle) {
			x = 50;
			y = 25+32*i;
		}

		if (i == mSelectedIndex) {
			gFont->SetScale(1.0f);
			int id = 0;
			if (mCategoryIndex != MAIN) {
				id = mCategories[team][mCategoryIndex].buttons[i].id;
				//gFont->DrawShadowedString(mCategories[team][mCategoryIndex].buttons[i].name,x,y,JGETEXT_CENTER);
			}
			else {
				int cid = mCategories[team][mCategoryIndex].buttons[i].id;
				id = mCategories[team][cid].buttons[0].id;
			}
			
			if (!mIsOldStyle) {
				mRenderer->FillCircle(x,y,30,ARGB(200,255,255,255));
				mRenderer->DrawCircle(x,y,30,ARGB(255,255,255,255));
				mRenderer->RenderQuad(mGuns[id].mGroundQuad,x,y-5,0,1.4f,1.4f);
				gFont->DrawShadowedString(mCategories[team][mCategoryIndex].buttons[i].name,x,y,JGETEXT_CENTER);
			}
			else {
				mRenderer->FillRect(x,y,200,25,ARGB(200,255,255,255));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				mRenderer->RenderQuad(mGuns[id].mGroundQuad,x+25,y+12,0,1.4f,1.4f);
				gFont->DrawShadowedString(mCategories[team][mCategoryIndex].buttons[i].name,x+50,y+3);
			}
			//mRenderer->FillPolygon(x,y,150,3,M_PI-theta,ARGB(200,255,255,255));
			//mRenderer->FillRect(x-75,y-25,150,50,ARGB(200,255,255,255));
		}
		else {
			gFont->SetScale(0.75f);
			int id = 0;
			if (mCategoryIndex != MAIN) {
				id = mCategories[team][mCategoryIndex].buttons[i].id;
			}
			else {
				int cid = mCategories[team][mCategoryIndex].buttons[i].id;
				id = mCategories[team][cid].buttons[0].id;
			}

			if (!mIsOldStyle) {
				mRenderer->FillCircle(x,y,20,ARGB(200,45,50,50));
				mRenderer->DrawCircle(x,y,20,ARGB(255,255,255,255));
				mRenderer->RenderQuad(mGuns[id].mGroundQuad,x,y-5);
				gFont->DrawShadowedString(mCategories[team][mCategoryIndex].buttons[i].name,x,y+3,JGETEXT_CENTER);
			}
			else {
				mRenderer->FillRect(x,y,200,25,ARGB(200,45,50,50));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				mRenderer->RenderQuad(mGuns[id].mGroundQuad,x+25,y+12);
				gFont->DrawShadowedString(mCategories[team][mCategoryIndex].buttons[i].name,x+50,y+6);
			}

			//mRenderer->FillPolygon(x,y,150,3,M_PI-theta,ARGB(200,50,50,50));
			//mRenderer->FillRect(x-75,y-25,150,50,ARGB(200,50,50,50));
		}
		//mRenderer->FillRect(x-50,y-10,100,20,ARGB(200,50,50,50));
		
		theta -= step;
	}

	if (mCategoryIndex == MAIN) {
		float x = 140;
		float y = SCREEN_HEIGHT_2;
		if (mIsOldStyle) {
			x = 50;
			y = 25+32*size;
		}
		if (!mIsOldStyle) {
			if (mSelectedIndex == -1) {
				gFont->SetScale(1.0f);
				mRenderer->FillCircle(x,y,30,ARGB(200,255,255,255));
				mRenderer->DrawCircle(x,y,30,ARGB(255,255,255,255));
				gFont->DrawShadowedString("ammo",x,y,JGETEXT_CENTER);
			}
			else {
				gFont->SetScale(0.75f);
				mRenderer->FillCircle(x,y,20,ARGB(200,45,50,50));
				mRenderer->DrawCircle(x,y,20,ARGB(255,255,255,255));
				gFont->DrawShadowedString("ammo",x,y+3,JGETEXT_CENTER);
			}
		}
		else {
			if (mSelectedIndex == -1) {
				gFont->SetScale(1.0f);
				mRenderer->FillRect(x,y,200,25,ARGB(200,255,255,255));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				gFont->DrawShadowedString("ammo",x+50,y+3);
			}
			else {
				gFont->SetScale(0.75f);
				mRenderer->FillRect(x,y,200,25,ARGB(200,45,50,50));
				mRenderer->DrawRect(x,y,200,25,ARGB(255,255,255,255));
				gFont->DrawShadowedString("ammo",x+50,y+6);
			}
		}
	}

	gFont->SetScale(0.6f);
	if (mCategoryIndex != MAIN && mSelectedIndex != -1) {
		int id = mCategories[team][mCategoryIndex].buttons[mSelectedIndex].id;
		mRenderer->RenderQuad(mGuns[id].mGroundQuad,330,155,0,1.4f,1.4f);

		float x = 310;
		float y = 175;
		int width = 0;
		float value = 0.0f;
		float min = 0.0f;
		float max = 1.0f;

		if (id != 25 && id != 27) {
			gFont->DrawString("Damage:",x,y);
			mRenderer->FillRect(x+60,y+2,90,5,ARGB(255,100,100,100));

			value = mGuns[id].mDamage;
			min = 5;
			max = 40;
			if (id >= 1 && id <= 6) { //pistols
			}
			else if (id >= 7 && id <= 8) { //shotguns
				if (id == 7) value *= 6;
				else if (id == 8) value *= 4;
				max = 100;
			}
			else if (id >= 9 && id <= 13) { //smgs
			}
			else if (id == 16 || (id >= 21 && id <= 23)) { //snipers
				max = 100;
			}
			else if (id >= 14 && id <= 20) { //rifles
			}
			else if (id == 24) { //machine guns
			}
			else if (id >= 25) { //nades
				max = 100;
			}
			width = 90*((value-min)/(max-min));
			if (width > 90) width = 90;
			else if (width < 0) width = 0;
			mRenderer->FillRect(x+60,y+2,width,5,ARGB(255,255,255,255));
			y += 12;
		}

		if (id != 25 && id != 26 && id != 27) {
			gFont->DrawString("Accuracy:",x,y);
			mRenderer->FillRect(x+60,y+2,90,5,ARGB(255,100,100,100));

			value = M_PI-mGuns[id].mSpread;
			min = M_PI_2+M_PI_4/2;
			max = M_PI;
			if (id >= 1 && id <= 6) { //pistols
			}
			else if (id >= 7 && id <= 8) { //shotguns
			}
			else if (id >= 9 && id <= 13) { //smgs
			}
			else if (id == 16 || (id >= 21 && id <= 23)) { //snipers
				min = 0;
			}
			else if (id >= 14 && id <= 20) { //rifles
			}
			else if (id == 24) { //machine guns
			}
			else if (id >= 25) { //nades
				value = 0;
			}

			width = 90*((value-min)/(max-min));
			if (width > 90) width = 90;
			else if (width < 0) width = 0;
			mRenderer->FillRect(x+60,y+2,width,5,ARGB(255,255,255,255));
			y += 12;

			gFont->DrawString("Fire Rate:",x,y);
			mRenderer->FillRect(x+60,y+2,90,5,ARGB(255,100,100,100));

			value = 2000-mGuns[id].mDelay;
			min = 1850;
			max = 1950;
			if (id >= 1 && id <= 6) { //pistols
			}
			else if (id >= 7 && id <= 8) { //shotguns
				min = 500;
				max = 3000;
			}
			else if (id >= 9 && id <= 13) { //smgs
			}
			else if (id == 16 || (id >= 21 && id <= 23)) { //snipers
				min = 250;
				max = 3500;
			}
			else if (id >= 14 && id <= 20) { //rifles
			}
			else if (id == 24) { //machine guns
			}
			else if (id >= 25) { //nades
				value = 0;
			}
			width = 90*((value-min)/(max-min));
			if (width > 90) width = 90;
			else if (width < 0) width = 0;
			mRenderer->FillRect(x+60,y+2,width,5,ARGB(255,255,255,255));
			y += 12;
		}


		char buffer[16];
		sprintf(buffer,"$%i",mGuns[id].mCost);
		gFont->DrawString("Price",x,y);
		gFont->DrawString(buffer,x+60,y);
		
		

	}

	//gFont->printf(SCREEN_WIDTH_2,SCREEN_HEIGHT_2,"%f",angle);
	/*if (mMainController->IsActive()) {
		mMainController->Render();
	}
	else {
		gFont->DrawString("[^] Return", 220, 90);
		for (int i=PISTOLS; i<=EQUIPMENT; i++) {
			if (mControllers[i]->IsActive()) {
				mControllers[i]->Render();
			}
		}
	}*/
	
	/*for (int i=PISTOLS; i<=EQUIPMENT; i++) {
		if (mControllers[i]->IsActive()) {
			int id = mControllers[i]->GetGuiObject(mControllers[i]->GetCurr())->GetId();
			mRenderer->RenderQuad(mGuns[id].mGroundQuad,236,126);
			char buffer[16];
			sprintf(buffer,"Price: $%i",mGuns[id].mCost);
			gFont->DrawString(buffer,220,142);
		}
	}*/
}

//------------------------------------------------------------------------------------------------
void BuyMenu::Enable()
{	
	mIsSelected = false;
	mIsActive = true;
	mCategoryIndex = MAIN;
	if (!mIsOldStyle) {
		mSelectedIndex = -1;
	}
	else {
		mSelectedIndex = 0;
	}
	/*mMainController->SetCurr(0);
	mMainController->SetActive(true);
	mMainController->mLastKey = PSP_CTRL_UP;
	mMainController->mKeyRepeatDelay = JGUI_INITIAL_DELAY;
	for (int i=PISTOLS; i<=EQUIPMENT; i++) {
		mControllers[i]->SetCurr(0);
		mControllers[i]->SetActive(false);
	}*/
}

//------------------------------------------------------------------------------------------------
void BuyMenu::Disable()
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
int BuyMenu::GetChoice()
{
	return mChoice;
}

/*//------------------------------------------------------------------------------------------------
void BuyMenu::ButtonPressed(int controllerId, int controlId)
{
	if (controllerId == MAIN) {
		if (controlId >= PISTOLS && controlId <= EQUIPMENT) {
			mMainController->SetActive(false);
			mControllers[controlId]->SetActive(true);
		}
		else if (controlId == PRIMARYAMMO) {
			mChoice = -1;
			mIsSelected = true;
			Disable();
		}
		else if (controlId == SECONDARYAMMO) {
			mChoice = -2;
			mIsSelected = true;
			Disable();
		}
	}
	else if (controllerId >= PISTOLS && controllerId <= EQUIPMENT) {
		mChoice = controlId;
		mIsSelected = true;
		Disable();
	}

}*/

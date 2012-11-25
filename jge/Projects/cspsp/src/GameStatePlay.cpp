#include "GameStatePlay.h"

GameStatePlay::GameStatePlay(GameApp* parent): Game(parent) 
{
	/*mSpecState = FREELOOK;
	mSpecDead = false;
	mRoundTimer = 0;
	mRoundEndTimer = 0;
	mTimer = 0;
	mSwitchTimer = 0;
	//mMusic = NULL;
	//mFont = NULL;
	mGuiController = NULL;
	mSpec = NULL;
	cross = false;*/
}

GameStatePlay::~GameStatePlay() {}

void GameStatePlay::Create()
{
	mIsOnline = false;

	srand((int)time(NULL));

	//mSpecX = 0;
	//mSpecY = 0;
	
	//mHud = new Hud();

	//mWinner = NONE;

	for (int i=0; i<28; i++) {
		mGuns[i] = gGuns[i];
	}


	mGuiController = new JGuiController(150, this);
	mGuiController->SetActive(false);
	if (mGuiController)
	{	
		mGuiController->Add(new MenuItem(1, gFont, "resume", SCREEN_WIDTH_2, 100, TYPE_MAIN, JGETEXT_CENTER, true));
		mGuiController->Add(new MenuItem(2, gFont, "restart round", SCREEN_WIDTH_2, 125, TYPE_MAIN, JGETEXT_CENTER));
		mGuiController->Add(new MenuItem(3, gFont, "change teams", SCREEN_WIDTH_2, 150, TYPE_MAIN, JGETEXT_CENTER));
		mGuiController->Add(new MenuItem(4, gFont, "leave game", SCREEN_WIDTH_2, 175, TYPE_MAIN, JGETEXT_CENTER));
		//mGuiController->Add(new MenuItem(4, gFont, "quit", SCREEN_WIDTH-20, 240, TYPE_MAIN, JGETEXT_RIGHT));
	}

	//mMusic = mSoundSystem->LoadMusic("sfx/Raindrops.mp3");
	#ifdef WIN32
	mMusic = mSoundSystem->LoadMusic("sfx/Raindrops.mp3");
	#else
	//mMusic = mSoundSystem->LoadMusic("sfx/Raindrops.mp3");
	MP3_Init(1);
	MP3_Load("sfx/Raindrops.mp3");
	#endif

	Init();

	mAStar = new AStar();
}


void GameStatePlay::Destroy()
{

	SAFE_DELETE(gRadarQuad);
	SAFE_DELETE(gDryFireRifleSound);
	SAFE_DELETE(gDryFirePistolSound);
	SAFE_DELETE(gDeploySound);
	SAFE_DELETE(gPickUpSound);

	for (unsigned int i=0;i<mPeople.size();i++) {
		delete mPeople[i];
	}
	mPeople.clear();

	/*for (int i=0;i<mNumGuns;i++) {
		delete mGuns[i];
	}*/

	SAFE_DELETE(gParticleEngine);

	SAFE_DELETE(gSfxManager);

	SAFE_DELETE(mMap);

	SAFE_DELETE(mGrid);
	SAFE_DELETE(mAStar);
	/**
	if (mMusic)
		mEngine->FreeMusic(mMusic);**/

	SAFE_DELETE(mGuiController);

	SAFE_DELETE(mCamera);

	SAFE_DELETE(mMusic);

	for (int i=0; i<3; i++) {
		SAFE_DELETE(gParticleSystems[i]);
	}
	#ifdef WIN32
	//mEngine->StopMP3();
	//mEngine->FreeMP3();
	#else
	MP3_Stop();
    MP3_FreeTune();
	#endif
}


void GameStatePlay::Start()
{
	mRenderer->EnableVSync(true);

	mMovementStyle = ABSOLUTE1;
	char* movement = GetConfig("data/config.txt","movement");
	if (movement != NULL) {
		if (strcmp(movement,"relative") == 0) {
			mMovementStyle = RELATIVE1;
		}
		else if (strcmp(movement,"absolute") == 0) {
			mMovementStyle = ABSOLUTE1;
		}
		delete movement;
	}

	mFriendlyFire = ON;
	char* ff = GetConfig("data/config.txt","ff");
	if (ff != NULL) {
		if (strcmp(ff,"on") == 0) {
			mFriendlyFire = ON;
		}
		else if (strcmp(ff,"off") == 0) {
			mFriendlyFire = OFF;
		}
		delete ff;
	}

	mMenuStyle = ANALOG;
	char* menu = GetConfig("data/config.txt","menu");
	if (menu != NULL) {
		if (strcmp(menu,"analog") == 0) {
			mMenuStyle = ANALOG;
		}
		else if (strcmp(menu,"dirpad") == 0) {
			mMenuStyle = DIRPAD;
		}
		delete menu;
	}
	mTeamMenu->mIsOldStyle = !(mMenuStyle == ANALOG);

	char* name = GetConfig("data/config.txt","name");
	if (name == NULL) {
		mPlayer = new Person(gPlayersQuads[0][0], gPlayersDeadQuads[0][0], &mBullets, &mGunObjects, NONE, "player", mMovementStyle);
	}
	else {
		mPlayer = new Person(gPlayersQuads[0][0], gPlayersDeadQuads[0][0], &mBullets, &mGunObjects, NONE, name, mMovementStyle);
		delete name;
	}

	mPlayer->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);
	mPlayer->SetState(DEAD);
	mPlayer->mTeam = NONE;
	mPeople.push_back(mPlayer);

	mHud->mPlayer = mPlayer;

	mSpec = mPlayer;
	mSpecIndex = 0;
	mSpecState = NONE;
	mScoreState = 0;

	mGameType = TEAM;
	mRespawnTimer = 3000.0f;
	mFFAWinner = NULL;

	mIsChatEnabled = true;
	mChatTimer = 0.0f;

	NewGame();

	mTeamMenu->Enable();

	if (mGuiController)
		mGuiController->SetActive(false);

	//mMusic = mEngine->LoadMusic("Res/smarioa.mod");
	//mEngine->PlayMusic(mMusic);
	mMusicSwitch = OFF;
	char* music = GetConfig("data/config.txt","music");
	if (music != NULL) {
		if (strcmp(music,"on") == 0) {
			mMusicSwitch = ON;
			//mSoundSystem->PlayMusic(mMusic,true);
			#ifdef WIN32
			mSoundSystem->PlayMusic(mMusic,true);
			#else
			//mSoundSystem->PlayMusic(mMusic,true);
			//MP3_Load("sfx/Raindrops.mp3");
			MP3_Play(); 
			#endif
		}
		else if (strcmp(music,"off") == 0) {
			mMusicSwitch = OFF;
		}
		delete music;
	}
}


void GameStatePlay::End()
{
	Game::End();

	//mSoundSystem->StopMusic(mMusic);
	if (mMusicSwitch == ON) {
		#ifdef WIN32
		mSoundSystem->StopMusic(mMusic);
		#else
		//mSoundSystem->StopMusic(mMusic);
		MP3_Stop();
		//MP3_FreeTune();
		#endif
	}

}

void GameStatePlay::CheckInput(float dt)
{
	/**
	bool up = mEngine->GetButtonState(PSP_CTRL_UP);
	bool down = mEngine->GetButtonState(PSP_CTRL_DOWN);
	bool right = mEngine->GetButtonState(PSP_CTRL_RTRIGGER);
	bool left = mEngine->GetButtonState(PSP_CTRL_LTRIGGER);
	if (up && (right || left))
	{
		if (right) {
			mPlayer->Move(.1f, M_PI/4);
		}
		if (left) {
			mPlayer->Move(.1f, -M_PI/4);
		}
	}
	else if (down && (right || left))
	{
		if (right) {
			mPlayer->Move(-.1f, -M_PI/4);
		}
		if (left) {
			mPlayer->Move(-.1f, M_PI/4);
		}
	}
	else if (right)
	{
		mPlayer->Move(.1f, M_PI/2);
	}
	else if (left)
	{
		mPlayer->Move(-.1f, M_PI/2);
	}
	else if (up)
	{
		mPlayer->Move(.1f,0);
	}
	else if (down)
	{
		mPlayer->Move(-.1f,0);
	}
	else {
		mPlayer->SetSpeed(0.0f);
	}**/

	float aX = mEngine->GetAnalogX()-127.5f;
	float aY = mEngine->GetAnalogY()-127.5f;

	/**if (mEngine->GetButtonState(PSP_CTRL_UP))
	{
		mPlayer->Move(0.1f, 0);
	}
	else if (mEngine->GetButtonState(PSP_CTRL_DOWN)) 
	{
		mPlayer->Move(0.1f, M_PI);
	}
	else if (mEngine->GetButtonState(PSP_CTRL_RIGHT)) 
	{
		mPlayer->Move(0.1f, M_PI_2);
	}
	else if (mEngine->GetButtonState(PSP_CTRL_LEFT)) 
	{
		mPlayer->Move(0.1f, -M_PI_2);
	}**/
	
	if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
		float angle = atan2f(aX,-aY);
		float speed = (sqrtf(aX*aX + aY*aY)/127.5f)*0.1f;
		if (speed > 0.1f) {
			speed = 0.1f;
		}
		mPlayer->Move(speed, angle);
	}
	else {
		mPlayer->SetMoveState(NOTMOVING);
	}

	if (mEngine->GetButtonState(PSP_CTRL_LTRIGGER))
	{
		mPlayer->RotateFacing(-0.005f*dt);
	}
	if (mEngine->GetButtonState(PSP_CTRL_RTRIGGER))
	{
		mPlayer->RotateFacing(0.005f*dt);
	}

	/*if (mPlayer->mGunIndex == PRIMARY || mPlayer->mGunIndex == KNIFE) {
		if (mEngine->GetButtonState(PSP_CTRL_CROSS) && !cross)
		{
			mPlayer->Fire();
			mSwitchTimer = 0;
		}
	}
	else if (mPlayer->mGunIndex == SECONDARY || mPlayer->mGunIndex == GRENADE) {
		if (mEngine->GetButtonClick(PSP_CTRL_CROSS))
		{
			mPlayer->Fire();
			mSwitchTimer = 0;
		}

	}*/

	if (mEngine->GetButtonClick(PSP_CTRL_SQUARE))
	{
		mPlayer->Reload();
	}
	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE))
	{
		mPlayer->SwitchNext();
		mSwitchTimer = 1500;
	}
	if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE))
	{
		mPlayer->Drop(mPlayer->mGunIndex);
	}

	if (mEngine->GetButtonState(PSP_CTRL_CROSS) && !cross)
	{
		bool fire = true;
		if (mPlayer->mGunIndex == SECONDARY) {
			if (mPlayer->mHasFired) {
				fire = false;
			}
		}

		if (fire) {
			std::vector<Bullet*> bullets = mPlayer->Fire();
			mSwitchTimer = 0;
			if (bullets.size() > 0) {
				//mCamera->Recoil(mPlayer->mFacingAngle,mPlayer->GetCurrentGun()->mGun->mDelay/80.0f,mPlayer->GetCurrentGun()->mGun->mDelay);
			}
		}
	}
	else {
		if (mPlayer->mIsFiring) {
			mPlayer->StopFire();
		}
	}

	//hacked
	if (!mEngine->GetButtonState(PSP_CTRL_CROSS) && cross) {
		cross = false;
	}
}

void GameStatePlay::CheckCollisions()
{
	std::vector<Person*> mPeopleTemp = mPeople;
	for(unsigned int i=0; i<mPeople.size(); i++) {
		if (mPeople[i]->mState == DEAD) continue;
		for(unsigned int j=0; j<mPeopleTemp.size(); j++) {
			if (mPeopleTemp[j]->mState == DEAD) continue;
			if (mPeople[i] != mPeopleTemp[j]) {
				float x = mPeople[i]->mX;
				float y = mPeople[i]->mY;
				float x2 = mPeopleTemp[j]->mX;
				float y2 = mPeopleTemp[j]->mY;
				float dx = x-x2;
				float dy = y-y2;
				float dist = dx*dx+dy*dy;
				float r = 32;
				if (dist < 35*35) {
					if (mGameType == FFA || mFriendlyFire == ON || mPeople[i]->mTeam != mPeople[j]->mTeam) {
						if (mPeople[i]->mGunIndex == KNIFE && mPeople[i]->mState == ATTACKING) {
							mPeople[i]->mState = DRYFIRING;
							float angle = atan2f((y2-y),x2-x)+M_PI;
							float anglediff = fabs(fabs(angle-mPeople[i]->mFacingAngle)-M_PI);
							if (anglediff <= 0.5f) {
								gParticleEngine->GenerateParticles(BLOOD,x2,y2,5);
								mMap->AddDecal(x2,y2,DECAL_BLOOD);
								gSfxManager->PlaySample(gKnifeHitSound,x,y);
								mPeopleTemp[j]->TakeDamage(mPeople[i]->mGuns[KNIFE]->mGun->mDamage);
								if (mPeopleTemp[j]->mState == DEAD) {
									UpdateScores(mPeople[i],mPeopleTemp[j],mPeople[i]->mGuns[KNIFE]->mGun);
								}
								if (mSpec->mState == DEAD) {
									NewSpec(mPeople[i],i);
								}
							}
						}
						if (mPeopleTemp[j]->mState == DEAD) continue; //just in case the person dies in above if statement
						if (mPeopleTemp[j]->mGunIndex == KNIFE && mPeopleTemp[j]->mState == ATTACKING) {
							mPeopleTemp[j]->mState = DRYFIRING;
							float angle = atan2f((y-y2),x-x2)+M_PI;
							float anglediff = fabs(fabs(angle-mPeopleTemp[j]->mFacingAngle)-M_PI);
							if (anglediff <= 0.5f) {
								gParticleEngine->GenerateParticles(BLOOD,x,y,5);
								mMap->AddDecal(x,y,DECAL_BLOOD);
								gSfxManager->PlaySample(gKnifeHitSound,x2,y2);
								mPeople[i]->TakeDamage(mPeopleTemp[j]->mGuns[KNIFE]->mGun->mDamage);
								if (mPeople[i]->mState == DEAD) {
									UpdateScores(mPeopleTemp[j],mPeople[i],mPeople[j]->mGuns[KNIFE]->mGun);
								}
								if (mSpec->mState == DEAD) {
									NewSpec(mPeopleTemp[j],j);
								}
							}
						}
						if (mPeople[i]->mState == DEAD) continue; //just in case the person dies in above if statement
					}
				}
			}
		}
		for (unsigned int j=0;j<mGunObjects.size();j++) {
			//if (mGunObjects[j] == NULL) continue;
			if (!mGunObjects[j]->mOnGround) continue;
			float x = mPeople[i]->mX;
			float y = mPeople[i]->mY;
			float x2 = mGunObjects[j]->mX;
			float y2 = mGunObjects[j]->mY;
			float dx = x-x2;
			float dy = y-y2;
			float dist = dx*dx+dy*dy;
			float r = 16;
			if (dist < r*r) {
				if (mPeople[i]->PickUp(mGunObjects[j])) {
					mGunObjects.erase(mGunObjects.begin()+j);
					j--;
				}
			}
		}
	}
	for(unsigned int i=0; i<mBullets.size(); i++) {
		if (mBullets[i]->mType == TYPE_GRENADE) continue;
		if (mBullets[i]->pX == mBullets[i]->mX && mBullets[i]->pY == mBullets[i]->mY) continue;
		if (mBullets[i]->mParent == NULL) continue;
		
		Vector2D p1(mBullets[i]->mX,mBullets[i]->mY);
		Vector2D p2(mBullets[i]->pX,mBullets[i]->pY);
		Line l1(p1,p2);

		for(unsigned int j=0; j<mPeople.size(); j++) {
			//if (mPeople[j]->mHealth <= 0) continue;
			if (mPeople[j]->mState == DEAD) continue;
			if (mBullets[i]->mParent != mPeople[j]) { // prevent checking a bullet against its parent
				if (mFriendlyFire == OFF && mBullets[i]->mParent->mTeam == mPeople[j]->mTeam) continue;

				Vector2D pos(mPeople[j]->mX,mPeople[j]->mY);
				Circle circle(pos,16);

				Vector2D d;
				float l;
				if (LineCircleIntersect(l1,circle,d,l,false)) {
					gParticleEngine->GenerateParticles(BLOOD,mBullets[i]->mX,mBullets[i]->mY,5);
					mMap->AddDecal(mBullets[i]->mX,mBullets[i]->mY,DECAL_BLOOD);
					gSfxManager->PlaySample(gHitSounds[rand()%3],mBullets[i]->mX,mBullets[i]->mY);
					mPeople[j]->TakeDamage(mBullets[i]->mDamage);
					mBullets[i]->mState = 1;
					if (mPeople[j]->mState == DEAD) {
						UpdateScores(mBullets[i]->mParent,mPeople[j],mBullets[i]->mParentGun);
					}
					if (mSpec->mState == DEAD) {
						NewSpec(mBullets[i]->mParent,j);
						/*if (mSpec == mPlayer) {
							mSpecX = mBullets[i]->mParent->GetX();
							mSpecY = mBullets[i]->mParent->GetY();
							if (mBuyMenu->IsActive) {
								mBuyMenu->IsActive = false;
							}
						}
						//mPlayerDead = true;
						mSpec = mBullets[i]->mParent;
						mSpecIndex = j;
						if (mSpec->mState == DEAD) {
							bool alldead = true;
							for (int k=0; k<mPeople.size(); k++) {
								mSpecIndex = (j+1)%mPeople.size();
								mSpec = mPeople[mSpecIndex];
								if (mSpec->mState != DEAD) {
									alldead = false;
									break;
								}
							}
							if (alldead) {
								mSpecState = FREELOOK;
							}
						}*/
					}
				}
			}
		}
	}
	Game::CheckCollisions();

	/**for (unsigned int i=0; i<mPeople.size(); i++) {
		if (mPeople[i]->mState == DEAD) continue;
		if (mPeople[i]->mHealth <= 0) {
			//SortPeople();
			if (mPeople[i]->mTeam == CT) {
				mNumRemainingCTs--;
			}
			else if (mPeople[i]->mTeam == T) {
				mNumRemainingTs--;
			}
			/**
			if (mPeople[i] != mPlayer) {
				delete mPeople[i];
			}
			mPeople.erase(mPeople.begin()+i);
			
			i--; // makes up for the erase to prevent skipping people

			if (mNumRemainingCTs == 0 || mNumRemainingTs == 0) {
				ResetRound();
				break;
			}**/
		/**}
	}**/
}

void GameStatePlay::Update(float dt)
{

	if (mEngine->GetButtonClick(PSP_CTRL_START))
	{
		if (!gDanzeff->mIsActive) {
			mGuiController->SetActive(!mGuiController->IsActive());
		}
		//return;
	}

	if (mGuiController != NULL && mGuiController->IsActive())
	{
		mGuiController->Update(dt);
		return;
	}
	else if (mTeamMenu->mIsActive) {
		if (!mEngine->GetButtonState(PSP_CTRL_SELECT)) {
			mTeamMenu->Update(dt);
			if (mTeamMenu->mIsSelected) {
				//mTeamMenu->Disable();
				int team = mTeamMenu->GetTeam();
				int type = mTeamMenu->GetType();
				mSpecState = FREELOOK;

				//if (team != mPlayer->mTeam) {
					if (mPlayer->mTeam != NONE) {
						for (int i=mPeople.size()-1; i>=0; i--) {
							if (mPeople[i]->mTeam == NONE) {
								mPeople[i]->mTeam = mPlayer->mTeam;
								break;
							}
						}
					}

					if (team != NONE) {
						
						mPlayer->mTeam = team;
						if (team < 2 && type < 4) {
							mPlayer->SetQuads(gPlayersQuads[team][type],gPlayersDeadQuads[team][type]);
							//mPlayer->mQuads = gPlayersQuads[team][type];
							//mPlayer->mDeadQuad = gPlayersDeadQuads[team][type];
						}
						/*if (mPlayer->mTeam == CT) {
							mPlayer->mQuad = gPlayersQuads[0];
							mPlayer->mDeadQuad = gPlayersDeadQuads[0];
						}
						else if (mPlayer->mTeam == T) {
							mPlayer->mQuad = gPlayersQuads[1];
							mPlayer->mDeadQuad = gPlayersDeadQuads[1];
						}*/
						for (int i=mPeople.size()-1; i>=0; i--) {
							if (mPeople[i]->mTeam == mPlayer->mTeam && mPeople[i] != mPlayer) {
								mPeople[i]->mTeam = NONE;
								mPeople[i]->SetState(DEAD);
								break;
								/*delete mPeople[i];
								mPeople[i] = NULL;
								mPeople.erase(mPeople.begin()+i);
								break;*/
							}
						}
						/*delete mBuyMenu;
						mBuyMenu = new BuyMenu(mPlayer,mGuns);
						mBuyMenu->cross = &cross;
						mBuyMenu->mPickUpSound = gPickUpSound;*/

						ResetRound();
					}
					else {
						mPlayer->mTeam = team;
						mPlayer->SetState(DEAD);
						ResetRound();
					}
				//}
			}
		}
		if (!mTeamMenu->mIsActive) {
			mSpecState = FREELOOK;
		}
		StopInput();
	}
	else if (gDanzeff->mIsActive) {
			gDanzeff->Update(dt);
			if (gDanzeff->mString.length() > 31) {
				gDanzeff->mString = gDanzeff->mString.substr(0,31);
			}
			strcpy(mChatString,gDanzeff->mString.c_str());
			
			if (mEngine->GetButtonState(PSP_CTRL_START)) {
				if (strlen(mChatString) > 0) {
					if (mIsChatEnabled) {
						mHud->AddChatEvent(mPlayer->mName,mChatString,mPlayer->mTeam,mPlayer->mState == DEAD,mIsTeamOnlyChat);
						mChatTimer += 2000.0f;
					}
					else {
						mHud->AddMessageEvent("Don't spam the chat");
					}
				}
				gDanzeff->Disable();
			}
			else if (mEngine->GetButtonState(PSP_CTRL_SELECT)) {
				gDanzeff->Disable();
			}
			
			StopInput();
		}
	else if (mBuyMenu->mIsActive) {
		mBuyMenu->Update(dt);
		if (mBuyMenu->mIsSelected) {
			if (mBuyTimer > 0.0f || mGameType != TEAM) {
				int choice = mBuyMenu->GetChoice();
				Buy(choice);
			}
			else {
				mHud->SetMessage("Buying time has ended!");
			}
		}
		StopInput();
	}
	else {
		if (!mEngine->GetButtonState(PSP_CTRL_SELECT)) {
			if (mEngine->GetButtonClick(PSP_CTRL_UP)) {
				gDanzeff->Enable();
				strcpy(mChatString,"");
				mIsTeamOnlyChat = false;
			}
			else if (mEngine->GetButtonClick(PSP_CTRL_RIGHT)) {
				gDanzeff->Enable();
				strcpy(mChatString,"");
				mIsTeamOnlyChat = true;
			}
		}
		if (mEngine->GetButtonClick(PSP_CTRL_DOWN)) {
			if (mPlayer->mIsInBuyZone) {
				mBuyMenu->Enable();
			}
		}

		if (mPlayer->mState != DEAD) {
			CheckInput(dt*mTimeMultiplier);
		}
		else {
			CheckSpecInput(dt*mTimeMultiplier);
		}
	}
	/**
	if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE))	// do a screen shot when TRIANGLE is pressed
	{
		char s[80];
		sprintf(s, "screenshot.png");
		mEngine->ScreenShot(s);
	}**/
	if (mEngine->GetButtonClick(PSP_CTRL_NOTE))
	{
		//ResetRound();
		if (mPlayer->mMovementStyle == RELATIVE1) {
			mPlayer->mMovementStyle = ABSOLUTE1;
		}
		else if (mPlayer->mMovementStyle == ABSOLUTE1) {
			mPlayer->mMovementStyle = RELATIVE1;
		}
	}
	//float dt = mEngine->GetDelta();	// get number of milliseconds passed since last frame

	Game::Update(dt);
	Game::CheckCollisions();
	
	Game::UpdateCamera(dt);

	for (unsigned int i=0; i<mBullets.size(); i++) {
		if (mBullets[i]->mType == TYPE_GRENADE && mBullets[i]->mState == BULLET_DEAD) {
			Explode((Grenade*)mBullets[i]);
			Bullet* bullet = mBullets[i];
			mBullets[i] = mBullets.back();
			mBullets.back() = bullet;
			mBullets.pop_back();
			delete bullet;
			i--;
			/*delete mBullets[i];
			mBullets[i] = NULL;
			mBullets.erase(mBullets.begin()+i);
			i--;*/
		}
	}

	if (mRoundEndTimer >= mRoundEndTime*0.33f) {
		ResetRound();
	}
	/*if (mNumRemainingCTs == 0 || mNumRemainingTs == 0) {
		mRoundEndTimer += dt;
		if (mRoundEndTimer >= 3000) {
			ResetRound();
		}
	}

	mMap->Update(dt);
	gParticleEngine->Update(dt);

	mRoundTimer -= dt;
	if (mRoundState == FREEZETIME) {

		for(unsigned int i=0; i<mPeople.size(); i++)
		{
			mPeople[i]->mIsActive = false;
		}			
		if (mRoundTimer < 0) {
			mRoundTimer = 120000;
			mRoundState = STARTED;
			for(unsigned int i=0; i<mPeople.size(); i++)
			{
				mPeople[i]->mIsActive = true;
			}			
		}
	}
	else if (mRoundState != FREEZETIME) {
		if (mRoundTimer < 0) {
			mRoundTimer = 0;
		}
	}

	for(unsigned int i=0; i<mPeople.size(); i++)
	{
		//if (mPeople[i]->mState == DEAD) continue;
		mPeople[i]->Update(dt);
	}
	for(unsigned int i=0; i<mBullets.size(); i++)
	{
		mBullets[i]->Update(dt);
	}
	for(unsigned int i=0; i<mGunObjects.size(); i++)
	{
		mGunObjects[i]->Update(dt);
	}

	if (mPlayer->mState != DEAD) {
		mCamera->mTX = mPlayer->GetX() + ((mPlayer->GetX()-mPlayer->mOldX)*0.5f + cosf(mPlayer->mFacingAngle))*500/dt;
		mCamera->mTY = mPlayer->GetY() + ((mPlayer->GetY()-mPlayer->mOldY)*0.5f + sinf(mPlayer->mFacingAngle))*500/dt;
	}
	else {
		if (mSpecState == FREELOOK) {
			mCamera->mTX = mSpecX;
			mCamera->mTY = mSpecY;
		}
		else if (mSpecState == THIRDPERSON) {
			mCamera->mTX = mSpec->GetX() + (mSpec->GetX()-mSpec->mOldX)/dt*400;
			mCamera->mTY = mSpec->GetY() + (mSpec->GetY()-mSpec->mOldY)/dt*400;
		}
	}

	mCamera->Update(dt);

	mHud->Update(dt);

	if (mSwitchTimer != 0) {
		mSwitchTimer -= dt;
		if (mSwitchTimer < 0) {
			mSwitchTimer = 0;
		}
	}*/


}


void GameStatePlay::Render() 
{
	Game::Render();
}

void GameStatePlay::NewGame() {

	char* map;
	#ifdef WIN32
	map = GetConfig("data/config.txt","map");
	#else
	map = gMapName;
	#endif

	strcpy(mName,map);

	mMap->Load(map,mGuns);
	//delete map;
	mCamera->SetBounds(0,0,mMap->mCols*32,mMap->mRows*32);//mCamera->SetBounds(0+SCREEN_WIDTH_2,0+SCREEN_HEIGHT_2,mMap->mCols*32-SCREEN_WIDTH_2,mMap->mRows*32-SCREEN_HEIGHT_2);
	mNumCTs = mMap->mNumCTs;
	mNumTs = mMap->mNumTs;

	mGrid->Rebuild(mMap->mCols*32,mMap->mRows*32,128);
	for (unsigned int i=0;i<mMap->mCollisionLines.size();i++) {
		mGrid->HashCollisionLine(&(mMap->mCollisionLines[i]));
	}
	mAStar->Rebuild(&(mMap->mNodes),mGrid);

	mRoundFreezeTime = 3;
	mRoundTime = 120;
	mRoundEndTime = 3;
	mBuyTime = 60;

	mNumCTWins = 0;
	mNumTWins = 0;
	//std::vector<Person*> mCTs;
	//std::vector<Person*> mTs;

	for (int i=0;i<mNumCTs;i++) {
		char buffer[10];
		char numberbuffer[3];
		sprintf(numberbuffer,"%i",i); 
		strcpy(buffer,"CTbot");
		strcat(buffer,numberbuffer);

		int type = rand()%4;
		AI *ct = new AI(gPlayersQuads[CT][type], gPlayersDeadQuads[CT][type], &mBullets, &mGunObjects, mMap->mNodes, CT, buffer, ABSOLUTE1);
		//ct->mSpawn = mMap->mCTSpawns[i];
		//ct->SetPosition(mMap->mCTSpawns[i]->x,mMap->mCTSpawns[i]->y);
		//ct->SetTotalRotation(M_PI_2);
		ct->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);
		//ct->mCollisionPoints = &mMap->mCollisionPoints;
		ct->mGrid = mGrid;
		ct->mPeople = &mPeople;
		ct->mAStar = mAStar;
		ct->mGameGuns = &mGuns;
		mPeople.push_back(ct);
		//mCTs.push_back(ct);
	}

	for (int i=0;i<mNumTs;i++) {
		char buffer[10];
		char numberbuffer[3];
		sprintf(numberbuffer,"%i",i); 
		strcpy(buffer,"Tbot");
		strcat(buffer,numberbuffer);

		int type = rand()%4;
		AI *t = new AI(gPlayersQuads[T][type], gPlayersDeadQuads[T][type], &mBullets, &mGunObjects, mMap->mNodes, T, buffer, ABSOLUTE1);
		//t->mSpawn = mMap->mTSpawns[i];
		//t->SetPosition(mMap->mTSpawns[i]->x,mMap->mTSpawns[i]->y);
		//t->SetTotalRotation(M_PI_2);
		t->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);
		//t->mCollisionPoints = &mMap->mCollisionPoints;
		t->mGrid = mGrid;
		t->mPeople = &mPeople;
		t->mAStar = mAStar;
		t->mGameGuns = &mGuns;
		mPeople.push_back(t);
		//mTs.push_back(t);
	}	

	/*for (int i=0;i<20;i++) {
		char buffer[10];
		char numberbuffer[3];
		sprintf(numberbuffer,"%i",i); 
		strcpy(buffer,"spec");
		strcat(buffer,numberbuffer);

		Person* p = new Person(gPlayersQuads[0][0], gPlayersDeadQuads[0][0], &mBullets, &mGunObjects, NONE, buffer, mMovementStyle);
		p->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);
		p->SetState(DEAD);
		p->mTeam = NONE;
		mPeople.push_back(p);
	}*/


	/*for (unsigned int i=0;i<mCTs.size();i++) {
		mCTs[i]->mEnemies = mTs;
	}
	for (unsigned int i=0;i<mTs.size();i++) {
		mTs[i]->mEnemies = mCTs;
	}
	mCTs.clear();
	mTs.clear();*/

	mBuyMenu = new BuyMenu(mPlayer,mGuns);
	mBuyMenu->cross = &cross;
	mBuyMenu->mPickUpSound = gPickUpSound;
	mBuyMenu->mIsOldStyle = !(mMenuStyle == ANALOG);

	ResetRound();

}

void GameStatePlay::ResetRound() {
	mTimeMultiplier = 1.0f;
	mNumRounds++;

	mRoundState = FREEZETIME;
	mRoundTimer = mRoundFreezeTime;
	mRoundEndTimer = 0;
	mBuyTimer = mBuyTime;
	mFFAWinner = NULL;

	if (mPlayer->mTeam != NONE) {
		mSpec = mPlayer;
	}

	mGrid->ClearCells();
		
	for (unsigned int i=0;i<mBullets.size();i++) {
		delete mBullets[i];
	}
	mBullets.clear();
	for (unsigned int i=0;i<mGunObjects.size();i++) {
		delete mGunObjects[i];
	}
	mGunObjects.clear();
	
	for (int i=0; i<3; i++) {
		gParticleSystems[i]->Stop(true);
	}

	mMap->Reset();
	mMap->ClearDecals();

	mNumRemainingCTs = mNumCTs;
	mNumRemainingTs = mNumTs;

	int ctspawnindex = rand()%mMap->mNumCTs;
	int tspawnindex = rand()%mMap->mNumTs;

	for (unsigned int i=0;i<mPeople.size();i++) {
		if (mPeople[i]->mTeam == NONE) continue;

		bool isDead = (mPeople[i]->mState == DEAD)? true:false;

		mPeople[i]->Reset();

		if (mPeople[i]->mTeam == CT) {
			mPeople[i]->Teleport(mMap->mCTSpawns[ctspawnindex]->x,mMap->mCTSpawns[ctspawnindex]->y);
			ctspawnindex++;
			if (ctspawnindex >= mMap->mNumCTs) {
				ctspawnindex = 0;
			}
		}
		else {
			mPeople[i]->Teleport(mMap->mTSpawns[tspawnindex]->x,mMap->mTSpawns[tspawnindex]->y);
			tspawnindex++;
			if (tspawnindex >= mMap->mNumTs) {
				tspawnindex = 0;
			}
		}

		if (isDead) {
			Gun* gun;
			if (mPeople[i]->mTeam == CT) {
				gun = &mGuns[2];
			}
			else {
				gun = &mGuns[1];
			}
			GunObject* gunobject = new GunObject(gun,gun->mClip,gun->mClip*(gun->mNumClips-1));
			if (!mPeople[i]->PickUp(gunobject)) {
				delete gunobject;
			}
		}

		/*float dist = 0.0f;
		for (unsigned int j=0;j<mMap->mNodes.size();j++) {
			float x = mPeople[i]->mX - mMap->mNodes[j]->mX;
			float y = mPeople[i]->mY - mMap->mNodes[j]->mY;
			float disttemp = x*x + y*y;
			if (disttemp < dist || dist == 0.0f) {
				dist = disttemp;
				mPeople[i]->mNode = mMap->mNodes[j];
				mPeople[i]->mPrevNode = mPeople[i]->mNode;
			}
		}*/
	}	

	if (mPlayer->mTeam != NONE) {
		mCamera->mX = mPlayer->mX;
		mCamera->mY = mPlayer->mY;
	}

	if (mWinner == NONE) {
		if (mPlayer->mMoney < 3400) {
			mPlayer->mMoney += 1400;
			if (mPlayer->mMoney > 3400) {
				mPlayer->mMoney = 3400;
			}
		}
	}
	else {
		for (unsigned int i=0;i<mPeople.size();i++) {
			if (mPeople[i]->mTeam == mWinner) {
				mPeople[i]->mMoney += 3250;
				if (mPeople[i]->mMoney > 16000) {
					mPeople[i]->mMoney = 16000;
				}
			}
			else {
				if (mPeople[i]->mMoney < 3400) {
					mPeople[i]->mMoney += 1400;
					if (mPeople[i]->mMoney > 3400) {
						mPeople[i]->mMoney = 3400;
					}
				}
			}
		}
	}
	mWinner = NONE;
	std::sort(mPeople.begin(),mPeople.end(),SortByScore());

	Hash();
}	


void GameStatePlay::Explode(Grenade* grenade) {
	if (grenade->mGrenadeType == FLASH || grenade->mGrenadeType == HE) {
		for (unsigned int i=0; i<mPeople.size(); i++) {
			if (mPeople[i]->mState == DEAD) continue;

			if (grenade->mGrenadeType == FLASH) {
				/*bool visible = true;
				Line l1(grenade->mX,grenade->mY,mPeople[i]->mX,mPeople[i]->mY);

				mGrid->LineOfSight(grenade->mX,grenade->mY,mPeople[i]->mX,mPeople[i]->mY);
				for (unsigned int j=0; j<mMap->mCollisionPoints.size()-1; j++) {
					if (mMap->mCollisionPoints[j].x == -1 || mMap->mCollisionPoints[j+1].x == -1) continue;
					if (mMap->mCollisionPoints[j].bullets == false) continue;

					Vector2D p1(mMap->mCollisionPoints[j].x,mMap->mCollisionPoints[j].y);
					Vector2D p2(mMap->mCollisionPoints[j+1].x,mMap->mCollisionPoints[j+1].y);
					//if (p1 == p2) continue;
					if (fabs(p1.x-p2.x) < EPSILON && fabs(p1.y-p2.y) < EPSILON) continue;

					Line l2(p1,p2);
					if (mMap->mCollisionPoints[i].bullets == true) {
						Vector2D d;
						if (LineLineIntersect(l1,l2,d)) {
							visible = false;
							break;
						}
					}
				}

				if (!visible) continue;*/

				if (!mGrid->LineOfSight(grenade->mX,grenade->mY,mPeople[i]->mX,mPeople[i]->mY)) continue;

				int a = 500;
				if (mPeople[i]->mGunIndex == PRIMARY) {
					if (mPeople[i]->GetCurrentGun()->mGun->mId == 19 || mPeople[i]->GetCurrentGun()->mGun->mId == 20) {
						a = 800;
					}
					else if (mPeople[i]->GetCurrentGun()->mGun->mId == 21 || mPeople[i]->GetCurrentGun()->mGun->mId == 22 || mPeople[i]->GetCurrentGun()->mGun->mId == 23) {
						a = 1400;
					}
					else if (mPeople[i]->GetCurrentGun()->mGun->mId == 16) {
						a = 1100;
					}
				}
				float cameraX = mPeople[i]->mX + (cosf(mPeople[i]->mFacingAngle))*a/16.6f;//dt
				float cameraY = mPeople[i]->mY + (sinf(mPeople[i]->mFacingAngle))*a/16.6f;//dt

				if (fabs(grenade->mX-cameraX) <= SCREEN_WIDTH_2+20 && fabs(grenade->mY-cameraY) <= SCREEN_HEIGHT_2+20) {
					float dx = grenade->mX-mPeople[i]->mX;
					float dy = grenade->mY-mPeople[i]->mY;
					float distance = 0.0f;
					if (fabs(dx) >= EPSILON || fabs(dy) >= EPSILON) {
						 distance = sqrtf(dx*dx+dy*dy);
					}
					if (distance < 20.0f) {
						distance = 20.0f;
					}

					Vector2D facingdir(cosf(mPeople[i]->mFacingAngle),sinf(mPeople[i]->mFacingAngle));
					Vector2D grenadedir(grenade->mX-mPeople[i]->mX,grenade->mY-mPeople[i]->mY);
					if (fabs(grenadedir.x) < EPSILON && fabs(grenadedir.y) < EPSILON) {
						grenadedir = facingdir;
					}
					grenadedir.Normalize();

					float dot = facingdir.Dot(grenadedir);
					dot += 1.0f; // 0-2
					dot /= 4.0f; // 0-0.5
					dot += 0.5f; // 0.5-1

					float intensity = 20.0f/distance;
					mPeople[i]->ReceiveFlash(intensity*dot);
				}
			}
			else if (grenade->mGrenadeType == HE) {
				if (grenade->mParent == NULL) return;
				if (mGameType != FFA && mFriendlyFire == OFF && mPeople[i] != grenade->mParent && mPeople[i]->mTeam == grenade->mParent->mTeam) continue;

				if (!mGrid->LineOfSight(grenade->mX,grenade->mY,mPeople[i]->mX,mPeople[i]->mY)) continue;
				
				float dx = grenade->mX-mPeople[i]->mX;
				float dy = grenade->mY-mPeople[i]->mY;
				float distance = 0.0f;
				if (fabs(dx) >= EPSILON || fabs(dy) >= EPSILON) {
					 distance = sqrtf(dx*dx+dy*dy);
				}
				if (distance < 40.0f) {
					distance = 40.0f;
				}
				else if (distance > 200.0f) {
					continue;
				}

				int damage = (int)((40.0f/distance)*grenade->mParentGun->mDamage);
				mPeople[i]->TakeDamage(damage);

				gParticleEngine->GenerateParticles(BLOOD,mPeople[i]->mX,mPeople[i]->mY,5);
				mMap->AddDecal(mPeople[i]->mX,mPeople[i]->mY,DECAL_BLOOD);
				gSfxManager->PlaySample(gHitSounds[rand()%3],mPeople[i]->mX,mPeople[i]->mY);

				mHitTime = 1000;
				gSfxManager->PlaySample(gHitIndicatorSound);

				if (mPeople[i] == mPlayer) {
					float angle = atan2f(dy,dx)+M_PI;
					mHud->AddDamageIndicator(angle);
				}

				if (mPeople[i]->mState == DEAD) {
					UpdateScores(grenade->mParent,mPeople[i],grenade->mParentGun);
				}
				if (mSpec->mState == DEAD) {
					NewSpec(grenade->mParent,i);
				}
			}
		}
	}
	Game::Explode(grenade);
}


void GameStatePlay::StopInput() {
	mPlayer->SetMoveState(NOTMOVING);
}

void GameStatePlay::ButtonPressed(int controllerId, int controlId)
{
	switch (controlId)
	{
	case 1:
		mGuiController->SetActive(false);
		cross = true;
		break;
	case 2:
		ResetRound();
		mGuiController->SetActive(false);
		break;
	case 3:
		if (mBuyMenu->mIsActive) {
			mBuyMenu->Disable();
		}
		mTeamMenu->Enable();
		mGuiController->SetActive(false);
		break;
	case 4:
		//End();
		mParent->SetNextState(GAME_STATE_MENU);
		break;
	case 5:	
		mEngine->End();
		break;
	}
}

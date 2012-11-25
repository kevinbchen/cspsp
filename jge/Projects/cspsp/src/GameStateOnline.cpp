#include "GameStateOnline.h"

GameStateOnline::GameStateOnline(GameApp* parent): Game(parent) 
{
}

GameStateOnline::~GameStateOnline() {}

void GameStateOnline::Create()
{
	mIsOnline = true; //IMPORTANT : SETS GAME FLAG TO ONLINE

	gReconnect = false;
	//mCamera = new Camera(0,0,0,0,1000,1000);
	//mMap = new TileMap(&mGunObjects);
	socket = new Socket();
	mRoundBit = 0;
	mClock = 0.0f;
	mTime = 0.0f;
	mSTime = 0.0f;
	mPing = 200.0f;
	mSPing = 200.0f;

	mUdpManager = new UdpManager(socket,&mTime,&mPing,&mRoundBit);
	x = 5;
	y = 5;
	dx = 5;
	dy = 5;
	cross = false;
	cross2 = false;

	//mGuns = new Gun*[25];

	Init();

	mGuiController = new JGuiController(150, this);
	mGuiController->SetActive(false);
	if (mGuiController) {	
		mGuiController->Add(new MenuItem(1, gFont, "resume", SCREEN_WIDTH_2, 100, TYPE_MAIN, JGETEXT_CENTER, true));
		mGuiController->Add(new MenuItem(2, gFont, "change teams", SCREEN_WIDTH_2, 125, TYPE_MAIN, JGETEXT_CENTER));
	}

	mUpdateTimer = 0.0f;
	mUpdating = false;

	mAdminPlayersListBox = new ListBox(0,51,SCREEN_WIDTH,171,20,8);
}


void GameStateOnline::Destroy()
{

}


void GameStateOnline::Start()
{
	dltimer = 0.0f;
	dlsum = 0;
	dl = 0;

	gReconnect = false;

	mUdpManager->Reset();

	FormatText(mName,gServerName,200.0f,0.7f);
	//strcpy(mName,gServerName);

	mState = CONNECTING;
	//mIsLoaded = false;

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

	mFriendlyFire = ON;

	//mMap->Load("iceworld");
	mPlayer = new PersonOnline(gPlayersQuads[0][0], gPlayersDeadQuads[0][0], &mBullets, &mGunObjects, gTeam, gDisplayName, mMovementStyle);
	//((PersonOnline*)mPlayer)->mUdpManager = mUdpManager;
	((PersonOnline*)mPlayer)->mIsPlayerOnline = true;
	((PersonOnline*)mPlayer)->mId = -1;
	//mPlayer->mGuns[KNIFE] = new GunObject(mGuns[0],0,0);
	mPlayer->SetState(DEAD);
	mPlayer->mTeam = NONE;
	mPeople.push_back(mPlayer);

	mHud->mPlayer = mPlayer;

	UpdateIcon(((PersonOnline*)mPlayer)->mIconTexture,gIcon);

	mSpec = mPlayer;
	mSpecIndex = 0;
	mSpecState = NONE;

	mBuyMenu = NULL;

	mRespawnTimer = 5000.0f;
	mFFAWinner = NULL;

	mIsChatEnabled = true;
	mChatTimer = 0.0f;

	/*Gun *gun;
	if (mPlayer->mGuns[SECONDARY] == NULL) {
		if (mPlayer->mTeam == CT) {
			gun = gGuns[2];
		}
		else if (mPlayer->mTeam == T) {
			gun = gGuns[1];
		}
		mPlayer->mGuns[SECONDARY] = new GunObject(gun,gun->mClip,gun->mClip*(gun->mNumClips-1));
	}
	if (mPlayer->GetCurrentGun() == NULL) {
		mPlayer->mGunIndex = KNIFE;
		if (mPlayer->mGuns[SECONDARY] != NULL) {
			mPlayer->mGunIndex = SECONDARY;
		}
		if (mPlayer->mGuns[PRIMARY] != NULL) {
			mPlayer->mGunIndex = PRIMARY;
		}
	}*/
	SocketConnectUdp(socket,gServerIP,gServerPort);

	mTimer = 0.0f;
	mRenderer->EnableVSync(true);

	mNumPlayers = 0;
	mNumCTs = 0;
	mNumTs = 0;
	mNumRemainingCTs = 0;
	mNumRemainingTs = 0;

	mNumFlags[CT] = 0;
	mNumFlags[T] = 0;
	mFlagX[CT] = 0.0f;
	mFlagY[CT] = 0.0f;
	mFlagX[T] = 0.0f;
	mFlagY[T] = 0.0f;
	mIsFlagHome[CT] = true;
	mIsFlagHome[T] = true;
	mFlagHolder[CT] = NULL;
	mFlagHolder[T] = NULL;

	mRoundBit = 0;
	/*mBuyMenu = new BuyMenu(mPlayer,mGuns);
	mBuyMenu->cross = &cross;
	mBuyMenu->mPickUpSound = gPickUpSound;*/

	mSendMovementTimer = 0.0f;

	for (int i=0; i<256; i++) {
		mMoves[i] = Move();
		mMoves[i].time = -1.0f;
	}
	mMovesIndex = 0;

	mClock = 0.0f;
	mTime = 0.0f;
	mSTime = 0.0f;
	mPing = 200.0f;
	mSPing = 200.0f;

	mIsResetting = false;

	mDownloadId = -1;
	mDownloadAmount = 0;
	mMapFile = NULL;


	Packet packet = Packet();
	packet.WriteInt8(CONNECT);
	packet.WriteChar(mPlayer->mName);
	packet.WriteChar(gName);
	mUdpManager->SendReliable(packet,true);
	packet.Clear();

	/*Packet packet = Packet();
	packet.WriteInt8(NEWPLAYER);
	packet.WriteChar(mPlayer->mName);
	packet.WriteChar(gName);
	packet.WriteInt8(mPlayer->mMovementStyle);
	packet.WriteInt8(NETVERSION);
	packet.WriteData((char*)gIcon,300);
	//packet.WriteId(ackcounter++);
	//packet.SetId(ackcounter);
	//ackcounter++;
	mUdpManager->SendReliable(packet,true);
	packet.Clear();*/

	/*mUdpManager->Update(0);
	while (mUdpManager->mReliablePackets.size() > 0) {
		char buffer[256];

		int n = SocketRecv(socket,buffer, sizeof(buffer));

		if (n > 0) {
			Packet recvpacket = Packet(buffer,n);
			HandlePacket(recvpacket);
		}
	}*/
	mCamera->mX = 0.0f;
	mCamera->mY = 0.0f;

	mUpdating = false;
	/*mUpdateTimer = 0.0f;
	mUpdating = true;
	gKills = gKills2/7;
	gDeaths = gDeaths2/7;
	char decoding[2000];
	char data[2000];

	sprintf(data,DecodeText(decoding,"208201222161138215139207206208209215162137201138201201198216205215162137201138216201216215206211211161138215139205213161138215139212212214217161138205"),
									gKey,gKills,gDeaths,gSessionKey,gServerIP,gServerPort);
		// key=%s&kills=%d&deaths=%d&session=%s&ip=%s&port=%i
	strcpy(decoding,"");
	gHttpManager->SendRequest("/accounts/update.html",data,REQUEST_POST);*/

	mTimeMultiplier = 1.0f;
	mIsAdmin = false;
	mIsAdminMenuEnabled = false;
}


void GameStateOnline::End()
{
	if (mState == DOWNLOADING) {
		if (mMapFile != NULL) {
			fclose(mMapFile);
		}
		char file[128];
		sprintf(file,"maps/%s/map.txt",mMapName);
		remove(file); 

		sprintf(file,"maps/%s/tile.png",mMapName);
		remove(file); 

		sprintf(file,"maps/%s/overview.png",mMapName);
		remove(file); 

		char directory[128];
		sprintf(directory,"maps/%s",mMapName);

		#ifdef WIN32
		rmdir(directory);
		#else
		sceIoRmdir(directory);
		#endif
	}


	Packet sendpacket = Packet();
	sendpacket.WriteInt8(REMOVEPLAYER);
	sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
	mUdpManager->Send(sendpacket);
	mUdpManager->Update(0);

	Game::End();
	mPeopleMap.clear();
	mGunObjectsMap.clear();

	SocketClose(socket);

	mAdminPlayersListBox->Clear();

	mGuiController->SetActive(false);
	//WlanTerm();
}

void GameStateOnline::CheckInput(float dt)
{
	if (mEngine->GetButtonState(PSP_CTRL_LTRIGGER))
	{
		mPlayer->RotateFacing(-0.005f*dt*mTimeMultiplier);
	}
	if (mEngine->GetButtonState(PSP_CTRL_RTRIGGER))
	{
		mPlayer->RotateFacing(0.005f*dt*mTimeMultiplier);
	}

	if (mEngine->GetButtonClick(PSP_CTRL_SQUARE))
	{
		if (mPlayer->Reload()) {
			Packet sendpacket = Packet();
			sendpacket.WriteInt8(STARTRELOAD);
			mUdpManager->SendReliable(sendpacket,true);	
		}
	}

	if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE))
	{
		mPlayer->SwitchNext();
		/*Packet sendpacket = Packet();
		sendpacket.WriteInt8(SWITCHGUN);
		sendpacket.WriteInt8(mPlayer->mGunIndex);
		mUdpManager->SendReliable(sendpacket,true);*/
		mSwitchTimer = 1500;
	}
	if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE))
	{
		int index = mPlayer->mGunIndex;
		int gunid = ((GunObjectOnline*)mPlayer->GetCurrentGun())->mId;
		if (mPlayer->Drop(mPlayer->mGunIndex)) {
			Packet sendpacket = Packet();
			sendpacket.WriteInt8(DROPGUN);
			sendpacket.WriteInt8(index);
			sendpacket.WriteInt16(gunid);
			//sendpacket.WriteFloat(mClock);

			mUdpManager->SendReliable(sendpacket,true);
		}
	}
	if (mEngine->GetButtonState(PSP_CTRL_CROSS) && !cross)
	{
		//if (!cross2 && mPlayer->mState == NORMAL) {
			//cross2 = true;

			/*Packet sendpacket = Packet();
			sendpacket.WriteInt8(STARTFIRE);
			sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
			mUdpManager->SendReliable(sendpacket);*/
			//((PersonOnline*)mPlayer)->mIsFiring = true;

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
					Packet sendpacket = Packet();
					if (bullets.size() == 1) {
						sendpacket.WriteInt8(NEWBULLET);
						//sendpacket.WriteInt16(mBulletCounter);
						sendpacket.WriteInt8(bullets[0]->mParentGun->mId); //guntype
						sendpacket.WriteInt16((int)bullets[0]->mX);
						sendpacket.WriteInt16((int)bullets[0]->mY);
						//sendpacket.WriteInt16((int)bullets[0]->pX);
						//sendpacket.WriteInt16((int)bullets[0]->pY);

						int angle = (int)(bullets[0]->mAngle*(65535/(2*M_PI)))-32768;
						sendpacket.WriteInt16(angle);

						//sendpacket.WriteInt8(bullets[0]->mDamage);
						//sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
						sendpacket.WriteInt8(mPlayer->GetCurrentGun()->mClipAmmo);
						sendpacket.WriteFloat(mTime);

						mUdpManager->SendReliable(sendpacket);
					}
					else {
						sendpacket.WriteInt8(NEWSHOTGUNBULLET);
						//sendpacket.WriteInt8(bullets.size());
						//sendpacket.WriteInt16((int)bullets[0]->pX);
						//sendpacket.WriteInt16((int)bullets[0]->pY);
						
						sendpacket.WriteInt8(bullets[0]->mParentGun->mId); //guntype
						sendpacket.WriteInt16((int)bullets[0]->mX);
						sendpacket.WriteInt16((int)bullets[0]->mY);	
						//int speed = (int)(bullets[0]->mSpeed*10);
						//sendpacket.WriteInt8(speed);

						//sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
						sendpacket.WriteInt8(mPlayer->GetCurrentGun()->mClipAmmo);
						sendpacket.WriteFloat(mTime);

						for (int i=0; i<bullets.size(); i++) {
							int angle = (int)(bullets[i]->mAngle*(65535/(2*M_PI)))-32768;
							sendpacket.WriteInt16(angle);
							//sendpacket.WriteInt8(bullets[i]->mDamage);
						}
					}
					mUdpManager->SendReliable(sendpacket);
				}
				//((PersonOnline*)mPlayer)->mHasFired = true;
			}
		//}
		/*if (mPlayer->Fire()) {
			Packet sendpacket = Packet();
			sendpacket.WriteInt16(FIRE);
			sendpacket.WriteInt16(((PersonOnline*)mPlayer)->mId);
			mUdpManager->SendReliable(sendpacket);
		}*/
	}
	else {
		//if (cross2) {
			//cross2 = false;

			/*Packet sendpacket = Packet();
			sendpacket.WriteInt8(ENDFIRE);
			sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
			mUdpManager->SendReliable(sendpacket);	*/
			//((PersonOnline*)mPlayer)->mIsFiring = false;
			//((PersonOnline*)mPlayer)->mHasFired = false;
		//}
		std::vector<Bullet*> bullets = mPlayer->StopFire();
		//mSwitchTimer = 0;
		
		if (bullets.size() > 0) {
			Packet sendpacket = Packet();
			if (bullets.size() == 1) {
				sendpacket.WriteInt8(NEWGRENADE);
				//sendpacket.WriteInt16(mBulletCounter);
				sendpacket.WriteInt16((int)bullets[0]->mX);
				sendpacket.WriteInt16((int)bullets[0]->mY);
				//sendpacket.WriteInt16((int)bullets[0]->pX);
				//sendpacket.WriteInt16((int)bullets[0]->pY);

				int angle = (int)(bullets[0]->mAngle*(65535/(2*M_PI)))-32768;
				sendpacket.WriteInt16(angle);

				int speed = (int)(bullets[0]->mSpeed*(65535/0.2f))-32768;
				sendpacket.WriteInt16(speed);

				sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
				sendpacket.WriteFloat(mTime);

				mUdpManager->SendReliable(sendpacket,true);

				sendpacket.Clear();
				/*sendpacket.WriteInt8(SWITCHGUN);
				sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
				sendpacket.WriteInt8(mPlayer->mGunIndex);
				mUdpManager->SendReliable(sendpacket,true);*/
			}
		}
	}

	if (!mEngine->GetButtonState(PSP_CTRL_CROSS) && cross) {
		cross = false;
	}
	//if (mTimer > 20) {


	
	mSendMovementTimer += dt;

	if (mSendMovementTimer > 30.0f) {
		//if (mPlayer->mIsActive) {
			int aX = mEngine->GetAnalogX()-128;
			int aY = mEngine->GetAnalogY()-128;
			if (!mPlayer->mIsActive) {
				aX = 0;
				aY = 0;
			}
			if (aX >= 20 || aX <= -20 || aY >= 20 || aY <= -20) {
				float angle = atan2f(aX,-aY);
				float speed = (sqrtf(aX*aX + aY*aY)/127.5f)*0.1f;
				if (speed > 0.1f) {
					speed = 0.1f;
				}
				//mPlayer->Move(speed, angle);
				//sendpacket.WriteInt8((int)(speed*100));
				//sendpacket.WriteInt8((int)((mPlayer->mAngle*(255/(2*M_PI)))-128));
			}
			else {
				//sendpacket.WriteInt8(0);
				//sendpacket.WriteInt8(0);
				//mPlayer->SetMoveState(NOTMOVING);
			}

			//if (mSendMovementTimer > 30.0f) {
			
			Packet sendpacket = Packet();
			sendpacket.WriteInt8(PLAYERMOVE);//sendpacket.WriteInt8(MOVE);
			sendpacket.WriteInt8(aX);
			sendpacket.WriteInt8(aY);
			int facingangle = (int)(mPlayer->mFacingAngle*(255/(2*M_PI)))-128;
			sendpacket.WriteInt8(facingangle);
			int state = mPlayer->mGunIndex;
			if (((PersonOnline*)mPlayer)->mIsFiring) {
				state |= 128;
			}
			sendpacket.WriteInt8(state);
			sendpacket.WriteFloat(mTime);		
			mUdpManager->Send(sendpacket);
				
			//}

			Input input = {aX,aY,(float)((facingangle+128)/(255.0f/(2*M_PI)))};
			((PersonOnline*)mPlayer)->ReceiveInput(input,mTime);
			State movementState = {(int)((PersonOnline*)mPlayer)->mSX,(int)((PersonOnline*)mPlayer)->mSY,mPlayer->mSpeed,mPlayer->mAngle};
			Move move = {input,movementState,mTime};
			mMoves[mMovesIndex] = move;
			AdvanceMoves(mMovesIndex);

			mSendMovementTimer = 0.0f;
		//}
	}
}

void GameStateOnline::CheckCollisions()
{
	std::vector<Person*> mPeopleTemp = mPeople;
	for(unsigned int i=0; i<mPeople.size(); i++) {
		if (mPeople[i]->mState == DEAD || mPeople[i]->mTeam == NONE) continue;
		for(unsigned int j=0; j<mPeopleTemp.size(); j++) {
			if (mPeopleTemp[j]->mState == DEAD || mPeople[j]->mTeam == NONE) continue;
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
								//mPeopleTemp[j]->TakeDamage(mPeople[i]->mGuns[KNIFE]->mGun->mDamage);
								if (mPeopleTemp[j]->mState == DEAD) {
									//UpdateScores(mPeople[i],mPeopleTemp[j],mPeople[i]->mGuns[KNIFE]->mGun);
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
								//mPeople[i]->TakeDamage(mPeopleTemp[j]->mGuns[KNIFE]->mGun->mDamage);
								if (mPeople[i]->mState == DEAD) {
									//UpdateScores(mPeopleTemp[j],mPeople[i],mPeople[i]->mGuns[KNIFE]->mGun);
								}
							}
						}
						if (mPeople[i]->mState == DEAD) continue; //just in case the person dies in above if statement
					}
				}
			}
		}
	}
	mPeopleTemp.clear();

	for(unsigned int k=0; k<mBullets.size(); k++) {
		if (mBullets[k]->mType == TYPE_GRENADE) continue;
		if (mBullets[k]->pX == mBullets[k]->mX && mBullets[k]->pY == mBullets[k]->mY) continue;
		if (mBullets[k]->mParent == NULL) continue;
		Vector2D p1(mBullets[k]->mX,mBullets[k]->mY);
		Vector2D p2(mBullets[k]->pX,mBullets[k]->pY);
		Vector2D O = p1;
		Vector2D D = p2-p1;
		for(unsigned int j=0; j<mPeople.size(); j++) {
			//if (mPeople[j]->mHealth <= 0) continue;
			if (mPeople[j]->mState == DEAD || mPeople[j]->mTeam == NONE) continue;
			if (mBullets[k]->mParent != mPeople[j]) { // prevent checking a bullet against its parent
				if (mFriendlyFire == OFF && mBullets[k]->mParent->mTeam == mPeople[j]->mTeam) continue;

				Vector2D C(mPeople[j]->mX,mPeople[j]->mY);
				float t = D.Dot(C-O)/D.Dot(D);
				if (t < 0) {
					t = 0;
				}
				else if (t > 1) {
					t = 1;
				}
		
				Vector2D closest = O+t*D;
				Vector2D d = C - closest;
				float ll = d.Dot(d);
				int r = 16;
				if (ll < (r * r)) {
					gParticleEngine->GenerateParticles(BLOOD,mBullets[k]->mX,mBullets[k]->mY,5);
					mMap->AddDecal(mBullets[k]->mX,mBullets[k]->mY,DECAL_BLOOD);
					gSfxManager->PlaySample(gHitSounds[rand()%3],mBullets[k]->mX,mBullets[k]->mY);
					//mPeople[j]->TakeDamage(mBullets[k]->mDamage);
					//mPeople[j]->SetMoveState(NOTMOVING);
					//mPeople[j]->mSpeed *= 0.1f;
					mBullets[k]->mState = 1;
					if (mPeople[j]->mState == DEAD) {
						//UpdateScores(mBullets[k]->mParent,mPeople[j],mBullets[k]->mParentGun);
					}
					/*if (mSpec->mState == DEAD) {
						if (mSpec == mPlayer) {
							mSpecX = mBullets[k]->mParent->GetX();
							mSpecY = mBullets[k]->mParent->GetY();
							if (mBuyMenu->IsActive) {
								mBuyMenu->IsActive = false;
							}
						}
						//mPlayerDead = true;
						mSpec = mBullets[k]->mParent;
						mSpecIndex = j;
						if (mSpec->mState == DEAD) {
							mSpecIndex = (j+1)%mPeople.size();
							mSpec = mPeople[mSpecIndex];
						}
					}*/
				}
			}
		}
	}
	Game::CheckCollisions();
}

void GameStateOnline::Update(float dt)
{
	if (mIsMapChanging) {
		mMapChangeTimer -= dt;
		if (mMapChangeTimer < 0.0f) {
			gReconnect = true;
			mMapChangeTimer = 5000.0f;
			mIsMapChanging = false;
			mParent->SetNextState(GAME_STATE_LOBBY);
		}
		return;
	}

	mPing = mPing*0.95f + mSPing*0.05f;

	mClock += dt*mTimeMultiplier;
	mTime += dt*mTimeMultiplier;
	mSTime += dt*mTimeMultiplier;
	float oldTime = mTime;
	mTime = 0.95f*mTime + 0.05f*mSTime;
	mRoundTimer += -(mTime-oldTime)*0.001f;
	/*if (fabs(mSTime-mTime) > 300.0f) {
		mTime = mSTime;
	}
	else {
		mTime = 0.95f*mTime + 0.05f*mSTime;
	}*/


	if (mState == ERROR) {
		if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
			mParent->SetNextState(GAME_STATE_LOBBY);
		}
		return;
	}
	if (mState == PLAYING) {

		if (mEngine->GetButtonClick(PSP_CTRL_START)) {
			if (!gDanzeff->mIsActive && !mIsAdminMenuEnabled) {
				mGuiController->SetActive(!mGuiController->IsActive());
			}
		}

		if (mGuiController->IsActive())
		{
			mGuiController->Update(dt);
			StopInput(dt);
		}
		else if (mIsMapChanging) {
			return; // do nothing
		}
		else if (mIsAdminMenuEnabled) {
			if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
				mIsAdminMenuEnabled = false;
			}
			else if (mEngine->GetButtonClick(PSP_CTRL_SQUARE)) { // kick
				AdminPlayerItem *item = (AdminPlayerItem*)mAdminPlayersListBox->GetItem();
				if (item) {
					char chatstring[32];
					sprintf(chatstring,"/kick %s",item->player->mName);
					Packet sendpacket = Packet();
					sendpacket.WriteInt8(CHAT);
					sendpacket.WriteChar(chatstring);
					sendpacket.WriteInt8(false);
					mUdpManager->SendReliable(sendpacket,true);
				}
				mIsAdminMenuEnabled = false;
			}	
			else if (mEngine->GetButtonClick(PSP_CTRL_TRIANGLE)) { // ban
				AdminPlayerItem *item = (AdminPlayerItem*)mAdminPlayersListBox->GetItem();
				if (item) {
					char chatstring[32];
					sprintf(chatstring,"/ban %s",item->player->mName);
					Packet sendpacket = Packet();
					sendpacket.WriteInt8(CHAT);
					sendpacket.WriteChar(chatstring);
					sendpacket.WriteInt8(false);
					mUdpManager->SendReliable(sendpacket,true);
				}
				mIsAdminMenuEnabled = false;
			}

			mAdminPlayersListBox->Update(dt);
		}
		else if (mTeamMenu->mIsActive) {
			if (!mEngine->GetButtonState(PSP_CTRL_SELECT)) {
				mTeamMenu->Update(dt);
				if (mTeamMenu->mIsSelected) {
					//mTeamMenu->Disable();
					int team = mTeamMenu->GetTeam();
					int type = mTeamMenu->GetType();
					mSpecState = FREELOOK;
					//if (team != NONE) {
						//mPlayer->mTeam = team;
						
						Packet sendpacket = Packet();
						sendpacket.WriteInt8(SWITCHTEAM);
						sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
						sendpacket.WriteInt8(team);
						sendpacket.WriteInt8(type);
						mUdpManager->SendReliable(sendpacket,true);
					//}
				}
				if (!mTeamMenu->mIsActive) {
					mSpecState = FREELOOK;
				}
			}
			StopInput(dt);
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
						Packet sendpacket = Packet();
						sendpacket.WriteInt8(CHAT);
						sendpacket.WriteChar(mChatString);
						sendpacket.WriteInt8(mIsTeamOnlyChat);
						
						mUdpManager->SendReliable(sendpacket,true);
						//mHud->AddChatEvent(mPlayer->mName,mChatString,mPlayer->mTeam,false);
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
			
			StopInput(dt);
		}
		else if (mBuyMenu->mIsActive) {
			mBuyMenu->Update(dt);
			if (mBuyMenu->mIsSelected) {
				if (mBuyTimer > 0.0f || mGameType != TEAM) {
					int choice = mBuyMenu->GetChoice();
					
					Packet sendpacket = Packet();
					sendpacket.WriteInt8(BUY);
					//sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
					sendpacket.WriteInt8(choice);
					
					mUdpManager->SendReliable(sendpacket,true);
				}
				else {
					mHud->SetMessage("Buying time has ended!");
				}
			}
			StopInput(dt);
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
				/*if (mBuyMenu->IsActive) {
					mBuyMenu->Update(dt);
				}*/
				if (!mIsResetting) {
					CheckInput(dt);
				}
			}
			else {
				CheckSpecInput(dt);
			}
		}


	}

	/*//IMPORTANT
	((PersonOnline*)mPlayer)->mOldSX = ((PersonOnline*)mPlayer)->mSX;
	((PersonOnline*)mPlayer)->mOldSY = ((PersonOnline*)mPlayer)->mSY;*/

	dltimer += dt;
	if (dltimer > 1000) {
		dltimer = 0;
		dl = dlsum;
		dlsum = 0;
	}

	char buffer[4096];

	int n;
	while (n = SocketRecv(socket,buffer, sizeof(buffer))) {
		if (n > 0) {
			dlsum += n;
		}
		if (n > 0) {
			Packet recvpacket = Packet(buffer,n);
			if (recvpacket.ReadInt8() == NETVERSION) {
				HandlePacket(recvpacket);
			}

			/*int type = recvpacket.ReadInt16();
			((PersonOnline*)mPlayer)->mSX = recvpacket.ReadInt16();
			((PersonOnline*)mPlayer)->mSY = recvpacket.ReadInt16();
			((PersonOnline*)mPlayer)->mSFacingAngle = recvpacket.ReadFloat();
			((PersonOnline*)mPlayer)->mSSpeed = recvpacket.ReadFloat();
			((PersonOnline*)mPlayer)->mSAngle = recvpacket.ReadFloat();
			/*float speed = recvpacket.ReadFloat();
			float angle = recvpacket.ReadFloat();
			//mPlayer->SetSpeed(recvpacket.ReadFloat());
			if (speed >= 0.001f) {
				mPlayer->SetSpeed(speed);
				mPlayer->SetAngle(angle);
			} 
			else {
				mPlayer->SetMoveState(NOTMOVING);
			}*/
			//dx = x;
		}
		else {
			break;
		}
	}
	while (true) {
		int newid = mUdpManager->mOrderId+1;
		if (newid > 32767) {
			newid = -32768;
		}
		bool more = false;
		bool handled = false; //for duplicate packets
		for (int i=0; i<mUdpManager->mBufferedPackets.size(); i++) {
			if (mUdpManager->mBufferedPackets[i].GetId() == newid) {
				if (!handled) {
					HandlePacket(mUdpManager->mBufferedPackets[i],false);
					handled = true;
				}
				//mUdpManager->mBufferedPackets.erase(mUdpManager->mBufferedPackets.begin()+i);
				//i--;
				more = true;
				//break;
			}
		}
		if (!more) break;
	}

	mUdpManager->Update(dt);

	if (mState != PLAYING) {
		if (mEngine->GetButtonClick(PSP_CTRL_CIRCLE)) {
			mParent->SetNextState(GAME_STATE_LOBBY);
			return;
		}

		if (mState == CONNECTING) {
			mTimer += dt;
			if (mTimer > 15000) {
				mState = ERROR;
				mErrorString = "Error: Could not connect to server";
			}
		}
		else if (mState == DOWNLOADING) {
			
		}
		else if (mState == MAP_ERROR) {
			if (mEngine->GetButtonClick(PSP_CTRL_CROSS)) {
				mState = DOWNLOADING;

				char directory[128];
				sprintf(directory,"maps/%s",mMapName);

				#ifdef WIN32
				_mkdir(directory);
				#else
				mkdir(directory, 0777);
				#endif


				FILE* file;

				char mapfile[128];
				sprintf(mapfile,"maps/%s/map.txt",mMapName);
				file = fopen(mapfile,"w");
				fclose(file);

				sprintf(mapfile,"maps/%s/tile.png",mMapName);
				file = fopen(mapfile,"w");
				fclose(file);

				if (mMapOverviewSize > 0) {
					sprintf(mapfile,"maps/%s/overview.png",mMapName);
					file = fopen(mapfile,"w");
					fclose(file);
				}

				Packet sendpacket = Packet();
				sendpacket.WriteInt8(MAPFILE);
				sendpacket.WriteChar(mMapName);
				sendpacket.WriteInt8(0);
				sendpacket.WriteInt32(0);
				mUdpManager->SendReliable(sendpacket,true);
				sendpacket.Clear();	
			}
		}

		return;
	}

	mTimer += dt;
	if (mTimer > 10000) {
		mState = ERROR;
		mErrorString = "Error: Connection with server timed out";
	}

	//SocketSend(socket,sendpacket.Data(), sendpacket.Length());

	//}


	Game::Update(dt);

	Game::CheckCollisions();

	Game::UpdateCamera(dt);

	for (unsigned int i=0; i<mBullets.size(); i++) {
		if (mBullets[i]->mType == TYPE_GRENADE && mBullets[i]->mState == BULLET_DEAD) {
			//Explode((Grenade*)mBullets[i]);
			Bullet* bullet = mBullets[i];
			mBullets[i] = mBullets.back();
			mBullets.back() = bullet;
			mBullets.pop_back();
			delete bullet;
			/*delete mBullets[i];
			mBullets[i] = NULL;
			mBullets.erase(mBullets.begin()+i);
			i--;*/
		}
	}

	/*for (unsigned int i=0; i<mPeople.size(); i++) {
		mPeople[i]->Update(dt);
	}
	for(unsigned int i=0; i<mGunObjects.size(); i++) {
		mGunObjects[i]->Update(dt);
	}
	for(unsigned int i=0; i<mBullets.size(); i++) {
		mBullets[i]->Update(dt);
	}
	if (mPlayer->mState != DEAD) {
		mCamera->mTX = mPlayer->GetX() + ((mPlayer->GetX()-mPlayer->mOldX)*0.5f + cosf(mPlayer->mFacingAngle))*500/dt;
		mCamera->mTY = mPlayer->GetY() + ((mPlayer->GetY()-mPlayer->mOldY)*0.5f + sinf(mPlayer->mFacingAngle))*500/dt;
	}
	gSfxManager->mX = mPlayer->GetX();
	gSfxManager->mY = mPlayer->GetY();

	mCamera->Update(dt);*/

	if (!mUpdating) {
		mUpdateTimer += dt;

		if (mUpdateTimer > UPDATETIME) {
			mUpdateTimer = 0.0f;
			mUpdating = true;

			gKills = gKills2/7;
			gDeaths = gDeaths2/7;
			char decoding[2000];
			char data[2000];
		
			sprintf(data,DecodeText(decoding,"208201222161138215139207206208209215162137201138201201198216205215162137201138216201216215206211211161138215139205213161138215139212212214217161138205"),
											gKey,gKills,gDeaths,gSessionKey,gServerIP,gServerPort);
				// key=%s&kills=%d&deaths=%d&session=%s&ip=%s&port=%i
			strcpy(decoding,"");
			gHttpManager->SendRequest("/accounts/update.html",data,REQUEST_POST);
		}
	}
	else if (mUpdating) {
		gHttpManager->Update(dt);

		char buffer[8192];
		int size = gHttpManager->GetResponse(buffer);
		if (size > 0) {
			if (strstr(buffer,"UPDATE")) {
				mUpdateTimer = 0.0f;
				mUpdating = false; 

				char* s;
				s = strstr(buffer,"UPDATE");
				s += strlen("UPDATE") + 2;
				if (*s == '1') { //suspended
					mParent->SetNextState(GAME_STATE_LOBBY);
					gLogout = true;
					return;
				}
			}
		}
	}

}


void GameStateOnline::Render() 
{
	if (mState != PLAYING) {
		mRenderer->ClearScreen(ARGB(255,0,0,0));
		//mRenderer->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,ARGB(255,0,0,0));
		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.75f);
		gFont->DrawString("[O] Cancel and Return to Lobby",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);

		gFont->SetScale(1.0f);
		if (mState == CONNECTING) {
			gFont->DrawString("Connecting...",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
		}
		else if (mState == DOWNLOADING) {
			char buffer[128];

			mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200,4,ARGB(255,100,100,100));
			if (mDownloadId == 0) {
				sprintf(buffer,"Downloading %s/map.txt...",mMapName);
				mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200*((float)mDownloadAmount/(float)mMapTextSize),4,ARGB(255,255,255,255));
			}
			else if (mDownloadId == 1) {
				sprintf(buffer,"Downloading %s/tile.png...",mMapName);
				mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200*((float)mDownloadAmount/(float)mMapImageSize),4,ARGB(255,255,255,255));
			}
			else if (mDownloadId == 2 && mMapOverviewSize > 0) {
				sprintf(buffer,"Downloading %s/overview.png...",mMapName);
				mRenderer->FillRect(SCREEN_WIDTH_2-100,SCREEN_HEIGHT-30-30,200*((float)mDownloadAmount/(float)mMapOverviewSize),4,ARGB(255,255,255,255));
			}
			else {
				sprintf(buffer,"Downloading %s...",mMapName);
			}
			gFont->DrawString(buffer,SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
		}
		else if (mState == ERROR) {
			gFont->DrawString(mErrorString.c_str(),SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);	
		}
		else if (mState == MAP_ERROR) {
			gFont->DrawString("Error: The server's map differs from yours",SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
			gFont->SetScale(0.75f);
			gFont->DrawString("[X] Overwrite Map",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-40,JGETEXT_CENTER);
		}
		/*gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(0.65f);
		gFont->printf(85,30,"id:%d, up:%d, dl:%d",mUdpManager->mOrderId,mUdpManager->upload,dl);
		gFont->printf(85,45,"reliable:%d, ordered:%d, buffered:%d",mUdpManager->mReliablePackets.size(),mUdpManager->mOrderedPackets.size(),mUdpManager->mBufferedPackets.size());*/

		return;
	}

	Game::Render();

	/*if (gDanzeff->mIsActive) {
		gFont->SetScale(0.7f);
		gFont->SetColor(ARGB(255,255,200,0));
		char buffer[132];
		strcpy(buffer,"say: ");
		strcat(buffer,mChatString);
		if (mPlayer->mState != DEAD) {
			gFont->DrawShadowedString(buffer,10,75);
			gFont->DrawShadowedString("|",10+gFont->GetStringWidth(buffer),75);
		}
		else {
			gFont->DrawShadowedString(buffer,10,40);
			gFont->DrawShadowedString("|",10+gFont->GetStringWidth(buffer),40);
		}
		gFont->SetColor(ARGB(255,0,0,0));
		gFont->DrawString("[START] Enter  [SELECT] Cancel",SCREEN_WIDTH-100,200,JGETEXT_CENTER);
		gDanzeff->Render(SCREEN_WIDTH-175,50);
	}*/

	if (mIsMapChanging) {
		char buffer[64];
		gFont->SetScale(0.75f);
		gFont->SetColor(ARGB(255,255,200,0));
		sprintf(buffer,"Changing to new map in %i...",(int)(mMapChangeTimer/1000.0f));
		gFont->DrawShadowedString(buffer, SCREEN_WIDTH_2, SCREEN_HEIGHT-40, JGETEXT_CENTER);
	}
	else if (mIsAdminMenuEnabled) {
		mRenderer->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,ARGB(200,0,0,0));

		gFont->SetColor(ARGB(255,255,255,255));
		gFont->SetScale(1.0f);
		gFont->DrawShadowedString("Admin Menu",SCREEN_WIDTH_2,10,JGETEXT_CENTER);
		gFont->SetScale(0.75f);

		gFont->DrawShadowedString("players",20,35);
		mRenderer->DrawLine(0,50,SCREEN_WIDTH,50,ARGB(255,255,255,255));

		mAdminPlayersListBox->Render();

		mRenderer->DrawLine(0,232,SCREEN_WIDTH,232,ARGB(255,255,255,255));
		gFont->DrawShadowedString("[[]] Kick     [^] Ban     [O] Return",SCREEN_WIDTH_2,SCREEN_HEIGHT_F-20,JGETEXT_CENTER);
	}

	/*gFont->SetColor(ARGB(255,0,0,0));
	gFont->SetScale(0.65f);
	gFont->printf(85,30,"id:%d, up:%d, dl:%d",mUdpManager->mOrderId,mUdpManager->upload,dl);
	gFont->printf(85,45,"reliable:%d, ordered:%d, buffered:%d",mUdpManager->mReliablePackets.size(),mUdpManager->mOrderedPackets.size(),mUdpManager->mBufferedPackets.size());*/
	

	//gFont->printf(10,100,"%f",mPlayer->mFacingAngle);
	//gFont->printf(10,150,"%f",mPlayer->mAngle);
	//gFont->printf(10,200,"%f",mEngine->GetFPS());
	//gFont->printf(10,10,"%d",counter);
	//gFont->printf(10,30,"%f",mPeople[1]->dx);
	//gFont->printf(10,30,"%f",mClock);
	//gFont->printf(10,50,"%i",mUdpManager->mOrderId);
}
	

void GameStateOnline::HandlePacket(Packet &packet, bool sendack) {
	if (mState == PLAYING) {
		mTimer = 0.0f;
	}

	int type;
	bool round;
	Packet sendpacket;

	while (packet.Index() < packet.Length()) {
		int startindex = packet.Index();
		char temp = packet.ReadInt8();
		type = temp & 127;
		round = false;
		if ((int)(temp & 128) == (int)mRoundBit) {
			round = true;
		}
		//printf("%s\n",(round)?"1":"0");
		sendpacket = Packet();

		switch (type) {
			case TIME: {
				float time = packet.ReadFloat();
				float stime = packet.ReadFloat();

				mSPing = mTime-time;
				if (mSPing < 10.0f) mSPing = 10.0f;
				else if (mSPing > 1000.0f) mSPing = 1000.0f;
				((PersonOnline*)mPlayer)->mSPing = mSPing;

				mUdpManager->mSTime = stime;
				mSTime = stime + mSPing*0.5f;

				break;
			}
			case CONNECT: {
				bool admin = packet.ReadInt8();
				packet.ReadChar(mMapName,32);
				int maptextsize = packet.ReadInt32();
				int mapimagesize = packet.ReadInt32();
				int mapoverviewsize = packet.ReadInt32();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				mUdpManager->mOrderId = ackid;

				mIsAdmin = admin;
				mGuiController->Remove(3);
				mGuiController->Remove(4);
				if (mIsAdmin) {
					mGuiController->Add(new MenuItem(3, gFont, "admin menu", SCREEN_WIDTH_2, 150, TYPE_MAIN, JGETEXT_CENTER));
				}
				mGuiController->Add(new MenuItem(4, gFont, "leave game", SCREEN_WIDTH_2, 175, TYPE_MAIN, JGETEXT_CENTER));
			
				mMapTextSize = maptextsize;
				mMapImageSize = mapimagesize;
				mMapOverviewSize = mapoverviewsize;

				bool valid = true;
				bool same = true;

				FILE* file;
				char mapfile[128];
				sprintf(mapfile,"maps/%s/map.txt",mMapName);
				file = fopen(mapfile,"rb");
				if (file != NULL) {
					fseek(file,0,SEEK_END);
					int textsize = ftell(file);
					if (mMapTextSize != textsize) {
						same = false;
					}
					fclose(file);
				}
				else {
					valid = false;
				}
				sprintf(mapfile,"maps/%s/tile.png",mMapName);
				file = fopen(mapfile,"rb");
				if (file != NULL) {
					fseek(file,0,SEEK_END);
					int imagesize = ftell(file);
					if (mMapImageSize != imagesize) {
						same = false;
					}
					fclose(file);
				}
				else {
					valid = false;
				}
				if (mMapOverviewSize > 0) {
					sprintf(mapfile,"maps/%s/overview.png",mMapName);
					file = fopen(mapfile,"rb");
					int overviewsize = 0;
					if (file != NULL) {
						fseek(file,0,SEEK_END);
						overviewsize = ftell(file);
						fclose(file);
					}
					if (mMapOverviewSize != overviewsize) {
						same = false;
					}
				}

				if (valid && !same) { // map files do not match
					mState = MAP_ERROR;
					break;
				}
				if (!valid) { // mpa files do not exist, download
					mState = DOWNLOADING;

					char directory[128];
					sprintf(directory,"maps/%s",mMapName);

					#ifdef WIN32
					_mkdir(directory);
					#else
					mkdir(directory, 0777);
					#endif

					FILE* file;
					
					char mapfile[128];
					sprintf(mapfile,"maps/%s/map.txt",mMapName);
					file = fopen(mapfile,"w");
					fclose(file);

					sprintf(mapfile,"maps/%s/tile.png",mMapName);
					file = fopen(mapfile,"w");
					fclose(file);

					if (mMapOverviewSize > 0) {
						sprintf(mapfile,"maps/%s/overview.png",mMapName);
						file = fopen(mapfile,"w");
						fclose(file);
					}

					Packet sendpacket = Packet();
					sendpacket.WriteInt8(MAPFILE);
					sendpacket.WriteChar(mMapName);
					sendpacket.WriteInt8(0);
					sendpacket.WriteInt32(0);
					mUdpManager->SendReliable(sendpacket,true);
					sendpacket.Clear();
					break;
				}
				
				if (mIsAdmin) {
					mAdminPlayersListBox->AddItem(new AdminPlayerItem((PersonOnline*)mPlayer));
				}

				Packet sendpacket = Packet();
				sendpacket.WriteInt8(NEWPLAYER);
				sendpacket.WriteChar(mPlayer->mName);
				sendpacket.WriteChar(gName);
				sendpacket.WriteInt8(mPlayer->mMovementStyle);
				sendpacket.WriteData((char*)gIcon,300);
				mUdpManager->SendReliable(sendpacket,true);
				sendpacket.Clear();

				break;
			}
			case GAMEINFO: {
				packet.ReadChar(mName,32);
				//char* mapname = packet.ReadChar();
				int gametype = packet.ReadInt8();
				int ff = packet.ReadInt8();
				//int numCTs = packet.ReadInt8();
				//int numTs = packet.ReadInt8();
				int numRemainingCTs = packet.ReadInt8();
				int numRemainingTs = packet.ReadInt8();
				float time = packet.ReadFloat();
				int freezetime = packet.ReadInt16();
				int roundtime = packet.ReadInt16();
				int endtime = packet.ReadInt16();
				int buytime = packet.ReadInt16();
				int respawntime = packet.ReadInt16();
				int invincibletime = packet.ReadInt16();
				int roundstate = packet.ReadInt8();
				float roundtimer = packet.ReadFloat();
				float endroundtimer = packet.ReadFloat();
				float buytimer = packet.ReadFloat();
				int numrounds = packet.ReadInt16();
				int numCTWins = packet.ReadInt16();
				int numTWins = packet.ReadInt16();
				int winner = packet.ReadInt8();
				int playerid = packet.ReadInt8();
				packet.ReadChar(mPlayer->mName,32);
				float timeMultiplier = packet.ReadFloat();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) mRoundBit ^= 128;
				if (mState == PLAYING) break;

				//mMapName = mapname;
				mGameType = gametype;

				mFriendlyFire = ff;
				//mNumCTs = numCTs;
				//mNumTs = numTs;
				mNumRemainingCTs = numRemainingCTs;
				mNumRemainingTs = numRemainingTs;
				mTime = time;
				mRoundFreezeTime = freezetime;
				mRoundTime = roundtime;
				mRoundEndTime = endtime;
				mBuyTime = buytime;
				mRespawnTime = respawntime;
				mRespawnTimer = mRespawnTime*1000;
				mInvincibleTime = invincibletime;
				mRoundState = roundstate;
				mRoundTimer = roundtimer;
				mRoundEndTimer = endroundtimer;
				mBuyTimer = buytimer;
				mNumRounds = numrounds;
				mNumCTWins = numCTWins;
				mNumTWins = numTWins;
				mWinner = winner;
				((PersonOnline*)mPlayer)->mId = playerid;
				mPeopleMap[playerid] = mPlayer;

				mTimeMultiplier = timeMultiplier;
				/*if (!mMap->Load(mapname,mGuns)) {
					mState = ERROR;
					mErrorString = "Error: Map could not be loaded";
					break;
				}

				mCamera->SetBounds(0.0f,0.0f,mMap->mCols*32.0f,mMap->mRows*32.0f);//mCamera->SetBounds(0+SCREEN_WIDTH_2,0+SCREEN_HEIGHT_2,mMap->mCols*32-SCREEN_WIDTH_2,mMap->mRows*32-SCREEN_HEIGHT_2);

				
				//mNumRemainingCTs = mMap->mNumRemainingCTs;
				//mNumRemainingTs = mMap->mNumRemainingTs;

				mTeamMenu->Enable();

				//mIsLoaded = true;
				mState = PLAYING;*/
				/*Packet sendpacket = Packet();
				sendpacket.WriteInt8(PLAYERICON);
				sendpacket.WriteInt8(playerid);
				sendpacket.WriteData((char*)gIcon,300);
				mUdpManager->SendReliable(sendpacket);
				sendpacket.Clear();*/

				break;
			}
			case GAMEINFOEND: {
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				mPlayer->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);

				mTeamMenu->Enable();

				mBuyMenu = new BuyMenu(mPlayer,mGuns);
				mBuyMenu->cross = &cross;
				mBuyMenu->mPickUpSound = gPickUpSound;
				mBuyMenu->mIsOldStyle = !(mMenuStyle == ANALOG);

				if (!mMap->Load(mMapName,mGuns,mGameType)) {
					mState = MAP_ERROR;
					break;
				}

				mCamera->SetBounds(0.0f,0.0f,mMap->mCols*32.0f,mMap->mRows*32.0f);//mCamera->SetBounds(0+SCREEN_WIDTH_2,0+SCREEN_HEIGHT_2,mMap->mCols*32-SCREEN_WIDTH_2,mMap->mRows*32-SCREEN_HEIGHT_2);

				mGrid->Rebuild(mMap->mCols*32,mMap->mRows*32,128);
				for (unsigned int i=0;i<mMap->mCollisionLines.size();i++) {
					mGrid->HashCollisionLine(&(mMap->mCollisionLines[i]));
				}
					
				mState = PLAYING;

				break;
			}
			case CTFINFO: {
				int numctflags = packet.ReadInt16();
				int numtflags = packet.ReadInt16();
				int ctflagx = packet.ReadInt16();
				int ctflagy = packet.ReadInt16();
				int tflagx = packet.ReadInt16();
				int tflagy = packet.ReadInt16();
				int ctflaghome = packet.ReadInt8();
				int tflaghome = packet.ReadInt8();
				int ctid = packet.ReadInt8();
				int tid = packet.ReadInt8();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				mNumFlags[CT] = numctflags;
				mNumFlags[T] = numctflags;
				mFlagX[CT] = ctflagx;
				mFlagY[CT] = ctflagy;
				mFlagX[T] = tflagx;
				mFlagY[T] = tflagy;
				mIsFlagHome[CT] = (ctflaghome == 1);
				mIsFlagHome[T] = (tflaghome == 1);
				if (ctid == -1) {
					mFlagHolder[CT] == NULL;
				}
				else {
					mFlagHolder[CT] = GetPerson(ctid);
				}
				if (tid == -1) {
					mFlagHolder[T] == NULL;
				}
				else {
					mFlagHolder[T] = GetPerson(tid);
				}

				break;
			}
			case PICKUPFLAG: {
				int team = packet.ReadInt8();
				int id = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				mIsFlagHome[team] = false;
				Person* player = GetPerson(id);
				mFlagHolder[team] = player;
				if (player != NULL) {
					player->mHasFlag = true;
					player->mMoney += 300;
					if (player->mMoney > 16000) {
						player->mMoney = 16000;
					}
					mHud->AddFlagEvent(player,0);
				}
				break;
			}
			case DROPFLAG: {
				int team = packet.ReadInt8();
				int id = packet.ReadInt8();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				mFlagHolder[team] = NULL;
				mFlagX[team] = x;
				mFlagY[team] = y;
				mIsFlagHome[team] = false;

				Person* player = GetPerson(id);
				if (player != NULL) {
					player->mHasFlag = false;
					mHud->AddFlagEvent(player,1);
				}
				break;
			}
			case RETURNFLAG: {
				int team = packet.ReadInt8();
				int id = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				mFlagX[team] = mMap->mFlagSpawn[team].x;
				mFlagY[team] = mMap->mFlagSpawn[team].y;
				mIsFlagHome[team] = true;

				Person* player = GetPerson(id);
				if (player != NULL) {
					player->mMoney += 300;
					if (player->mMoney > 16000) {
						player->mMoney = 16000;
					}
					mHud->AddFlagEvent(player,2);
				}
				break;
			}
			case CAPTUREFLAG: {
				int team = packet.ReadInt8();
				int id = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				mFlagX[team] = mMap->mFlagSpawn[team].x;
				mFlagY[team] = mMap->mFlagSpawn[team].y;
				mIsFlagHome[team] = true;
				mFlagHolder[team] = NULL;

				int team2 = (team == CT) ? T:CT;
				mNumFlags[team2] += 1;

				Person* player = GetPerson(id);
				if (player != NULL) {
					player->mHasFlag = false;
					player->mMoney += 1000;
					if (player->mMoney > 16000) {
						player->mMoney = 16000;
					}
					player->mNumKills += 5;

					mHud->AddFlagEvent(player,3);
				}

				break;
			}
			case GUNINFO: {
				int id = packet.ReadInt8();
				int delay = packet.ReadInt16();
				int damage = packet.ReadInt16();
				float spread = packet.ReadFloat();
				int clip = packet.ReadInt8();
				int numclips = packet.ReadInt8();
				int reloaddelay = packet.ReadInt16();
				float speed = packet.ReadFloat();
				float bulletspeed = packet.ReadFloat();
				float viewangle = packet.ReadFloat();
				int cost = packet.ReadInt16();
				int type = packet.ReadInt8();
				char name[15];
				packet.ReadChar(name,15);
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (id < 0 || id >= 28) break;
				
				//if (mGuns[id] != NULL) break;

				Gun gun = {gGunHandQuads[id],gGunGroundQuads[id],id,delay,damage,spread,clip,numclips,reloaddelay,speed,bulletspeed,viewangle,cost,type};
				mGuns[id] = gun;

				strcpy(mGuns[id].mName,name);
				mGuns[id].mFireSound = gGuns[id].mFireSound;
				mGuns[id].mReloadSound = gGuns[id].mReloadSound;
				mGuns[id].mDryFireSound = gGuns[id].mDryFireSound;

				int space = 120-clip;
				int w = 0; 
				if (clip != 0) {
					w = (int)floorf((float)space/clip);
				}
				if (w < 0) w = 0;
				mGuns[id].mAmmoBarWidth = w;

				/*char buffer[25];
				sprintf(buffer,"sfx/%s.wav",name);
				mGuns[id].mFireSound = mSoundSystem->LoadSample(buffer);
				sprintf(buffer,"sfx/%sreload.wav",name);
				mGuns[id].mReloadSound = mSoundSystem->LoadSample(buffer);

				if (mGuns[id].mType == PRIMARY) {
					mGuns[id].mDryFireSound = gDryFireRifleSound;
				}
				else if (mGuns[id].mType == SECONDARY) {
					mGuns[id].mDryFireSound = gDryFirePistolSound;
				}*/

				break;
			}
			case NEWMAP: {
				char mapname[32];
				packet.ReadChar(mapname,32);
				//strcpy(mapname,packet.ReadChar(length));
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (mState == PLAYING) {
					mIsMapChanging = true;
					mMapChangeTimer = 5000.0f;

					if (mTeamMenu->mIsActive) {
						mTeamMenu->Disable();
					}
					if (gDanzeff->mIsActive) {
						gDanzeff->Disable();
					}
					if (mBuyMenu->mIsActive) {
						mBuyMenu->Disable();
					}
				}
				else {
					gReconnect = true;
					mParent->SetNextState(GAME_STATE_LOBBY);
				}

				break;
			}
			case NEWPLAYER: {//new player; join game
				int id = packet.ReadInt8();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int team = packet.ReadInt8();
				int type = packet.ReadInt8();
				int state = packet.ReadInt8();
				int numkills = packet.ReadInt16();
				int numdeaths = packet.ReadInt16();
				int movementstyle = packet.ReadInt8();
				int money = packet.ReadInt16();
				char name[32];
				packet.ReadChar(name,32);
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				bool exists = false;
				/*for (int i=0; i<mPeople.size(); i++) {
					if (((PersonOnline*)mPeople[i])->mId == id) {
						exists = true;
					}
				}*/
				std::map<int,Person*>::iterator iter = mPeopleMap.find(id);
				if (iter != mPeopleMap.end()) {
					exists = true;
				}
				if (exists) break;

				mNumPlayers++;
				if (team == NONE) {
				}
				else if (team == CT) {
					mNumCTs++;
				}
				else if (team == T) {
					mNumTs++;
				}
				
				if (team >= 2 || type >= 4) break;

				int teamtemp = team;
				if (teamtemp == NONE) {
					teamtemp = CT;
				}

				PersonOnline* player = new PersonOnline(gPlayersQuads[teamtemp][type], gPlayersDeadQuads[teamtemp][type], &mBullets, &mGunObjects, team, name, RELATIVE1);

				player->mGuns[KNIFE] = new GunObject(&mGuns[0],0,0);
				player->mGunIndex = KNIFE;
				player->SetState(state);
				player->mTeam = team;
				player->mNumKills = numkills;
				player->mNumDeaths = numdeaths;
				player->mMovementStyle = movementstyle;
				player->mMoney = money;
				//player->mUdpManager = mUdpManager;
				player->mId = id;

				player->mX = x;
				player->mY = y;
				player->mOldX = x;
				player->mOldY = y;
				player->mSX = x;
				player->mSY = y;

				DWORD bits[100];
				int i=0;
				for (int y=0; y<10; y++) {
					for (int x=0; x<10; x++) {
						bits[(9-y)*10+x] = ARGB(0,0,0,0);
						i += 3;
					}
				}
				player->mIconTexture->UpdateBits(0,0,10,10,bits);

				mPeople.push_back(player);
				mPeopleMap[id] = player;

				std::sort(mPeople.begin(),mPeople.end(),SortByScore());

				if (mIsAdmin) {
					mAdminPlayersListBox->AddItem(new AdminPlayerItem(player));
				}

				char buffer[127];
				sprintf(buffer,"%s has joined the game",player->mName);
				mHud->AddMessageEvent(buffer);

				Packet sendpacket = Packet();
				sendpacket.WriteInt8(PLAYERICON);
				sendpacket.WriteInt8(id);
				mUdpManager->SendReliable(sendpacket);

				break;
			}
			case PLAYERICON: {//new player; join game
				int id = packet.ReadInt8();
				char icon[300];
				int datalength = packet.ReadData(icon);
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				
				DWORD bits[100];
				int i=0;
				for (int y=0; y<10; y++) {
					for (int x=0; x<10; x++) {
						int a = 255;
						if ((unsigned char)icon[i] == 255 && (unsigned char)icon[i+2] == 255 && (unsigned char)icon[i+1] == 0) {
							a = 0;
						}
						bits[(9-y)*10+x] = ARGB(a,(unsigned char)icon[i+2],(unsigned char)icon[i+1],(unsigned char)icon[i]);
						i += 3;
					}
				}
				player->mIconTexture->UpdateBits(0,0,10,10,bits);

				break;
			}
			case REMOVEPLAYER: {
				int id = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (id == -1) break;

				Person* player = GetPerson(id);

				for (int i=0; i<mPeople.size(); i++) {
					if (((PersonOnline*)mPeople[i])->mId == id) {

						if (mPeople[i] == mPlayer) {
							mState = ERROR;
							mErrorString = "Error: You have been removed from the server";

							Packet sendpacket = Packet();
							sendpacket.WriteInt8(REMOVEPLAYER);
							sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
							mUdpManager->Send(sendpacket);
							mUdpManager->Update(0);
							break;
						}

						mNumPlayers--;
						if (mPeople[i]->mTeam == CT) {
							mNumCTs--;
						}
						else if (mPeople[i]->mTeam == T) {
							mNumTs--;
						}

						if (mPeople[i]->mState != DEAD && (mPeople[i]->mTeam == CT || mPeople[i]->mTeam == T)) {
							mPeople[i]->Die();
							UpdateScores(NULL,mPeople[i],NULL);
						}
						
						if (mSpec == mPeople[i]) {
							NewSpec(mPeople[i],i);
						}

						for (int j=0; j<mBullets.size(); j++) {
							if (mBullets[j]->mParent == mPeople[i]) {
								mBullets[j]->mParent = NULL;
							}
						}
						if (mFFAWinner == mPeople[i]) {
							mFFAWinner = NULL;
						}

						if (mIsAdmin) { // remove from admin menu
							for (int j=0; j<mAdminPlayersListBox->mItems.size(); j++) {
								AdminPlayerItem *item = (AdminPlayerItem*)mAdminPlayersListBox->mItems[j];
								if (item->player == player) {
									mAdminPlayersListBox->RemoveItem(item);
									break;
								}
							}
						}

						char buffer[127];
						sprintf(buffer,"%s has left the game",mPeople[i]->mName);
						mHud->AddMessageEvent(buffer);

						delete mPeople[i];
						//mPeople[i] = NULL;
						mPeople.erase(mPeople.begin()+i);

						Hash();
						break;
					}
				}

				mPeopleMap.erase(id);

				break;
			}
			case SWITCHTEAM: {
				int id = packet.ReadInt8();
				int team = packet.ReadInt8();
				int type = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
	
				if (player->mTeam == NONE) {
				}
				else if (player->mTeam == CT) {
					mNumCTs--;
					if (player->mState != DEAD) {
						mNumRemainingCTs--;
					}
				}
				else if (player->mTeam == T) {
					mNumTs--;
					if (player->mState != DEAD) {
						mNumRemainingTs--;
					}
				}

				if (team == NONE) {
				}
				else if (team == CT) {
					mNumCTs++;
					if (player->mState != DEAD) {
						mNumRemainingCTs++;
					}
				}
				else if (team == T) {
					mNumTs++;
					if (player->mState != DEAD) {
						mNumRemainingTs++;
					}
				}

				player->mTeam = team;

				if (team < 2 && type < 4) {
					player->SetQuads(gPlayersQuads[team][type],gPlayersDeadQuads[team][type]);
					//player->mQuad = gPlayersQuads[team][type];
					//player->mDeadQuad = gPlayersDeadQuads[team][type];
				}
				/*if (player->mTeam == CT) {
					player->mQuad = gPlayersQuads[0];
					player->mDeadQuad = gPlayersDeadQuads[0];
				}
				else if (player->mTeam == T) {
					player->mQuad = gPlayersQuads[1];
					player->mDeadQuad = gPlayersDeadQuads[1];
				}*/

				/*if (player == mPlayer) {
					delete mBuyMenu;
					mBuyMenu = new BuyMenu(mPlayer,mGuns);
					mBuyMenu->cross = &cross;
					mBuyMenu->mPickUpSound = gPickUpSound;
				}*/

				char buffer[127];
				if (mGameType == FFA) {
					if (player->mTeam == NONE) {
						sprintf(buffer,"%s has joined the Spectators",player->mName);
						mHud->AddMessageEvent(buffer);
					}
				}
				else {
					if (player->mTeam == CT) {
						sprintf(buffer,"%s has joined the Counter-Terrorist Team",player->mName);
					}
					else if (player->mTeam == T) {
						sprintf(buffer,"%s has joined the Terrorist Team",player->mName);
					}
					else if (player->mTeam == NONE) {
						sprintf(buffer,"%s has joined the Spectators",player->mName);
					}
					mHud->AddMessageEvent(buffer);
				}

				if (mGameType != TEAM) {
					if (player == mPlayer) {
						mRespawnTimer = mRespawnTime*1000;
					}
				}
				break;
			}
			case PLAYERMOVE: {
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int speed = packet.ReadInt8();
				int angle = packet.ReadInt8();
				float time = packet.ReadFloat();

				if (mPlayer == NULL) break;
				if (mPlayer->mState == DEAD) break;
				if (time <= ((PersonOnline*)mPlayer)->mLastMoveTime) break;
				if (!round) break;

				float angleFloat = (float)((angle+128)/(255.0f/(2*M_PI)));
				float speedFloat = (float)(speed/100.0f);

				((PersonOnline*)mPlayer)->mLastMoveTime = time;
				State state = {x,y,speedFloat,angleFloat};

				//player->mSX = x;
				//player->mSY = y;
				//player->mSSpeed = speed;
				//player->mSAngle = angle;
				ClientCorrection(state, time);

				break;
			}
			case RESPAWN: {
				int id = packet.ReadInt8();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int active = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;

				player->Reset();
				player->Teleport(x,y);

				if (active == 1) {
					player->mIsActive = true;
				}

				if (mGameType != TEAM) {
					player->mInvincibleTime = mInvincibleTime*1000;
				}

				if (mPlayer == player) {
					for (int i=0; i<256; i++) {
						mMoves[i].time = -1.0f;
					}
					mMovesIndex = 0;
					mSpec = player;
				}
				if (mPlayer == player || mSpec == player) {
					mCamera->mTX = x;
					mCamera->mTY = y;
					mCamera->mX = x;
					mCamera->mY = y;
				}
				break;
			}
			case MOVE: {
				int id = packet.ReadInt8();
				int sx = packet.ReadInt16();
				int sy = packet.ReadInt16();
				int facingangle = packet.ReadInt8();
				int speed = packet.ReadInt8();
				int angle = packet.ReadInt8();
				int state = packet.ReadInt8(); // gunindex and isfiring
				//float time = packet.ReadFloat();

				if (!round) break;

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player == (PersonOnline*)mPlayer) break;
				if (player->mState == DEAD) break;
				//if (player->mLastMoveTime >= time) break;		

				float latency = mPing*0.5f;//(time-player->mLastMoveTime);//*0.75f;
				if (latency > 200.0f) {
					latency = 200.0f;
				}

				player->mSX = sx;
				player->mSY = sy;
				player->mOldSX = sx;
				player->mOldSY = sy;
				player->mSFacingAngle = (float)((facingangle+128)/(255.0f/(2*M_PI)));
				player->mSSpeed = (float)(speed/100.0f);
				player->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));

				bool isfiring = (state&128) != 0;

				if (player->mIsFiring == true && isfiring == false) {
					player->mHasFired = false;
				}
				player->mIsFiring = isfiring;

				int gunindex = state&127;
				if (gunindex < 0 || gunindex >= 5) break;
				if (gunindex != player->mGunIndex) {
					player->Switch(gunindex);
				}

				player->mSX += player->mSSpeed*cosf(player->mSAngle)*latency;
				player->mSY += player->mSSpeed*sinf(player->mSAngle)*latency;
				//player->mLastMoveTime = time;

				if (speed == 0) {
					player->SetMoveState(NOTMOVING);
				}
				else {
					player->Move(player->mSSpeed,player->mSAngle);
				}

				break;
			}
			case SWITCHGUN: {
				int id = packet.ReadInt8();
				int gunindex = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				if (gunindex < 0 || gunindex >= 5) break;

				player->Switch(gunindex);

				break;
			}
			case SPAWNGUN: {
				int id = packet.ReadInt16();
				int gunindex = packet.ReadInt8();
				int clipammo = packet.ReadInt8();
				int remainingammo = packet.ReadInt16();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				//mUdpManager->SendAck(ackid);
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				bool exists = false;
				for (int i=0; i<mGunObjects.size(); i++) {
					if (((GunObjectOnline*)mGunObjects[i])->mId == id) {
						exists = true;
					}
				}
				//HACK
				for (int i=0; i<mPeople.size(); i++) {
					for (int j=0; j<5; j++) {
						if (mPeople[i]->mGuns[j] == NULL) continue;
						if (((GunObjectOnline*)mPeople[i]->mGuns[j])->mId == id) {
							exists = true;
						}	
					}
				}

				if (exists) break;

				if (gunindex >= 28 || gunindex < 0) break;
				GunObjectOnline* gunobject = new GunObjectOnline(&mGuns[gunindex],clipammo,remainingammo);
				gunobject->mX = x;
				gunobject->mY = y;
				gunobject->mSX = x;
				gunobject->mSY = y;
				gunobject->mOldX = x;
				gunobject->mOldY = y;
				gunobject->mId = id;
				mGunObjects.push_back(gunobject);

				mGunObjectsMap[id] = gunobject;
				mGrid->HashGunObject(gunobject);
				break;
			}
			case PICKUPGUN: {
				int id = packet.ReadInt8();
				int gunid = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;

				//GunObject* gunobject = NULL;
				for (int i=0; i<mGunObjects.size(); i++) {
					if (((GunObjectOnline*)mGunObjects[i])->mId == gunid) {
						if (player->PickUp(mGunObjects[i])) {
							mGunObjects.erase(mGunObjects.begin()+i);
							/*if (sendack) {
								mUdpManager->SendAck(ackid);
							}*/
							Hash();
							break;
						}
					}
				}
				/*//HACK.
				for (int i=0; i<5; i++) {
					if (player->mGuns[i] == NULL) continue;
					if (((GunObjectOnline*)player->mGuns[i])->mId == gunid) {
						if (sendack) {
							mUdpManager->SendAck(ackid);
						}
					}
				}*/

				break;
			}
			case DROPGUN: {
				int id = packet.ReadInt8();
				int gunindex = packet.ReadInt8();
				int gunid = packet.ReadInt16();
				int sx = packet.ReadInt16();
				int sy = packet.ReadInt16();
				int angle = packet.ReadInt8();
				//float time = packet.ReadFloat();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;

				if (gunindex >= 5) break;
				if (gunindex == KNIFE) break;

				//if (time-mTime > TIMETHRESHOLD) break;

				bool previousround = !round;

				GunObjectOnline* gunobject = NULL;
				int index = -1;
				for (int i=0; i<mGunObjects.size(); i++) {
					if (((GunObjectOnline*)mGunObjects[i])->mId == gunid) {
						gunobject = (GunObjectOnline*)mGunObjects[i];
						index = i;
						break;
					}
				}
				//if (gunobject == NULL) break;

				if (player == mPlayer && gunobject != NULL) {
					float latency = (0.35f-gunobject->mSpeed)/0.001f;
					
					float dt = 16.6666f;

					gunobject->mSpeed = 0.35f;
					gunobject->mAngle = (float)((angle+128)/(255.0f/(2*M_PI)));
					gunobject->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));
					gunobject->mSX = sx;
					gunobject->mSY = sy;

					for (int i=0; i<(int)(latency/dt); i++) {
						gunobject->mSpeed -= 0.001f*dt;
						if (gunobject->mSpeed <= 0.0f) break;
						gunobject->mSX += gunobject->mSpeed*cosf(gunobject->mAngle)*dt;
						gunobject->mSY += gunobject->mSpeed*sinf(gunobject->mAngle)*dt;
					}

					if (previousround) {
						delete gunobject;
						mGunObjects.erase(mGunObjects.begin()+index);
						//Hash();
					}

				}
				else {
					GunObjectOnline* gunobject = (GunObjectOnline*)player->mGuns[gunindex];
					if (gunobject == NULL) break;

					//GunObject* gunobject = NULL;
					if (gunobject->mId == gunid) {
						if (player->Drop(gunindex)) {
						}
					}

					gunobject->mSX = sx;
					gunobject->mSY = sy;
					gunobject->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));

					if (previousround) {
						delete mGunObjects.back();
						mGunObjects.pop_back();
					}
				}
				
				if (!previousround) {
					//player->mLastDropTime = time;
				}

				break;
			}
			case DROPGUNDEAD: {
				int id = packet.ReadInt8();
				int gunindex = packet.ReadInt8();
				int gunid = packet.ReadInt16();
				int guntype = packet.ReadInt8();
				int sx = packet.ReadInt16();
				int sy = packet.ReadInt16();
				int angle = packet.ReadInt8();
				//float time = packet.ReadFloat();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;

				if (gunindex >= 5) break;
				if (gunindex == KNIFE) break;

				//if (time-mTime > TIMETHRESHOLD) break;

				bool previousround = !round;

				GunObjectOnline* gunobject = NULL;
				int index = -1;
				for (int i=0; i<mGunObjects.size(); i++) {
					if (((GunObjectOnline*)mGunObjects[i])->mId == gunid) {
						gunobject = (GunObjectOnline*)mGunObjects[i];
						index = i;
						break;
					}
				}
				//if (gunobject == NULL) break;

				if (player == mPlayer && gunobject != NULL) { //player dropped gun right before dying
					gunobject->mSpeed = 0.0f;
					gunobject->mAngle = (float)((angle+128)/(255.0f/(2*M_PI)));
					gunobject->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));
					gunobject->mSX = sx;
					gunobject->mSY = sy;
					gunobject->mX = sx;
					gunobject->mY = sy;
					gunobject->mOldX = sx;
					gunobject->mOldY = sy;
				}
				else if (player == mPlayer && gunobject != NULL && player->mGuns[gunindex] == NULL) { //player threw nade before dying
						GunObjectOnline* gunobject = new GunObjectOnline(&mGuns[guntype],1,0);
						gunobject->mX = sx;
						gunobject->mY = sy;
						gunobject->mOldX = sx;
						gunobject->mOldY = sy;
						gunobject->mSX = sx;
						gunobject->mSY = sy;
						gunobject->mRotation = angle;
						gunobject->mAngle = angle;
						gunobject->mSAngle = angle;
						gunobject->mSpeed = 0.0f;
						gunobject->mOnGround = false;
						mGunObjects.push_back(gunobject);
				}
				else {
					GunObjectOnline* gunobject = (GunObjectOnline*)player->mGuns[gunindex];
					if (gunobject == NULL) break;

					//GunObject* gunobject = NULL;
					if (gunobject->mId == gunid) {
						if (player->Drop(gunindex,0)) {
						}
					}

					gunobject->mSX = sx;
					gunobject->mSY = sy;
					gunobject->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));

					if (previousround) {
						delete mGunObjects.back();
						mGunObjects.pop_back();
					}
				}
				
				if (!previousround) {
					//player->mLastDropTime = time;
				}

				break;
			}
			case NEWGUN: {
				int id = packet.ReadInt8();
				int gunid = packet.ReadInt16();
				int gunindex = packet.ReadInt8();
				int clipammo = packet.ReadInt8();
				int remainingammo = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;

				bool exists = false;
				/*for (int i=0; i<mGunObjects.size(); i++) {
					if (((GunObjectOnline*)mGunObjects[i])->mId == gunid) {
						exists = true;
					}
				}*/
				std::map<int,GunObject*>::iterator iter = mGunObjectsMap.find(gunid);
				if (iter != mGunObjectsMap.end()) {
					exists = true;
				}
				if (exists) break;
				if (gunindex >= 28 || gunindex < 0) break;

				GunObjectOnline* gunobject = new GunObjectOnline(&mGuns[gunindex],clipammo,remainingammo);
				gunobject->mId = gunid;
				player->PickUp(gunobject);

				mGunObjectsMap[gunid] = gunobject;

				break;
			}
			case MOVEGUN: {
				int id = packet.ReadInt16();
				int sx = packet.ReadInt16();
				int sy = packet.ReadInt16();
				int angle = packet.ReadInt8();

				GunObjectOnline* gunobject = (GunObjectOnline*)GetGunObject(id);
				if (gunobject == NULL) break;

				gunobject->mSX = sx;
				gunobject->mSY = sy;
				gunobject->mSAngle = (float)((angle+128)/(255.0f/(2*M_PI)));
				break;
			}
			/*case FIRE: {
				int id = packet.ReadInt16();
				//int gunindex = packet.ReadInt16();
				//float angle = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				mUdpManager->SendAck(ackid);

				Person* player = NULL;
				for (int i=0; i<mPeople.size(); i++) {
					if (((PersonOnline*)mPeople[i])->mId == id) {
						player = mPeople[i];
					}
				}
				if (player == NULL) break;
				player->Fire();

				break;
			}*/
			case STARTFIRE: {
				int id = packet.ReadInt8();
				//int gunindex = packet.ReadInt16();
				//float angle = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				
				player->mIsFiring = true;

				break;
			}
			case ENDFIRE: {
				int id = packet.ReadInt8();
				//int gunindex = packet.ReadInt16();
				//float angle = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;

				player->mIsFiring = false;
				player->mHasFired = false;

				break;
			}
			case NEWBULLET: {
				int guntype = packet.ReadInt8();
				//int id = packet.ReadInt16();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				//int px = packet.ReadInt16();
				//int py = packet.ReadInt16();
				int angle = packet.ReadInt16();
				//int speed = packet.ReadInt8();
				//int damage = packet.ReadInt8();
				int parentid = packet.ReadInt8();
				int ammo = packet.ReadInt8();
				float time = packet.ReadFloat();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				/*bool exists = false;
				for (int i=0; i<mBullets.size(); i++) {
					if (mBullets[i]->mId == id) {
						exists = true;
					}
				}
				if (exists) break;*/

				if (!round) break;
				if (guntype < 0 || guntype >= 28) break;


				PersonOnline* player = (PersonOnline*)GetPerson(parentid);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				if (player->mLastFireTime >= time) break;

				GunObject *gunobject = NULL;
				int i = player->mGunIndex;
				while (true) {
					if (player->mGuns[i] != NULL) {
						if (player->mGuns[i]->mGun->mId == guntype) {
							gunobject = player->mGuns[i];
							break;
						}
					}
					i++;
					if (i >= 5) i = 0;
					if (i == player->mGunIndex) {
						break;
					}
				}
				
				if (gunobject == NULL) break; // player wasn't holding that type of gun...

				player->mLastFireTime = time;
				gunobject->mClipAmmo = ammo;

				float speed = 0.3f*mGuns[guntype].mBulletSpeed;
				float newangle = (float)((angle+32768)/(65535.0f/(2*M_PI)));
				BulletOnline* bullet = new BulletOnline(x,y,x-24*cosf(newangle),y-24*sinf(newangle),newangle,speed,0,player);
				//bullet->mId = id;

				float latency = mTime-time;
				if (latency > 500.0f) {
					latency = 500.0f;
				}
				else if (latency < 0.0f) {
					latency = 0.0f;
				}
				
				bullet->AddLatency(latency);
				//bullet->mSX += bullet->mSpeed*cosf(bullet->mAngle)*latency;
				//bullet->mSY += bullet->mSpeed*sinf(bullet->mAngle)*latency;
				mBullets.push_back(bullet);

					//player->GetCurrentGun()->mClipAmmo--;
				//gSfxManager->PlaySample(player->GetCurrentGun()->mGun->mFireSound,player->mX,player->mY);

				break;
			}
			case NEWSHOTGUNBULLET: {
				//int numbullets = packet.ReadInt8();
				//int px = packet.ReadInt16();
				//int py = packet.ReadInt16();		
				//int speed = packet.ReadInt8();
				int guntype = packet.ReadInt8();
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int parentid = packet.ReadInt8();
				int ammo = packet.ReadInt8();
				float time = packet.ReadFloat();

				PersonOnline* player = (PersonOnline*)GetPerson(parentid);

				float latency = mTime-time;
				if (latency > 500.0f) {
					latency = 500.0f;
				}
				else if (latency < 0.0f) {
					latency = 0.0f;
				}

				float speed = 0.3f;
				int numBullets = 0;
				if (guntype == 7) { //m3, 6 bullets
					numBullets = 6;
					speed *= mGuns[guntype].mBulletSpeed;
				}
				else if (guntype == 8) { // xm1014, 4 bullets
					numBullets = 4;
					speed *= mGuns[guntype].mBulletSpeed;
				}
				for (int i=0; i<numBullets; i++) {
					//int id = packet.ReadInt16();
					int angle = packet.ReadInt16();
					//int damage = packet.ReadInt8();

					if (!round) continue;

					if (player == NULL) continue;
					if (player->mState == DEAD) continue;
					if (player->mLastFireTime >= time) continue;

					float newangle = (float)((angle+32768)/(65535.0f/(2*M_PI)));
					BulletOnline* bullet = new BulletOnline(x,y,x-24*cosf(newangle),y-24*sinf(newangle),newangle,speed,0,player);
					//bullet->mId = id;

					bullet->AddLatency(latency);
					//bullet->mSX += bullet->mSpeed*cosf(bullet->mAngle)*latency;
					//bullet->mSY += bullet->mSpeed*sinf(bullet->mAngle)*latency;
					mBullets.push_back(bullet);
				}

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				if (!round) break;

				if (player == NULL) break;
				if (player->mState == DEAD) break;
				if (player->mLastFireTime >= time) break;

				player->mLastFireTime = time;

				player->GetCurrentGun()->mClipAmmo = ammo;

				break;
			}
			case NEWGRENADE: {
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				//int px = packet.ReadInt16();
				//int py = packet.ReadInt16();
				int angle = packet.ReadInt16();
				int speed = packet.ReadInt16();
				int parentid = packet.ReadInt8();
				float time = packet.ReadFloat();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				/*bool exists = false;
				for (int i=0; i<mBullets.size(); i++) {
					if (mBullets[i]->mId == id) {
						exists = true;
					}
				}
				if (exists) break;*/

				if (!round) break;

				PersonOnline* player = (PersonOnline*)GetPerson(parentid);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				//if (player->mLastFireTime >= time) break;
				if (player->mGuns[GRENADE] == NULL) break;

				//player->mLastFireTime = time;
				
				player->mIsFiring = false;
				player->mHasFired = false;

				int grenadetype = HE;
				if (player->mGuns[GRENADE]->mGun->mId == 25) { //FLASH
					grenadetype = FLASH;
				}
				else if (player->mGuns[GRENADE]->mGun->mId == 26) { //HE
					grenadetype = HE;
				}
				else if (player->mGuns[GRENADE]->mGun->mId == 27) { //SMOKE
					grenadetype = SMOKE;
				}

				float latency = mTime-time;
				if (latency > 500.0f) {
					latency = 500.0f;
				}
				else if (latency < 0.0f) {
					latency = 0.0f;
				}

				player->mState = ATTACKING; //so that grenade is deleted
				player->mGunIndex = GRENADE;

				float speedtemp = (float)((speed+32768)/(65535.0f/0.2f));
				speedtemp *= 2000.0f/(2000.0f-latency);
				float newangle = (float)((angle+32768)/(65535.0f/(2*M_PI)));

				Grenade* grenade = new Grenade(x,y,x-24*cosf(newangle),y-24*sinf(newangle),newangle,speedtemp,player,grenadetype);
				grenade->mTimer -= latency;

				mBullets.push_back(grenade);

				/*delete player->mGuns[GRENADE];
				player->mGuns[GRENADE] = NULL;*/
				if (player != mPlayer) {
					player->StopFire();
				}

				//gSfxManager->PlaySample(gFireInTheHoleSound,player->mX,player->mY);

				break;
			}
			case EXPLODEGRENADE: {
				int x = packet.ReadInt16();
				int y = packet.ReadInt16();
				int grenadetype = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) break;

				Explode(x,y,grenadetype);

				break;
			}
			case REMOVEBULLET: {
				/*int id = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				mUdpManager->SendAck(ackid);

				Bullet* bullet = NULL;
				for (int i=0; i<mBullets.size(); i++) {
					if (mBullets[i]->mId == id) {
						player = mPeople[i];
					}
				}
				if (player == NULL) break;
				player->Fire();

				break;*/
			}
			case STARTRELOAD: {
				int id = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				
				player->Reload();

				break;
			}
			case ENDRELOAD: {
				int id = packet.ReadInt8();
				int gunindex = packet.ReadInt16();
				int clipammo = packet.ReadInt16();
				int remainingammo = packet.ReadInt16();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				
				if (gunindex >= 5) break;
				if (player->mGuns[gunindex] == NULL) break;

				if (player->mGuns[gunindex]->mClipAmmo > clipammo) {
					player->mGuns[gunindex]->mClipAmmo = clipammo;
				}
				player->mGuns[gunindex]->mRemainingAmmo = remainingammo;

				break;
			}
			case HIT: {
				int id = packet.ReadInt8();
				int health = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				if (!round) break;

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				if (player->mHealth == health) break;

				player->mHealth = health;
				player->SetMoveState(NOTMOVING);
				player->mSpeed *= 0.1f;
				((PersonOnline*)player)->mSSpeed *= 0.1f;
				//player->TakeDamage(damage);

				break;
			}
			case HITINDICATOR: {
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				if (!round) break;

				mHitTime = 1000.0f;
				gSfxManager->PlaySample(gHitIndicatorSound);

				break;
			}
			case DAMAGEINDICATOR: {
				float angle = packet.ReadFloat();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				if (!round) break;

				mHud->AddDamageIndicator(angle);

				break;
			}
			case RECEIVEFLASH: {
				int id = packet.ReadInt8();
				int intensity = packet.ReadInt8(); 

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				if (player->mState == DEAD) break;
				
				player->ReceiveFlash((float)intensity/128.0f);

				break;
			}
			case KILLEVENT: {
				int attackerid = packet.ReadInt8();
				int victimid = packet.ReadInt8();
				int gunindex = packet.ReadInt8();
				int kills = packet.ReadInt16();
				int deaths = packet.ReadInt16();
				int money = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				int attackerindex = 0;
				Person* attacker = NULL;
				for (int i=0; i<mPeople.size(); i++) {
					if (((PersonOnline*)mPeople[i])->mId == attackerid) {
						attacker = mPeople[i];
						attackerindex = i;
					}
				}
				if (attacker == NULL) break;

				Person* victim = GetPerson(victimid);
				if (victim == NULL) break;
				if (victim->mState == DEAD) break;

				if (gunindex >= 28 || gunindex < -1) break;

				if (attacker == mPlayer) {
					if (gKills != -1 || gDeaths != -1) {
						gKills += kills-mPlayer->mNumKills;
						gKills2 += (kills-mPlayer->mNumKills)*7;
					}
				}
				if (victim == mPlayer) {
					if (gKills != -1 || gDeaths != -1) {
						gDeaths += deaths-mPlayer->mNumDeaths;
						gDeaths2 += (deaths-mPlayer->mNumDeaths)*7;
					}
				}

				if (gunindex != -1) {
					UpdateScores(attacker,victim,&mGuns[gunindex]);
				}
				else {
					UpdateScores(attacker,victim,NULL);
				}

				attacker->mNumKills = kills;
				victim->mNumDeaths = deaths;
				attacker->mMoney = money;

				victim->Die();

				if (mSpec->mState == DEAD) {
					NewSpec(attacker,attackerindex);
				}

				break;
			}
			case WINEVENT: {
				int code = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (mGameType == FFA) {
					mFFAWinner = GetPerson(code);
				}
				else if (mGameType == TEAM || mGameType == CTF) {
					if (mWinner == NONE) {
						if (code == T) {
							mNumTWins++;
							gSfxManager->PlaySample(gRoundEndSounds[T]);
						}
						else if (code == CT) {
							mNumCTWins++;
							gSfxManager->PlaySample(gRoundEndSounds[CT]);
						}
						else if (code == TIE) {
							gSfxManager->PlaySample(gRoundEndSounds[TIE]);
						}
					}
				}
				mWinner = code;

				break;
			}
			case RESETROUND: {
				int numrounds = packet.ReadInt16();
				int numCTWins = packet.ReadInt16();
				int numTWins = packet.ReadInt16();
				int numCTs = packet.ReadInt8();
				int numTs = packet.ReadInt8();
				int freezetime = packet.ReadInt16();
				int roundtime = packet.ReadInt16();
				int endtime = packet.ReadInt16();
				int buytime = packet.ReadInt16();

				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				if (!round) mRoundBit ^= 128;
				if (mNumRounds == numrounds) break;
				

				mNumRounds = numrounds;
				mNumCTWins = numCTWins;
				mNumTWins = numTWins;

				mNumCTs = numCTs;
				mNumTs = numTs;

				mNumRemainingCTs = numCTs;
				mNumRemainingTs = numTs;

				mRoundFreezeTime = freezetime;
				mRoundTime = roundtime;
				mRoundEndTime = endtime;
				mBuyTime = buytime;

				ResetRound();

				for (int i=0; i<256; i++) {
					mMoves[i].time = -1.0f;
				}
				mMovesIndex = 0;

				mIsResetting = true;
				
				break;
			}
			case RESETPLAYERS: {
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				for (unsigned int i=0;i<mPeople.size();i++) {
					if (mPeople[i]->mTeam == NONE) continue;

					mPeople[i]->Die();
					mPeople[i]->mNumKills = 0;
					mPeople[i]->mNumDeaths = 0;
					mPeople[i]->mMoney = 800;
				}
				break;
			}
			case RESETROUNDEND: {
				int money = packet.ReadInt16();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				mPlayer->mMoney = money;

				mIsResetting = false;

				break;
			}
			case CHAT: {
				int id = packet.ReadInt8();
				char string[127];
				packet.ReadChar(string,127);
				int isteamonly = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;

				int team = player->mTeam;
				if (mGameType == FFA && team != NONE) {
					if (player == mPlayer) {
						team = CT;
					}
					else {
						team = T;
					}
				}

				bool isdead = (player->mState == DEAD);
				mHud->AddChatEvent(player->mName,string,team,isdead,isteamonly);

				break;
			}
			case BUY: {
				int id = packet.ReadInt8();
				int money = packet.ReadInt16();
				int choice = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				Person* player = GetPerson(id);
				if (player == NULL) break;
				
				//if (player->mMoney == money) break;

				player->mMoney = money;
				if (player->mState == DEAD) break;

				Buy(player,choice);
				
				break;
			}
			case ERROR1: {
				char string[127];
				packet.ReadChar(string,127);
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}

				mState = ERROR;
				mErrorString = string;
				break;
			}
			case PING: { //keepalive packet
				/*float time = packet.ReadFloat();

				if (!round) break;

				float latency = mPlayer->mPing*0.5f;
				if (latency > 500.0f) {
					latency = 500.0f;
				}
				time += latency;

				if (fabs(mTime-time) > 10.0f) {
					mTime += (time-mTime)*0.5f;
				}*/
				sendpacket.WriteInt8(PING);
				mUdpManager->Send(sendpacket);
				sendpacket.Clear();

				break;
			}
			case PLAYERPING: {
				int id = packet.ReadInt8();
				int ping = packet.ReadInt16();

				PersonOnline* player = (PersonOnline*)GetPerson(id);
				if (player == NULL) break;
				
				player->mSPing = (float)ping;

				/*if (player == mPlayer) {
					mUdpManager->mSPing = (float)ping;
					if (mUdpManager->mSPing > 1000) mUdpManager->mSPing = 1000;
				}*/

				break;
			}
			case MAPFILE: {
				int fileid = packet.ReadInt8();
				int pos;
				char data[2048];
				int datalength;
				if (fileid == 0 || fileid == 1 || fileid == 2){
					pos = packet.ReadInt32();
					datalength = packet.ReadData(data);
				}
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}
		
				if (fileid == 0 || fileid == 1 || fileid == 2){
					if (mDownloadId != fileid) {
						mDownloadAmount = 0;
						mDownloadId = fileid;
						char mapfile[128];
						if (fileid == 0) {
							sprintf(mapfile,"maps/%s/map.txt",mMapName);
							mMapFile = fopen(mapfile,"rb+");
						}
						else if (fileid == 1) {
							fclose(mMapFile);
							sprintf(mapfile,"maps/%s/tile.png",mMapName);
							mMapFile = fopen(mapfile,"rb+");
						}
						else if (fileid == 2) {
							fclose(mMapFile);
							if (datalength > 0) {
								sprintf(mapfile,"maps/%s/overview.png",mMapName);
								mMapFile = fopen(mapfile,"rb+");
							}
							else {
								mState = RECONNECTING;
								gReconnect = true;
								mParent->SetNextState(GAME_STATE_LOBBY);
								return;
							}
						}
					}

					if (pos != mDownloadAmount) break;
					//file = fopen(mapfile,"r+");

					if (mMapFile == NULL) break;

					fseek(mMapFile,pos,SEEK_SET);
					fwrite(data,1,datalength,mMapFile);
					//fclose(file);

					mDownloadAmount += datalength;

					int total = 0;
					if (fileid == 0) {
						total = mMapTextSize;
					}
					else if (fileid == 1) {
						total = mMapImageSize;
					}
					else if (fileid == 2) {
						total = mMapOverviewSize;
					}

					if (mDownloadAmount < total) {
						/*Packet sendpacket = Packet();
						sendpacket.WriteInt8(MAPFILE);
						sendpacket.WriteChar(mMapName);
						sendpacket.WriteInt8(mDownloadId);
						sendpacket.WriteInt32(mDownloadAmount);
						mUdpManager->SendReliable(sendpacket,true);
						sendpacket.Clear();	*/
					}
					else {
						if (fileid == 0) {
							Packet sendpacket = Packet();
							sendpacket.WriteInt8(MAPFILE);
							sendpacket.WriteChar(mMapName);
							sendpacket.WriteInt8(1);
							sendpacket.WriteInt32(0);
							mUdpManager->SendReliable(sendpacket,true);
							sendpacket.Clear();	
						}
						else if (fileid == 1) {
							Packet sendpacket = Packet();
							sendpacket.WriteInt8(MAPFILE);
							sendpacket.WriteChar(mMapName);
							sendpacket.WriteInt8(2);
							sendpacket.WriteInt32(0);
							mUdpManager->SendReliable(sendpacket,true);
							sendpacket.Clear();	
						}
						else if (fileid == 2) {
							fclose(mMapFile);

							mState = RECONNECTING;
							gReconnect = true;
							mParent->SetNextState(GAME_STATE_LOBBY);
							return;
						}
					}
				}
				else if (fileid == 3) {
					// map change error..
					fclose(mMapFile);

					mState = RECONNECTING;
					gReconnect = true;
					mParent->SetNextState(GAME_STATE_LOBBY);
					return;
				}

				break;
			}
			case SERVERMESSAGE: {
				char string[127];
				packet.ReadChar(string,127);
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}
				
				mHud->AddChatEvent("Server",string,SERVER,false,false);
				break;
			}
			case MESSAGE: {
				int message = packet.ReadInt8();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid);
				}
				
				if (message == 0) { //not enough money
					mHud->SetMessage("Not enough money!");
				}
				else if (message == 1) { //not in buyzone
					mHud->SetMessage("Not in buyzone!");
				}
				else if (message == 2) { //buytime over
					mHud->SetMessage("Buying time has ended!");
				}
				else if (message == 3) { //too many players
					mHud->SetMessage("Too many players for this map!");
				}
				else if (message == 4) { //flag not in base
					mHud->SetMessage("Enemy still has our flag!");
				}
				break;
			}
			case ACK: {
				int id = packet.ReadInt16();
				mUdpManager->ReceiveAck(id);
				break;
			}
			case ORDEREDACK: {
				int id = packet.ReadInt16();
				mUdpManager->ReceiveAck(id,true);
				break;
			}
			case TIMEMULTIPLIER: {
				float timeMultiplier = packet.ReadFloat();
				int ackid = packet.ReadInt16();
				if (sendack) {
					mUdpManager->SendAck(ackid,true);
				}
				if (!mUdpManager->HandleSequence(ackid,packet,startindex)) {
					break;
				}

				mTimeMultiplier = timeMultiplier;
				
				break;
			}
		}
	}
}

void GameStateOnline::ResetRound() {
	//mNumRounds++;
	mTimeMultiplier = 1.0f;

	mRoundState = FREEZETIME;
	mRoundTimer = mRoundFreezeTime;
	mRoundEndTimer = 0;
	mBuyTimer = mBuyTime;
	mFFAWinner = NULL;

	if (mGameType == CTF) {
		mNumFlags[CT] = 0;
		mNumFlags[T] = 0;
		mFlagX[CT] = mMap->mFlagSpawn[CT].x;
		mFlagY[CT] = mMap->mFlagSpawn[CT].y;
		mFlagX[T] = mMap->mFlagSpawn[T].x;
		mFlagY[T] = mMap->mFlagSpawn[T].y;
		mIsFlagHome[CT] = true;
		mIsFlagHome[T] = true;
		mFlagHolder[CT] = NULL;
		mFlagHolder[T] = NULL;
	}

	if (mPlayer->mTeam != NONE) {
		mSpec = mPlayer;
	}
		
	mGrid->ClearCells();

	for (unsigned int i=0;i<mBullets.size();i++) {
		delete mBullets[i];
	}
	mBullets.clear();
	for (unsigned int i=0;i<mGunObjects.size();i++) {
		mGunObjectsMap.erase(((GunObjectOnline*)mGunObjects[i])->mId);
		delete mGunObjects[i];
	}
	mGunObjects.clear();

	for (int i=0; i<3; i++) {
		gParticleSystems[i]->Stop(true);
	}

	for (int i=0; i<256; i++) {
		mMoves[i].time = -1.0f;
	}
	mMovesIndex = 0;
	mClock = 0.0f;
	mTime = 0.0f;

	//mMap->Reset();
	mMap->ClearDecals();

	//mPeople.clear();
	mNumRemainingCTs = mNumCTs;
	mNumRemainingTs = mNumTs;

	cross2 = false;

	for (unsigned int i=0;i<mPeople.size();i++) {
		if (mPeople[i]->mTeam == NONE) continue;

		//mPeople[i]->Reset();
	}	

	if (mGameType == TEAM) {
		if (mWinner == NONE) {
			/*if (mPlayer->mMoney < 3400) {
				mPlayer->mMoney += 1400;
				if (mPlayer->mMoney > 3400) {
					mPlayer->mMoney = 3400;
				}
			}*/
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
	}
	mWinner = NONE;
}	

void GameStateOnline::ButtonPressed(int controllerId, int controlId)
{
	switch (controlId)
	{
	case 1:
		mGuiController->SetActive(false);
		cross = true;
		break;
	case 2:
		if (mBuyMenu->mIsActive) {
			mBuyMenu->Disable();
		}
		mTeamMenu->Enable();
		mGuiController->SetActive(false);
		break;
	case 3:
		mIsAdminMenuEnabled = true;
		mGuiController->SetActive(false);
		break;
	case 4: {
		mGuiController->SetActive(false);
		mParent->SetNextState(GAME_STATE_LOBBY);
		break;
	}
	case 5:
		break;
	}
}

Person* GameStateOnline::GetPerson(int id) {
	Person* person = NULL;
	/*for (int i=0; i<mPeople.size(); i++) {
		if (((PersonOnline*)mPeople[i])->mId == id) {
			person = mPeople[i];
		}
	}*/
	std::map<int,Person*>::iterator iter = mPeopleMap.find(id);
	if (iter != mPeopleMap.end()) {
		person = iter->second;
	}
	return person;
}
GunObject* GameStateOnline::GetGunObject(int id) {
	GunObject* gunobject = NULL;
	std::map<int,GunObject*>::iterator iter = mGunObjectsMap.find(id);
	if (iter != mGunObjectsMap.end()) {
		gunobject = iter->second;
	}
	return gunobject;
}
void GameStateOnline::StopInput(float dt) {
	/*Packet sendpacket = Packet();

	if (cross2) {
		cross2 = false;
		sendpacket.WriteInt8(ENDFIRE);
		sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
		mUdpManager->SendReliable(sendpacket);
		sendpacket.Clear();
	}

	sendpacket.WriteInt8(MOVE);
	sendpacket.WriteInt8(((PersonOnline*)mPlayer)->mId);
	int facingangle = (int)(mPlayer->mFacingAngle*(255/(2*M_PI)))-128;
	sendpacket.WriteInt8(facingangle);
	sendpacket.WriteInt8(0);
	sendpacket.WriteInt8(0);
	mUdpManager->Send(sendpacket);*/
	if (mPlayer->mState != DEAD) {
		mSendMovementTimer += dt;

		if (mSendMovementTimer > 30.0f) {
			//if (mPlayer->mIsActive) {

				Packet sendpacket = Packet();
				sendpacket.WriteInt8(PLAYERMOVE);//sendpacket.WriteInt8(MOVE);
				sendpacket.WriteInt8(0);
				sendpacket.WriteInt8(0);
				int facingangle = (int)(mPlayer->mFacingAngle*(255/(2*M_PI)))-128;
				sendpacket.WriteInt8(facingangle);
				int state = mPlayer->mGunIndex;
				sendpacket.WriteInt8(state);
				sendpacket.WriteFloat(mClock);		
				mUdpManager->Send(sendpacket);

				Input input = {0,0,(float)((facingangle+128)/(255.0f/(2*M_PI)))};
				((PersonOnline*)mPlayer)->ReceiveInput(input,mTime);
				State movementState = {(int)((PersonOnline*)mPlayer)->mSX,(int)((PersonOnline*)mPlayer)->mSY,mPlayer->mSpeed,mPlayer->mAngle};
				Move move = {input,movementState,mTime};
				mMoves[mMovesIndex] = move;
				AdvanceMoves(mMovesIndex);

				mSendMovementTimer = 0.0f;
			//}
		}
	}
}

void GameStateOnline::Buy(Person* player, int index) {
	if (index == -1) {
		if (player->mGuns[PRIMARY] != NULL) {
			if (player->mGuns[PRIMARY]->mRemainingAmmo != (player->mGuns[PRIMARY]->mGun->mNumClips-1)*player->mGuns[PRIMARY]->mGun->mClip) {
				player->mGuns[PRIMARY]->mRemainingAmmo = (player->mGuns[PRIMARY]->mGun->mNumClips-1)*player->mGuns[PRIMARY]->mGun->mClip;
				gSfxManager->PlaySample(gAmmoSound, player->mX, player->mY);
			}
		}
		if (player->mGuns[SECONDARY] != NULL) {
			if (player->mGuns[SECONDARY]->mRemainingAmmo != (player->mGuns[SECONDARY]->mGun->mNumClips-1)*player->mGuns[SECONDARY]->mGun->mClip) {
				player->mGuns[SECONDARY]->mRemainingAmmo = (player->mGuns[SECONDARY]->mGun->mNumClips-1)*player->mGuns[SECONDARY]->mGun->mClip;
				gSfxManager->PlaySample(gAmmoSound, player->mX, player->mY);
			}
		}
	}
	/*else if (index == -2) {
		if (player->mGuns[SECONDARY] != NULL) {
			if (player->mGuns[SECONDARY]->mRemainingAmmo != (player->mGuns[SECONDARY]->mGun->mNumClips-1)*player->mGuns[SECONDARY]->mGun->mClip) {
				player->mGuns[SECONDARY]->mRemainingAmmo = (player->mGuns[SECONDARY]->mGun->mNumClips-1)*player->mGuns[SECONDARY]->mGun->mClip;
				gSfxManager->PlaySample(gAmmoSound, player->mX, player->mY);
			}
		}
	}*/
	else {
		if (mGuns[index].mType == PRIMARY) {
			//mPlayer->Drop(PRIMARY);
			gSfxManager->PlaySample(gPickUpSound, player->mX, player->mY);
		}
		else if (mGuns[index].mType == SECONDARY) {
			//mPlayer->Drop(SECONDARY);
			gSfxManager->PlaySample(gPickUpSound, player->mX, player->mY);
		}
		else if (mGuns[index].mType == GRENADE) {
			//mPlayer->Drop(SECONDARY);
			gSfxManager->PlaySample(gPickUpSound, player->mX, player->mY);
		}
	}
}

void GameStateOnline::Explode(float x, float y, int type) {
	if (type == FLASH) { //FLASH
		gParticleSystems[PARTICLE_FLASH]->FireAt(x,y);
		mMap->AddDecal(x,y,DECAL_EXPLOSION);
		gSfxManager->PlaySample(gFlashbangSound,x,y);
	}
	else if (type == HE) { //HE
		gParticleSystems[PARTICLE_EXPLOSION]->FireAt(x,y);
		mMap->AddDecal(x,y,DECAL_EXPLOSION);
		gSfxManager->PlaySample(gHEGrenadeSounds[rand()%3],x,y);
		float dx = mCamera->GetX()-x;
		float dy = mCamera->GetY()-y;
		float dist = dx*dx+dy*dy;
		if (dist < 1000.0f) dist = 1000.0f;
		mCamera->Shake(100*1000.0f/dist,500);

		for (unsigned int i=0; i<mPeople.size(); i++) {
			if (mPeople[i]->mState == DEAD) continue;

			//if (mFriendlyFire == OFF && ((PersonOnline*)mPeople[i])->mId != id && mPeople[i]->mTeam == team) continue;

			if (!mGrid->LineOfSight(x,y,mPeople[i]->mX,mPeople[i]->mY)) continue;

			float distance = sqrtf((x-mPeople[i]->mX)*(x-mPeople[i]->mX)+(y-mPeople[i]->mY)*(y-mPeople[i]->mY));
			if (distance < 40.0f) {
				distance = 40.0f;
			}
			else if (distance > 200.0f) {
				continue;
			}

			gParticleEngine->GenerateParticles(BLOOD,mPeople[i]->mX,mPeople[i]->mY,5);
			mMap->AddDecal(mPeople[i]->mX,mPeople[i]->mY,DECAL_BLOOD);
			gSfxManager->PlaySample(gHitSounds[rand()%3],mPeople[i]->mX,mPeople[i]->mY);
		}
	}
	else if (type == SMOKE) { //SMOKE
		gParticleSystems[PARTICLE_SMOKE]->FireAt(x,y);
		gSfxManager->PlaySample(gSmokeGrenadeSound,x,y);
	}
}

void GameStateOnline::AdvanceMoves(int &index) {
	index++;
	if (index >= 256) {
		index -= 256;
	}
}

void GameStateOnline::ClientCorrection(State state, float time) {
	PersonOnline* player = (PersonOnline*)mPlayer;

	int index = -1;
	for (int i=0; i<256; i++) {
		if (mMoves[i].time < 0.0f) continue;
		if ((int)mMoves[i].time == (int)time) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		player->mCurrentTime = time;
		player->mSX = state.x;
		player->mSY = state.y;
		player->mOldSX = state.x;
		player->mOldSY = state.y;
		/*player->mX = state.x;
		player->mY = state.y;
		player->mOldX = state.x;
		player->mOldY = state.y;*/
		player->mSpeed = state.speed;
		player->mAngle = state.angle;

		player->mLastSX = state.x;
		player->mLastSY = state.y;
		return;
	}

	player->mLastSX = state.x;
	player->mLastSY = state.y;

	//player->mOldSX = state.x;
	//player->mOldSY = state.y;

	if (fabs(mMoves[index].state.x-state.x) > 5 || fabs(mMoves[index].state.y-state.y) > 5) {// rewind and apply correction
		player->mCurrentTime = time;

		//float tempx = player->mOldSX;
		//float tempy = player->mOldSY;
		player->mSX = state.x;
		player->mSY = state.y;

		player->mSpeed = state.speed;
		player->mAngle = state.angle;

		Input input = mMoves[index].input;
		mMoves[index].state = state;

		//AdvanceMoves(index);          // discard corrected move

		while (true) {
			AdvanceMoves(index);
			if (index == mMovesIndex) break;
			if (mMoves[index].time < 0.0f) break;

			input = mMoves[index].input;
			player->ReceiveInput(input,mMoves[index].time);

			State newstate = {player->mSX,player->mSY,player->mSpeed,player->mAngle};
			mMoves[index].state = newstate;
			/*player->mOldX = player->mX;
			player->mOldY = player->mY;
			player->mX = player->mSX;
			player->mY = player->mSY;*/
		}

		//nvm?
		//on correction, keep the safe old position
		player->mOldSX = state.x;
		player->mOldSY = state.y;
		//player->mOldSX = tempx;
		//player->mOldSY = tempy;
	}
}
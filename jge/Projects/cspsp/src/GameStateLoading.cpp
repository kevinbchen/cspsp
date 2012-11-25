#include "GameStateLoading.h"

GameStateLoading::GameStateLoading(GameApp* parent): GameState(parent) {}

GameStateLoading::~GameStateLoading() {
}


void GameStateLoading::Start()
{
	mStage = 0;
	mRenderer->ResetPrivateVRAM();

	mRenderer->EnableVSync(true);

	JTexture* bgTexture = mRenderer->LoadTexture("gfx/bg.png");					// load bg
	gBgQuad = new JQuad(bgTexture, 0.0f, 0.0f, 480.0f, 272.0f);
	JTexture* logoTexture = mRenderer->LoadTexture("gfx/logo.png");				// load logo
	gLogoQuad = new JQuad(logoTexture, 0.0f, 0.0f, 256.0f, 128.0f);
}


void GameStateLoading::End()
{
	mRenderer->EnableVSync(false);

}


void GameStateLoading::Update(float dt)
{
	/*if (mStage == 1)
	{
		mParent->LoadGameStates();
	}
	else if (mStage == 2)
	{
		mParent->SetNextState(GAME_STATE_MENU);
	}

	mStage++;*/

	mStage = Load(mStage);
	if (mStage == 8) {
		mParent->SetNextState(GAME_STATE_MENU);
	}
}


void GameStateLoading::Render()
{
	mRenderer->ClearScreen(ARGB(255,0,0,0));

	float p = ((float)mStage/8);

	mRenderer->RenderQuad(gBgQuad,0,0);
	mRenderer->RenderQuad(gLogoQuad,0,0);
	mRenderer->FillRect(SCREEN_WIDTH*p,0,SCREEN_WIDTH*(1-p),SCREEN_HEIGHT,ARGB(200,0,0,0));

	/*mRenderer->FillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,ARGB(255-(int)(p*255),0,0,0));
	mRenderer->FillRect(SCREEN_WIDTH_2-200,SCREEN_HEIGHT_2-4,400,8,ARGB(255,100,100,100));
	mRenderer->FillRect(SCREEN_WIDTH_2-200,SCREEN_HEIGHT_2-4,400*p,8,ARGB(255,255,255,255));*/


}

int GameStateLoading::Load(int stage) {
	switch (stage) {
		case 0: {
			/*gPlayersQuads = new JQuad*[mParent->mPlayersTexture->mTexWidth/32];
			for (int i=0;i<mParent->mPlayersTexture->mTexWidth/32;i++) {
				gPlayersQuads[i] = new JQuad(mParent->mPlayersTexture,i*32,0,32,32);
				gPlayersQuads[i]->SetHotSpot(16,10);
			}

			gPlayersDeadQuads = new JQuad*[mParent->mPlayersDeadTexture->mTexWidth/32];
			for (int i=0;i<mParent->mPlayersDeadTexture->mTexWidth/32;i++) {
				gPlayersDeadQuads[i] = new JQuad(mParent->mPlayersDeadTexture,i*32,0,29,47);
				gPlayersDeadQuads[i]->SetHotSpot(14,23);
			}*/
			JTexture* playersTexture = mRenderer->LoadTexture("gfx/players.png", true);
			JTexture* playersDeadTexture = mRenderer->LoadTexture("gfx/playersdead.png", true);
			
			for (int i=0; i<2; i++) {
				for (int j=0; j<4; j++) {
					/*for (int k=0; k<5; k++) {
						gPlayersQuads[i][j][k] = new JQuad(playersTexture,(j+i*4)*32,k*32,32,32);
						gPlayersQuads[i][j][k]->SetHotSpot(16,10);
					}*/
					int offsetX = (j+i*4)*32;
					gPlayersQuads[i][j][BODY] = new JQuad(playersTexture,offsetX+0,0,32,16);
					gPlayersQuads[i][j][BODY]->SetHotSpot(16,8);

					gPlayersQuads[i][j][HEAD] = new JQuad(playersTexture,offsetX+16,16,16,16);
					gPlayersQuads[i][j][HEAD]->SetHotSpot(8,7);

					gPlayersQuads[i][j][RIGHTARM] = new JQuad(playersTexture,offsetX+8,16,8,16);
					gPlayersQuads[i][j][RIGHTARM]->SetHotSpot(4.0f,4.0f);

					gPlayersQuads[i][j][RIGHTHAND] = new JQuad(playersTexture,offsetX+0,16,8,16);
					gPlayersQuads[i][j][RIGHTHAND]->SetHotSpot(4.0f,3.0f);

					gPlayersQuads[i][j][LEFTARM] = new JQuad(playersTexture,offsetX+8,16,8,16);
					gPlayersQuads[i][j][LEFTARM]->SetHotSpot(4.0f,4.0f);
					gPlayersQuads[i][j][LEFTARM]->SetHFlip(true);

					gPlayersQuads[i][j][LEFTHAND] = new JQuad(playersTexture,offsetX+0,16,8,16);
					gPlayersQuads[i][j][LEFTHAND]->SetHotSpot(4.0f,3.0f);
					gPlayersQuads[i][j][LEFTHAND]->SetHFlip(true);

					gPlayersQuads[i][j][LEGS] = new JQuad(playersTexture,offsetX+0,32,32,32);
					gPlayersQuads[i][j][LEGS]->SetHotSpot(16,16);

					gPlayersDeadQuads[i][j] = new JQuad(playersDeadTexture,(j+i*4)*32,64,29,47);
					gPlayersDeadQuads[i][j]->SetHotSpot(14,23);
				}
			}

			JTexture* radarTexture = mRenderer->LoadTexture("gfx/radar.png", true);
			gRadarQuad = new JQuad(radarTexture,0,0,64,64);
			//gRadarQuad->SetColor(ARGB(175,255,255,255));

			JTexture* buyzoneTexture = mRenderer->LoadTexture("gfx/buyzone.png", true);
			gBuyZoneQuad = new JQuad(buyzoneTexture,0,0,16,16);
			gBuyZoneQuad->SetColor(ARGB(175,255,255,255));

			JTexture* decalTexture = mRenderer->LoadTexture("gfx/decals.png",true);
			for (int i=0; i<5; i++) {
				gDecalQuads[i] = new JQuad(decalTexture,i*32.0f,0.0f,32.0f,32.0f);
				gDecalQuads[i]->SetHotSpot(16.0f,16.0f);
				if (i == 0) { //explosion
					gDecalQuads[i]->SetColor(ARGB(255,0,0,0));
				}
				else { //blood
					gDecalQuads[i]->SetColor(ARGB(255,200,0,0));
				}
			}

			gIconTexture = mRenderer->CreateTexture(10,10);
			gIconQuad = new JQuad(gIconTexture,0,0,10,10);
			break;
		}
		case 1: {

			JTexture* gunsTexture = mRenderer->LoadTexture("gfx/guns.png", true);
			JTexture* gunsgroundTexture = mRenderer->LoadTexture("gfx/gunsground.png", true);
			int k = 0;
			gGunHandQuads = new JQuad*[(gunsTexture->mTexWidth/32)*(gunsTexture->mTexHeight/32)];
			gGunGroundQuads = new JQuad*[(gunsgroundTexture->mTexWidth/32)*(gunsgroundTexture->mTexHeight/32)];
			for (int i=0;i<(gunsTexture->mTexHeight)/32;i++) {
				for (int j=0;j<(gunsTexture->mTexWidth)/32;j++) {
					gGunHandQuads[k] = new JQuad(gunsTexture,j*32.0f,i*32.0f,32.0f,32.0f);
					gGunHandQuads[k]->SetHotSpot(16.0f,8.0f);
					gGunGroundQuads[k] = new JQuad(gunsgroundTexture,j*32.0f,i*32.0f,32.0f,32.0f);
					gGunGroundQuads[k]->SetHotSpot(16.0f,16.0f);
					k++;
				}
			}

			JTexture* muzzleFlashTexture = mRenderer->LoadTexture("gfx/muzzleflash.png", true);
			for (int i=0; i<3; i++) {
				gMuzzleFlashQuads[i] = new JQuad(muzzleFlashTexture,i*32,0,32,32);
				gMuzzleFlashQuads[i]->SetHotSpot(16.0f,-16.0f);
			}

			JTexture* healthTexture = mRenderer->LoadTexture("gfx/health.png", true);
			gHealthBorderQuad = new JQuad(healthTexture,0,0,48,48);
			gHealthFillQuad = new JQuad(healthTexture,48,0,48,48);

			JTexture* ammobarTexture = mRenderer->LoadTexture("gfx/ammo.png", true);
			gAmmoBarQuad = new JQuad(ammobarTexture,0,0,128,64);
			gAmmoBarQuad->SetHotSpot(128,64);

			JTexture* damageIndicatorTexture = mRenderer->LoadTexture("gfx/damageindicator.png", true);
			gDamageIndicator = new JQuad(damageIndicatorTexture,0,0,128,32);
			gDamageIndicator->SetHotSpot(64,125);
			gDamageIndicator->SetColor(ARGB(128,255,255,255));

			JTexture* scoreIconsTexture = mRenderer->LoadTexture("gfx/scoreicons.png", true);
			for (int i=0; i<4; i++) {
				gScoreIconQuads[i] = new JQuad(scoreIconsTexture,16*i+3,2,10,13);
				gScoreIconQuads[i]->SetHotSpot(5,6);
			}

			JTexture* ctfTexture = mRenderer->LoadTexture("gfx/ctf.png", true);
			gFlagQuad = new JQuad(ctfTexture,0,0,32,32);
			gFlagQuad->SetHotSpot(16,16);
			gFlagHomeQuad = new JQuad(ctfTexture,32,0,32,32);
			gFlagHomeQuad->SetHotSpot(16,16);
			gFlagArrowQuad = new JQuad(ctfTexture,65,0,30,16);
			gFlagArrowQuad->SetHotSpot(15,22);
			gFlagRadarQuad = new JQuad(ctfTexture,96,0,8,8);
			gFlagRadarQuad->SetHotSpot(4,4);
			gHomeRadarQuad = new JQuad(ctfTexture,96+8,0,8,8);
			gHomeRadarQuad->SetHotSpot(4,4);
			break;
		}
		case 2: {
			gDryFireRifleSound = mSoundSystem->LoadSample("sfx/dryfire_rifle.wav");
			gDryFirePistolSound = mSoundSystem->LoadSample("sfx/dryfire_pistol.wav");
			gDeploySound = mSoundSystem->LoadSample("sfx/deploy.wav");
			gPickUpSound = mSoundSystem->LoadSample("sfx/pickup.wav");
			gAmmoSound = mSoundSystem->LoadSample("sfx/ammo.wav");
			gPinPullSound = mSoundSystem->LoadSample("sfx/pinpull.wav");
			gWalkSounds[0] = mSoundSystem->LoadSample("sfx/walk1.wav");
			gWalkSounds[1] = mSoundSystem->LoadSample("sfx/walk2.wav");
			gRicochetSounds[0] = mSoundSystem->LoadSample("sfx/ricochet1.wav");
			gRicochetSounds[1] = mSoundSystem->LoadSample("sfx/ricochet2.wav");
			gRicochetSounds[2] = mSoundSystem->LoadSample("sfx/ricochet3.wav");
			gRicochetSounds[3] = mSoundSystem->LoadSample("sfx/ricochet4.wav");
			gHitSounds[0] = mSoundSystem->LoadSample("sfx/hit1.wav");
			gHitSounds[1] = mSoundSystem->LoadSample("sfx/hit2.wav");
			gHitSounds[2] = mSoundSystem->LoadSample("sfx/hit3.wav");
			break;
		}	
		case 3: {
			gKnifeHitSound = mSoundSystem->LoadSample("sfx/knifehit.wav");
			gDieSounds[0] = mSoundSystem->LoadSample("sfx/die1.wav");
			gDieSounds[1] = mSoundSystem->LoadSample("sfx/die2.wav");
			gDieSounds[2] = mSoundSystem->LoadSample("sfx/die3.wav");
			gRoundEndSounds[T] = mSoundSystem->LoadSample("sfx/twin.wav");
			gRoundEndSounds[CT] = mSoundSystem->LoadSample("sfx/ctwin.wav");
			gRoundEndSounds[TIE] = mSoundSystem->LoadSample("sfx/rounddraw.wav");
			gHEGrenadeSounds[0] = mSoundSystem->LoadSample("sfx/hegrenade1.wav");
			gHEGrenadeSounds[1] = mSoundSystem->LoadSample("sfx/hegrenade2.wav");
			gHEGrenadeSounds[2] = mSoundSystem->LoadSample("sfx/hegrenade3.wav");
			gFlashbangSound = mSoundSystem->LoadSample("sfx/flashbang.wav");
			gSmokeGrenadeSound = mSoundSystem->LoadSample("sfx/smokegrenade.wav");
			gFireInTheHoleSound = mSoundSystem->LoadSample("sfx/fireinthehole.wav");
			gGrenadeBounceSound = mSoundSystem->LoadSample("sfx/grenadebounce.wav");
			gHitIndicatorSound = mSoundSystem->LoadSample("sfx/hitindicator.wav");
			break;
		}

		case 4: {
			gKeyFrameAnims[ANIM_PRIMARY] = Animation::LoadKeyFrames("PRIMARY");
			gKeyFrameAnims[ANIM_PRIMARY_FIRE] = Animation::LoadKeyFrames("PRIMARY_FIRE");
			gKeyFrameAnims[ANIM_SECONDARY] = Animation::LoadKeyFrames("SECONDARY");
			gKeyFrameAnims[ANIM_SECONDARY_FIRE] = Animation::LoadKeyFrames("SECONDARY_FIRE");
			gKeyFrameAnims[ANIM_KNIFE] = Animation::LoadKeyFrames("KNIFE");
			gKeyFrameAnims[ANIM_KNIFE_SLASH] = Animation::LoadKeyFrames("KNIFE_SLASH");
			gKeyFrameAnims[ANIM_GRENADE] = Animation::LoadKeyFrames("GRENADE");
			gKeyFrameAnims[ANIM_GRENADE_PULLBACK] = Animation::LoadKeyFrames("GRENADE_PULLBACK");
			gKeyFrameAnims[ANIM_PRIMARY_RELOAD] = Animation::LoadKeyFrames("PRIMARY_RELOAD");
			gKeyFrameAnims[ANIM_SECONDARY_RELOAD] = Animation::LoadKeyFrames("SECONDARY_RELOAD");
			break;
		}		
		case 5: {
			int mNumGuns = 0;
			FILE *file;
			file = fopen("data/guns.txt", "r"); 
			char line[1024]; 
			fgets(line,1024,file);  // Get one line from your file into a buffer 
			sscanf(line,"%d",&mNumGuns);  // read mCols and mRows of your map from the file 
			//mGuns = new Gun*[mNumGuns];
			char* s = line; 

			for (int i=0;i<mNumGuns;i++) { 
				if (!fgets(line,1024,file)) break; // read error, you should handle this properly 
				s = line; // This was what the problem was! 
				Gun gun;
				sscanf(s,"%d %d %d %f %d %d %d %f %f %f %d %d %s",&gun.mId,&gun.mDamage,&gun.mDelay,&gun.mSpread,&gun.mClip,&gun.mNumClips,&gun.mReloadDelay,&gun.mSpeed,&gun.mBulletSpeed,&gun.mViewAngle,&gun.mCost,&gun.mType,gun.mName);
				gun.mHandQuad = gGunHandQuads[gun.mId];
				gun.mGroundQuad = gGunGroundQuads[gun.mId];
				gGuns[i] = gun;

				//strcpy(gGuns[i].mName,name);
				char buffer[128];

				if (gGuns[i].mId == 25 || gGuns[i].mId == 26 || gGuns[i].mId == 27) {
					gGuns[i].mFireSound = gPinPullSound;
				}
				else {
					sprintf(buffer,"sfx/%s.wav",gGuns[i].mName);
					gGuns[i].mFireSound = mSoundSystem->LoadSample(buffer);
				}

				if (gGuns[i].mId != 0 && gGuns[i].mId != 25 && gGuns[i].mId != 26 && gGuns[i].mId != 27) {
					sprintf(buffer,"sfx/%sreload.wav",gGuns[i].mName);
					gGuns[i].mReloadSound = mSoundSystem->LoadSample(buffer);
				}

				//mGuns[i] = new Gun(gGunHandQuads[a],gGunGroundQuads[a],a,c,b,d,e,f,g,h,l,m,name);
				if (gGuns[i].mType == PRIMARY) {
					gGuns[i].mDryFireSound = gDryFireRifleSound;
				}
				else if (gGuns[i].mType == SECONDARY) {
					gGuns[i].mDryFireSound = gDryFirePistolSound;
				}

				int clip = gGuns[i].mClip;
				int space = 120-clip;
				int w = 0; 
				if (clip != 0) {
					w = (int)floorf((float)space/clip);
				}
				if (w < 0) w = 0;
				gGuns[i].mAmmoBarWidth = w;

				//mGuns[i] = gGuns[i];
			}
			fclose(file);

			break;
		}
		case 6: {
			//gTTFont = new JTTFont();
			//gTTFont->Load("data/tahomabd.ttf",20);
			//gTTFont->PreCacheASCII();

			gFont = new JLBFont("data/f3", 16, true);
			gFont->SetBase(0);
			//gFontBackdrop = new JLBFont("data/f3backdrop", 16, true);
			//gFontBackdrop->SetBase(0);
			//gHudFont = new JLBFont("data/hud", 17, true);
			//gHudFont->SetBase(0);
			//gHudFont->SetBlendMode(BLEND_COLORADD);

			JTexture* particlesTexture = mRenderer->LoadTexture("gfx/particles.png", true);

			JQuad* quad = new JQuad(particlesTexture,32,0,32,32);
			quad->SetHotSpot(16.0f, 16.0f);
			gParticleEngine = new ParticleEngine(100);
			gParticleEngine->SetQuad(quad);

			quad = new JQuad(particlesTexture,32,0,32,32);
			quad->SetHotSpot(16.0f, 16.0f);
			gParticleSystems[PARTICLE_EXPLOSION] = new hgeParticleSystem("gfx/explosion.psi",quad);
			quad = new JQuad(particlesTexture,64,64,32,32);
			quad->SetHotSpot(16.0f, 16.0f);
			gParticleSystems[PARTICLE_FLASH] = new hgeParticleSystem("gfx/flash.psi",quad);
			quad = new JQuad(particlesTexture,0,96,32,32);
			quad->SetHotSpot(16.0f, 16.0f);
			gParticleSystems[PARTICLE_SMOKE] = new hgeParticleSystem("gfx/smoke.psi",quad);

			
			gSfxManager = new SfxManager();
			//mParticles = new JParticleSystem(500);
			//mParticles->SetQuad(mParticlesQuad);
			gDanzeff = new Danzeff();

			gSocket = new Socket();
			gHttpManager = new HttpManager();

			gLastKey = 0;
			gKeyRepeatDelay = 0;
			break;
		}

		case 7: {
			mParent->LoadGameStates();
			break;
		}
	}

	return ++stage;

}

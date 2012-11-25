#include "GameStateSplash.h"
GameStateSplash::GameStateSplash(GameApp* parent): GameState(parent) {}
GameStateSplash::~GameStateSplash() {}

void GameStateSplash::Create() 
{
	//mSplashTex = mRenderer->LoadTexture("gfx/splash.png", true);
	//mSplashQuad = new JQuad(mSplashTex, 0.0f, 0.0f, 480.0f, 272.0f);
}

void GameStateSplash::Destroy() 
{
	//SAFE_DELETE(mSplashQuad);

	//SAFE_DELETE(mSplashTex);
}


void GameStateSplash::Start()
{
	mMode = 0;
	mAlpha = 0.0f;
	
	mRenderer->EnableVSync(true);
}


void GameStateSplash::End()
{
	mRenderer->EnableVSync(false);
}


void GameStateSplash::Update(float dt)
{
	mParent->SetNextState(GAME_STATE_LOADING);
	return;
	/**if (mEngine->GetButtonState(PSP_CTRL_CROSS) || mEngine->GetButtonState(PSP_CTRL_CIRCLE) )		// skip everything...
	{
		mParent->SetNextState(GAME_STATE_LOADING);
		return;
	}**/

	if (mMode==0)
	{
		mAlpha += FADING_SPEED*dt;
		if (mAlpha > 255.0f)
		{
			mAlpha = 255.0f;
			mMode = 1;
			mTimer = 0.0f;
		}
	}
	else if (mMode==1)
	{
		mParent->LoadGameStates();
		mMode = 2;
		/*mTimer += dt;
		if (mTimer > SPLASH_DELAY)
			mMode = 2;*/
	}
	else if (mMode==2)
	{
		mAlpha -= FADING_SPEED*dt;
		if (mAlpha < 0.0f)
		{
			mAlpha = 0.0f;
			mParent->SetNextState(GAME_STATE_LOADING);
		}
		
	}
}


void GameStateSplash::Render() 
{
	return;
	mRenderer->ClearScreen(ARGB(0,0,0,0));

	int alpha = (int)mAlpha;
	
	//mSplashQuad->SetColor(ARGB(alpha,255,255,255));
	//float x, y, width, height;
	//mSplashQuad->GetTextureRect(&x, &y, &width, &height);
	//mRenderer->RenderQuad(mSplashQuad, 0, 0);

//		pspDebugScreenSetXY(2, 0);
//		pspDebugScreenPrintf("fps:%4.2f %4.2f %d", mParent->Timer_GetFPS(), mAlpha, mMode);

}


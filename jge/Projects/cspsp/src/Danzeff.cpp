
#include "Danzeff.h"
#include "Globals.h"

JRenderer* Danzeff::mRenderer = NULL;
JGE* Danzeff::mEngine = NULL;

//------------------------------------------------------------------------------------------------
Danzeff::Danzeff()
{
	mString = "";
	mIsActive = false;
	cross = true;

	mEngine = JGE::GetInstance();
	mRenderer = JRenderer::GetInstance();

	holding = false;
	dirty = true;
	shifted = false;
	mode = 0;
	initialized = false;
	selected_x = 1;
	selected_y = 1;

	int a;
	for (a = 0; a < guiStringsSize; a++)
	{
		keyBits[a] = mRenderer->LoadTexture(guiStrings[a]);
		if (keyBits[a] == NULL)
		{
			//ERROR! out of memory.
			//free all previously created surfaces and set initialized to false
			int b;
			for (b = 0; b < a; b++)
			{
				SAFE_DELETE(keyBits[b]);
				keyBits[b] = NULL;
			}
			initialized = false;
			return;
		}
	}
}

//------------------------------------------------------------------------------------------------
Danzeff::~Danzeff()
{

}

//------------------------------------------------------------------------------------------------
void Danzeff::Enable()
{
	mIsActive = true;	
	cross = true;
	mString = "";
}

//------------------------------------------------------------------------------------------------
void Danzeff::Disable()
{
	mIsActive = false;	
}

//------------------------------------------------------------------------------------------------
void Danzeff::Update(float dt)
{
	int c = GetInput(dt);

	if (c < 5) {
	}
	else if (c == 8) {
		if (mString.length() > 0) {
			mString.erase(mString.length()-1,1);
		}
	}
	else {
		mString += (char)c;
	}
}

//------------------------------------------------------------------------------------------------
int Danzeff::GetInput(float dt)
{
	//Work out where the analog stick is selecting
	int x = 1;
	int y = 1;
	if (mEngine->GetAnalogX() < 85)     x -= 1;
	else if (mEngine->GetAnalogX() > 170) x += 1;

	if (mEngine->GetAnalogY() < 85)     y -= 1;
	else if (mEngine->GetAnalogY() > 170) y += 1;

	if (selected_x != x || selected_y != y) //If they've moved, update dirty
	{
		dirty = true;
		selected_x = x;
		selected_y = y;
	}
	//if they are changing shift then that makes it dirty too
	if ((!shifted && (mEngine->GetButtonState(PSP_CTRL_RTRIGGER))) || (shifted && !(mEngine->GetButtonState(PSP_CTRL_RTRIGGER))))
		dirty = true;

	unsigned int pressed = 0; //character they have entered, 0 as that means 'nothing'
	shifted = (mEngine->GetButtonState(PSP_CTRL_RTRIGGER))?true:false;

	if (!holding)
	{
		if (mEngine->GetButtonState(PSP_CTRL_CROSS) ||  mEngine->GetButtonState(PSP_CTRL_CIRCLE) || mEngine->GetButtonState(PSP_CTRL_TRIANGLE) || mEngine->GetButtonState(PSP_CTRL_SQUARE)) //pressing a char select button
		{
			int innerChoice = -1;
			if (mEngine->GetButtonState(PSP_CTRL_TRIANGLE) && !cross) {
				if (KeyRepeated(PSP_CTRL_TRIANGLE,dt)) {
					innerChoice = 0;
				}
			}
			else if (mEngine->GetButtonState(PSP_CTRL_SQUARE) && !cross) {
				if (KeyRepeated(PSP_CTRL_SQUARE,dt)) {
					innerChoice = 1;
				}
			}
			else if (mEngine->GetButtonState(PSP_CTRL_CROSS) && !cross) {
				if (KeyRepeated(PSP_CTRL_CROSS,dt)) {
					innerChoice = 2;
				}
			}
			else if (mEngine->GetButtonState(PSP_CTRL_CIRCLE) && !cross) {
				if (KeyRepeated(PSP_CTRL_CIRCLE,dt)) {
					innerChoice = 3;
				}
			}

			//Now grab the value out of the array
			if (innerChoice != -1) {
				pressed = modeChar[ mode*2 + shifted][y][x][innerChoice];
			}
		}
		else if (mEngine->GetButtonClick(PSP_CTRL_LTRIGGER)) //toggle mode
		{
			if (KeyRepeated(PSP_CTRL_SQUARE,dt)) {
				dirty = true;
				mode++;
				mode %= MODE_COUNT;
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_DOWN))
		{
			if (KeyRepeated(PSP_CTRL_DOWN,dt)) {
				//pressed = '\n';
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_UP))
		{
			if (KeyRepeated(PSP_CTRL_UP,dt)) {
				pressed = 8; //backspace
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_LEFT))
		{
			if (KeyRepeated(PSP_CTRL_LEFT,dt)) {
				pressed = DANZEFF_LEFT; //LEFT
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_RIGHT))
		{
			if (KeyRepeated(PSP_CTRL_RIGHT,dt)) {
				pressed = DANZEFF_RIGHT; //RIGHT
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_SELECT))
		{
			if (KeyRepeated(PSP_CTRL_SELECT,dt)) {
				pressed = DANZEFF_SELECT; //SELECT
			}
		}
		else if (mEngine->GetButtonState(PSP_CTRL_START))
		{
			if (KeyRepeated(PSP_CTRL_START,dt)) {
				pressed = DANZEFF_START; //START
			}
		}
		else {
			cross = false;
			gLastKey = 0;
		}
	}

	//holding = !mEngine->GetButtonState(PSP_CTRL_RTRIGGER); //RTRIGGER doesn't set holding

	return pressed;
}

//------------------------------------------------------------------------------------------------
void Danzeff::Render(float x, float y)
{
	//printf("Drawing Keyboard at %i,%i\n", selected_x, selected_y);
	dirty = false;

	///Draw the background for the selected keyboard either transparent or opaque
	///this is the whole background image, not including the special highlighted area
	//if center is selected then draw the whole thing opaque
	int shiftint = 0;
	if (shifted) {
		shiftint = 1;
	}
	else {
		shiftint = 0;
	}

	JQuad* quad = NULL;
	if (selected_x == 1 && selected_y == 1) {

		quad = new JQuad(keyBits[4*mode + shiftint*2],0.0f,0.0f,150.0f,150.0f);
		mRenderer->RenderQuad(quad,x,y);
	} 
	else {
		quad = new JQuad(keyBits[4*mode + shiftint*2],0.0f,0.0f,150.0f,150.0f);
		quad->SetColor(ARGB(128,255,255,255));
		mRenderer->RenderQuad(quad,x,y);
	}
	if (quad) {
		delete quad;
	}

	DrawOffset(keyBits[4*mode + shiftint*2 + 1],x,y,selected_x*43,selected_y*43,selected_x*64,selected_y*64,64,64);

	mRenderer->FillRect(x,y-27.0f,150.0f,27.0f,ARGB(200,50,50,50));
	gFont->SetBase(0);
	gFont->SetColor(ARGB(255,255,255,255));
	gFont->SetScale(0.7f);
	gFont->DrawString("[ANALOG + XO^[]] Type",x+75.0f,y-25.0f,JGETEXT_CENTER);
	gFont->DrawString("[L] Toggle     [R] Shift",x+75.0f,y-12.0f,JGETEXT_CENTER);
}

//------------------------------------------------------------------------------------------------
void Danzeff::DrawOffset(JTexture* tex, int x, int y, int screenX, int screenY, int offsetX, int offsetY, int intWidth, int intHeight)
{
	JQuad* quad = NULL;

	quad = new JQuad(tex,offsetX,offsetY,intWidth,intHeight);

	mRenderer->RenderQuad(quad,x+screenX,y+screenY);

	if (quad) delete quad;
}

//------------------------------------------------------------------------------------------------
/*bool Danzeff::KeyRepeated(u32 key, float dt)
{
	
	bool doKey = false;
	if (mLastKey != key)
	{
		mLastKey = key;
		doKey = true;
		mKeyRepeatDelay = INITIAL_DELAY;
	}
	else
	{
		mKeyRepeatDelay -= dt;
		if (mKeyRepeatDelay <= 0.0f)
		{
			mKeyRepeatDelay = REPEAT_DELAY;
			doKey = true;
		}
	}

	return doKey;
}*/
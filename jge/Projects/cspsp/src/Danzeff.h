#ifndef INCLUDED_KEYBOARDS_DANZEFF_H
#define INCLUDED_KEYBOARDS_DANZEFF_H

//danzeff is BSD licensed, if you do make a new renderer then please share it back and I can add it
//to the original distribution.

//Set which renderer target to build for
/* #define DANZEFF_SDL */
/* #define DANZEFF_SCEGU */

#include "JGE.h"
#include "JRenderer.h"
#include <string>

#define DANZEFF_LEFT   1
#define DANZEFF_RIGHT  2
#define DANZEFF_SELECT 3
#define DANZEFF_START  4

#define guiStringsSize 8 /* size of guistrings array */
#define PICS_BASEDIR "gfx/danzeff/"
#define MODE_COUNT 2

#define INITIAL_DELAY 500
#define REPEAT_DELAY 50

static char *guiStrings[] =
{
	PICS_BASEDIR "keys.png", PICS_BASEDIR "keys_s.png",
	PICS_BASEDIR "keys_c.png", PICS_BASEDIR "keys_s_c.png",
	PICS_BASEDIR "nums.png", PICS_BASEDIR "nums_s.png",
	PICS_BASEDIR "nums_c.png", PICS_BASEDIR "nums_s_c.png"
};

static char modeChar[MODE_COUNT*2][3][3][5] =
{
	{	//standard letters
		{ ",abc",  ".def","!ghi" },
		{ "-jkl","\010m n", "?opq" },
		{ "(rst",  ":uvw",")xyz" }
	},

	{	//capital letters
		{ "^ABC",  "@DEF","*GHI" },
		{ "_JKL","\010M N", "\"OPQ" },
		{ "=RST",  ";UVW","/XYZ" }
	},

	{	//numbers
		{ "\0\0\0001","\0\0\0002","\0\0\0003" },
		{ "\0\0\0004",  "\010\0 5","\0\0\0006" },
		{ "\0\0\0007","\0\0\0008", "\0\00009" }
	},

	{	//special characters
		{ "'(.)",  "\"<'>","-[_]" },
		{ "!{?}","\010\0 \0", "+\\=/" },
		{ ":@;#",  "~$`%","*^|&" }
	}
};

class Danzeff
{
private:
	static JGE* mEngine;
	static JRenderer* mRenderer;
	bool holding;     //user is holding a button
	bool dirty;        //keyboard needs redrawing
	bool shifted;     //user is holding shift
	int mode;             //charset selected. (0 - letters or 1 - numbers)
	bool initialized; //keyboard is initialized

	//Position on the 3-3 grid the user has selected (range 0-2)
	int selected_x;
	int selected_y;
	
	JTexture* keyBits[guiStringsSize];

	int GetInput(float dt);
	void DrawOffset(JTexture* tex, int x, int y, int screenX, int screenY, int offsetX, int offsetY, int intWidth, int intHeight);

	//u32 mLastKey;
	//float mKeyRepeatDelay;
	//bool KeyRepeated(u32 key, float dt);

protected:

public:
	std::string mString;
	bool mIsActive;
	bool cross;

	Danzeff();
	~Danzeff();

	bool IsInitialized();
	bool Dirty();

	void Enable();
	void Disable();

	void ClearString();

	void Update(float dt);
	void Render(float x, float y);

};
#endif

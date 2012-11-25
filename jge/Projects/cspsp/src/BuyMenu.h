#ifndef _BUYMENU_H_
#define _BUYMENU_H_

#include "JGE.h"
#include "JRenderer.h"
#include "JGui.h"
#include "MenuItem.h"
#include "Person.h"
#include "GunObject.h"
#include "SfxManager.h"
#include "Menu.h"

#define MAIN 0
#define PISTOLS 1
#define SHOTGUNS 2
#define SMG 3
#define RIFLES 4
#define MACHINEGUNS 5
#define EQUIPMENT 6
#define PRIMARYAMMO 7
#define SECONDARYAMMO 8


//------------------------------------------------------------------------------------------------
class BuyMenu
{
private:
	static JGE* mEngine;
	static JRenderer* mRenderer;

	Category mCategories[2][EQUIPMENT+1];	
	int mCategoryIndex;
	int mSelectedIndex;

	Person* mPlayer;
	int mChoice;
	Gun mGuns[28];

	float angle;

protected:

public:
	bool mIsActive;
	bool mIsSelected;
	bool* cross;
	JSample* mPickUpSound;
	bool mIsOldStyle;

	BuyMenu(Person* player, Gun guns[]);
	~BuyMenu();

	void Update(float dt);
	void Render();
	void Enable();
	void Disable();
	int GetChoice();
	//virtual void ButtonPressed(int controllerId, int controlId);
};

#endif

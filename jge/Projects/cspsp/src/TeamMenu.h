#ifndef _TEAMMENU_H_
#define _TEAMMENU_H_

#include "JGE.h"
#include "JRenderer.h"
#include "JGui.h"
#include "MenuItem.h"
#include "Person.h"
#include "Menu.h"

#define MAIN1 2

#define NONE -1
#define T 0
#define CT 1
#define CANCEL 2


//------------------------------------------------------------------------------------------------
class TeamMenu
{
private:
	static JGE* mEngine;
	static JRenderer* mRenderer;
	
	Category mCategories[3];	
	int mCategoryIndex;
	int mSelectedIndex;

	int mTeam;
	int mType;

	float angle;

	Person* mCT;
	Person* mT;

	std::vector<char*> mTeamLines;
	std::vector<char*> mCTFLines;
	std::vector<char*> mFFALines;

protected:

public:
	
	bool mIsActive;
	bool mIsSelected;
	bool* cross;
	bool mIsOldStyle;
	int* mGameType;

	TeamMenu();
	~TeamMenu();

	void Update(float dt);
	void Render();
	void Enable();
	void Disable();
	int GetTeam();
	int GetType();
};

#endif

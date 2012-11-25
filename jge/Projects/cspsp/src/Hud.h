#ifndef __HUD_H__
#define __HUD_H__

#include <vector>
#include "JRenderer.h"
#include "JLBFont.h"
#include "Person.h"
#include "PersonOnline.h"

#define SERVER -2
#define NONE -1
#define T 0
#define CT 1
#define MESSAGEEVENT 0
#define CHATEVENT 1

struct ChatEvent {
	char name[32];
	char string[127];
	int team;
	bool isDead;
	bool isTeamOnly;
};

struct MessageEvent {
	int type;
	union {
		char string[127];
		ChatEvent chatevent;
	};
};

struct KillEvent {
	char attackername[32];
	int attackerteam;
	char victimname[32];
	int victimteam;
	JQuad* weapon;
};

struct DamageIndicator {
	float angle;
	float timer;
};

class Hud
{
private:
	static JRenderer* mRenderer;
	std::vector<KillEvent> mKillEvents;
	std::vector<MessageEvent> mMessageEvents;
	float mKillEventTimer;
	float mMessageEventTimer;
	int mMessageEventCounter;
	char mMessage[127];
	float mMessageTimer;

	Person *mEventPerson;
	int mEventType;
	float mEventTime;
	char mEventText[127];


	std::vector<DamageIndicator> mDamageIndicators;
	
public:

	Person *mPlayer;
	bool mIsOnline;
	int *mGameType;

	Hud();
	~Hud();
	
	void Update(float dt);
	void Render();

	void AddKillEvent(Person* attacker, JQuad* weapon, Person* victim);

	void AddFlagEvent(Person* person, int action);

	void AddMessageEvent(char* string);

	void AddChatEvent(char* name, char* string, int team, bool isdead, bool isteamonly);

	void SetMessage(char* message);

	void AddDamageIndicator(float angle);

	void Reset();

};


#endif

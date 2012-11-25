#include "Hud.h"
#include "Globals.h"

JRenderer* Hud::mRenderer = NULL;

Hud::Hud()
{
	mRenderer = JRenderer::GetInstance();

	mKillEventTimer = 0;
	mMessageEventTimer = 0;
	mMessageEventCounter = 0;

	mMessageTimer = 0;

	strcpy(mMessage,"");

	mEventTime = 0;
	mEventType = 0;

	strcpy(mEventText,"");
}
	
Hud::~Hud()
{
	mKillEvents.clear();
	mMessageEvents.clear();
}


void Hud::Update(float dt)
{
	if (mKillEvents.size() > 0) {
		mKillEventTimer += dt;

		if (mKillEventTimer > 3000) {
			//delete mKillEvents[0];
			mKillEvents.erase(mKillEvents.begin());
			mKillEventTimer = 0;
		}
		/*if (mKillEvents.size() > 3) {
			//delete mKillEvents[0];
			mKillEvents.erase(mKillEvents.begin());
			mKillEventTimer = 0;
		}*/
	}

	if (mMessageEventCounter > 0) {
		mMessageEventTimer += dt;

		if (mMessageEventTimer > 5000) {
			//delete mChatEvents[0];

			mMessageEventCounter--;
			//mMessageEvents.erase(mMessageEvents.begin());
			mMessageEventTimer = 0;
		}
		/*if (mChatEvents.size() > 4) {
			//delete mChatEvents[0];
			mChatEvents.erase(mChatEvents.begin());
			mChatEventTimer = 0;
		}*/
	}

	if (mMessageTimer < 3000) {
		mMessageTimer += dt;
		if (mMessageTimer >= 3000) {
			strcpy(mMessage,"");
		}
	}

	for (int i=0; i<mDamageIndicators.size(); i++) {
		mDamageIndicators[i].timer -= dt;
		if (mDamageIndicators[i].timer < 0.0f) {
			mDamageIndicators.erase(mDamageIndicators.begin()+i);
			i--;
		}
	}

	if (mEventTime > 0) {
		mEventTime -= dt;
		if (mEventTime < 0) {
			mEventTime = 0;
		}
	}
}


void Hud::Render()
{
	gFont->SetScale(0.7f);

	int ystart = 5;
	if (mPlayer->mState == DEAD) {
		ystart = 37;
	}
	for(unsigned int i=0; i<mKillEvents.size(); i++) {
		float x = 475.0f;
		float y = ystart+i*15.0f;

		/*if (strcmp(mKillEvents[i].victimname,"nataku92") == 0) {
			gFont->SetColor(ARGB(255,0,128,0));
			gFont->DrawString(" [CSPSP dev]",x,y,JGETEXT_RIGHT);
			x -= gFont->GetStringWidth(" [CSPSP dev]");
		}*/
		if (mKillEvents[i].victimteam == CT) {
			gFont->SetColor(ARGB(255,153,204,255));
		}
		else if (mKillEvents[i].victimteam == T) {
			gFont->SetColor(ARGB(255,255,64,64));
		}
		gFont->DrawShadowedString(mKillEvents[i].victimname,x,y,JGETEXT_RIGHT);
		x -= gFont->GetStringWidth(mKillEvents[i].victimname);

		#ifdef WIN32
		mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE);
		#else
		mKillEvents[i].weapon->mBlend = GU_TFX_ADD;
		#endif

		x -= 20.0f;
		mKillEvents[i].weapon->SetColor(ARGB(175,255,128,0));
		mRenderer->RenderQuad(mKillEvents[i].weapon,x,y+5);
		mKillEvents[i].weapon->SetColor(ARGB(255,255,255,255));

		#ifdef WIN32
		mRenderer->SetTexBlend(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
		#else
		mKillEvents[i].weapon->mBlend = GU_TFX_MODULATE;
		#endif

		if (strcmp(mKillEvents[i].victimname,mKillEvents[i].attackername) == 0) continue; //suicide

		x -= 22.0f;
		/*if (strcmp(mKillEvents[i].attackername,"nataku92") == 0) {
			gFont->SetColor(ARGB(255,0,128,0));
			gFont->DrawString(" [CSPSP dev]",x,y,JGETEXT_RIGHT);
			x -= gFont->GetStringWidth(" [CSPSP dev]");
		}*/
		if (mKillEvents[i].attackerteam == CT) {
			gFont->SetColor(ARGB(255,153,204,255));
		}
		else if (mKillEvents[i].attackerteam == T) {
			gFont->SetColor(ARGB(255,255,64,64));
		}
		gFont->DrawShadowedString(mKillEvents[i].attackername,x,y,JGETEXT_RIGHT);
	}

	int startindex = 0;
	if (gDanzeff->mIsActive) {
	}
	else {
		startindex = mMessageEvents.size()-mMessageEventCounter;
	}
	ystart = SCREEN_HEIGHT-60-mMessageEvents.size()*15.0f;
	for(unsigned int i=startindex; i<mMessageEvents.size(); i++) {
		int alpha = 255;//55+i/9.0f*200;

		if (mMessageEvents[i].type == MESSAGEEVENT) {
			gFont->SetColor(ARGB(alpha,255,200,0));
			gFont->DrawShadowedString(mMessageEvents[i].string, 10.0f, ystart+i*15.0f);
		}
		else if (mMessageEvents[i].type == CHATEVENT) {
			float x = 10.0f;
			float y = ystart+i*15.0f;
			gFont->SetColor(ARGB(alpha,255,200,0));
			if (mMessageEvents[i].chatevent.team == NONE) {
				//gFont->DrawShadowedString("*SPEC* ",x,y);
				//x += gFont->GetStringWidth("*SPEC* ");
			}
			else {
				if (mMessageEvents[i].chatevent.isTeamOnly) {
					gFont->DrawShadowedString("(team) ",x,y);
					x += gFont->GetStringWidth("(team) ");
				}
				if (mMessageEvents[i].chatevent.isDead) {
					gFont->DrawShadowedString("*DEAD* ",x,y);
					x += gFont->GetStringWidth("*DEAD* ");
				}
			}

			if (mMessageEvents[i].chatevent.team == CT) {
				gFont->SetColor(ARGB(alpha,153,204,255));
			}
			else if (mMessageEvents[i].chatevent.team == T) {
				gFont->SetColor(ARGB(alpha,255,64,64));
			}
			else if (mMessageEvents[i].chatevent.team == NONE) {
				gFont->SetColor(ARGB(alpha,220,220,220));
			}
			else if (mMessageEvents[i].chatevent.team == SERVER) {
				gFont->SetColor(ARGB(alpha,255,200,0));
			}

			gFont->DrawShadowedString(mMessageEvents[i].chatevent.name,x,y);
			x += gFont->GetStringWidth(mMessageEvents[i].chatevent.name);

			/*if (strcmp(mMessageEvents[i].chatevent.name,"nataku92") == 0) {
				gFont->SetColor(ARGB(255,0,128,0));
				gFont->DrawString(" [CSPSP dev]",x,y);
				x += gFont->GetStringWidth(" [CSPSP dev]");
			}*/

			gFont->SetColor(ARGB(alpha,255,200,0));
			gFont->DrawShadowedString(": ",x,y);
			x += gFont->GetStringWidth(": ");

			gFont->DrawShadowedString(mMessageEvents[i].chatevent.string,x,y);
		}
	}

	if (mMessageTimer < 3000) {
		gFont->SetScale(0.75f);
		gFont->SetColor(ARGB(255,255,200,0));
		gFont->DrawShadowedString(mMessage,SCREEN_WIDTH_2,SCREEN_HEIGHT_2,JGETEXT_CENTER);
	}

	for (int i=0; i<mDamageIndicators.size(); i++) {
		int alpha = 128*(mDamageIndicators[i].timer/1000.0f);
		if (alpha > 128) alpha = 128;
		gDamageIndicator->SetColor(ARGB(alpha,255,255,255));
		mRenderer->RenderQuad(gDamageIndicator,SCREEN_WIDTH_2,SCREEN_HEIGHT_2,mDamageIndicators[i].angle);
	}

	if (mEventTime > 0.0f && mEventPerson != NULL) {
		char buffer[256];
		float y = SCREEN_HEIGHT-50;

		float scale = 0.75f;
		if (mEventTime > 1950) {
			scale = 0.75f+(2000-mEventTime)/50.0f*0.25f;
		}
		else if (mEventTime > 1900) {
			scale = 1.0f;
		}
		else if (mEventTime > 1700) {
			scale = 1.0f-(1900-mEventTime)/200.0f*0.25f;
		}
		y -= (scale-0.75f)*10;

		gFont->SetScale(scale);
		gFont->SetColor(ARGB(255,255,200,0));
		if (mEventType == 0) {
			strcpy(buffer,mEventText);
			float w1 = gFont->GetStringWidth(buffer);
			float w = w1 + gFont->GetStringWidth(mEventPerson->mName);
			if (mIsOnline) {
				w += 20;
			}
			float x = SCREEN_WIDTH_2-w*0.5f;

			gFont->SetColor(ARGB(255,255,200,0));
			gFont->DrawShadowedString(buffer,x,y);
			x += w1;
			if (mIsOnline) {
				mRenderer->RenderQuad(((PersonOnline*)mEventPerson)->mIconQuad,x+5,y+2,0.0f,scale/0.75f,scale/0.75f);
				x += 20;
			}
			if (*mGameType == FFA) {
				if (mEventPerson == mPlayer) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else {
					gFont->SetColor(ARGB(255,255,64,64));
				}
			}
			else {
				if (mEventPerson->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else if (mEventPerson->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
			}
			gFont->DrawShadowedString(mEventPerson->mName,x,y);
		}
		else if (mEventType == 1) {
			strcpy(buffer,mEventText);
			
			float w1 = gFont->GetStringWidth(mEventPerson->mName);
			float w = w1 + gFont->GetStringWidth(buffer);
			if (mIsOnline) {
				w += 15;
			}
			float x = SCREEN_WIDTH_2-w*0.5f;

			if (mIsOnline) {
				mRenderer->RenderQuad(((PersonOnline*)mEventPerson)->mIconQuad,x,y+2,0.0f,scale/0.75f,scale/0.75f);
				x += 15;
			}
			if (*mGameType == FFA) {
				if (mEventPerson == mPlayer) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else {
					gFont->SetColor(ARGB(255,255,64,64));
				}
			}
			else {
				if (mEventPerson->mTeam == CT) {
					gFont->SetColor(ARGB(255,153,204,255));
				}
				else if (mEventPerson->mTeam == T) {
					gFont->SetColor(ARGB(255,255,64,64));
				}
			}
			gFont->DrawShadowedString(mEventPerson->mName,x,y);
			x += w1;
			gFont->SetColor(ARGB(255,255,200,0));
			gFont->DrawShadowedString(buffer,x,y);
		}
		else if (mEventType == 2) {
			sprintf(buffer,"You%s",mEventText);

			gFont->SetColor(ARGB(255,255,200,0));
			gFont->DrawShadowedString(buffer,SCREEN_WIDTH_2,y,JGETEXT_CENTER);
		}
	}
}

void Hud::AddKillEvent(Person* attacker, JQuad* weapon, Person* victim) {
	if (mKillEvents.size() == 0) {
		mKillEventTimer = 0.0f;
	}
	KillEvent killevent = KillEvent();
	strcpy(killevent.attackername,attacker->mName);
	if (*mGameType == FFA) {
		if (attacker == mPlayer) {
			killevent.attackerteam = CT;
		}
		else {
			killevent.attackerteam = T;
		}
		if (victim == mPlayer) {
			killevent.victimteam = CT;
		}
		else {
			killevent.victimteam = T;
		}
	}
	else {
		killevent.attackerteam = attacker->mTeam;
		killevent.victimteam = victim->mTeam;
	}
	strcpy(killevent.victimname,victim->mName);
	killevent.weapon = weapon;
	mKillEvents.push_back(killevent);

	if (attacker == mPlayer) {
		mEventPerson = victim;
		mEventType = 0;
		mEventTime = 2000;
		strcpy(mEventText,"You killed ");
		if (attacker == victim) {
			mEventType = 2;
			strcpy(mEventText," killed yourself");
		}
	}
	else if (victim == mPlayer) {
		mEventPerson = attacker;
		mEventType = 1;
		mEventTime = 2000;
		strcpy(mEventText," killed you");
	}
	if (mKillEvents.size() > 5) {
		mKillEvents.erase(mKillEvents.begin());
	}
}

void Hud::AddFlagEvent(Person* person, int action) {
	if (person == mPlayer && action == 1) return;

	// 0 - pickup, 1 - drop, 2 - return, 3 - capture
	mEventPerson = person;
	mEventTime = 2000;
	if (person == mPlayer) {
		mEventType = 2;

	}
	else {
		mEventType = 1;
	}


	if (action == 0) {
		if (person->mTeam == mPlayer->mTeam) {
			if (person == mPlayer) {
				strcpy(mEventText," have the enemy flag");
			}
			else {
				strcpy(mEventText," has the enemy flag");
			}
		}
		else {
			strcpy(mEventText," has our flag");
		}
	}
	else if (action == 1) {
		if (person->mTeam == mPlayer->mTeam) {
			strcpy(mEventText," dropped the enemy flag");
		}
		else {
			strcpy(mEventText," dropped our flag");
		}
	}
	else if (action == 2) {
		if (person->mTeam == mPlayer->mTeam) {
			strcpy(mEventText," returned our flag");
		}
		else {
			strcpy(mEventText," returned the enemy flag");
		}
	}
	else if (action == 3) {
		if (person->mTeam == mPlayer->mTeam) {
			strcpy(mEventText," captured the enemy flag");
		}
		else {
			strcpy(mEventText," captured our flag");
		}
	}
}

void Hud::AddMessageEvent(char* string) {
	MessageEvent message = MessageEvent();
	message.type = MESSAGEEVENT;
	strcpy(message.string,string);
	mMessageEvents.push_back(message);
	if (mMessageEvents.size() > 9) {
		mMessageEvents.erase(mMessageEvents.begin());
	}
	if (mMessageEventCounter < 3) {
		mMessageEventCounter++;
		mMessageEventTimer = 0;
	}
}

void Hud::AddChatEvent(char* name, char* string, int team, bool isdead, bool isteamonly) {
	MessageEvent message = MessageEvent();
	message.type = CHATEVENT;
	strcpy(message.chatevent.name,name);
	strcpy(message.chatevent.string,string);
	message.chatevent.team = team;
	message.chatevent.isDead = isdead;
	message.chatevent.isTeamOnly = isteamonly;
	mMessageEvents.push_back(message);
	if (mMessageEvents.size() > 9) {
		mMessageEvents.erase(mMessageEvents.begin());
	}
	if (mMessageEventCounter < 3) {
		mMessageEventCounter++;
		mMessageEventTimer = 0;
	}
}

void Hud::SetMessage(char* message) {
	if (strcmp(mMessage,message) != 0) {
		strcpy(mMessage,message);
		mMessageTimer = 0;
	}
}

void Hud::AddDamageIndicator(float angle) {
	DamageIndicator damageIndicator = {angle-M_PI_2,1500};
	mDamageIndicators.push_back(damageIndicator);
}

void Hud::Reset() {
	mKillEventTimer = 0;
	mKillEvents.clear();
	mMessageEvents.clear();
	mMessageTimer = 0;
	mMessageEventTimer = 0;
	mMessageEventCounter = 0;
	strcpy(mMessage,"");
	mDamageIndicators.clear();
	mEventTime = 0;
}
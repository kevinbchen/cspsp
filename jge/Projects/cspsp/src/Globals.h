#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "JGE.h"
#include "JLBFont.h"
#include <time.h>
#include <vector>
#include "Person.h"
#include "AI.h"
#include "TileMap.h"
#include "ParticleEngine.h"
#include "SfxManager.h"
#include "Bullet.h"
#include "GunObject.h"
#include "BuyMenu.h"
#include "Camera.h"
#include "Hud.h"
#include "Danzeff.h"
#include "HGE/hgeparticle.h"
#include "Wlan.h"
#include "HttpManager.h"

enum {
	PARTICLE_EXPLOSION = 0,
	PARTICLE_FLASH,
	PARTICLE_SMOKE
};

extern char* gServerName;
extern char* gServerIP;
extern int gServerPort;
extern char* gMapName;
extern int gTeam;
extern bool gReconnect;
extern bool gLogout;
extern char gName[32];
extern char gDisplayName[32];
extern char gEncodedName[100];
extern char gEncodedDisplayName[100];
extern char gKey[64];
extern char gSessionKey[32];
extern int gKills;
extern int gDeaths;
extern int gKills2;
extern int gDeaths2;
extern unsigned char gIcon[300];

extern JLBFont* gFont;
extern JLBFont* gFontBackdrop;
extern JLBFont* gHudFont;

extern JQuad* gBgQuad;
extern JQuad* gLogoQuad;

extern Gun gGuns[28];
extern JQuad** gGunHandQuads;
extern JQuad** gGunGroundQuads;
extern JQuad* gPlayersQuads[2][4][NUM_QUADS];
extern JQuad* gPlayersDeadQuads[2][4];
extern JQuad* gRadarQuad;
extern JQuad* gBuyZoneQuad;
extern JQuad* gDecalQuads[5];
extern JQuad* gMuzzleFlashQuads[3];
extern JQuad* gHealthBorderQuad;
extern JQuad* gHealthFillQuad;
extern JQuad* gAmmoBarQuad;
extern JQuad* gIconQuad;
extern JTexture* gIconTexture;
extern JQuad* gDamageIndicator;
extern JQuad* gScoreIconQuads[4];
extern JQuad* gFlagQuad;
extern JQuad* gFlagHomeQuad;
extern JQuad* gFlagArrowQuad;
extern JQuad* gFlagRadarQuad;
extern JQuad* gHomeRadarQuad;

extern JSample* gDryFireRifleSound;
extern JSample* gDryFirePistolSound;
extern JSample* gDeploySound;
extern JSample* gPickUpSound;
extern JSample* gAmmoSound;
extern JSample* gPinPullSound;
extern JSample* gFireInTheHoleSound;
extern JSample* gWalkSounds[2];
extern JSample* gRicochetSounds[4];
extern JSample* gHitSounds[3];
extern JSample* gKnifeHitSound;
extern JSample* gDieSounds[3];
extern JSample* gRoundEndSounds[3];
extern JSample* gHEGrenadeSounds[3];
extern JSample* gFlashbangSound;
extern JSample* gSmokeGrenadeSound;
extern JSample* gGrenadeBounceSound;
extern JSample* gHitIndicatorSound;

extern KeyFrameAnim* gKeyFrameAnims[11];

extern ParticleEngine* gParticleEngine;
extern hgeParticleSystem* gParticleSystems[3];
extern SfxManager* gSfxManager;
extern Danzeff* gDanzeff;
extern Socket* gSocket;
extern HttpManager* gHttpManager;

extern char* GetConfig(const char *location, char searchstr[]);

extern u32 gLastKey;
extern float gKeyRepeatDelay;
extern bool KeyRepeated(u32 key, float dt);

extern char gBuffer[8192];
bool ReadHTTP(char* string);

void FormatText(std::vector<char*> &lines, char text[], float width, float scale);

void FormatText(char* buffer, char text[], float width, float scale);

char* DecodeText(char* buffer, char* text);
void EncodeText(char* dest, char* text);

void DrawShadowedString(const char *string, float x, float y, int align = JGETEXT_LEFT);

void UpdateIcon(JTexture* texture, unsigned char* data);
#endif

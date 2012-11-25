#include "Globals.h"

char* gServerName;
char* gServerIP;
int gServerPort;
char* gMapName;
int gTeam;
bool gReconnect;
bool gLogout;
char gName[32];
char gDisplayName[32];
char gEncodedName[100];
char gEncodedDisplayName[100];
char gKey[64];
char gSessionKey[32];
int gKills;
int gDeaths;
int gKills2;
int gDeaths2;
unsigned char gIcon[300];

JLBFont* gFont;
JLBFont* gFontBackdrop;
JLBFont* gHudFont;

JQuad* gBgQuad;
JQuad* gLogoQuad;

Gun gGuns[28];
JQuad** gGunHandQuads;
JQuad** gGunGroundQuads;
JQuad* gPlayersQuads[2][4][NUM_QUADS];
JQuad* gPlayersDeadQuads[2][4];
JQuad* gRadarQuad;
JQuad* gBuyZoneQuad;
JQuad* gDecalQuads[5];
JQuad* gMuzzleFlashQuads[3];
JQuad* gHealthBorderQuad;
JQuad* gHealthFillQuad;
JQuad* gAmmoBarQuad;
JQuad* gIconQuad;
JTexture* gIconTexture;
JQuad* gDamageIndicator;
JQuad* gScoreIconQuads[4];
JQuad* gFlagQuad;
JQuad* gFlagHomeQuad;
JQuad* gFlagArrowQuad;
JQuad* gFlagRadarQuad;
JQuad* gHomeRadarQuad;


JSample* gDryFireRifleSound;
JSample* gDryFirePistolSound;
JSample* gDeploySound;
JSample* gPickUpSound;
JSample* gAmmoSound;
JSample* gPinPullSound;
JSample* gFireInTheHoleSound;
JSample* gWalkSounds[2];
JSample* gRicochetSounds[4];
JSample* gHitSounds[3];
JSample* gKnifeHitSound;
JSample* gDieSounds[3];
JSample* gRoundEndSounds[3];
JSample* gHEGrenadeSounds[3];
JSample* gFlashbangSound;
JSample* gSmokeGrenadeSound;
JSample* gGrenadeBounceSound;
JSample* gHitIndicatorSound;

KeyFrameAnim* gKeyFrameAnims[11];

ParticleEngine* gParticleEngine;
hgeParticleSystem* gParticleSystems[3];
SfxManager* gSfxManager;
Danzeff* gDanzeff;
Socket* gSocket;
HttpManager* gHttpManager;

char* GetConfig(const char *location, char searchstr[]) {
    FILE *file;
	file = fopen(location, "r"); 
	if (file == NULL) return NULL;
	else {
		char line[1024]; 
		char key[20];
		char value[64];
		char* valueptr = new char[64];	//*************** initialize pointer
		while (!feof(file)) {
			fgets(line,1024,file);
			sscanf(line,"%s %*s %s",key,value);
			if (strcmp(searchstr,key) == 0) {
				strcpy(valueptr, (char*)value); //*************** something that i don't understand D:
				fclose(file);
				return valueptr;
			}
		}
	}
	fclose(file);
	return NULL;
}

u32 gLastKey;
float gKeyRepeatDelay;
bool KeyRepeated(u32 key, float dt)
{

	bool doKey = false;
	if (gLastKey != PSP_CTRL_UP)
	{
		gLastKey = PSP_CTRL_UP;
		doKey = true;
		gKeyRepeatDelay = JGUI_INITIAL_DELAY;
	}
	else
	{
		gKeyRepeatDelay -= dt;
		if (gKeyRepeatDelay <= 0.0f)
		{
			gKeyRepeatDelay = 50;//JGUI_REPEAT_DELAY;
			doKey = true;
		}
	}

	return doKey;
}

char gBuffer[8192];
bool ReadHTTP(char* string) {
	char* s;

	if (strstr(string,"chunked")) {
		s = strstr(string,"\r\n\r\n");
		s += 4;
	}
	else {
		s = string;
	}
	
	while (s) {
		int length = strtol(s,&s,16);
		if (length == 0) {
			return true;
		}
		s += 2; // \r\n

		strncat(gBuffer,&s[0],length);
		s += length;
	}

	return false;
}


void FormatText(std::vector<char*> &lines, char text[], float width, float scale) {
	gFont->SetScale(scale);

	string line = "";
	string word = "";

	for (int i=0; i<strlen(text); i++) {
		if (text[i] == '\r' || text[i] == '\n') {
			if (text[i] == '\r') i++;
			if (gFont->GetStringWidth(line.c_str()) + gFont->GetStringWidth(word.c_str()) + gFont->GetStringWidth(" ") > width) {
				char* buffer = new char[256];
				strcpy(buffer,line.c_str());
				lines.push_back(buffer);
				buffer = new char[256];
				strcpy(buffer,word.c_str());
				lines.push_back(buffer);
				line = "";
				word = "";
			}
			else {
				line += word + " ";
				char* buffer = new char[256];
				strcpy(buffer,line.c_str());
				lines.push_back(buffer);
				line = "";
				word = "";
			}
		}
		else if (text[i] == ' ') {
			if (gFont->GetStringWidth(line.c_str()) + gFont->GetStringWidth(word.c_str()) + gFont->GetStringWidth(" ") > width) {
				char* buffer = new char[256];
				strcpy(buffer,line.c_str());
				lines.push_back(buffer);
				line = "";
				line += word + " ";
				word = "";
			}
			else {
				line += word + " ";
				word = "";
			}
		}
		else {
			if (gFont->GetStringWidth((word+text[i]).c_str()) > width) {
				char* buffer = new char[256];
				strcpy(buffer,word.c_str());
				lines.push_back(buffer);
				line = "";
				word = "";
			}
			word += text[i];
		}
	}
	if (gFont->GetStringWidth(line.c_str()) + gFont->GetStringWidth(word.c_str()) + gFont->GetStringWidth(" ") > width) {
		char* buffer = new char[256];
		strcpy(buffer,line.c_str());
		lines.push_back(buffer);
		line = "";
		buffer = new char[256];
		strcpy(buffer,word.c_str());
		lines.push_back(buffer);
	}
	else {
		line += word;
		char* buffer = new char[256];
		strcpy(buffer,line.c_str());
		lines.push_back(buffer);
		line = "";
	}

	//return lines;
}

void FormatText(char* buffer, char text[], float width, float scale) {
	gFont->SetScale(scale);

	strcpy(buffer,"");

	for (int i=0; i<strlen(text); i++) {
		buffer[i] = text[i];
		buffer[i+1] = '\0';
		if (gFont->GetStringWidth(buffer) >= width) {
			buffer[i] = '\0';
			break;
		}
	}
}

char* DecodeText(char* buffer, char* text) {
	//char buffer[2000];
	strcpy(buffer,"");
	for (int i=0; i<strlen(text); i+=3) {
		int factor = 100;
		if ((i/3)%2 == 0) factor = 101;

		char numberBuffer[4];
		strcpy(numberBuffer,"");
		numberBuffer[0] = text[i];
		numberBuffer[1] = text[i+1];
		numberBuffer[2] = text[i+2];
		numberBuffer[3] = '\0';

		int number = 0;
		sscanf(numberBuffer,"%d",&number);
		number -= factor;

		sprintf(numberBuffer,"%c",number);
		strcat(buffer,numberBuffer);
	}

	return buffer;
}

void EncodeText(char* dest, char* text) {
	strcpy(dest,"");
	for (int i=0; i<strlen(text); i++) {
		int factor = 100;
		if (i%2 == 0) factor = 101;

		char numberBuffer[4];
		int number = text[i];

		sprintf(numberBuffer,"%d",number+factor);

		strcat(dest,numberBuffer);
	}
	//return buffer;
}

void DrawShadowedString(const char *string, float x, float y, int align) {
	PIXEL_TYPE color = gFont->GetColor();
	gFont->SetColor(ARGB(200,0,0,0));
	gFont->DrawString(string,x+1,y+1,align);

	/*gFontBackdrop->SetColor(ARGB(200,0,0,0));
	gFontBackdrop->SetScale(gFont->GetScale());
	gFontBackdrop->DrawString(string,x,y,align);*/

	gFont->SetColor(color);
	gFont->DrawString(string,x,y,align);
}

void UpdateIcon(JTexture* texture, unsigned char* data) {
	DWORD bits[100];
	int i=0;
	for (int y=0; y<10; y++) {
		for (int x=0; x<10; x++) {
			int a = 255;
			if (data[i] == 255 && data[i+2] == 255 && data[i+1] == 0) {
				a = 0;
			}
			bits[(9-y)*10+x] = ARGB(a,data[i+2],data[i+1],data[i]);
			i += 3;
		}
	}
	texture->UpdateBits(0,0,10,10,bits);
}
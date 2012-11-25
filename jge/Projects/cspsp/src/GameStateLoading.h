#ifndef _GAME_STATE_LOADING_H_
#define _GAME_STATE_LOADING_H_


#include "GameState.h"
#include "Game.h"

class GameStateLoading: public GameState
{
private:
	int mStage;


public:
	GameStateLoading(GameApp* parent);
	~GameStateLoading();

	void Start();
	void End();
	void Update(float dt);
	void Render();

	int Load(int stage);
};


#endif


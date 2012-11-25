#include "GameState.h"

GameState::GameState(GameApp* parent)
{
	mParent = parent;
	mEngine = JGE::GetInstance();
	mRenderer = JRenderer::GetInstance();
	mSoundSystem = JSoundSystem::GetInstance();
}
	

GameState::~GameState()
{
}

void GameState::Create() {
}

void GameState::Destroy() {
}

void GameState::Start() {
}

void GameState::End() {
}

void GameState::Update(float dt) {
}

void GameState::Render() {
}

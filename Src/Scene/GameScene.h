#pragma once
#include "SceneBase.h"
class SceneManager;
class EffectManager;
class Stage;
class SkyDome;
class Player;

struct EffectTime
{
	float dethEffectTime = 120.0f;

};

class GameScene : public SceneBase
{

public:

	GameScene(SceneManager* manager);
	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void DrawDebug(void);
	void Release(void) override;

private:

	Stage* mStage;
	SkyDome* mSkyDome;
	Player* mPlayer;
	EffectManager* effect_;
	EffectTime effectTimere_;

	float effectTime_;
};

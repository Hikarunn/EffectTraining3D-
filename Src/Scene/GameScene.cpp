#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Common/KeyCheck.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/GravityManager.h"
#include "../Manager/EffectManager.h"
#include "../Object/Common/Capsule.h"
#include "../Object/Common/Collider.h"
#include "../Object/SkyDome.h"
#include "../Object/Stage.h"
#include "../Object/Player.h"
#include "../Object/Planet.h"
#include "GameScene.h"

GameScene::GameScene(SceneManager* manager) : SceneBase(manager)
{
	mPlayer = nullptr;
	mSkyDome = nullptr;
	mStage = nullptr;
}

void GameScene::Init(void)
{

	mPlayer = new Player(mSceneManager);
	mSceneManager->GetGravityManager()->SetPlayer(mPlayer);
	mPlayer->Init();

	// スカイドーム
	mSkyDome = new SkyDome(mSceneManager, mPlayer->GetTransform());
	mSkyDome->Init();

	mStage = new Stage(mSceneManager, mPlayer);
	mStage->Init();

	// ステージの設定
	mStage->ChangeStage(Stage::NAME::MAIN_PLANET);

	mSceneManager->GetCamera()->SetTransform(mPlayer->GetTransform());
	mSceneManager->GetCamera()->ChangeMode(Camera::MODE::FOLLOW);

	//effect_ = new EffectManager();

	//effect_->Init();
	
}

void GameScene::Update(void)
{

	if (keyTrgDown[KEY_SYS_START])
	{
		//mSceneManager->ChangeScene(SCENE_ID::TITLE, true);
	}

	mSkyDome->Update();

	mStage->Update();

	mPlayer->Update();

	//effect_->StepAll();
}

void GameScene::Draw(void)
{

	mSkyDome->Draw();

	mStage->Draw();

	mPlayer->Draw();

	//effect_->RenderAll();

	// デバッグ表示
	DrawDebug();

	// ヘルプ
	DrawFormatString(840, 20, 0x000000, "移動　　：WASD");
	DrawFormatString(840, 40, 0x000000, "カメラ　：矢印キー");
	DrawFormatString(840, 60, 0x000000, "ダッシュ：右Shift");
	DrawFormatString(840, 80, 0x000000, "ジャンプ：＼(バクスラ)");

}

void GameScene::DrawDebug(void)
{
}

void GameScene::Release(void)
{

	mSkyDome->Release();
	delete mSkyDome;

	mStage->Release();
	delete mStage;

	mPlayer->Release();
	delete mPlayer;

	/*effect_->Terminate();
	delete effect_;*/

}

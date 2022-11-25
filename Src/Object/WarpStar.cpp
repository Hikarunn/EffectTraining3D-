#include <DxLib.h>
#include<EffekseerForDXLib.h>//
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "Common/Transform.h"
#include "Common/Capsule.h"
#include "Player.h"
#include "WarpStar.h"

WarpStar::WarpStar(SceneManager* manager, Player* player, Transform transform)
{
	mSceneManager = manager;
	mResourceManager = manager->GetResourceManager();
	mPlayer = player;
	mTransform = transform;

	mState = STATE::NONE;
}

void WarpStar::Init(void)
{

	// モデルの基本情報
	mTransform.SetModel(
		mSceneManager->GetResourceManager()->LoadModelDuplicate(
			ResourceManager::SRC::WARP_STAR)
	);
	mTransform.Update();

	//Zが無回転の状態を保持しておく//
	VECTOR angle = mTransform.quaRot.ToEuler();
	mWarpQua = Quaternion::Euler(angle.x, angle.y, 0.0f);

	//ワープ準備用の移動座標(相対座標を使ってワールド座標を作る)//
	mWarpReservePos = VAdd(mTransform.pos, mWarpQua.PosAxis(WARP_RELATIVE_POS));//

	//回転エフェクト
	mEffectRotParticle = mResourceManager->Load(
		ResourceManager::SRC::WARP_STAR_ROT_EFF).mHandleId;

	ChangeState(STATE::IDLE);

}

void WarpStar::Update(void)
{

	switch (mState)
	{
	case WarpStar::STATE::IDLE:
		UpdateIdle();
		break;
	case WarpStar::STATE::RESERVE:
		UpdateReserve();
		break;
	case WarpStar::STATE::MOVE:
		UpdateMove();
		break;
	}

}

void WarpStar::UpdateIdle(void)
{
	//回転//
	RotateZ(SPEED_ROT_IDLE);
	
	//エフェクト//
	PlayEffectRotParticle();

	//キャラクターが範囲(球体と球体)に入ったら準備Stateにする//
	//キャラクターの位置とワープスターの位置
	VECTOR wPos = mTransform.pos;
	VECTOR cPos = mPlayer->GetCapsule()->GetCenter();
	//キャラクターがワープスターの範囲に入ったら準備Stateにする
	//球体(スター)と点(キャラ)の衝突判定
	VECTOR diff = VSub(cPos, wPos);
	float dis = AsoUtility::SqrMagnitude(diff);
	if (dis < RADIUS * RADIUS)
	{
		//範囲に入った
		ChangeState(STATE::RESERVE);
		return;
	}

}

void WarpStar::UpdateReserve(void)//
{

	//回転//
	RotateZ(SPEED_ROT_RESERVE);

	//エフェクト//
	PlayEffectRotParticle();

	if (mPlayer->IsWarpMove())
	{
		ChangeState(STATE::MOVE);
	}
}

void WarpStar::UpdateMove(void)//
{

	if (mPlayer->IsPlay())
	{
		ChangeState(STATE::IDLE);
	}

}

void WarpStar::Draw(void)
{
	MV1DrawModel(mTransform.modelId);

}

void WarpStar::Release(void)
{
}

Transform* WarpStar::GetTransform(void)
{
	return &mTransform;
}

void WarpStar::ChangeState(STATE state)
{

	mState = state;
	switch (mState)
	{
	case WarpStar::STATE::IDLE:
		break;
	case WarpStar::STATE::RESERVE://
		//プレイヤーの状態を変更//
		mPlayer->StartWarp(TIME_WARP_RESERVE, mWarpQua, mWarpReservePos);

		break;
	case WarpStar::STATE::MOVE:
		break;
	}

}

void WarpStar::RotateZ(float speed)//
{

	
	//Z回転//
	auto axis = Quaternion::AngleAxis(
		AsoUtility::Deg2RadD(speed), AsoUtility::AXIS_Z);
	mTransform.quaRot = mTransform.quaRot.Mult(axis);

	mTransform.Update();

}

void WarpStar::PlayEffectRotParticle(void)
{

	mTime++;
	if (mTime % 5);
	{

		//エフェクトを再生する//
		int playerHandle = PlayEffekseer3DEffect(mEffectRotParticle);

		//エフェクトの大きさ//
		SetScalePlayingEffekseer3DEffect(playerHandle, 5.0f, 5.0f, 5.0f);

		//モデルの回転をに合わせる//
		Quaternion rot = mTransform.quaRot;
		VECTOR angle = rot.ToEuler();
		SetRotationPlayingEffekseer3DEffect(playerHandle,
			angle.x, angle.y, angle.z);

		//エフェクトの位置を変える//
		VECTOR pos = VAdd(mTransform.pos, rot.PosAxis(EFFECT_RELATIVE_POS));
		SetPosPlayingEffekseer3DEffect(
			playerHandle, pos.x, pos.y, pos.z);

		//ジャンプ中に煙が出ている//課題

	}


}



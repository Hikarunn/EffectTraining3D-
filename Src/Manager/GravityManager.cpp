#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Object/Common/Transform.h"
#include "../Object/Planet.h"
#include "../Object/Player.h"
#include "GravityManager.h"

GravityManager::GravityManager(SceneManager* manager)
{
	mSceneManager = manager;

	mActivePlanet = nullptr;
	mPlayer = nullptr;
	mPreDirGravity = AsoUtility::DIR_D;
	mDirGravity = AsoUtility::DIR_D;
	mDirUpGravity = AsoUtility::DIR_U;
	mLastHitNormal = AsoUtility::DIR_U;
	mLastHitPos = AsoUtility::VECTOR_ZERO;
	mSlerpPow = 0.0f;
	mStep = 0.0f;

	mState = STATE::IDLE;
}

void GravityManager::Init(void)
{
	mDirGravity = AsoUtility::DIR_D;
	mPreDirGravity = AsoUtility::VECTOR_ZERO;
	mStep = -1.0f;
	mSlerpPow = DEFAULT_POW_SLERP;
}

void GravityManager::Update(void)
{

	switch (mState)
	{
	case GravityManager::STATE::IDLE:
		UpdateIdle();
		break;
	case GravityManager::STATE::STAGE_CHANGE:
		UpdateChangeStage();
		break;
	}

	Calculate();

}

void GravityManager::UpdateIdle(void)
{
}

void GravityManager::UpdateChangeStage(void)
{
}

void GravityManager::Calculate(void)//
{

	//重力方向の計算//
	mDirGravity = CalcDirGravity();

	//重力の反対方向//
	mDirUpGravity = VScale(mDirGravity, -1.0f);

	//２つのベクトル間のの回転量を求める
	VECTOR up = mTransform.GetUp();
	Quaternion toRot = 
		Quaternion::FromToRotation(up, mDirUpGravity);

	//到達したい方向に向かせる//
	//mTransform.quaRot = toRot.Mult(mTransform.quaRot);

	//球面補間
	toRot = toRot.Mult(mTransform.quaRot);
	//球面補間
	mTransform.quaRot = Quaternion::Slerp(mTransform.quaRot,toRot,mSlerpPow);

}

Planet* GravityManager::GetActivePlanet(void)
{
	return mActivePlanet;
}

void GravityManager::ChangeActivePlanet(Planet* planet)
{
	// 新しい惑星をセット
	mActivePlanet = planet;
}

void GravityManager::SetPlayer(Player* player)
{
	mPlayer = player;
}

Transform* GravityManager::GetTransform(void)
{
	return &mTransform;
}

VECTOR GravityManager::GetDirGravity(void)
{
	return mDirGravity;
}

VECTOR GravityManager::GetDirUpGravity(void)
{
	return mDirUpGravity;
}

float GravityManager::GetPower(void)
{
	float ret = 0.0f;
	if (mActivePlanet != nullptr)
	{
		ret = mActivePlanet->GetGravityPow();
	}
	return ret;
}

Quaternion GravityManager::GetPremQuaRot(void)
{
	return mPreDirGravity;
}

void GravityManager::SetLastHitNormal(VECTOR normal)
{
	mLastHitNormal = normal;
}

void GravityManager::SetLastHitPos(VECTOR pos)
{
	mLastHitPos = pos;
}

VECTOR GravityManager::GetLastHitPos(void)
{
	return mLastHitPos;
}

void GravityManager::ChangeState(STATE state)
{

	mState = state;
	switch (mState)
	{
	case GravityManager::STATE::IDLE:
		break;
	case GravityManager::STATE::STAGE_CHANGE:
		break;
	}

}

VECTOR GravityManager::CalcDirGravity(void)
{

	VECTOR ret = AsoUtility::DIR_D;

	if (mActivePlanet == nullptr || mPlayer == nullptr)
	{
		return ret;
	}

	// ステージタイプによって、重力方向の計算を変える
	Planet::TYPE stageType = mActivePlanet->GetType();
	switch (stageType)
	{
	case Planet::TYPE::GROUND:
		ret = AsoUtility::DIR_D;
		break;
	case Planet::TYPE::SPHERE://
	{

		VECTOR planetPos = mActivePlanet->GetTransform()->pos;
		VECTOR playerPos = mPlayer->GetTransform()->pos;
		//重力方向を返す
		ret = VNorm(VSub(planetPos, playerPos));
	}
		break;
	case Planet::TYPE::TRANS_ROT://作る人だけ※
	{
		//【色々な重力方向】
		//球体惑星(FALL_PLAMET)以降のステージ
		//ステージ(モデル)の下方向をretに代入する
		
		//重力方向を返す
		//ret = VNorm(VSub());

	}
		break;
	case Planet::TYPE::TRANS_CAMERA_FIXED://※
		break;
	case Planet::TYPE::ROAD://※
	{
		//ステージ(地面)真下の方向をretに代入する
		
	}
		break;
	}

	return ret;

}

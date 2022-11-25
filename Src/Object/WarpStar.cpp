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

	// ���f���̊�{���
	mTransform.SetModel(
		mSceneManager->GetResourceManager()->LoadModelDuplicate(
			ResourceManager::SRC::WARP_STAR)
	);
	mTransform.Update();

	//Z������]�̏�Ԃ�ێ����Ă���//
	VECTOR angle = mTransform.quaRot.ToEuler();
	mWarpQua = Quaternion::Euler(angle.x, angle.y, 0.0f);

	//���[�v�����p�̈ړ����W(���΍��W���g���ă��[���h���W�����)//
	mWarpReservePos = VAdd(mTransform.pos, mWarpQua.PosAxis(WARP_RELATIVE_POS));//

	//��]�G�t�F�N�g
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
	//��]//
	RotateZ(SPEED_ROT_IDLE);
	
	//�G�t�F�N�g//
	PlayEffectRotParticle();

	//�L�����N�^�[���͈�(���̂Ƌ���)�ɓ������珀��State�ɂ���//
	//�L�����N�^�[�̈ʒu�ƃ��[�v�X�^�[�̈ʒu
	VECTOR wPos = mTransform.pos;
	VECTOR cPos = mPlayer->GetCapsule()->GetCenter();
	//�L�����N�^�[�����[�v�X�^�[�͈̔͂ɓ������珀��State�ɂ���
	//����(�X�^�[)�Ɠ_(�L����)�̏Փ˔���
	VECTOR diff = VSub(cPos, wPos);
	float dis = AsoUtility::SqrMagnitude(diff);
	if (dis < RADIUS * RADIUS)
	{
		//�͈͂ɓ�����
		ChangeState(STATE::RESERVE);
		return;
	}

}

void WarpStar::UpdateReserve(void)//
{

	//��]//
	RotateZ(SPEED_ROT_RESERVE);

	//�G�t�F�N�g//
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
		//�v���C���[�̏�Ԃ�ύX//
		mPlayer->StartWarp(TIME_WARP_RESERVE, mWarpQua, mWarpReservePos);

		break;
	case WarpStar::STATE::MOVE:
		break;
	}

}

void WarpStar::RotateZ(float speed)//
{

	
	//Z��]//
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

		//�G�t�F�N�g���Đ�����//
		int playerHandle = PlayEffekseer3DEffect(mEffectRotParticle);

		//�G�t�F�N�g�̑傫��//
		SetScalePlayingEffekseer3DEffect(playerHandle, 5.0f, 5.0f, 5.0f);

		//���f���̉�]���ɍ��킹��//
		Quaternion rot = mTransform.quaRot;
		VECTOR angle = rot.ToEuler();
		SetRotationPlayingEffekseer3DEffect(playerHandle,
			angle.x, angle.y, angle.z);

		//�G�t�F�N�g�̈ʒu��ς���//
		VECTOR pos = VAdd(mTransform.pos, rot.PosAxis(EFFECT_RELATIVE_POS));
		SetPosPlayingEffekseer3DEffect(
			playerHandle, pos.x, pos.y, pos.z);

		//�W�����v���ɉ����o�Ă���//�ۑ�

	}


}



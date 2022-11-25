#include <iostream>
#include <math.h>
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Effect/EffectBase.h"
#include "EffectManager.h"

//�@��{���
static EffectBaseInfo_ effectBaseInfo_[(int)EffectNum::effectNum_];

// �G�t�F�N�g���
static EffectInfo_ effectInfo_[(int)EFFECT_MAXMNUM];


bool EffectManager::Init(void)
{
	effectID_ = EffectID::NONE;
	time_ = 0;

	int num_;
	EffectInfo_* info_;
	EffectBaseInfo_* effectBInfo_;

	effectBInfo_ = effectBaseInfo_;
	for (num_ = 0; num_ < (int)EffectNum::effectNum_;num_++,effectBInfo_++)
	{
		effectBaseInfo_->graphHandle_ = -1;

		if (effectBase_->Init(num_) == NULL)
		{
			if (!effectBase_->Init((int)effectBInfo_))
			{
				return false;
			}
		}
	}

	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		info_->useFlag_ = false;
	}
//	effectBase_->Init(effectBaseInfo_->graphHandle_);

	return true;
}

void EffectManager::Terminate(void)
{
	int num_;
	EffectBaseInfo_* effectBInfo_;
	EffectInfo_* info_;

	// �G�t�F�N�g�����ׂč폜����
	
	DeleteAll();

	// �G�t�F�N�g�̐������J��Ԃ�
	effectBInfo_ = effectBaseInfo_;
	for (num_ = 0; num_ < (int)EffectNum::effectNum_; num_++, effectBInfo_++)
	{
		// �G�t�F�N�g�ʂ̌�n���֐�������ꍇ�͌Ă�
		if (effectBase_->Terminate(num_) != NULL)
		{
			effectBase_->Terminate((int)effectBInfo_);
		}

		// �G�t�F�N�g�Ŏg�p���Ă����摜�n���h�����폜
		if (effectBInfo_->graphHandle_ != -1)
		{
			DeleteGraph(effectBInfo_->graphHandle_);
			effectBInfo_->graphHandle_ = -1;
		}

	}

	//effectBase_->Terminate(effectBaseInfo_->graphHandle_);
}

EffectDInfo *EffectManager::Create(EffectNum _effectNum)
{
	int num_;
	EffectInfo_* info_;

	// �g�p����Ă��Ȃ��G�t�F�N�g�\���̂�T��
	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		if (!info_->useFlag_)
		{
			break;
		}
	}

	// ���ׂĂ̍\���̂��g�p����Ă��܂�����G���[
	if (num_ == EFFECT_MAXMNUM)
	{
		return NULL;
	}

	// �\���̂��u�g�p���v�ɋA��
	info_->useFlag_ = true;

	// �I�����N�G�X�g������Ă��邩�ǂ����̃t���O��|��
	info_->endRequest = false; 

	// ���̃G�t�F�N�g����ۑ�
	info_->effect_ = _effectNum;

	// �G�t�F�N�g�̊�{���̃A�h���X���Z�b�g
	info_->baseInfo_ = &effectBaseInfo_[(int)_effectNum];

	// �G�t�F�N�g�ʂ̃f�[�^���i�[���Ă��郁�����̈�̃A�h���X��ۑ�����|�C���^��������
	info_->subData_ = NULL;

	// �G�t�F�N�g�ʂ̍쐬���ɌĂԊ֐��̌Ăяo��
	if (effectBase_->Create(info_))
	{
		return NULL;
	}

	return info_;
	//effectBase_->Create(effectInfo_->subData_);
}

void EffectManager::EndRequest(EffectInfo_ *_effectInfo)
{
	// �I�����N�G�X�g������Ă��邩�ǂ����̃t���O�𗧂Ă�
	_effectInfo->endRequest = true;
}

void EffectManager::Delete(EffectInfo_ *_effectInfo)
{
	// ���łɍ\���̂��g�p����Ă��Ȃ������ꍇ���������ɏI��
	if (_effectInfo->useFlag_)
	{
		return;
	}

	// �폜���Ɏ��s����֐����������ꍇ�͎��s
	if (effectBase_->Delete(_effectInfo->subData_) != NULL)
	{
		effectBase_->Delete(_effectInfo/*->subData_*/);
	}

	// �G�t�F�N�g�ʂ̏�񂪂������ꍇ�̓��������J������
	if (_effectInfo->subData_ != NULL)
	{
		free(_effectInfo->subData_);
		_effectInfo->subData_ = NULL;
	}

	// �\���̂��g�p���Ă��邩�ǂ����̃t���O��|��
	_effectInfo->useFlag_ = false;

	//effectBase_->Delete(effectInfo_->subData_);
}

void EffectManager::DeleteAll(void)
{
	int num_;
	EffectInfo_* info_;

	// ���ׂĂ̍\���̂ɑ΂��č폜���������s
	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		effectBase_->Delete(info_);
	}
}

void EffectManager::StepAll(float _stepTime)
{
	int num_;
	EffectInfo_* info_;

	// ���ׂĂ̗L���ȃG�t�F�N�g�ɑ΂��ď�ԑJ�ڏ������s��
	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		if (!info_->useFlag_)
		{
			continue;
		}
		effectBase_->Step(info_->endRequest, info_->useFlag_, _stepTime, info_->subData_);
	}


	//time_ = scene_->GetDeltaTime();
//	effectBase_->Step(effectInfo_->endRequest,effectInfo_->useFlag_,time_,effectInfo_->subData_ );
}

void EffectManager::RenderAll(void)
{
	int num_;
	EffectInfo_* info_;

	// ���ׂĂ̗L���ȃG�t�F�N�g�ɑ΂��ĕ`�揈�����s��
	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		if (info_->useFlag_)
		{
			continue;
		}
		effectBase_->Render(info_->subData_,info_->baseInfo_->graphHandle_);
	}

//	effectBase_->Render(effectInfo_->subData_,effectBaseInfo_->graphHandle_);
}

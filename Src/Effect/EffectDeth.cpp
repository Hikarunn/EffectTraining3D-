#include <DxLib.h>
#include <math.h>
#include "../Math/Mathematics.h"
#include "../Manager/EffectManager.h"
#include "EffectBase.h"
#include "EffectDeth.h"




static int polygonInfoBufferSize;
static EffectDethPolygonInfo* polygonInfoBuffer;



bool EfffectDeth::Init(int _graphHandle)
{
	// �G�t�F�N�g�̉摜�ǂݍ���
	_graphHandle = LoadGraph("Data/Image/Image/Particle01.png");

	if (_graphHandle < 0)
	{
		return false;
	}
	return true;
}

bool EfffectDeth::Terminate(int _graphHandle)
{
	// �|���S�����i�[�p�Ɋm�ۂ���������
	if (polygonInfoBuffer != NULL)
	{
		free(polygonInfoBuffer);
		polygonInfoBuffer = NULL;
	}
	return true;
}

bool EfffectDeth::Create(void* _subData)
{
	EffectDInfo* info_;
	// �G�t�F�N�g�̏��\���̂��i�[���郁�����̈�m��
	_subData = malloc(sizeof(EffectDInfo));
	if (_subData == NULL)
	{
		return false;
	}

	info_ = (EffectDInfo*)_subData;

	// �p�[�e�B�N���̏����i�[���郁�����̗̈�̃A�h���X��ۑ�����ϐ�������������
	info_->partiicle_ = NULL;

	// ����I��
	return true;
}

bool EfffectDeth::Delete(void* _subData)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;

	//�p�[�e�B�N���̏����i�[���Ă����������̈�����
	if (info_->partiicle_ != NULL) {
		free(info_->partiicle_);
		info_->partiicle_ = NULL;
	}
	// ����I��
	return true;
}

bool EfffectDeth::Step(bool _endRequest, bool _useFlag, float _stepTime,void* _subData)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;
	EffectDethParticleInfo* particleInfo_;
	int num_;
	int validNum_;

	// �L���ȃp�[�e�B�N���̐���������
	validNum_ = 0;

	// �p�[�e�B�N���̐������J��Ԃ�
	particleInfo_ = info_->partiicle_;
	for (num_ = 0; num_ < info_->particleNum; num_++, particleInfo_++)
	{
		// �s�����x���O�ȉ��̏ꍇ�͎��̃��[�v��
		if (particleInfo_->alpha_ <= 0.0f)
		{
			continue;
		}

		// �L���ȃp�[�e�B�N���̐��𑝂₷
		validNum_++;

		// �傫����1.0f�����̏ꍇ�͑傫������������
		if (particleInfo_->sizeRate_ < 0.99999f)
		{
			particleInfo_->sizeRate_ += _stepTime * SIZERATE_SPEED;
			if (particleInfo_->sizeRate_ > 1.0f)
			{
				particleInfo_->sizeRate_ = 1.0f;
			}
		}
		else
		{
			// �p�x��ύX
			particleInfo_->angleSpeed_ += _stepTime * particleInfo_->angleSpeed_;

			// �\�����Ԃ��O���傫�����ŕ���
			if (particleInfo_->visibleTime_ > 0.0f)
			{
				// �\�����Ԃ��O���傫���ꍇ�\�����Ԃ����炷
				particleInfo_->visibleTime_ -= _stepTime;
			}
			else
			{
				// �\�����Ԃ��O�ȉ���������s�����x�����炷
				particleInfo_->alpha_ -= _stepTime * ALPHA_DOWN_SPEED;
			}
			// �㏸�҂����Ԃ��O���傫�����ŕ���
			if (particleInfo_->upWait_ > 0.0f)
			{
				// �㏸�҂����Ԃ��O���傫��������㏸�҂����Ԃ����炷
				particleInfo_->upWait_ -= _stepTime;
			}
			else
			{
				// �㏸�҂����Ԃ��O�̏ꍇ

				// �㏸���x���グ��
				if (particleInfo_->upSpeed_ < particleInfo_->upMaxSpeed_)
				{
					particleInfo_->upSpeed_ += _stepTime * UPACCEL;
					if (particleInfo_->upSpeed_ > particleInfo_->upMaxSpeed_)
					{
						particleInfo_->upSpeed_ = particleInfo_->upMaxSpeed_;
					}

				}

				// �p�[�e�B�N�����㏸������
				particleInfo_->pos_.y += particleInfo_->upSpeed_ * _stepTime;

				// �p�[�e�B�N�����W����̐�����������
				if (particleInfo_->distance_ < particleInfo_->distanceMax_)
				{
					particleInfo_->distance_ += _stepTime * H_DISTANCE_SPEED;
					if (particleInfo_->distance_ > particleInfo_->distanceMax_)
					{
						particleInfo_->distance_ = particleInfo_->distanceMax_;
					}
				}

			}
		}
	}
	// �I�����N�G�X�g������L���ȃp�[�e�B�N�������O��������G�t�F�N�g���폜����
	if (_endRequest || validNum_ == 0)
	{
		//Effect::Delete(effectInfo);
		Delete(_subData);

		//// ���łɍ\���̂��g�p����Ă��Ȃ������ꍇ���������I��
		//if (_useFlag)
		//{
		//	
		//}
		//// �폜���Ɏ��s����֐������݂���ꍇ�̓����������
		

		//if(effectFunc)
	}
	// ����I��
	return true;

}

bool EfffectDeth::Render(void* _subData, int _graphHandle)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;
	EffectDethParticleInfo* particleInfo_;
	int num_;
	VECTOR drawPos_;
	// Z�o�b�t�@���g�p����ݒ�ɂ���
	SetUseZBufferFlag(TRUE);

	// Z�o�b�t�@���ւ̏������݂͂����Ȃ�Ȃ�
	SetWriteZBufferFlag(FALSE);

	// �`��F���Z�b�g
	SetDrawBright(info_->color.r, info_->color.g, info_->color.b);

	// �p�[�e�B�N���̐������J��Ԃ�
	particleInfo_ = info_->partiicle_;
	for (num_ = 0; info_->particleNum; num_++, particleInfo_++)
	{
		// �s�����x���O�ȉ��̃p�[�e�B�N���͉������Ȃ�
		if (particleInfo_->alpha_ <= 0.0f)
		{
			continue;
		}

		// �����u�����h�ɐݒ�
		SetDrawBlendMode(DX_BLENDMODE_SUB, (int)(particleInfo_->alpha_ * 255.0f));

		// �p�[�e�B�N���ɕ`�悷����W
		drawPos_ = particleInfo_->pos_;
		drawPos_.x = sin(particleInfo_->alpha_) * particleInfo_->distance_;
		drawPos_.x = cos(particleInfo_->alpha_) * particleInfo_->distance_;

		// �p�[�e�B�N����`��
		DrawBillboard3D(drawPos_, 0.5f, 0.5f, particleInfo_->size_ * particleInfo_->sizeRate_, 0.0f, _graphHandle, TRUE);
	}
	// �`��u�����h���[�h��W���ɖ߂�
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// �`��P�x���W���ɖ߂�
	SetDrawBright(255, 255, 255);

	// �y�o�b�t�@���g�p���Ȃ��ݒ�ɖ߂�
	SetUseZBufferFlag(FALSE);

	// ����I��
	return true;
}

bool EfffectDeth::Setup(void* _subData, COLOR_U8 _color, int _modelHandle)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;
	EffectDethParticleInfo* particleInfo_;
	int num_;
	float createNum_;
	VECTOR* polygonPos2_;
	VECTOR* polygonPos3_;
	MV1_REF_POLYGONLIST refMesh_;
	EffectDethPolygonInfo* polygonInfo_;
	float totalAreaSize_;
	
	int tempParticleNum_;

	// info_,createNum,refMesh_,polygonInfo_,rate1_,rate2_

	// �G�t�F�N�g�̐F��ۑ�
	info_->color = _color;



	// �G�t�F�N�g�����s����RD���f���̎Q�Ɨp���b�V�����Z�b�g�A�b�v
	MV1RefreshReferenceMesh(_modelHandle, -1, TRUE, TRUE);

	// �Q�Ɨp���b�V�����擾
	refMesh_ = MV1GetReferenceMesh(_modelHandle, -1, TRUE, TRUE);

	// �|���S�����������m�ۍς݂̊i�[�p���������炠�ӂ��ꍇ�̓������̈�̍Ċm��
	ResourceReSecuring(refMesh_);
	if (ResourceReSecuring(refMesh_) == false)
	{
		return false;
	}

	// �|���S���̏����\�z����
	polygonInfo_ = polygonInfoBuffer;
	totalAreaSize_ = 0.0f;
	for (num_ = 0; num_ < refMesh_.PolygonNum; num_++, polygonInfo_++);
	{
		// �|���S���̍��W���擾
		polygonInfo_->pos_ = refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[0]].Position;
		polygonPos2_ = &refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[1]].Position;
		polygonPos3_ = &refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[2]].Position;

		// ���W�O������W�P�A���W�Q�ւ̃x�N�g�����Z�o
		polygonInfo_->vec1_ = VSub(*polygonPos2_, polygonInfo_->pos_);
		polygonInfo_->vec2_ = VSub(*polygonPos3_, polygonInfo_->pos_);

		// �|���S���̖ʐς��Z�o
		polygonInfo_->areaSize_ - VSize(VCross(polygonInfo_->vec1_, polygonInfo_->vec2_)) / 2.0f;

		// �|���S���̖ʐς̍��v�ɉ��Z
		totalAreaSize_ += polygonInfo_->areaSize_;
	}

	createNum_ = 0.0f;
	particleInfo_ = info_->partiicle_;
	PolygonNumRepetition(info_, refMesh_, particleInfo_, polygonInfo_, createNum_, num_);

	// �m�ۂ��郁�����̈�Ɋi�[����p�[�e�B�N�����p�̐����Z�o
	tempParticleNum_ = (int)(totalAreaSize_ * CREATE_RATE) + ADD_PARTICLE_NUM;

	// �p�[�e�B�N�������i�[���郁�����̈�̊m��
	info_->partiicle_ = (EffectDethParticleInfo*)malloc
	(tempParticleNum_ * sizeof(EffectDethParticleInfo));

	// �������̈�̊m�ۂɎ��s������G���[�I��
	if (info_->partiicle_ == NULL)
	{
		return false;
	}

	
	// ����
	return true;

}

bool EfffectDeth::ResourceReSecuring(MV1_REF_POLYGONLIST _refMesh)

{

	
	// �|���S�����������m�ۍς݂̊i�[�p���������炠�ӂ��ꍇ�̓������̈�̍Ċm��
	if (polygonInfoBufferSize < _refMesh.PolygonNum)
	{
		// ���łɊm�ۂ��Ă��郁�����̈悪����ꍇ�͉��
		if (polygonInfoBuffer != NULL)
		{
			free(polygonInfoBuffer);
			polygonInfoBuffer = NULL;
		}

		// �|���S�����i�[�p�̃��������m��
		polygonInfoBuffer = (EffectDethPolygonInfo*)malloc
		(sizeof(EffectDethPolygonInfo) * _refMesh.PolygonNum);

		// �������̈�̊m�ۂɎ��s�����ꍇ�͎��s�I��
		if (polygonInfoBuffer == NULL)
		{
			return false;
		}

		// �m�ۂ����������̈�ɓ���|���S�����̐���ۑ�
		polygonInfoBufferSize = _refMesh.PolygonNum;

	}
	// ����I��
	return true;
}

void EfffectDeth::PolygonNumRepetition(EffectDInfo* _info, MV1_REF_POLYGONLIST _refMesh,EffectDethParticleInfo* _particleInfo,
	EffectDethPolygonInfo* _polygonInfo, float _createNum, int _num)
{
	float rate1_;
	float rate2_;

	// �|���S���̐������J��Ԃ�
	
	_particleInfo = _info->partiicle_;
	_polygonInfo = polygonInfoBuffer;
	_info->particleNum = 0;
	for (_num = 0; _num < _refMesh.PolygonNum; _num++, _polygonInfo++)
	{
		// �p�[�e�B�N���쐬�搔�J�E���^�Ƀ|���S���̖ʐς�����̃p�[�e�B�N���̐������Z����
		_createNum += _polygonInfo->areaSize_ * CREATE_RATE;

		//�p�[�e�B�N���쐬�����̃J�E���^����1.0f�ȏ�Ȃ烋�[�v
		while (_createNum >= 1.0f)
		{
			// �p�[�e�B�N���쐬�搔�J�E���^����1.0f������
			_createNum -= 1.0f;

			// �p�[�e�B�N���̍��W���|���S���\�ʏォ�烉���_���Ō���
			rate1_ = math->GetRandomFloat(1.0f, 0.0f);
			rate2_ = math->GetRandomFloat(1.0f - rate1_, 0.0f);
			_polygonInfo->pos_ = VAdd(_polygonInfo->pos_, VAdd(VScale(_polygonInfo->vec1_, rate1_), VScale(_polygonInfo->vec2_, rate2_)));

			// �p�[�e�B�N���̏㏸�҂����ԁi�����_���j
			_particleInfo->upWait_ = math->GetRandomFloat(MAX_UPWAIT, MIN_UPWAIT);

			// �p�[�e�B�N���̏㏸�ő呬�x�i�����_���j
			_particleInfo->upMaxSpeed_ = math->GetRandomFloat(MAX_UPSPEED, MIN_UPSPEED);

			// �㏸���x������
			_particleInfo->upSpeed_ = 0.0f;

			// �����p�x�i�����_���j
			_particleInfo->angle_ = math->GetRandomFloat(DX_TWO_PI_F, 0.0f);

			// �p���x�i�����_���j
			_particleInfo->angleSpeed_ = math->GetRandomFloat(MAX_ANGLESPEED, MIN_ANGLESPEED);

			// �p���x�̌����𔽓]�i�T�O���j
			if (GetRand(100) < 50)
			{
				_particleInfo->angleSpeed_ = -_particleInfo->angleSpeed_;
			}

			// �ő吅�������i�����_���j
			_particleInfo->distanceMax_ = math->GetRandomFloat(MAX_H_DISTANCE, MIN_H_DISTANCE);

			// ���������̏�����
			_particleInfo->distance_ = 0.0f;

			// �\�����ԁi�����_���j
			_particleInfo->visibleTime_ = math->GetRandomFloat(MAX_VISIBLETIME, MIN_VISIBLETIME);

			// �s�����x������
			_particleInfo->alpha_ = 1.0f;

			// �傫���i�����_���j
			_particleInfo->size_ = math->GetRandomFloat(MAX_SIZE, MIN_SIZE);


			// �傫������������
			_particleInfo->sizeRate_ = 0.0f;

			//�|�C���^�ւ̃A�h���X���p�[�e�B�N������i�߂�
			_particleInfo++;

			// �p�[�e�B�N���̐�������₷
			_info->particleNum++;
		}
	}
}

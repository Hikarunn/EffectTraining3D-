#pragma once
#include "EffectBase.h"

//class EffectBase;
class Mathematics;

// 
static constexpr float CREATE_RATE = 0.025f;

// 
static constexpr float MAX_UPWAIT = 1.0f;
static constexpr float MIN_UPWAIT = 0.01f;

// 
//static constexpr float MAX_UPWAIT = 1.0f;

// 
static constexpr float MAX_UPSPEED = 400.0f;
static constexpr float MIN_UPSPEED = 100.0f;

// 
static constexpr float UPACCEL = 500.0f;

// 
static constexpr float MAX_ANGLESPEED = 6.0f;
static constexpr float MIN_ANGLESPEED = 1.5f;

// 
static constexpr float MAX_H_DISTANCE = 100.0f;
static constexpr float MIN_H_DISTANCE = 20.0f;

// 
static constexpr float H_DISTANCE_SPEED = 200.0f;

// 
static constexpr float MAX_VISIBLETIME = 0.5f;
static constexpr float MIN_VISIBLETIME = 0.2f;

// 
static constexpr float ALPHA_UP_SPEED = 16.0f;
static constexpr float ALPHA_DOWN_SPEED = 4.0f;

// 
static constexpr float MAX_SIZE = 16.0f;
static constexpr float MIN_SIZE = 4.0f;

// 
static constexpr float SIZERATE_SPEED = 3.0f;


static constexpr int ADD_PARTICLE_NUM = 10;


// �G�t�F�N�g�|���S����̏��
typedef struct EffectDethPolygonInfo
{
	// �|���S���ʐ�
	float areaSize_;

	// �|���S�����_�O����P�ւ̃x�N�g��
	VECTOR vec1_;

	// �|���S�����_�O����Q�ւ̃x�N�g��
	VECTOR vec2_;

	// �|���S�����_�O�̒��_
	VECTOR pos_;
};

// �G�t�F�N�g�̃p�[�e�B�N����̏��
typedef struct EffectDethParticleInfo
{
	// ���W
	VECTOR pos_;

	// �㏸�҂�����
	float upWait_;

	// �㏸�ő呬�x
	float upMaxSpeed_;

	// �㏸���x
	float upSpeed_;

	// �p�x
	float angle_;

	// �p���x
	float angleSpeed_;

	// �p�[�e�B�N�����W����̍ő吅������
	float distanceMax_;

	// �p�[�e�B�N�����W����̐�������
	float distance_;

	// �\������
	float visibleTime_;

	// �s�����x
	float alpha_;

	// �傫��
	float size_;

	// �傫����
	float sizeRate_;
};


class EfffectDeth :public EffectBase
{
public:
	bool Init(int _graphHandle)override;
	bool Terminate(int _graphHandle)override;
	bool Create(void* _subDate)override;
	bool Delete(void* _subDate)override;
	bool Step(bool _endRequest, bool _useFlag, float _stepTime,void* _subData)override;
	bool Render(void* _subDate, int _graphHandle)override;
	bool Setup(void* _subDate,COLOR_U8 _color,int _modelHandle)override;


private:

	// �G�t�F�N�g�̏��
	typedef struct EffectDInfo
	{
		// �G�t�F�N�g�̐F
		COLOR_U8 color;

		// �p�[�e�B�N���̐�
		int particleNum;

		// �p�[�e�B�N���̏����i�[���Ă��郁�����̈�̃A�h���X
		EffectDethParticleInfo* partiicle_;

		// 
		void* subData_;
	};
	// �|���S�����������m�ۍς݂̊i�[�p���������炠�ӂ��ꍇ�̓������̈�̍Ċm��
	bool ResourceReSecuring(MV1_REF_POLYGONLIST _refMesh_);


	// �|���S���̐������J��Ԃ�
	void PolygonNumRepetition(EffectDInfo* _info, MV1_REF_POLYGONLIST _refMesh, EffectDethParticleInfo* _particleInfo,
							EffectDethPolygonInfo* _polygonInfo,float _createNum,int _num);

	Mathematics* math;
};


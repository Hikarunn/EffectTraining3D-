#pragma once

class EffectBase;
class SceneManager;

static constexpr int EFFECT_MAXMNUM = 256;


enum class EffectID
{
	NONE,
	DETH,
};


enum  class EffectNum
{
	effectDeth_,
	effectDamage,
	effectNum_
};


// �G�t�F�N�g�̊�{���
typedef struct EffectBaseInfo
{
	// �G�t�F�N�g���g�p����O���t�B�b�N�n���h��
	int graphHandle_;
}EffectBaseInfo_;

// �G�t�F�N�g�̏��
typedef struct EffectDInfo
{
	// �\���̂��g�p���Ă��邩�ǂ����̃t���O
	bool useFlag_;

	// �G�t�F�N�g�̏I�������N�G�X�g����Ă��邩�ǂ����̃t���O
	bool endRequest;

	// �G�t�F�N�g�̖{��
	EffectNum effect_;

	// �G�t�F�N�g�̊�{�\����
	EffectBaseInfo_* baseInfo_;

	// �G�t�F�N�g�ʂ̃f�[�^���i�[���Ă��郁�����̈�̃A�h���X
	void* subData_;
}EffectInfo_;


class EffectManager
{
public:

	
	bool Init(void);
	void Terminate(void);
	EffectInfo_* Create(EffectNum _effectNum);
	void EndRequest(EffectInfo_ *_effectNum);
	void Delete(EffectInfo_ *_effectNum);
	void DeleteAll(void);
	void StepAll(float _stepTime);
	void RenderAll(void);
	
private:
	SceneManager* scene_;
	EffectID effectID_;
	EffectBase* effectBase_;
	/*EffectInfo* effectInfo_; */
	//EffectBaseInfo effectBaseInfo_;

	
	float time_;
};
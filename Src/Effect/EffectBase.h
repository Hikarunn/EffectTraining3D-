#pragma once
class EffectManager;

class EffectBase
{
public:

	EffectBase(EffectManager* _effect);
	~EffectBase();

	

	// ��{���̏�����
	virtual bool Init(int _graphHandle) = 0;

	// ��{���̌�n��
	virtual bool Terminate(int _graphHandle) = 0;

	// �G�t�F�N�g�̍쐬
	virtual bool Create(void* _subDate) = 0;

	// �G�t�F�N�g�폜���ɌĂ΂��֐�
	virtual bool Delete(void* _subDate) = 0;

	// ��ԑJ��
	virtual bool Step(bool _endRequest,bool _useFlag, float stepTime,void* _subData) = 0;

	// �`��
	virtual bool Render(void* _subDate,int _graphHandle) = 0;

	// �����p
	virtual bool Setup(void* _subDate, COLOR_U8 _color, int _modelHandle) = 0;

	EffectManager* GetEffect(void);

private:
	EffectManager* effect_;


};


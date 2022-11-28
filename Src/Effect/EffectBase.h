#pragma once
class EffectManager;

class EffectBase
{
public:

	EffectBase(EffectManager* _effect);
	~EffectBase();

	

	// ��{���̏�����
	virtual bool Init(void);

	// ��{���̌�n��
	virtual bool Terminate(void);

	// �G�t�F�N�g�̍쐬
	virtual bool Create(void);

	// �G�t�F�N�g�폜���ɌĂ΂��֐�
	virtual bool Delete(void);

	// ��ԑJ��
	virtual bool Step(bool _endRequest,bool _useFlag, float stepTime) ;

	// �`��
	virtual bool Render(void) ;

	// �����p
	virtual bool Setup( COLOR_U8 _color) ;

	EffectManager* GetEffect(void);

private:
	EffectManager* effect_;


};


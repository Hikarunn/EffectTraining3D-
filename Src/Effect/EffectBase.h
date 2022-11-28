#pragma once
class EffectManager;

class EffectBase
{
public:

	EffectBase(EffectManager* _effect);
	~EffectBase();

	

	// 基本情報の初期化
	virtual bool Init(void);

	// 基本情報の後始末
	virtual bool Terminate(void);

	// エフェクトの作成
	virtual bool Create(void);

	// エフェクト削除時に呼ばれる関数
	virtual bool Delete(void);

	// 状態遷移
	virtual bool Step(bool _endRequest,bool _useFlag, float stepTime) ;

	// 描画
	virtual bool Render(void) ;

	// 準備用
	virtual bool Setup( COLOR_U8 _color) ;

	EffectManager* GetEffect(void);

private:
	EffectManager* effect_;


};


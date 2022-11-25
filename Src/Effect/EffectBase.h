#pragma once
class EffectManager;

class EffectBase
{
public:

	EffectBase(EffectManager* _effect);
	~EffectBase();

	

	// 基本情報の初期化
	virtual bool Init(int _graphHandle) = 0;

	// 基本情報の後始末
	virtual bool Terminate(int _graphHandle) = 0;

	// エフェクトの作成
	virtual bool Create(void* _subDate) = 0;

	// エフェクト削除時に呼ばれる関数
	virtual bool Delete(void* _subDate) = 0;

	// 状態遷移
	virtual bool Step(bool _endRequest,bool _useFlag, float stepTime,void* _subData) = 0;

	// 描画
	virtual bool Render(void* _subDate,int _graphHandle) = 0;

	// 準備用
	virtual bool Setup(void* _subDate, COLOR_U8 _color, int _modelHandle) = 0;

	EffectManager* GetEffect(void);

private:
	EffectManager* effect_;


};


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


// エフェクトの基本情報
typedef struct EffectBaseInfo
{
	// エフェクトが使用するグラフィックハンドル
	int graphHandle_;
}EffectBaseInfo_;

// エフェクトの情報
typedef struct EffectDInfo
{
	// 構造体を使用しているかどうかのフラグ
	bool useFlag_;

	// エフェクトの終了がリクエストされているかどうかのフラグ
	bool endRequest;

	// エフェクトの本体
	EffectNum effect_;

	// エフェクトの基本構造体
	EffectBaseInfo_* baseInfo_;

	// エフェクト別のデータを格納しているメモリ領域のアドレス
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
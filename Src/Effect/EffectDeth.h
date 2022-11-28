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


// エフェクトポリゴン一つの情報
typedef struct EffectDethPolygonInfo
{
	// ポリゴン面積
	float areaSize_;

	// ポリゴン頂点０から１へのベクトル
	VECTOR vec1_;

	// ポリゴン頂点０から２へのベクトル
	VECTOR vec2_;

	// ポリゴン頂点０の頂点
	VECTOR pos_;
};

// エフェクトのパーティクル一つの情報
typedef struct EffectDethParticleInfo
{
	// 座標
	VECTOR pos_;

	// 上昇待ち時間
	float upWait_;

	// 上昇最大速度
	float upMaxSpeed_;

	// 上昇速度
	float upSpeed_;

	// 角度
	float angle_;

	// 角速度
	float angleSpeed_;

	// パーティクル座標からの最大水平距離
	float distanceMax_;

	// パーティクル座標からの水平距離
	float distance_;

	// 表示時間
	float visibleTime_;

	// 不透明度
	float alpha_;

	// 大きさ
	float size_;

	// 大きさ率
	float sizeRate_;
}EffectDethParticleInfo_;


class EfffectDeth :public EffectBase
{
public:
	bool Init(void)override;
	bool Terminate(void)override;
	bool Create(void)override;
	bool Delete(void)override;
	bool Step(bool _endRequest, bool _useFlag, float _stepTime)override;
	bool Render(void)override;
	bool Setup(COLOR_U8 _color)override;


private:
	typedef struct EffectDethInfo
	{
		// エフェクトの色
		COLOR_U8 color_;

		// パーティクルの数
		int particleNum_;

		// パーティクルの情報を格納しているメモリ領域のアドレス
		EffectDethParticleInfo* partiicle_;


	}EffectDethInfo_;
	
	
	// データを格納しているメモリ領域のアドレス
	void* subData_;
	
	// ポリゴン数が多く確保済みの格納用メモリからあふれる場合はメモリ領域の再確保
	bool ResourceReSecuring(MV1_REF_POLYGONLIST _refMesh_);


	// ポリゴンの数だけ繰り返し
	void PolygonNumRepetition( MV1_REF_POLYGONLIST _refMesh, EffectDethParticleInfo* _particleInfo,
							EffectDethPolygonInfo* _polygonInfo,float _createNum,int _num);

	// グラフィックスハンドル
	int graphHandle_;

	Mathematics* math; 

	EffectDethInfo_* effectDethInfo_;
	
	// ポリゴンの情報を格納しているメモリ領域のアドレス
	EffectDethPolygonInfo* polygonInfoBuffer_;

};


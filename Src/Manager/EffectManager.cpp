#include <iostream>
#include <math.h>
#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Effect/EffectBase.h"
#include "EffectManager.h"

//　基本情報
static EffectBaseInfo_ effectBaseInfo_[(int)EffectNum::effectNum_];

// エフェクト情報
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

	// エフェクトをすべて削除する
	
	DeleteAll();

	// エフェクトの数だけ繰り返し
	effectBInfo_ = effectBaseInfo_;
	for (num_ = 0; num_ < (int)EffectNum::effectNum_; num_++, effectBInfo_++)
	{
		// エフェクト別の後始末関数がある場合は呼ぶ
		if (effectBase_->Terminate(num_) != NULL)
		{
			effectBase_->Terminate((int)effectBInfo_);
		}

		// エフェクトで使用していた画像ハンドルを削除
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

	// 使用されていないエフェクト構造体を探す
	info_ = effectInfo_;
	for (num_ = 0; num_ < EFFECT_MAXMNUM; num_++, info_++)
	{
		if (!info_->useFlag_)
		{
			break;
		}
	}

	// すべての構造体が使用されてしまったらエラー
	if (num_ == EFFECT_MAXMNUM)
	{
		return NULL;
	}

	// 構造体を「使用中」に帰る
	info_->useFlag_ = true;

	// 終了リクエストがされているかどうかのフラグを倒す
	info_->endRequest = false; 

	// 何のエフェクトかを保存
	info_->effect_ = _effectNum;

	// エフェクトの基本情報のアドレスをセット
	info_->baseInfo_ = &effectBaseInfo_[(int)_effectNum];

	// エフェクト別のデータを格納しているメモリ領域のアドレスを保存するポインタを初期化
	info_->subData_ = NULL;

	// エフェクト別の作成時に呼ぶ関数の呼び出し
	if (effectBase_->Create(info_))
	{
		return NULL;
	}

	return info_;
	//effectBase_->Create(effectInfo_->subData_);
}

void EffectManager::EndRequest(EffectInfo_ *_effectInfo)
{
	// 終了リクエストがされているかどうかのフラグを立てる
	_effectInfo->endRequest = true;
}

void EffectManager::Delete(EffectInfo_ *_effectInfo)
{
	// すでに構造体が使用されていなかった場合何もせずに終了
	if (_effectInfo->useFlag_)
	{
		return;
	}

	// 削除時に実行する関数があった場合は実行
	if (effectBase_->Delete(_effectInfo->subData_) != NULL)
	{
		effectBase_->Delete(_effectInfo/*->subData_*/);
	}

	// エフェクト別の情報があった場合はメモリを開放する
	if (_effectInfo->subData_ != NULL)
	{
		free(_effectInfo->subData_);
		_effectInfo->subData_ = NULL;
	}

	// 構造体を使用しているかどうかのフラグを倒す
	_effectInfo->useFlag_ = false;

	//effectBase_->Delete(effectInfo_->subData_);
}

void EffectManager::DeleteAll(void)
{
	int num_;
	EffectInfo_* info_;

	// すべての構造体に対して削除処理を実行
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

	// すべての有効なエフェクトに対して状態遷移処理を行う
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

	// すべての有効なエフェクトに対して描画処理を行う
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

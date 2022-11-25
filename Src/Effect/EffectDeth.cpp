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
	// エフェクトの画像読み込み
	_graphHandle = LoadGraph("Data/Image/Image/Particle01.png");

	if (_graphHandle < 0)
	{
		return false;
	}
	return true;
}

bool EfffectDeth::Terminate(int _graphHandle)
{
	// ポリゴン情報格納用に確保したメモリ
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
	// エフェクトの情報構造体を格納するメモリ領域確保
	_subData = malloc(sizeof(EffectDInfo));
	if (_subData == NULL)
	{
		return false;
	}

	info_ = (EffectDInfo*)_subData;

	// パーティクルの情報を格納するメモリの領域のアドレスを保存する変数を初期化する
	info_->partiicle_ = NULL;

	// 正常終了
	return true;
}

bool EfffectDeth::Delete(void* _subData)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;

	//パーティクルの情報を格納していたメモリ領域を解放
	if (info_->partiicle_ != NULL) {
		free(info_->partiicle_);
		info_->partiicle_ = NULL;
	}
	// 正常終了
	return true;
}

bool EfffectDeth::Step(bool _endRequest, bool _useFlag, float _stepTime,void* _subData)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;
	EffectDethParticleInfo* particleInfo_;
	int num_;
	int validNum_;

	// 有効なパーティクルの数を初期化
	validNum_ = 0;

	// パーティクルの数だけ繰り返し
	particleInfo_ = info_->partiicle_;
	for (num_ = 0; num_ < info_->particleNum; num_++, particleInfo_++)
	{
		// 不透明度が０以下の場合は次のループへ
		if (particleInfo_->alpha_ <= 0.0f)
		{
			continue;
		}

		// 有効なパーティクルの数を増やす
		validNum_++;

		// 大きさが1.0f未満の場合は大きさ率をあげる
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
			// 角度を変更
			particleInfo_->angleSpeed_ += _stepTime * particleInfo_->angleSpeed_;

			// 表示時間が０より大きいかで分岐
			if (particleInfo_->visibleTime_ > 0.0f)
			{
				// 表示時間が０より大きい場合表示時間を減らす
				particleInfo_->visibleTime_ -= _stepTime;
			}
			else
			{
				// 表示時間が０以下だったら不透明度を減らす
				particleInfo_->alpha_ -= _stepTime * ALPHA_DOWN_SPEED;
			}
			// 上昇待ち時間が０より大きいかで分岐
			if (particleInfo_->upWait_ > 0.0f)
			{
				// 上昇待ち時間が０より大きかったら上昇待ち時間を減らす
				particleInfo_->upWait_ -= _stepTime;
			}
			else
			{
				// 上昇待ち時間が０の場合

				// 上昇速度を上げる
				if (particleInfo_->upSpeed_ < particleInfo_->upMaxSpeed_)
				{
					particleInfo_->upSpeed_ += _stepTime * UPACCEL;
					if (particleInfo_->upSpeed_ > particleInfo_->upMaxSpeed_)
					{
						particleInfo_->upSpeed_ = particleInfo_->upMaxSpeed_;
					}

				}

				// パーティクルを上昇させる
				particleInfo_->pos_.y += particleInfo_->upSpeed_ * _stepTime;

				// パーティクル座標からの水平方向距離
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
	// 終了リクエストがされ有効なパーティクル数も０だったらエフェクトを削除する
	if (_endRequest || validNum_ == 0)
	{
		//Effect::Delete(effectInfo);
		Delete(_subData);

		//// すでに構造体が使用されていなかった場合何もせず終了
		//if (_useFlag)
		//{
		//	
		//}
		//// 削除時に実行する関数が存在する場合はメモリを解放
		

		//if(effectFunc)
	}
	// 正常終了
	return true;

}

bool EfffectDeth::Render(void* _subData, int _graphHandle)
{
	EffectDInfo* info_ = (EffectDInfo*)_subData;
	EffectDethParticleInfo* particleInfo_;
	int num_;
	VECTOR drawPos_;
	// Zバッファを使用する設定にする
	SetUseZBufferFlag(TRUE);

	// Zバッファをへの書き込みはおこなわない
	SetWriteZBufferFlag(FALSE);

	// 描画色をセット
	SetDrawBright(info_->color.r, info_->color.g, info_->color.b);

	// パーティクルの数だけ繰り返し
	particleInfo_ = info_->partiicle_;
	for (num_ = 0; info_->particleNum; num_++, particleInfo_++)
	{
		// 不透明度が０以下のパーティクルは何もしない
		if (particleInfo_->alpha_ <= 0.0f)
		{
			continue;
		}

		// 減衰ブレンドに設定
		SetDrawBlendMode(DX_BLENDMODE_SUB, (int)(particleInfo_->alpha_ * 255.0f));

		// パーティクルに描画する座標
		drawPos_ = particleInfo_->pos_;
		drawPos_.x = sin(particleInfo_->alpha_) * particleInfo_->distance_;
		drawPos_.x = cos(particleInfo_->alpha_) * particleInfo_->distance_;

		// パーティクルを描画
		DrawBillboard3D(drawPos_, 0.5f, 0.5f, particleInfo_->size_ * particleInfo_->sizeRate_, 0.0f, _graphHandle, TRUE);
	}
	// 描画ブレンドモードを標準に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 描画輝度も標準に戻す
	SetDrawBright(255, 255, 255);

	// Ｚバッファを使用しない設定に戻す
	SetUseZBufferFlag(FALSE);

	// 正常終了
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

	// エフェクトの色を保存
	info_->color = _color;



	// エフェクトを実行する３Dモデルの参照用メッシュをセットアップ
	MV1RefreshReferenceMesh(_modelHandle, -1, TRUE, TRUE);

	// 参照用メッシュを取得
	refMesh_ = MV1GetReferenceMesh(_modelHandle, -1, TRUE, TRUE);

	// ポリゴン数が多く確保済みの格納用メモリからあふれる場合はメモリ領域の再確保
	ResourceReSecuring(refMesh_);
	if (ResourceReSecuring(refMesh_) == false)
	{
		return false;
	}

	// ポリゴンの情報を構築する
	polygonInfo_ = polygonInfoBuffer;
	totalAreaSize_ = 0.0f;
	for (num_ = 0; num_ < refMesh_.PolygonNum; num_++, polygonInfo_++);
	{
		// ポリゴンの座標を取得
		polygonInfo_->pos_ = refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[0]].Position;
		polygonPos2_ = &refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[1]].Position;
		polygonPos3_ = &refMesh_.Vertexs[refMesh_.Polygons[num_].VIndex[2]].Position;

		// 座標０から座標１、座標２へのベクトルを算出
		polygonInfo_->vec1_ = VSub(*polygonPos2_, polygonInfo_->pos_);
		polygonInfo_->vec2_ = VSub(*polygonPos3_, polygonInfo_->pos_);

		// ポリゴンの面積を算出
		polygonInfo_->areaSize_ - VSize(VCross(polygonInfo_->vec1_, polygonInfo_->vec2_)) / 2.0f;

		// ポリゴンの面積の合計に加算
		totalAreaSize_ += polygonInfo_->areaSize_;
	}

	createNum_ = 0.0f;
	particleInfo_ = info_->partiicle_;
	PolygonNumRepetition(info_, refMesh_, particleInfo_, polygonInfo_, createNum_, num_);

	// 確保するメモリ領域に格納するパーティクル情報用の数を算出
	tempParticleNum_ = (int)(totalAreaSize_ * CREATE_RATE) + ADD_PARTICLE_NUM;

	// パーティクル情報を格納するメモリ領域の確保
	info_->partiicle_ = (EffectDethParticleInfo*)malloc
	(tempParticleNum_ * sizeof(EffectDethParticleInfo));

	// メモリ領域の確保に失敗したらエラー終了
	if (info_->partiicle_ == NULL)
	{
		return false;
	}

	
	// 成功
	return true;

}

bool EfffectDeth::ResourceReSecuring(MV1_REF_POLYGONLIST _refMesh)

{

	
	// ポリゴン数が多く確保済みの格納用メモリからあふれる場合はメモリ領域の再確保
	if (polygonInfoBufferSize < _refMesh.PolygonNum)
	{
		// すでに確保しているメモリ領域がある場合は解放
		if (polygonInfoBuffer != NULL)
		{
			free(polygonInfoBuffer);
			polygonInfoBuffer = NULL;
		}

		// ポリゴン情報格納用のメモリを確保
		polygonInfoBuffer = (EffectDethPolygonInfo*)malloc
		(sizeof(EffectDethPolygonInfo) * _refMesh.PolygonNum);

		// メモリ領域の確保に失敗した場合は失敗終了
		if (polygonInfoBuffer == NULL)
		{
			return false;
		}

		// 確保したメモリ領域に入るポリゴン情報の数を保存
		polygonInfoBufferSize = _refMesh.PolygonNum;

	}
	// 正常終了
	return true;
}

void EfffectDeth::PolygonNumRepetition(EffectDInfo* _info, MV1_REF_POLYGONLIST _refMesh,EffectDethParticleInfo* _particleInfo,
	EffectDethPolygonInfo* _polygonInfo, float _createNum, int _num)
{
	float rate1_;
	float rate2_;

	// ポリゴンの数だけ繰り返し
	
	_particleInfo = _info->partiicle_;
	_polygonInfo = polygonInfoBuffer;
	_info->particleNum = 0;
	for (_num = 0; _num < _refMesh.PolygonNum; _num++, _polygonInfo++)
	{
		// パーティクル作成画数カウンタにポリゴンの面積あたりのパーティクルの数を加算する
		_createNum += _polygonInfo->areaSize_ * CREATE_RATE;

		//パーティクル作成が数のカウンタから1.0f以上ならループ
		while (_createNum >= 1.0f)
		{
			// パーティクル作成画数カウンタから1.0fを引く
			_createNum -= 1.0f;

			// パーティクルの座標をポリゴン表面上からランダムで決定
			rate1_ = math->GetRandomFloat(1.0f, 0.0f);
			rate2_ = math->GetRandomFloat(1.0f - rate1_, 0.0f);
			_polygonInfo->pos_ = VAdd(_polygonInfo->pos_, VAdd(VScale(_polygonInfo->vec1_, rate1_), VScale(_polygonInfo->vec2_, rate2_)));

			// パーティクルの上昇待ち時間（ランダム）
			_particleInfo->upWait_ = math->GetRandomFloat(MAX_UPWAIT, MIN_UPWAIT);

			// パーティクルの上昇最大速度（ランダム）
			_particleInfo->upMaxSpeed_ = math->GetRandomFloat(MAX_UPSPEED, MIN_UPSPEED);

			// 上昇速度初期化
			_particleInfo->upSpeed_ = 0.0f;

			// 初期角度（ランダム）
			_particleInfo->angle_ = math->GetRandomFloat(DX_TWO_PI_F, 0.0f);

			// 角速度（ランダム）
			_particleInfo->angleSpeed_ = math->GetRandomFloat(MAX_ANGLESPEED, MIN_ANGLESPEED);

			// 角速度の向きを反転（５０％）
			if (GetRand(100) < 50)
			{
				_particleInfo->angleSpeed_ = -_particleInfo->angleSpeed_;
			}

			// 最大水平距離（ランダム）
			_particleInfo->distanceMax_ = math->GetRandomFloat(MAX_H_DISTANCE, MIN_H_DISTANCE);

			// 水平距離の初期化
			_particleInfo->distance_ = 0.0f;

			// 表示時間（ランダム）
			_particleInfo->visibleTime_ = math->GetRandomFloat(MAX_VISIBLETIME, MIN_VISIBLETIME);

			// 不透明度初期化
			_particleInfo->alpha_ = 1.0f;

			// 大きさ（ランダム）
			_particleInfo->size_ = math->GetRandomFloat(MAX_SIZE, MIN_SIZE);


			// 大きさ率を初期化
			_particleInfo->sizeRate_ = 0.0f;

			//ポインタへのアドレスをパーティクル一つ分進める
			_particleInfo++;

			// パーティクルの数を一つ増やす
			_info->particleNum++;
		}
	}
}

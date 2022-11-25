#include "DxLib.h"
#include "Mathematics.h"

void Mathematics::CatmullRom(VECTOR* pos0, VECTOR* pos1, VECTOR* pos2, VECTOR* pos3, float DestPosRate, VECTOR* DestPos)
{
	VECTOR v0;
	VECTOR v1;
	float  t2;
	float  t3;

	// Catmull-Rom 補間を使用して、Pos1 ～ Pos2 の補間座標を算出する

	v0.x = (pos2->x - pos0->x) / 2.0f;
	v0.y = (pos2->y - pos0->y) / 2.0f;
	v0.z = (pos2->z - pos0->z) / 2.0f;

	v1.x = (pos3->x - pos1->x) / 2.0f;
	v1.y = (pos3->y - pos1->y) / 2.0f;
	v1.z = (pos3->z - pos1->z) / 2.0f;

	t2 = DestPosRate * DestPosRate;
	t3 = t2 * DestPosRate;

	DestPos->x = (2 * pos1->x - 2 * pos2->x + v0.x + v1.x) * t3 +
		(-3 * pos1->x + 3 * pos2->x - 2 * v0.x - v1.x) * t2 +
		v0.x * DestPosRate + pos1->x;

	DestPos->y = (2 * pos1->y - 2 * pos2->y + v0.y + v1.y) * t3 +
		(-3 * pos1->y + 3 * pos2->y - 2 * v0.y - v1.y) * t2 +
		v0.y * DestPosRate + pos1->y;

	DestPos->z = (2 * pos1->z - 2 * pos2->z + v0.z + v1.z) * t3 +
		(-3 * pos1->z + 3 * pos2->z - 2 * v0.z - v1.z) * t2 +
		v0.z * DestPosRate + pos1->z;
}

void Mathematics::GetCatmullRomPosList(VECTOR* inPos, int posNum, int divNum, VECTOR* destPos)
{
	VECTOR  SPos;
	VECTOR  EPos;
	VECTOR* Pos[4];
	VECTOR* dp;
	int     i;
	int     j;
	int     num;
	float   sm;

	// ０番目の座標の制御点は１番目の座標から０番目の座標の方向へ
	// ０番目の座標から移動した座標とする
	SPos.x = inPos[0].x + (inPos[0].x - inPos[1].x) / 2.0f;
	SPos.y = inPos[0].y + (inPos[0].y - inPos[1].y) / 2.0f;
	SPos.z = inPos[0].z + (inPos[0].z - inPos[1].z) / 2.0f;

	// 最後の座標の制御点は最後の一つ前の座標から最後の座標の方向へ
	// 最後の座標から移動した座標とする
	EPos.x = inPos[posNum - 1].x + (inPos[posNum - 1].x - inPos[posNum - 2].x) / 2.0f;
	EPos.y = inPos[posNum - 1].y + (inPos[posNum - 1].y - inPos[posNum - 2].y) / 2.0f;
	EPos.z = inPos[posNum - 1].z + (inPos[posNum - 1].z - inPos[posNum - 2].z) / 2.0f;

	// Catmull-Rom 補間を使用して全座標間の補間座標を算出する
	dp = destPos;
	num = posNum - 1;
	sm = 1.0f / divNum;
	for (i = 0; i < num; i++)
	{
		VECTOR v0;
		VECTOR v1;
		float  t2;
		float  t3;
		float  t;
		VECTOR par1;
		VECTOR par2;

		Pos[0] = i - 1 < 0 ? &SPos : &inPos[i - 1];
		Pos[1] = &inPos[i];
		Pos[2] = &inPos[i + 1];
		Pos[3] = i + 2 >= posNum ? &EPos : &inPos[i + 2];

		v0.x = (Pos[2]->x - Pos[0]->x) / 2.0f;
		v0.y = (Pos[2]->y - Pos[0]->y) / 2.0f;
		v0.z = (Pos[2]->z - Pos[0]->z) / 2.0f;

		v1.x = (Pos[3]->x - Pos[1]->x) / 2.0f;
		v1.y = (Pos[3]->y - Pos[1]->y) / 2.0f;
		v1.z = (Pos[3]->z - Pos[1]->z) / 2.0f;

		par1.x = 2 * Pos[1]->x - 2 * Pos[2]->x + v0.x + v1.x;
		par1.y = 2 * Pos[1]->y - 2 * Pos[2]->y + v0.y + v1.y;
		par1.z = 2 * Pos[1]->z - 2 * Pos[2]->z + v0.z + v1.z;

		par2.x = -3 * Pos[1]->x + 3 * Pos[2]->x - 2 * v0.x - v1.x;
		par2.y = -3 * Pos[1]->y + 3 * Pos[2]->y - 2 * v0.y - v1.y;
		par2.z = -3 * Pos[1]->z + 3 * Pos[2]->z - 2 * v0.z - v1.z;

		t = 0.0f;
		for (j = 0; j < divNum; j++, dp++, t += sm)
		{
			t2 = t * t;
			t3 = t2 * t;
			dp->x = par1.x * t3 + par2.x * t2 + v0.x * t + Pos[1]->x;
			dp->y = par1.y * t3 + par2.y * t2 + v0.y * t + Pos[1]->y;
			dp->z = par1.z * t3 + par2.z * t2 + v0.z * t + Pos[1]->z;
		}
	}
	*dp = inPos[posNum - 1];
}

int Mathematics::GetCatmullRomPosNum(int posNum, int divNum)
{
	// 出力座標の数は『座標間の数』×『一つの座標間の補間座標の数』＋『最後の座標』
	return (posNum - 1) * divNum + 1;
}

bool Mathematics::ParamChangeFloat(float* param, float targetParam, float stepTime, float speed)
{
	// 変数の値が目標とする値より小さいかどうかで処理を分岐
	if (*param < targetParam)
	{
		// 目標の値より小さい場合は変数の値に変化値を加算する
		*param += speed * stepTime;

		// 目標の値に達しているかどうかの判定
		if (*param >= targetParam)
		{
			// 達していたら変数に目標の値を代入して true を返す
			*param = targetParam;
			return true;
		}
	}
	else
	{
		// 目標の値より小さい場合は変数の値から変化値を減算する
		*param -= speed * stepTime;

		// 目標の値に達しているかどうかの判定
		if (*param <= targetParam)
		{
			// 達していたら変数に目標の値を代入して true を返す
			*param = targetParam;
			return true;
		}
	}

	// ここに来たらまだ目標の値に達していないということなので false を返す
	return false;
}

bool Mathematics::ParamChangeAngle(float* param, float targetParam, float stepTime, float speed)
{
	float RelateAngle;

	// 目標とする角度と変数の角度との差を算出
	RelateAngle = targetParam - *param;

	// 角度の差が１８０度以上の場合は角度の差を求める方向を逆転させる
	if (RelateAngle < -DX_PI_F)
	{
		RelateAngle += DX_TWO_PI_F;
	}
	else
		if (RelateAngle > DX_PI_F)
		{
			RelateAngle -= DX_TWO_PI_F;
		}

	// 角度の差を０に近づける
	if (ParamChangeFloat(&RelateAngle, 0.0f, stepTime, speed))
	{
		// 角度の差が０になったら目標とする角度になるということなので
		// 変数に目標の角度を代入して true を返す
		*param = targetParam;
		return true;
	}

	// 目標の角度から角度の差を引いたものを新しい角度とする
	*param = targetParam - RelateAngle;

	// まだ目標の角度に達していないので false を返す
	return false;
}

float Mathematics::GetRandomFloat(float maxF, float minF)
{
	return GetRand(10000) * (maxF - minF) / 10000.0f + minF;
}

int Mathematics::GetRandomInt(int maxI, int minI)
{
	return GetRand(maxI - minI) + minI;
}

float Mathematics::GetRandomFloatStructure(MinMaxFloat* minMaxF)
{
	return GetRandomFloat(minMaxF->maxF_, minMaxF->minF_);
}

int Mathematics::GetRandomIntStructure(MinMaxInt* minMaxI)
{
	return GetRandomInt(minMaxI->maxI_, minMaxI->minI_);
}

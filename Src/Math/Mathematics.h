#pragma once


// 指定範囲からランダムに値を決定する情報（浮動小数点型）
struct MinMaxFloat
{
	// 最大値
	float maxF_;

	// 最小値
	float minF_;
};
// 指定範囲からランダムに値を決定する情報（整数型）
struct MinMaxInt
{
	// 最大値
	int maxI_;

	// 最小値
	int minI_;
};


class Mathematics
{

public:

	// Catmull-Rom 補間を利用して指定の4点のPos1～Pos2を補間する座標を取得する
	//pos0  補間で使用する座標０（制御点として使用する）
	//pos1　補間で使用する座標１
	//pos2　補間で使用する座標２
	//pos3　補間で使用する座標３
	//DestPosRate　取得する座標の位置
	//DestPos　計算結果の座標を代入する変数のアドレス
	void CatmullRom(VECTOR* pos0, VECTOR* pos1, VECTOR* pos2, VECTOR* pos3, float DestPosRate, VECTOR* DestPos);



	// 座標配列の間を Catmull-Rom を使用して補間した座標配列を取得する
	// inPos 補間を行う座標配列の先頭アドレス
	// posNum  InPos の座標の数
	// divNum  一座標間の補間座標の数
	// desPos 結果の座標を格納する座標配列の先頭アドレス
	void GetCatmullRomPosList(VECTOR* inPos, int posNum, int divNum, VECTOR* destPos);

	// GetCatmullRomPosList に渡す「InPos の座標の数」と「一座標間の補間座標の数」から
	// 結果を格納する座標配列に必要な要素数を取得する
	// 戻り値 : 結果を格納する座標配列に必要な要素数
	// posNum　inPosの座標の数
	// divNum  一座標間の保管座標の数
	int GetCatmullRomPosNum(int posNum, int divNum);

	// 浮動小数点型の変数の値を指定の値に近づける
	// （目標とする辺数に代入されている角度の差が2.0f＊π以内である必要がある
	// 戻り値：変数の角度が指定の角度に達したか
	// param　操作対象の変数アドレス	
	// targetParam　目標とする角度
	// stepTime　推移させる時間
	// speed　角度の変化速度
	bool ParamChangeFloat(float* param, float targetParam, float stepTime, float speed);

	// 浮動小数点型の変数に代入されている角度を指定の角度に近づける処理を行う
	// （目標とする辺数に代入されている角度の差が2.0f＊π以内である必要がある
	// 戻り値：変数の角度が指定の角度に達したか
	// param　操作対象の変数アドレス
	// targetParam　目標とする角度
	// stepTime　推移させる時間
	// speed　角度の変化速度
	bool ParamChangeAngle(float* param, float targetParam, float stepTime, float speed);

	// 指定の範囲からランダムで決定した値を取得する（浮動小数点型）
	float GetRandomFloat(float maxF, float minF);

	// 指定の範囲からランダムで決定した値を取得する（整数型）
	int GetRandomInt(int maxI, int minI);

	// 指定の範囲からランダムで決定した値を取得する（浮動小数点型）
	float GetRandomFloatStructure(MinMaxFloat* minMaxF/* 最大値と最小値の情報が入った構造体のアドレス*/);

	// 指定の範囲からランダムで決定した値を取得する（整数型）
	int GetRandomIntStructure(MinMaxInt* minMaxI/* 最大値と最小値の情報が入った構造体のアドレス*/);

};


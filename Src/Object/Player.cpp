#include <string>
#include<EffekseerForDXLib.h>//Effekseer
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/GravityManager.h"
#include "../Manager/Camera.h"
#include "Common/AnimationController.h"
#include "../Manager/EffectManager.h"
#include "../Effect/EffectDeth.h"
#include "Common/Capsule.h"
#include "Common/Collider.h"
#include "Common/SpeechBalloon.h"
#include "Planet.h"
#include "Player.h"

Player::Player(SceneManager* manager)
{
	mSceneManager = manager;
	mResourceManager = manager->GetResourceManager();
	mGravityManager = manager->GetGravityManager();
	effectManager_ = manager->GetEffectManager();

	mAnimationController = nullptr;
	mState = STATE::NONE;
}

void Player::Init(void)
{

	// モデルの基本設定
	mTransform.SetModel(mResourceManager->LoadModelDuplicate(
		ResourceManager::SRC::PLAYER));
	mTransform.scl = AsoUtility::VECTOR_ONE;
	mTransform.pos = { 0.0f, -30.0f, 0.0f };
	mTransform.quaRot = Quaternion();
	mTransform.quaRotLocal = 
		Quaternion::Euler({ 0.0f, AsoUtility::Deg2RadF(180.0f), 0.0f });
	mTransform.Update();

	//丸影画像//
	mImgShadow = LoadGraph("Data/Image/Shadow.png");

	//足煙画像//
	mEffectSmoke = mResourceManager->Load(
		ResourceManager::SRC::FOOT_SMOKE).mHandleId;//

	//ワープエフェクト//
	mEffectWarpOrbit = mResourceManager->Load(
		ResourceManager::SRC::WARP_ORBIT).mHandleId;

	//モデルフレーム//
	mFrameLeftHand = MV1SearchFrame
	(mTransform.modelId, "mixamorig:LeftHand");
	mFrameRightHand = MV1SearchFrame
	(mTransform.modelId, "mixamorig:RightHand");

	//ジャンプ系変数の初期化//
	mIsJump = false;
	mStepJump = 0.0f;
	mJumpPow = AsoUtility::VECTOR_ZERO;

	//カプセルコライダ//
	mCapsule = new Capsule(&mTransform);
	mCapsule->SetRelativePosTop({0.0f,110.0f,0.0f});//
	mCapsule->SetRelativePosDown({0.0f,30.0f,0.0f});//
	mCapsule->SetRadius(20.0f);
	//

	// アニメーションの設定
	InitAnimation();

	//初期状態
	ChangeState(STATE::PLAY);

}

void Player::InitAnimation(void)
{

	std::string path = "Data/Model/Player/";
	mAnimationController = new AnimationController(mSceneManager, mTransform.modelId);
	mAnimationController->Add((int)ANIM_TYPE::IDLE, path + "Idle.mv1", 20.0f);
	mAnimationController->Add((int)ANIM_TYPE::RUN, path + "Run.mv1", 20.0f);
	mAnimationController->Add((int)ANIM_TYPE::FAST_RUN, path + "FastRun.mv1", 20.0f);
	mAnimationController->Add((int)ANIM_TYPE::JUMP, path + "Jump.mv1", 60.0f);
	mAnimationController->Add((int)ANIM_TYPE::WARP_PAUSE, path + "WarpPose.mv1", 60.0f);
	mAnimationController->Add((int)ANIM_TYPE::FLY, path + "Flying.mv1", 60.0f);
	mAnimationController->Add((int)ANIM_TYPE::FALLING, path + "Falling.mv1", 80.0f);
	mAnimationController->Add((int)ANIM_TYPE::VICTORY, path + "Victory.mv1", 60.0f);

	mAnimationController->Play((int)ANIM_TYPE::IDLE);

}

void Player::Update(void)
{

	switch (mState)
	{
	case Player::STATE::NONE:
		break;
	case Player::STATE::PLAY:
		UpdatePlay();
		break;
	case Player::STATE::WARP_RESERVE:
		UpdateWarpReserve();
		break;
	case Player::STATE::WARP_MOVE:
		UpdateWarpMove();
		break;
	case Player::STATE::DEAD:
		break;
	case Player::STATE::VICTORY:
		break;
	case Player::STATE::END:
		break;
	}

	mTransform.Update();
	mAnimationController->Update();
	
}

void Player::UpdatePlay(void)
{

	//移動処理(mMovePow・mMoveDirを作る)
	ProcessMove();

	//ジャンプ処理//
	ProcessJump();

	//傾斜計算//
	CalcSlope();

	//重力による移動計算//
	CalcGravityPow();

	//衝突判定//
	Collision();

	//回転//Transformを加える
	mTransform.quaRot = mGravityManager->GetTransform()->quaRot;
	mTransform.quaRot = mTransform.quaRot.Mult(mPlayerRotY);

	//歩きエフェクト//
	effectManager_->Create(EffectNum::effectDeth_);
	

	//EffectFootSmoke();

}

void Player::Draw(void)
{

	// モデルの描画
	MV1DrawModel(mTransform.modelId);

	//丸影//
	DrawShadow();

	effectManager_->RenderAll();

	// デバッグ用描画
	DrawDebug();

}

void Player::DrawShadow(void)
{

	// プレイヤーの影を描画

	float PLAYER_SHADOW_HEIGHT = 300.0f;//
	float PLAYER_SHADOW_SIZE = 30.0f;//

	int i, j;
	MV1_COLL_RESULT_POLY_DIM HitResDim;
	MV1_COLL_RESULT_POLY* HitRes;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;
	int ModelHandle;

	// ライティングを無効にする
	SetUseLighting(FALSE);

	// Ｚバッファを有効にする
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP にする( テクスチャの端より先は端のドットが延々続く )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// 影を落とすモデルの数だけ繰り返し
	for (auto c : mColliders)
	{
		// チェックするモデルは、jが0の時はステージモデル、1以上の場合はコリジョンモデル
		ModelHandle = c->mModelId;


		// プレイヤーの直下に存在する地面のポリゴンを取得
		HitResDim = MV1CollCheck_Capsule(
			ModelHandle, -1,
			//pl.Position
			mTransform.pos, VAdd(mTransform.pos,
				VGet(0.0f, -PLAYER_SHADOW_HEIGHT, 0.0f)),
			PLAYER_SHADOW_SIZE);

		// 頂点データで変化が無い部分をセット
		Vertex[0].dif = GetColorU8(255, 255, 255, 255);
		Vertex[0].spc = GetColorU8(0, 0, 0, 0);
		Vertex[0].su = 0.0f;
		Vertex[0].sv = 0.0f;
		Vertex[1] = Vertex[0];
		Vertex[2] = Vertex[0];

		// 球の直下に存在するポリゴンの数だけ繰り返し
		HitRes = HitResDim.Dim;
		for (i = 0; i < HitResDim.HitNum; i++, HitRes++)
		{
			// ポリゴンの座標は地面ポリゴンの座標
			Vertex[0].pos = HitRes->Position[0];
			Vertex[1].pos = HitRes->Position[1];
			Vertex[2].pos = HitRes->Position[2];

			// ちょっと持ち上げて重ならないようにする
			SlideVec = VScale(HitRes->Normal, 0.5f);
			Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
			Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
			Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

			// ポリゴンの不透明度を設定する
			Vertex[0].dif.a = 0;
			Vertex[1].dif.a = 0;
			Vertex[2].dif.a = 0;

			
			if (HitRes->Position[0].y > mTransform.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[0].dif.a = 128 * (1.0f - fabs(HitRes->Position[0].y - mTransform.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[1].y > mTransform.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[1].dif.a = 128 * (1.0f - fabs(HitRes->Position[1].y - mTransform.pos.y) / PLAYER_SHADOW_HEIGHT);

			if (HitRes->Position[2].y > mTransform.pos.y - PLAYER_SHADOW_HEIGHT)
				Vertex[2].dif.a = 128 * (1.0f - fabs(HitRes->Position[2].y - mTransform.pos.y) / PLAYER_SHADOW_HEIGHT);

			// ＵＶ値は地面ポリゴンとプレイヤーの相対座標から割り出す
			Vertex[0].u = (HitRes->Position[0].x - mTransform.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[0].v = (HitRes->Position[0].z - mTransform.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].u = (HitRes->Position[1].x - mTransform.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[1].v = (HitRes->Position[1].z - mTransform.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].u = (HitRes->Position[2].x - mTransform.pos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			Vertex[2].v = (HitRes->Position[2].z - mTransform.pos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(Vertex, 1, mImgShadow, TRUE);
		}

		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(HitResDim);
	}

	// ライティングを有効にする
	SetUseLighting(TRUE);

	// Ｚバッファを無効にする
	SetUseZBuffer3D(FALSE);


}

void Player::DrawDebug(void)
{

	int white = 0xffffff;
	int black = 0x000000;
	int red = 0xff0000;
	int green = 0x00ff00;
	int blue = 0x0000ff;
	int yellow = 0xffff00;
	int purpl = 0x800080;

	VECTOR v;

	// キャラ基本情報
	//-------------------------------------------------------
	// キャラ座標
	v = mTransform.pos;
	DrawFormatString(20, 60, black, "キャラ座標 ： (%0.2f, %0.2f, %0.2f)",
		v.x, v.y, v.z
	);
	//-------------------------------------------------------
	//衝突判定(重力)//別になくてもよい
	DrawLine3D(mGravHitUp, mGravHitDown, 0x000000);

	//カプセルコライダ//
	mCapsule->Draw();

	//傾斜角//
	DrawFormatString(20, 80, black, "傾斜角     ：  %0.2f",
		mSlopeAngleDeg
	);

}

void Player::Release(void)
{
	delete mCapsule;//
}

void Player::ProcessMove(void)
{

	//mMovePow・mMoveDirを作る
	//移動量をゼロ
	mMovePow = AsoUtility::VECTOR_ZERO;

	//X軸を除いた、カメラ角度（XY平面）を取得
	Quaternion cameraRot = mSceneManager->GetCamera()->GetQuaRotOutX();

	//回転したい方向
	double rotRad = 0.0f;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	//カメラの方向に進みたい//
	if (CheckHitKey(KEY_INPUT_W))
	{
		dir = cameraRot.GetForward();
		rotRad += AsoUtility::Deg2RadF(0.0f);
	}
	if (CheckHitKey(KEY_INPUT_S))
	{
		dir = cameraRot.GetBack();
		rotRad += AsoUtility::Deg2RadF(180.0f);
	}
	if (CheckHitKey(KEY_INPUT_A))
	{
		dir = cameraRot.GetLeft();
		rotRad += AsoUtility::Deg2RadF(270.0f);
	}
	if (CheckHitKey(KEY_INPUT_D))
	{
		dir = cameraRot.GetRight();
		rotRad += AsoUtility::Deg2RadF(90.0f);
	}

	//-モーション------------------------------------
	//ボタンが押されたら移動量を作る//
	if (!AsoUtility::EqualsVZero(dir) && (mIsJump || IsEndLanding()))
	{
		//
		if (!mIsJump && (IsEndLanding()))
		{
			if (CheckHitKey(KEY_INPUT_RSHIFT))
			{
				mSpeed = SPEED_RUN;
				//早く走るモーション
				mAnimationController->Play((int)ANIM_TYPE::FAST_RUN);
			}
			else
			{
				mSpeed = SPEED_MOVE;
				//走るモーション
				mAnimationController->Play((int)ANIM_TYPE::RUN);
			}
		}

		//移動量(mMovePow)=VScale(方向,スピード)
		mMovePow = VScale(dir, mSpeed);

		//回転処理・向かせたい回転を設定する
		//mGoalQuaRotを設定する関数
		SetGoalRotate(rotRad);
	}
	else
	{

		if (!mIsJump && (IsEndLanding()))
		{
			//待機モーション
			mAnimationController->Play((int)ANIM_TYPE::IDLE);
		}

	}

	//移動方向に応じた回転を自動的に行う
	//mGoalQuaRotをもとに球面補完を行う
	//mPlayerRotYを設定する

	Rotate();

}

void Player::ProcessJump(void)//
{

	/*
	①ジャンプボタンが押されたらジャンプする
	②mJumpPowを作る
	③ジャンプの入力受付時間を作る
	④アニメーションもつける
	*/
	bool isHitKey = CheckHitKey(KEY_INPUT_BACKSLASH);
	if (isHitKey && (mIsJump || IsEndLanding()))
	{
		
		if (mIsJump)
		{
			//ジャンプアニメーション//
			//mAnimationController->Play((int)ANIM_TYPE::JUMP);
			//ループしない//
			//mAnimationController->Play((int)ANIM_TYPE::JUMP,false);
			//切り取りアニメーション//
			//mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 25.0f);

			//切り取りつつ、再生が終わったら別のループ再生//
			mAnimationController->Play((int)ANIM_TYPE::JUMP, false, 13.0f, 25.0f);
			mAnimationController->SetEndLoop(23.0f, 25.0f, 0.5f);
		}
		//ジャンプの入力時間を減らす//
		mStepJump += mSceneManager->GetDeltaTime();
		if (mStepJump < TIME_JUMP_IN)
		{
			mJumpPow = VScale(mGravityManager->GetDirUpGravity(), POW_JUMP);
		}
			//ジャンプフラグ
			mIsJump = true;
	}

	//if (!isHitKey)
	//{
	//	//ボタンをはなしたらジャンプ力に加算しない//
	//}


}

void Player::SetGoalRotate(double rotRad)
{
	VECTOR cameraRot = mSceneManager->GetCamera()->GetAngles();
	Quaternion axis = Quaternion::AngleAxis(
		(double)cameraRot.y + rotRad, AsoUtility::AXIS_Y);
	
	//現在のゴール回転と新たなゴール回転の角度差(Deg)を取る
	double annggleDiff = Quaternion::Angle(axis, mGoalQuaRotY);

	//しきい値でリセットするか判断----------------------------課題？
	if (annggleDiff > 0.1)
	{
		//回転時間をリセット
		mStepRotTime = TIME_ROT;

	}
	//回転情報を更新
	mGoalQuaRotY = axis;

}

void Player::Rotate(void)
{

	mStepRotTime -= mSceneManager->GetDeltaTime();
	mPlayerRotY = Quaternion::Slerp(
		mPlayerRotY, mGoalQuaRotY, (TIME_ROT - mStepRotTime) / TIME_ROT);

}

void Player::CalcSlope(void)//
{

	//重力の反対方向//
	VECTOR  gravityUp = mGravityManager->GetDirUpGravity();

	//重力の反対方向から、衝突した地面の法線方向に向けた回転量を計算//
	Quaternion up2GNorQua =
		Quaternion::FromToRotation(gravityUp, mHitNormal);

	//取得した回転量の軸と角度を計算する//
	float angle;
	float* anglePtr = &angle;
	VECTOR axis;
	up2GNorQua.ToAngleAxis(anglePtr, &axis);

	//90度足して、傾斜ベクトルへの回転を取得する
	Quaternion slopeQ = Quaternion::AngleAxis(
		angle + AsoUtility::Deg2RadD(90.0), axis);

	//地面の傾斜線//
	mSlopeDir = slopeQ.PosAxis(gravityUp);

	//傾斜の角度//
	mSlopeAngleDeg = AsoUtility::AngleDeg(gravityUp, mSlopeDir);

	//傾斜によるキャラクターの移動//
	if (AsoUtility::SqrMagnitude(mJumpPow) == 0.0f)
	{
		float CHECK_ANGLE = 120.0f;
		if (mSlopeAngleDeg >= CHECK_ANGLE)
		{
			float diff = abs(mSlopeAngleDeg - CHECK_ANGLE);
			mSlopePow = VScale(mSlopeDir, diff / 3.0f);
			mMovePow = VAdd(mMovePow, mSlopePow);
		}
	}

}

void Player::CalcGravityPow(void)
{

	//mJumpPowを作る//
	//重力方向//
	VECTOR dirGravity = mGravityManager->GetDirGravity();

	//重力の強さ//
	float gravityPow = mGravityManager->GetPower();

	//mJumpPowを作る処理=重力を作る//
	VECTOR gravity = VScale(dirGravity,gravityPow);
	mJumpPow = VAdd(mJumpPow, gravity);

	//内積を使ってジャンプか落下かを見分ける//
	float dot = VDot(dirGravity, mJumpPow);
	if (dot >= 0.0f)
	{
		//落下
		//重力方向なら、ジャンプ力をなくす
		mJumpPow = gravity;
	
	}

}

Transform* Player::GetTransform(void)
{
	return &mTransform;
}

void Player::AddCollider(Collider* collider)
{
	mColliders.push_back(collider);//メインステージの衝突判定をする
}

void Player::ClearCollider(void)
{
	mColliders.clear();
}

void Player::UpdateWarpReserve(void)//
{

	mStepWarp -= mSceneManager->GetDeltaTime();
	if (mStepWarp < 0.0f)
	{
		mTransform.quaRot = mWarpQua; mTransform.pos = mWarpReservePos;
		ChangeState(STATE::WARP_MOVE);
		return;
	}
	else
	{
		float t = 1.0f - (mStepWarp / mTimeWarp);
		mTransform.quaRot = Quaternion::Slerp(mReserveStartQua, mWarpQua, t);
		mTransform.pos = AsoUtility::Lerp(mReserveStartPos, mWarpReservePos, t);
	}

}

void Player::UpdateWarpMove(void)//
{
	VECTOR dir = mWarpQua.GetForward();
	mTransform.pos =
		VAdd(mTransform.pos, VScale(dir, 30.0f));
	Stage::NAME name = mGravityManager->GetActivePlanet()->GetName();
	if (name != mPreWarpName)
	{
		// エフェクト停止
		StopOrbitEffect();
		// 落下アニメーション
		mAnimationController->Play((int)ANIM_TYPE::JUMP, true, 13.0f, 25.0f);
		mAnimationController->SetEndLoop(23.0f, 25.0f, 5.0f);
		ChangeState(Player::STATE::PLAY);
		return;
	}
	mTransform.Update();

	// エフェクトの位置を同期//
	SyncWarpOrbitPos();

}

Capsule* Player::GetCapsule(void)//
{
	return mCapsule;
}

bool Player::IsPlay(void)//
{
	return mState == STATE::PLAY;
}

bool Player::IsWarpMove(void)//
{
	return mState == STATE::WARP_MOVE;
}

void Player::StartWarp(float time, Quaternion goalRot, VECTOR goalPos)//
{
	mTimeWarp = time;
	mStepWarp = time;
	mWarpQua = goalRot;
	mWarpReservePos = goalPos;
	mPreWarpName = mGravityManager->GetActivePlanet()->GetName();
	ChangeState(STATE::WARP_RESERVE);
}

void Player::ChangeState(STATE state)
{

	mState = state;
	switch (mState)
	{
	case Player::STATE::NONE:
		break;
	case Player::STATE::PLAY:
		break;
	case Player::STATE::WARP_RESERVE:
		//--------
		mJumpPow = AsoUtility::VECTOR_ZERO;
		// ワープ準備開始時のプレイヤー情報
		mReserveStartQua = mTransform.quaRot;
		mReserveStartPos = mTransform.pos;
		mAnimationController->Play((int)Player::ANIM_TYPE::WARP_PAUSE);
		//---------
		break;
	case Player::STATE::WARP_MOVE:
		//--------
		// 正面を向いているはずなのでリセット
		mPlayerRotY = Quaternion();
		mGoalQuaRotY = Quaternion();
		mAnimationController->Play((int)Player::ANIM_TYPE::FLY);
		// エフェクト再生
		EffectWarpOrbit();
		//-------
		break;
	case Player::STATE::DEAD:
		break;
	case Player::STATE::VICTORY:
		break;
	case Player::STATE::END:
		break;
	}

}

void Player::Collision(void)
{

	//現在座標から移動後座標を求める
	mMovedPos = VAdd(mTransform.pos, mMovePow);

	//衝突判定(カプセル)//
	CollisionCapsule();

	//衝突判定(重力)//
	CollisionGravity();

	//移動//
	mMoveDiff = VSub(mMovedPos, mTransform.pos);//差分計算
	mTransform.pos = mMovedPos;

}

void Player::CollisionCapsule(void)
{
	//カプセルを移動後座標に移動させる//
	Transform trans = Transform(&mTransform);
	trans.pos = mMovedPos;
	trans.Update();
	Capsule cap = mCapsule->Copy(&trans);

	//カプセルと地面の衝突//
	for (auto c : mColliders)
	{
		auto hits = MV1CollCheck_Capsule(
			c->mModelId,-1,
			cap.GetPosDown(),cap.GetPosTop(),cap.GetRadius()
		);
		//ステージ全体//
		//MV1CollCheck_Capsule(衝突させたいモデル、衝突させたいフレーム(全部の場合は-1)、xpos,ypos,zpos)
		
		//細かい衝突判定//
		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			for (int tryCnt = 0; tryCnt < 10; tryCnt++)
			{
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosDown(), cap.GetPosTop(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]
				);

				if (pHit)
				{
					mMovedPos = VAdd(mMovedPos, VScale(hit.Normal, 1.0f));//fは移動させる値//
					//カプセルを移動//
					trans.pos = mMovedPos;
					trans.Update();
					cap = mCapsule->Copy(&trans);
					continue;
				}
				break;

			}

		}

		//検出した衝突情報を後始末//
		MV1CollResultPolyDimTerminate(hits);

	}


}

void Player::CollisionGravity(void)
{

	//ジャンプ量を移動後座標に加算//
	mMovedPos = VAdd(mMovedPos, mJumpPow);
	
	//重力方向//
	VECTOR dirGravity = mGravityManager->GetDirGravity();//重力
	VECTOR dirUpGravity = mGravityManager->GetDirUpGravity();//反対方向
	
	//重力の強さ//
	float gravityPow = mGravityManager->GetPower();
	
	float checkPow = 10.0f;
	mGravHitUp = VAdd(mMovedPos, VScale(dirUpGravity, gravityPow));
	mGravHitUp = VAdd(mGravHitUp, VScale(dirUpGravity, checkPow * 2.0f));
	mGravHitDown = VAdd(mMovedPos, VScale(dirGravity, checkPow));

	//地面との衝突//c　Collider
	for (auto c : mColliders)
	{
		auto hit = MV1CollCheck_Line(c->mModelId, -1, mGravHitUp, mGravHitDown);
		if (hit.HitFlag > 0)
		{

			//傾斜計算用に衝突判定を保存しておく//
			mHitNormal = hit.Normal;
			mHitPos = hit.HitPosition;

			//衝突地点から、少し上に移動//
			mMovedPos = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));

			//ジャンプリセット//
			mJumpPow = AsoUtility::VECTOR_ZERO;
			mStepJump = 0.0f;//

			if (mIsJump)//
			{
				//着地モーション//
				mAnimationController->Play((int)ANIM_TYPE::JUMP,
					false, 29.0f, 45.0f, false, true);
			}

			mIsJump = false;
		}
	}


	//ジャンプ量を移動後座標に追加
	mMovedPos = VAdd(mMovedPos, mMovePow);

}

bool Player::IsEndLanding(void)
{

	bool ret = true;

	if (mAnimationController->GetPlayType() != (int)ANIM_TYPE::JUMP)
	{
		return ret;
	}
	if (mAnimationController->IsEnd())
	{
		return ret;
	}

	return false;
}

void Player::EffectFootSmoke(void)
{

	int playerHandle = -1;

	float len = AsoUtility::SqrMagnitude(mMoveDiff);

	//if (!mIsJump)//課題//ジャンプしていないときだけに足煙//-----------------
	//ジャンプ中に煙が出ている*if (mIsJump == false)*//課題?
	if (mIsJump == false)
	{

		//if (mStepFootSmoke % 50 == 0 && len >= 1.0f);
		//{

		//	//エフェクトを再生する//
		//	playerHandle = PlayEffekseer3DEffect(mEffectSmoke);

		//	//エフェクトの大きさを変える//
		//	SetScalePlayingEffekseer3DEffect(playerHandle, 5.0f, 5.0f, 5.0f);

		//	//エフェクトの位置を変える//
		//	SetPosPlayingEffekseer3DEffect(
		//		playerHandle, mTransform.pos.x, mTransform.pos.y, mTransform.pos.z);

		//}
		if (mStepFootSmoke % 140 == 0 && len >= 1.0f);
		{

			//エフェクトを再生する//
			playerHandle = PlayEffekseer3DEffect(mEffectSmoke);

			//エフェクトの大きさを変える//
			SetScalePlayingEffekseer3DEffect(playerHandle, 15.0f, 15.0f, 15.0f);

			//エフェクトの位置を変える//
			SetPosPlayingEffekseer3DEffect(
				playerHandle, mTransform.pos.x, mTransform.pos.y, mTransform.pos.z);

		}

		mStepFootSmoke++;
	}

}

void Player::EffectWarpOrbit(void)//
{
	// エフェクト再生
	mHandleWarpOrbitL = PlayEffekseer3DEffect(mEffectWarpOrbit);
	mHandleWarpOrbitR = PlayEffekseer3DEffect(mEffectWarpOrbit);
	// 大きさ
	SetScalePlayingEffekseer3DEffect(mHandleWarpOrbitL, 10.0f, 10.0f, 10.0f);
	SetScalePlayingEffekseer3DEffect(mHandleWarpOrbitR, 10.0f, 10.0f, 10.0f);
	// エフェクトの位置
	SyncWarpOrbitPos();
	// エフェクトの回転
	VECTOR euler = mTransform.quaRot.ToEuler();
	SetRotationPlayingEffekseer3DEffect(
		mHandleWarpOrbitL, euler.x, euler.y, euler.z);
	SetRotationPlayingEffekseer3DEffect(
		mHandleWarpOrbitR, euler.x, euler.y, euler.z);
}

void Player::SyncWarpOrbitPos(void)
{
	VECTOR pos;
	pos = MV1GetFramePosition(mTransform.modelId, mFrameLeftHand); 
	SetPosPlayingEffekseer3DEffect(mHandleWarpOrbitL, pos.x, pos.y, pos.z);
	pos = MV1GetFramePosition(mTransform.modelId, mFrameRightHand);
	SetPosPlayingEffekseer3DEffect(mHandleWarpOrbitR, pos.x, pos.y, pos.z);
}

void Player::StopOrbitEffect(void)
{
	StopEffekseer3DEffect(mHandleWarpOrbitL);
	StopEffekseer3DEffect(mHandleWarpOrbitR);
}

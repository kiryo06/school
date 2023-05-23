﻿// 2016 Takeru Yui All Rights Reserved.
#include "Player.h"
#include "Camera.h"

/// <summary>
/// コンストラクタ
/// </summary>
Camera::Camera()
{
	//奥行0.1～1000までをカメラの描画範囲とする
	SetCameraNearFar(0.1f, 1000.0f);

	pos = VGet(0, 0, 0);
}

/// <summary>
/// デストラクタ
/// </summary>
Camera::~Camera()
{
	// 処理なし.
}

/// <summary>
/// 更新
/// </summary>
void Camera::Update(const Player& player, float chipSize)
{
	// 大きさ0.5のマップチップを15個配置する
	// プレイヤーの地面のY位置を0としたいので、地上部のマップチップの数は14個
	// その真ん中に表示するので半分を計算で出す
	// プレイヤーのX座標には追従したいのでplayerのXを使う
	VECTOR playerPos = player.GetPos();
	VECTOR cameraPos = VGet(playerPos.x, chipSize * 14.0f * 0.5f, playerPos.z - 10.0f);

	// 注視する視点は、カメラと平行にまっすぐz=0地点
	VECTOR lookPos = VGet(cameraPos.x, cameraPos.y, 0.0f);

	pos = cameraPos;

	// カメラに位置を反映.
	SetCameraPositionAndTarget_UpVecY(pos, lookPos);
}
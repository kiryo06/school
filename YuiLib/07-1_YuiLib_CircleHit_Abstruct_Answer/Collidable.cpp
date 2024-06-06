﻿// 2024 Takeru Yui All Rights Reserved.
#include "DxLib.h"
#include "YuiLib.h"

using namespace YuiLib;

/// <summary>
/// コンストラクタ
/// </summary>
Collidable::Collidable(Priority priority, GameObjectTag tag)
	: priority	(priority)
	, tag		(tag)
{
	// 処理なし
}
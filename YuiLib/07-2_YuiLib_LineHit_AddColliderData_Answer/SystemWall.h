﻿#pragma once

class Physics;
class Collidable;
class YuiLib::ColliderDataCircle2D;

/// <summary>
/// 敵クラス。物理・衝突判定をするため、Collidableを継承
/// </summary>
class SystemWall : public YuiLib::Collidable
{
public:
	SystemWall();
	void Initialize(YuiLib::Physics* physics);
	void Finalize(YuiLib::Physics* physics);
	void Update();

	// 衝突したとき
	void OnCollide(const Collidable& colider) override;

private:
	// 線の当たり判定型
	YuiLib::ColliderDataLine2D* lineColliderData;
};
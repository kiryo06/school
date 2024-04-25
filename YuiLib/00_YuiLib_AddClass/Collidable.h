// 2024 Takeru Yui All Rights Reserved.
#pragma once

namespace YuiLib {

class Rigidbody;

/// <summary>
/// 衝突できるもの
/// </summary>
class Collidable
{
public:
	void OnCollide();	// 衝突したとき

private:
	Rigidbody rigidbody;
};

}


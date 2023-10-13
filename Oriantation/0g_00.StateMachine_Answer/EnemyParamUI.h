#pragma once

#include <functional>

class EnemyParam;
class EnemyParamModel;
class EnemyUiHp;

// OnDamage関数の関数ポインタ定義
typedef std::function<void(int)> OnDamageDelegate;

/// <summary>
/// EnemyのUIとりまとめ (Controller, Presenter)
/// モデルであるEnemyParamを実質コントロール可能だが、直接の書き換えは行わない
/// </summary>
class EnemyParamUI
{
public:
	const EnemyParam& GetParam() const { return paramModel.GetParam(); }
	void Draw();
	void OnDamage(int damage);

private:
	EnemyParamModel paramModel;
	EnemyUiHp hpUi;
};


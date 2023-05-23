// 2023 Takeru Yui All Rights Reserved.
#include "EnemyManager.h"
#include "EnemyBase.h"
#include "EnemyNormal.h"

// �䂭�䂭�͑S�ĊO���t�@�C�����I
const int EnemyLifeSoft = 30;
const int EnemyLifeNormal = 40;
const int EnemyLifeHard = 50;
const char* EnemyGlaphPath1 = "data/texture/EpicEnemy.png";
const char* EnemyGlaphPath2 = "data/texture/EpicEnemy2.png";
const char* EnemyGlaphPath3 = "data/texture/EpicEnemy3.png";

const EnemyBase::Data dataList[] =
{
	{EnemyGlaphPath1,0, 50, EnemyLifeSoft, 3, 0},
	{EnemyGlaphPath2,0, 100, EnemyLifeNormal, 2, 0},
	{EnemyGlaphPath3,0, 150, EnemyLifeHard, 4, 10},
};

/// <summary>
/// ������
/// </summary>
void EnemyManager::Init()
{
	for (const auto& data : dataList)
	{
		EnemyBase* enemy = new EnemyNormal();
		enemy->Init(data);
		enemies.push_back(enemy);
	}
}

/// <summary>
/// �X�V
/// </summary>
void EnemyManager::Update()
{
	for (auto item : enemies)
	{
		item->Update();
	}
}

/// <summary>
/// �`��
/// </summary>
void EnemyManager::Draw()
{
	for (auto item : enemies)
	{
		item->Draw();
	}
}

/// <summary>
/// �I��
/// </summary>
void EnemyManager::Finalize()
{
	for (auto item : enemies)
	{
		delete item;
		item = nullptr;
	}
}
#include "DxLib.h"
#include "BackGround3D.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
BackGround3D::BackGround3D()
	: modelHandle(-1)
{
	// ���f�����[�h
	modelHandle = MV1LoadModel("data/model/obstructStatic/obstructStatic.pmd");
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
BackGround3D::~BackGround3D()
{
	MV1DeleteModel(modelHandle);// ���f���̃A�����[�h
}

/// <summary>
/// �X�V
/// </summary>
void BackGround3D::Update()
{
	MV1SetPosition(modelHandle, VGet(0, 0, 0));
}

/// <summary>
/// �`��
/// </summary>
void BackGround3D::Draw()
{
	MV1DrawModel(modelHandle);
}
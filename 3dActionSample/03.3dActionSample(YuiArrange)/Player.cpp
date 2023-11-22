#include <math.h>
#include "DxLib.h"
#include "Camera.h"
#include "Input.h"
#include "Stage.h"
#include "Player.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
Player::Player()
	:  Position				(VGet(0.0f, 0.0f, 0.0f))
	 , TargetMoveDirection	(VGet(1.0f, 0.0f, 0.0f))
	 , Angle				(0.0f)
	 , CurrentJumpPower		(0.0f)
	 , ModelHandle			(-1)
	 , ShadowHandle			(-1)
	 , CurrentState			(State::STAND)
	 , CurrentPlayAnim		(-1)
	 , CurrentAnimCount		(0)
	 , PrevPlayAnim			(-1)
	 , PrevAnimCount		(0)
	 , AnimBlendRate		(1.0f)
	 , IsMove				(false)
{
	// �����Ȃ�
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Player::~Player()
{
	Unload();
}

/// <summary>
/// ������
/// </summary>
void Player::Load()
{
	// ���f���̓ǂݍ���
	ModelHandle = MV1LoadModel("DxChara.x");

	// �e�`��p�̉摜�̓ǂݍ���
	ShadowHandle = LoadGraph("Shadow.tga");

	// �A�j���[�V�����̃u�����h����������
	AnimBlendRate = 1.0f;

	// ������Ԃł̓A�j���[�V�����̓A�^�b�`����Ă��Ȃ��ɂ���
	CurrentPlayAnim = -1;
	PrevPlayAnim = -1;

	// ���������Ă���A�j���[�V�������Đ�
	PlayAnim(AnimKind::STOP);
}

/// <summary>
/// �A�����[�h
/// </summary>
void Player::Unload()
{
	// ���f���̍폜
	if (ShadowHandle >= 0)
	{
		MV1DeleteModel(ModelHandle);
		ModelHandle = -1;
	}
	// �e�p�摜�̍폜
	if (ShadowHandle >= 0)
	{
		DeleteGraph(ShadowHandle);
		ShadowHandle = -1;
	}
}

/// <summary>
/// �X�V
/// </summary>
void Player::Update(const Input& input, const Camera& camera, Stage& stage)
{
	// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
	DisableRootFrameZMove();

	// �p�b�h���͂ɂ���Ĉړ��p�����[�^��ݒ肷��
	VECTOR	UpMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR	LeftMoveVec;	// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR	MoveVec;		// ���̃t���[���̈ړ��x�N�g��
	State prevState = CurrentState;
	CurrentState = UpdateMoveParameterWithPad(input, camera, UpMoveVec, LeftMoveVec, MoveVec);

	// �A�j���[�V�����X�e�[�g�̍X�V
	UpdateAnimationState(prevState);

	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	UpdateAngle();

	// �ړ��x�N�g�������ɃR���W�������l�����v���C���[���ړ�
	Move(MoveVec, stage);

	// �A�j���[�V��������
	UpdateAnimation();
}

/// <summary>
/// �`��
/// </summary>
void Player::Draw(const Stage& stage)
{
	MV1DrawModel(ModelHandle);
	DrawShadow(stage);
}

/// <summary>
/// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
/// </summary>
void Player::DisableRootFrameZMove()
{
	// HACK:
	// �EDX���C�u�����̃��f���t�@�C�����ɂ́A�����̃��b�V���i�|���S���̏W���j��J�����A���C�g�����邱�Ƃ��ł���
	// �E�u���������̃t�@�C���́A�e�q�֌W��������AUnity�̃q�G�����L�[�݂����ɁA�K�w�\��������
	// �E���̊K�w���ꂼ��ɂ͖��O���t������ Root-Meshes-Model1
	//											         |-Model2
	// �E���̖��O�̕t�����K�w�̂��Ƃ�DX���C�u�����ł̓t���[���Ƃ���
	// �E��Ԑe�̊K�w���u���[�g�t���[���v�ƌĂԁB���[�g�t���[���͈��
	// 
	// HACK: ���̂��߂ɁH���f���̈�Ԑe�t���[���i�e�K�w�j��Z�������̈ړ��p�����[�^���[���ɂ��Ă���

	MATRIX LocalMatrix;

	// ���[�U�[�s�����������
	MV1ResetFrameUserLocalMatrix(ModelHandle, 2);

	// ���݂̃��[�g�t���[���̍s����擾����
	LocalMatrix = MV1GetFrameLocalMatrix(ModelHandle, 2);

	// �y�������̕��s�ړ������𖳌��ɂ���
	LocalMatrix.m[3][2] = 0.0f;

	// ���[�U�[�s��Ƃ��ĕ��s�ړ������𖳌��ɂ����s������[�g�t���[���ɃZ�b�g����
	MV1SetFrameUserLocalMatrix(ModelHandle, 2, LocalMatrix);
}

/// <summary>
/// �p�b�h���͂ɂ���Ĉړ��p�����[�^��ݒ肷��
/// </summary>
Player::State Player::UpdateMoveParameterWithPad(const Input& input, const Camera& camera, VECTOR& UpMoveVec, VECTOR& LeftMoveVec, VECTOR& MoveVec)
{
	State nextState = CurrentState;

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���̓J�����̎�����������x�����𔲂�������
	UpMoveVec = VSub(camera.GetTarget(), camera.GetEye());
	UpMoveVec.y = 0.0f;

	// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���͏���������Ƃ��̕����x�N�g���Ƃx���̃v���X�����̃x�N�g���ɐ����ȕ���
	LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));

	// ��̃x�N�g���𐳋K��( �x�N�g���̒������P�D�O�ɂ��邱�� )
	UpMoveVec = VNorm(UpMoveVec);
	LeftMoveVec = VNorm(LeftMoveVec);

	// ���̃t���[���ł̈ړ��x�N�g����������
	MoveVec = VGet(0.0f, 0.0f, 0.0f);

	// �ړ��������ǂ����̃t���O��������Ԃł́u�ړ����Ă��Ȃ��v��\��FALSE�ɂ���
	bool IsPressMoveButton = false;

	// �p�b�h�̂R�{�^���ƍ��V�t�g���ǂ����������Ă��Ȃ�������v���C���[�̈ړ�����
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (input.GetNowFrameInput() & PAD_INPUT_C) == 0)
	{
		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�č������Ɉړ�����
		if (input.GetNowFrameInput() & PAD_INPUT_LEFT)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveVec = VAdd(MoveVec, LeftMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			IsPressMoveButton = true;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�ĉE�����Ɉړ�����
			if (input.GetNowFrameInput() & PAD_INPUT_RIGHT)
			{
				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
				MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));

				// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
				IsPressMoveButton = true;
			}

		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă�������Ɉړ�����
		if (input.GetNowFrameInput() & PAD_INPUT_UP)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			MoveVec = VAdd(MoveVec, UpMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			IsPressMoveButton = true;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̕����Ɉړ�����
			if (input.GetNowFrameInput() & PAD_INPUT_DOWN)
			{
				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
				MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));

				// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
				IsPressMoveButton = true;
			}

		// �v���C���[�̏�Ԃ��u�W�����v�v�ł͂Ȃ��A���{�^���P��������Ă�����W�����v����
		if (CurrentState != State::JUMP && (input.GetNowFrameNewInput() & PAD_INPUT_A))
		{
			// �x�������̑��x���Z�b�g
			CurrentJumpPower = JumpPower;

			// ��Ԃ��u�W�����v�v�ɂ���
			nextState = State::JUMP;
		}
	}

	// �W�����v��ԂȂ�d�͓K�p
	if (CurrentState == State::JUMP)
	{
		// �x�������̑��x���d�͕����Z����
		CurrentJumpPower -= Gravity;
	}

	// �ړ��{�^���������ꂽ���ǂ����ŏ����𕪊�
	if (IsPressMoveButton)
	{
		// �������܂Łu�����~�܂�v��Ԃ�������
		if (CurrentState == State::STAND)
		{
			// ��Ԃ��u����v�ɂ���
			nextState = State::RUN;
		}

		// �ړ��x�N�g���𐳋K���������̂��v���C���[�������ׂ������Ƃ��ĕۑ�
		TargetMoveDirection = VNorm(MoveVec);

		// �v���C���[�������ׂ������x�N�g�����v���C���[�̃X�s�[�h�{�������̂��ړ��x�N�g���Ƃ���
		MoveVec = VScale(TargetMoveDirection, MoveSpeed);
	}
	else
	{
		// ���̃t���[���ňړ����Ă��Ȃ��āA����Ԃ��u����v��������
		if (CurrentState == State::RUN)
		{
			// ��Ԃ��u�����~��v�ɂ���
			nextState = State::STAND;
		}
	}

	// �ړ��x�N�g���̂x�������x�������̑��x�ɂ���
	MoveVec.y = CurrentJumpPower;

	return nextState;
}

/// <summary>
/// �ړ�����
/// </summary>
void Player::Move(const VECTOR& MoveVector, Stage& stage)
{
	// HACK: �ړ�������0.01�����Ŕ����Ɉړ����Ă����ꍇ�͂�����ړ����ăo�O��
	// x����y�������� 0.01f �ȏ�ړ������ꍇ�́u�ړ������v�t���O���P�ɂ���
	if (fabs(MoveVector.x) > 0.01f || fabs(MoveVector.z) > 0.01f)
	{
		IsMove = true;
	}
	else
	{
		IsMove = false;
	}

	// �����蔻������āA�V�������W��ۑ�����
	VECTOR OldPos = Position;							// �ړ��O�̍��W	
	VECTOR NextPos = VAdd(Position, MoveVector);		// �ړ���̍��W

	Position = stage.CheckCollision(*this, NextPos, MoveVector);

	// �v���C���[�̃��f���̍��W���X�V����
	MV1SetPosition(ModelHandle, Position);
}

/// <summary>
/// �A�j���[�V�����X�e�[�g�̍X�V
/// </summary>
void Player::UpdateAnimationState(State prevState)
{
	// �����~�܂肩�瑖��ɕς������
	if (prevState == State::STAND && CurrentState == State::RUN)
	{
		// ����A�j���[�V�������Đ�����
		PlayAnim(AnimKind::RUN);
	}
	// ���肩�痧���~�܂�ɕς������
	else if (prevState == State::RUN && CurrentState == State::STAND)
	{
		// �����~��A�j���[�V�������Đ�����
		PlayAnim(AnimKind::STOP);
	}
	// ��Ԃ��u�W�����v�v�̏ꍇ��
	else if (CurrentState == State::JUMP)
	{
		// �����������Ă��Ċ��Đ�����Ă���A�j���[�V�������㏸���p�̂��̂������ꍇ��
		if (CurrentJumpPower < 0.0f && MV1GetAttachAnim(ModelHandle, CurrentPlayAnim) == 2)
		{
			// �������悤�̃A�j���[�V�������Đ�����
			PlayAnim(AnimKind::JUMP);
		}
	}
}

/// <summary>
/// �v���C���[�̉�]����
/// </summary>
void Player::UpdateAngle()
{
	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	float TargetAngle;			// �ڕW�p�x
	float difference;			// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	TargetAngle = static_cast<float>(atan2(TargetMoveDirection.x, TargetMoveDirection.z));

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	// �ŏ��͒P���Ɉ����Z
	difference = TargetAngle - Angle;

	// ����������炠������̍����P�W�O�x�ȏ�ɂȂ邱�Ƃ͖����̂�
	// ���̒l���P�W�O�x�ȏ�ɂȂ��Ă�����C������
	if (difference < -DX_PI_F)
	{
		difference += DX_TWO_PI_F;
	}
	else if (difference > DX_PI_F)
	{
		difference -= DX_TWO_PI_F;
	}

	// �p�x�̍����O�ɋ߂Â���
	if (difference > 0.0f)
	{
		// �����v���X�̏ꍇ�͈���
		difference -= AngleSpeed;
		if (difference < 0.0f)
		{
			difference = 0.0f;
		}
	}
	else
	{
		// �����}�C�i�X�̏ꍇ�͑���
		difference += AngleSpeed;
		if (difference > 0.0f)
		{
			difference = 0.0f;
		}
	}

	// ���f���̊p�x���X�V
	Angle = TargetAngle - difference;
	MV1SetRotationXYZ(ModelHandle, VGet(0.0f, Angle + DX_PI_F, 0.0f));
}

/// <summary>
/// �v���C���[�̃A�j���[�V�������Đ�����
/// </summary>
void Player::PlayAnim(AnimKind PlayAnim)
{
	// HACK: �w�肵���ԍ��̃A�j���[�V�������A�^�b�`���A���O�ɍĐ����Ă����A�j���[�V�����̏���prev�Ɉڍs���Ă���
	// ����ւ����s���̂ŁA�P�O�̃��[�V���������L����������f�^�b�`����
	if (PrevPlayAnim != -1)
	{
		MV1DetachAnim(ModelHandle, PrevPlayAnim);
		PrevPlayAnim = -1;
	}

	// ���܂ōĐ����̃��[�V�������������̂̏���Prev�Ɉړ�����
	PrevPlayAnim = CurrentPlayAnim;
	PrevAnimCount = CurrentAnimCount;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	CurrentPlayAnim = MV1AttachAnim(ModelHandle, static_cast<int>(PlayAnim));
	CurrentAnimCount = 0.0f;

	// �u�����h����Prev���L���ł͂Ȃ��ꍇ�͂P�D�O��( ���݃��[�V�������P�O�O���̏�� )�ɂ���
	AnimBlendRate = PrevPlayAnim == -1 ? 1.0f : 0.0f;
}

/// <summary>
/// �v���C���[�̃A�j���[�V��������
/// </summary>
void Player::UpdateAnimation()
{
	float AnimTotalTime;		// �Đ����Ă���A�j���[�V�����̑�����

	// �u�����h�����P�ȉ��̏ꍇ�͂P�ɋ߂Â���
	if (AnimBlendRate < 1.0f)
	{
		AnimBlendRate += AnimBlendSpeed;
		if (AnimBlendRate > 1.0f)
		{
			AnimBlendRate = 1.0f;
		}
	}

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (CurrentPlayAnim != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(ModelHandle, CurrentPlayAnim);

		// �Đ����Ԃ�i�߂�
		CurrentAnimCount += PlayAnimSpeed;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (CurrentAnimCount >= AnimTotalTime)
		{
			CurrentAnimCount = static_cast<float>(fmod(CurrentAnimCount, AnimTotalTime));
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(ModelHandle, CurrentPlayAnim, CurrentAnimCount);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(ModelHandle, CurrentPlayAnim, AnimBlendRate);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (PrevPlayAnim != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		AnimTotalTime = MV1GetAttachAnimTotalTime(ModelHandle, PrevPlayAnim);

		// �Đ����Ԃ�i�߂�
		PrevAnimCount += PlayAnimSpeed;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (PrevAnimCount > AnimTotalTime)
		{
			PrevAnimCount = static_cast<float>(fmod(PrevAnimCount, AnimTotalTime));
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(ModelHandle, PrevPlayAnim, PrevAnimCount);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(ModelHandle, PrevPlayAnim, 1.0f - AnimBlendRate);
	}
}

/// <summary>
/// �v���C���[�̉e��`��
/// </summary>
void Player::DrawShadow(const Stage& stage)
{
	MV1_COLL_RESULT_POLY_DIM HitResultDim;
	MV1_COLL_RESULT_POLY* HitResult;
	VERTEX3D Vertex[3];
	VECTOR SlideVec;

	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	HitResultDim = MV1CollCheck_Capsule(stage.GetModelHandle(), -1, Position, VAdd(Position, VGet(0.0f, -ShadowHeight, 0.0f)), ShadowSize);

	// ���_�f�[�^�ŕω��������������Z�b�g
	Vertex[0].dif = GetColorU8(255, 255, 255, 255);
	Vertex[0].spc = GetColorU8(0, 0, 0, 0);
	Vertex[0].su = 0.0f;
	Vertex[0].sv = 0.0f;
	Vertex[1] = Vertex[0];
	Vertex[2] = Vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	HitResult = HitResultDim.Dim;
	for (int i = 0; i < HitResultDim.HitNum; i++, HitResult++)
	{
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		Vertex[0].pos = HitResult->Position[0];
		Vertex[1].pos = HitResult->Position[1];
		Vertex[2].pos = HitResult->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		SlideVec = VScale(HitResult->Normal, 0.5f);
		Vertex[0].pos = VAdd(Vertex[0].pos, SlideVec);
		Vertex[1].pos = VAdd(Vertex[1].pos, SlideVec);
		Vertex[2].pos = VAdd(Vertex[2].pos, SlideVec);

		// �|���S���̕s�����x��ݒ肷��
		Vertex[0].dif.a = 0;
		Vertex[1].dif.a = 0;
		Vertex[2].dif.a = 0;
		if (HitResult->Position[0].y > Position.y - ShadowHeight)
			Vertex[0].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(HitResult->Position[0].y - Position.y) / ShadowHeight)));

		if (HitResult->Position[1].y > Position.y - ShadowHeight)
			Vertex[1].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(HitResult->Position[1].y - Position.y) / ShadowHeight)));

		if (HitResult->Position[2].y > Position.y - ShadowHeight)
			Vertex[2].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(HitResult->Position[2].y - Position.y) / ShadowHeight)));

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		Vertex[0].u = (HitResult->Position[0].x - Position.x) / (ShadowSize * 2.0f) + 0.5f;
		Vertex[0].v = (HitResult->Position[0].z - Position.z) / (ShadowSize * 2.0f) + 0.5f;
		Vertex[1].u = (HitResult->Position[1].x - Position.x) / (ShadowSize * 2.0f) + 0.5f;
		Vertex[1].v = (HitResult->Position[1].z - Position.z) / (ShadowSize * 2.0f) + 0.5f;
		Vertex[2].u = (HitResult->Position[2].x - Position.x) / (ShadowSize * 2.0f) + 0.5f;
		Vertex[2].v = (HitResult->Position[2].z - Position.z) / (ShadowSize * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(Vertex, 1, ShadowHandle, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(HitResultDim);

	// ���C�e�B���O��L���ɂ���
	SetUseLighting(TRUE);

	// �y�o�b�t�@�𖳌��ɂ���
	SetUseZBuffer3D(FALSE);
}

/// <summary>
/// �V��ɓ���������
/// </summary>
void Player::OnHitRoof()
{
	// �x�������̑��x�͔��]
	CurrentJumpPower = -CurrentJumpPower;
}

/// <summary>
/// ���ɓ���������
/// </summary>
void Player::OnHitFloor()
{
	// �x�������̈ړ����x�͂O��
	CurrentJumpPower = 0.0f;

	// �����W�����v���������ꍇ�͒��n��Ԃɂ���
	if (CurrentState == State::JUMP)
	{
		// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
		if (IsMove)
		{
			// �ړ����Ă���ꍇ�͑����Ԃ�
			PlayAnim(AnimKind::RUN);
			CurrentState = State::RUN;
		}
		else
		{
			// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
			PlayAnim(AnimKind::STOP);
			CurrentState = State::STAND;
		}

		// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
		AnimBlendRate = 1.0f;
	}
}

/// <summary>
/// �������m�肵���Ƃ�
/// </summary>
void Player::OnFall()
{
	if (CurrentState != State::JUMP)
	{
		// �W�����v��(�������j�ɂ���
		CurrentState = State::JUMP;

		// ������Ƃ����W�����v����
		CurrentJumpPower = FallUpPower;

		// �A�j���[�V�����͗������̂��̂ɂ���
		PlayAnim(AnimKind::JUMP);
	}
}
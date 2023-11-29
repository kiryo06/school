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
	:  position				(VGet(0.0f, 0.0f, 0.0f))
	 , targetMoveDirection	(VGet(1.0f, 0.0f, 0.0f))
	 , angle				(0.0f)
	 , currentJumpPower		(0.0f)
	 , modelHandle			(-1)
	 , shadowHandle			(-1)
	 , currentState			(State::Stand)
	 , currentPlayAnim		(-1)
	 , currentAnimCount		(0)
	 , prevPlayAnim			(-1)
	 , prevAnimCount		(0)
	 , animBlendRate		(1.0f)
	 , isMove				(false)
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
	modelHandle = MV1LoadModel("DxChara.x");

	// �e�`��p�̉摜�̓ǂݍ���
	shadowHandle = LoadGraph("Shadow.tga");

	// �A�j���[�V�����̃u�����h����������
	animBlendRate = 1.0f;

	// ������Ԃł̓A�j���[�V�����̓A�^�b�`����Ă��Ȃ��ɂ���
	currentPlayAnim = -1;
	prevPlayAnim = -1;

	// ���������Ă���A�j���[�V�������Đ�
	PlayAnim(AnimKind::Stop);
}

/// <summary>
/// �A�����[�h
/// </summary>
void Player::Unload()
{
	// ���f���̍폜
	if (shadowHandle >= 0)
	{
		MV1DeleteModel(modelHandle);
		modelHandle = -1;
	}
	// �e�p�摜�̍폜
	if (shadowHandle >= 0)
	{
		DeleteGraph(shadowHandle);
		shadowHandle = -1;
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
	VECTOR	upMoveVec;		// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR	leftMoveVec;	// �����{�^���u���v����͂������Ƃ��̃v���C���[�̈ړ������x�N�g��
	VECTOR	moveVec;		// ���̃t���[���̈ړ��x�N�g��
	State prevState = currentState;
	currentState = UpdateMoveParameterWithPad(input, camera, upMoveVec, leftMoveVec, moveVec);

	// �A�j���[�V�����X�e�[�g�̍X�V
	UpdateAnimationState(prevState);

	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	UpdateAngle();

	// �ړ��x�N�g�������ɃR���W�������l�����v���C���[���ړ�
	Move(moveVec, stage);

	// �A�j���[�V��������
	UpdateAnimation();
}

/// <summary>
/// �`��
/// </summary>
void Player::Draw(const Stage& stage)
{
	MV1DrawModel(modelHandle);
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

	MATRIX localMatrix;

	// ���[�U�[�s�����������
	MV1ResetFrameUserLocalMatrix(modelHandle, 2);

	// ���݂̃��[�g�t���[���̍s����擾����
	localMatrix = MV1GetFrameLocalMatrix(modelHandle, 2);

	// �y�������̕��s�ړ������𖳌��ɂ���
	localMatrix.m[3][2] = 0.0f;

	// ���[�U�[�s��Ƃ��ĕ��s�ړ������𖳌��ɂ����s������[�g�t���[���ɃZ�b�g����
	MV1SetFrameUserLocalMatrix(modelHandle, 2, localMatrix);
}

/// <summary>
/// �p�b�h���͂ɂ���Ĉړ��p�����[�^��ݒ肷��
/// </summary>
Player::State Player::UpdateMoveParameterWithPad(const Input& input, const Camera& camera, VECTOR& upMoveVec, VECTOR& leftMoveVec, VECTOR& moveVec)
{
	State nextState = currentState;

	// �v���C���[�̈ړ������̃x�N�g�����Z�o
	// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���̓J�����̎�����������x�����𔲂�������
	upMoveVec = VSub(camera.GetTarget(), camera.GetPosition());
	upMoveVec.y = 0.0f;

	// �����{�^���u���v���������Ƃ��̃v���C���[�̈ړ��x�N�g���͏���������Ƃ��̕����x�N�g���Ƃx���̃v���X�����̃x�N�g���ɐ����ȕ���
	leftMoveVec = VCross(upMoveVec, VGet(0.0f, 1.0f, 0.0f));

	// ��̃x�N�g���𐳋K��( �x�N�g���̒������P�D�O�ɂ��邱�� )
	upMoveVec = VNorm(upMoveVec);
	leftMoveVec = VNorm(leftMoveVec);

	// ���̃t���[���ł̈ړ��x�N�g����������
	moveVec = VGet(0.0f, 0.0f, 0.0f);

	// �ړ��������ǂ����̃t���O��������Ԃł́u�ړ����Ă��Ȃ��v��\��FALSE�ɂ���
	bool isPressMoveButton = false;

	// �p�b�h�̂R�{�^���ƍ��V�t�g���ǂ����������Ă��Ȃ�������v���C���[�̈ړ�����
	if (CheckHitKey(KEY_INPUT_LSHIFT) == 0 && (input.GetNowFrameInput() & PAD_INPUT_C) == 0)
	{
		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�č������Ɉړ�����
		if (input.GetNowFrameInput() & PAD_INPUT_LEFT)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			moveVec = VAdd(moveVec, leftMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			isPressMoveButton = true;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă���������猩�ĉE�����Ɉړ�����
			if (input.GetNowFrameInput() & PAD_INPUT_RIGHT)
			{
				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
				moveVec = VAdd(moveVec, VScale(leftMoveVec, -1.0f));

				// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
				isPressMoveButton = true;
			}

		// �����{�^���u���v�����͂��ꂽ��J�����̌��Ă�������Ɉړ�����
		if (input.GetNowFrameInput() & PAD_INPUT_UP)
		{
			// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g�������Z����
			moveVec = VAdd(moveVec, upMoveVec);

			// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
			isPressMoveButton = true;
		}
		else
			// �����{�^���u���v�����͂��ꂽ��J�����̕����Ɉړ�����
			if (input.GetNowFrameInput() & PAD_INPUT_DOWN)
			{
				// �ړ��x�N�g���Ɂu���v�����͂��ꂽ���̈ړ��x�N�g���𔽓]�������̂����Z����
				moveVec = VAdd(moveVec, VScale(upMoveVec, -1.0f));

				// �ړ��������ǂ����̃t���O���u�ړ������v�ɂ���
				isPressMoveButton = true;
			}

		// �v���C���[�̏�Ԃ��u�W�����v�v�ł͂Ȃ��A���{�^���P��������Ă�����W�����v����
		if (currentState != State::Jump && (input.GetNowFrameNewInput() & PAD_INPUT_A))
		{
			// �x�������̑��x���Z�b�g
			currentJumpPower = JumpPower;

			// ��Ԃ��u�W�����v�v�ɂ���
			nextState = State::Jump;
		}
	}

	// �W�����v��ԂȂ�d�͓K�p
	if (currentState == State::Jump)
	{
		// �x�������̑��x���d�͕����Z����
		currentJumpPower -= Gravity;
	}

	// �ړ��{�^���������ꂽ���ǂ����ŏ����𕪊�
	if (isPressMoveButton)
	{
		// �������܂Łu�����~�܂�v��Ԃ�������
		if (currentState == State::Stand)
		{
			// ��Ԃ��u����v�ɂ���
			nextState = State::Run;
		}

		// �ړ��x�N�g���𐳋K���������̂��v���C���[�������ׂ������Ƃ��ĕۑ�
		targetMoveDirection = VNorm(moveVec);

		// �v���C���[�������ׂ������x�N�g�����v���C���[�̃X�s�[�h�{�������̂��ړ��x�N�g���Ƃ���
		moveVec = VScale(targetMoveDirection, MoveSpeed);
	}
	else
	{
		// ���̃t���[���ňړ����Ă��Ȃ��āA����Ԃ��u����v��������
		if (currentState == State::Run)
		{
			// ��Ԃ��u�����~��v�ɂ���
			nextState = State::Stand;
		}
	}

	// �ړ��x�N�g���̂x�������x�������̑��x�ɂ���
	moveVec.y = currentJumpPower;

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
		isMove = true;
	}
	else
	{
		isMove = false;
	}

	// �����蔻������āA�V�������W��ۑ�����
	position = stage.CheckCollision(*this, MoveVector);

	// �v���C���[�̃��f���̍��W���X�V����
	MV1SetPosition(modelHandle, position);
}

/// <summary>
/// �A�j���[�V�����X�e�[�g�̍X�V
/// </summary>
void Player::UpdateAnimationState(State prevState)
{
	// �����~�܂肩�瑖��ɕς������
	if (prevState == State::Stand && currentState == State::Run)
	{
		// ����A�j���[�V�������Đ�����
		PlayAnim(AnimKind::Run);
	}
	// ���肩�痧���~�܂�ɕς������
	else if (prevState == State::Run && currentState == State::Stand)
	{
		// �����~��A�j���[�V�������Đ�����
		PlayAnim(AnimKind::Stop);
	}
	// ��Ԃ��u�W�����v�v�̏ꍇ��
	else if (currentState == State::Jump)
	{
		// �����������Ă��Ċ��Đ�����Ă���A�j���[�V�������㏸���p�̂��̂������ꍇ��
		if (currentJumpPower < 0.0f)
		{
			// �������悤�̃A�j���[�V�������Đ�����
			if (MV1GetAttachAnim(modelHandle, currentPlayAnim) == static_cast<int>(AnimKind::Jump))
			{
				PlayAnim(AnimKind::Fall);
			}
		}
		else
		{
			PlayAnim(AnimKind::Jump);
		}
	}
}

/// <summary>
/// �v���C���[�̉�]����
/// </summary>
void Player::UpdateAngle()
{
	// �v���C���[�̈ړ������Ƀ��f���̕������߂Â���
	float targetAngle;			// �ڕW�p�x
	float difference;			// �ڕW�p�x�ƌ��݂̊p�x�Ƃ̍�

	// �ڕW�̕����x�N�g������p�x�l���Z�o����
	targetAngle = static_cast<float>(atan2(targetMoveDirection.x, targetMoveDirection.z));

	// �ڕW�̊p�x�ƌ��݂̊p�x�Ƃ̍�������o��
	// �ŏ��͒P���Ɉ����Z
	difference = targetAngle - angle;

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
	angle = targetAngle - difference;
	MV1SetRotationXYZ(modelHandle, VGet(0.0f, angle + DX_PI_F, 0.0f));
}

/// <summary>
/// �v���C���[�̃A�j���[�V�������Đ�����
/// </summary>
void Player::PlayAnim(AnimKind nextPlayAnim)
{
	// HACK: �w�肵���ԍ��̃A�j���[�V�������A�^�b�`���A���O�ɍĐ����Ă����A�j���[�V�����̏���prev�Ɉڍs���Ă���
	// ����ւ����s���̂ŁA�P�O�̃��[�V���������L����������f�^�b�`����
	if (prevPlayAnim != -1)
	{
		MV1DetachAnim(modelHandle, prevPlayAnim);
		prevPlayAnim = -1;
	}

	// ���܂ōĐ����̃��[�V�������������̂̏���Prev�Ɉړ�����
	prevPlayAnim = currentPlayAnim;
	prevAnimCount = currentAnimCount;

	// �V���Ɏw��̃��[�V���������f���ɃA�^�b�`���āA�A�^�b�`�ԍ���ۑ�����
	currentPlayAnim = MV1AttachAnim(modelHandle, static_cast<int>(nextPlayAnim));
	currentAnimCount = 0.0f;

	// �u�����h����Prev���L���ł͂Ȃ��ꍇ�͂P�D�O��( ���݃��[�V�������P�O�O���̏�� )�ɂ���
	animBlendRate = prevPlayAnim == -1 ? 1.0f : 0.0f;
}

/// <summary>
/// �v���C���[�̃A�j���[�V��������
/// </summary>
void Player::UpdateAnimation()
{
	float animTotalTime;		// �Đ����Ă���A�j���[�V�����̑�����

	// �u�����h�����P�ȉ��̏ꍇ�͂P�ɋ߂Â���
	if (animBlendRate < 1.0f)
	{
		animBlendRate += AnimBlendSpeed;
		if (animBlendRate > 1.0f)
		{
			animBlendRate = 1.0f;
		}
	}

	// �Đ����Ă���A�j���[�V�����P�̏���
	if (currentPlayAnim != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, currentPlayAnim);

		// �Đ����Ԃ�i�߂�
		currentAnimCount += PlayAnimSpeed;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (currentAnimCount >= animTotalTime)
		{
			currentAnimCount = static_cast<float>(fmod(currentAnimCount, animTotalTime));
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(modelHandle, currentPlayAnim, currentAnimCount);

		// �A�j���[�V�����P�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(modelHandle, currentPlayAnim, animBlendRate);
	}

	// �Đ����Ă���A�j���[�V�����Q�̏���
	if (prevPlayAnim != -1)
	{
		// �A�j���[�V�����̑����Ԃ��擾
		animTotalTime = MV1GetAttachAnimTotalTime(modelHandle, prevPlayAnim);

		// �Đ����Ԃ�i�߂�
		prevAnimCount += PlayAnimSpeed;

		// �Đ����Ԃ������Ԃɓ��B���Ă�����Đ����Ԃ����[�v������
		if (prevAnimCount > animTotalTime)
		{
			prevAnimCount = static_cast<float>(fmod(prevAnimCount, animTotalTime));
		}

		// �ύX�����Đ����Ԃ����f���ɔ��f������
		MV1SetAttachAnimTime(modelHandle, prevPlayAnim, prevAnimCount);

		// �A�j���[�V�����Q�̃��f���ɑ΂��锽�f�����Z�b�g
		MV1SetAttachAnimBlendRate(modelHandle, prevPlayAnim, 1.0f - animBlendRate);
	}
}

/// <summary>
/// �v���C���[�̉e��`��
/// </summary>
void Player::DrawShadow(const Stage& stage)
{
	// ���C�e�B���O�𖳌��ɂ���
	SetUseLighting(FALSE);

	// �y�o�b�t�@��L���ɂ���
	SetUseZBuffer3D(TRUE);

	// �e�N�X�`���A�h���X���[�h�� CLAMP �ɂ���( �e�N�X�`���̒[����͒[�̃h�b�g�����X���� )
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// �v���C���[�̒����ɑ��݂���n�ʂ̃|���S�����擾
	auto hitResultDim = MV1CollCheck_Capsule(stage.GetModelHandle(), -1, position, VAdd(position, VGet(0.0f, -ShadowHeight, 0.0f)), ShadowSize);

	// ���_�f�[�^�ŕω��������������Z�b�g
	VERTEX3D vertex[3];
	vertex[0].dif = GetColorU8(255, 255, 255, 255);
	vertex[0].spc = GetColorU8(0, 0, 0, 0);
	vertex[0].su = 0.0f;
	vertex[0].sv = 0.0f;
	vertex[1] = vertex[0];
	vertex[2] = vertex[0];

	// ���̒����ɑ��݂���|���S���̐������J��Ԃ�
	auto hitResult = hitResultDim.Dim;
	for (int i = 0; i < hitResultDim.HitNum; i++, hitResult++)
	{
		// �|���S���̍��W�͒n�ʃ|���S���̍��W
		vertex[0].pos = hitResult->Position[0];
		vertex[1].pos = hitResult->Position[1];
		vertex[2].pos = hitResult->Position[2];

		// ������Ǝ����グ�ďd�Ȃ�Ȃ��悤�ɂ���
		auto slideVec = VScale(hitResult->Normal, 0.5f);
		vertex[0].pos = VAdd(vertex[0].pos, slideVec);
		vertex[1].pos = VAdd(vertex[1].pos, slideVec);
		vertex[2].pos = VAdd(vertex[2].pos, slideVec);

		// �|���S���̕s�����x��ݒ肷��
		vertex[0].dif.a = 0;
		vertex[1].dif.a = 0;
		vertex[2].dif.a = 0;
		if (hitResult->Position[0].y > position.y - ShadowHeight)
			vertex[0].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(hitResult->Position[0].y - position.y) / ShadowHeight)));

		if (hitResult->Position[1].y > position.y - ShadowHeight)
			vertex[1].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(hitResult->Position[1].y - position.y) / ShadowHeight)));

		if (hitResult->Position[2].y > position.y - ShadowHeight)
			vertex[2].dif.a = static_cast<BYTE>(128 * (1.0f - static_cast<float>(fabs(hitResult->Position[2].y - position.y) / ShadowHeight)));

		// �t�u�l�͒n�ʃ|���S���ƃv���C���[�̑��΍��W���犄��o��
		vertex[0].u = (hitResult->Position[0].x - position.x) / (ShadowSize * 2.0f) + 0.5f;
		vertex[0].v = (hitResult->Position[0].z - position.z) / (ShadowSize * 2.0f) + 0.5f;
		vertex[1].u = (hitResult->Position[1].x - position.x) / (ShadowSize * 2.0f) + 0.5f;
		vertex[1].v = (hitResult->Position[1].z - position.z) / (ShadowSize * 2.0f) + 0.5f;
		vertex[2].u = (hitResult->Position[2].x - position.x) / (ShadowSize * 2.0f) + 0.5f;
		vertex[2].v = (hitResult->Position[2].z - position.z) / (ShadowSize * 2.0f) + 0.5f;

		// �e�|���S����`��
		DrawPolygon3D(vertex, 1, shadowHandle, TRUE);
	}

	// ���o�����n�ʃ|���S�����̌�n��
	MV1CollResultPolyDimTerminate(hitResultDim);

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
	currentJumpPower = -currentJumpPower;
}

/// <summary>
/// ���ɓ���������
/// </summary>
void Player::OnHitFloor()
{
	// �x�������̈ړ����x�͂O��
	currentJumpPower = 0.0f;

	// �����W�����v���������ꍇ�͒��n��Ԃɂ���
	if (currentState == State::Jump)
	{
		// �ړ����Ă������ǂ����Œ��n��̏�ԂƍĐ�����A�j���[�V�����𕪊򂷂�
		if (isMove)
		{
			// �ړ����Ă���ꍇ�͑����Ԃ�
			PlayAnim(AnimKind::Run);
			currentState = State::Run;
		}
		else
		{
			// �ړ����Ă��Ȃ��ꍇ�͗����~���Ԃ�
			PlayAnim(AnimKind::Stop);
			currentState = State::Stand;
		}

		// ���n���̓A�j���[�V�����̃u�����h�͍s��Ȃ�
		animBlendRate = 1.0f;
	}
}

/// <summary>
/// �������m�肵���Ƃ�
/// </summary>
void Player::OnFall()
{
	if (currentState != State::Jump)
	{
		// �W�����v��(�������j�ɂ���
		currentState = State::Jump;

		// ������Ƃ����W�����v����
		currentJumpPower = FallUpPower;

		// �A�j���[�V�����͗������̂��̂ɂ���
		PlayAnim(AnimKind::Jump);
	}
}
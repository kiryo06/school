#include "DxLib.h"
#include "Camera.h"
#include "Input.h"
#include "Player.h"
#include "Stage.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
Camera::Camera()
	:  angleH	(0)
	 , angleV	(0)
	 , position	(VGet(0, 0, 0))	
	 , target	(VGet(0, 0, 0))
{
	// �J�����̏��������p�x�͂P�W�O�x
	angleH = DX_PI_F;

	// �����p�x�͂O�x
	angleV = 0.0f;
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Camera::~Camera()
{
	// �����Ȃ�
}

/// <summary>
/// �X�V
/// </summary>
void Camera::Update(const Input& input, const Player& player, const Stage& stage)
{
	// �p�b�h�̂R�{�^�����A�V�t�g�L�[��������Ă���ꍇ�̂݊p�x�ύX������s��
	if (CheckHitKey(KEY_INPUT_LSHIFT) || (input.GetNowFrameInput() & PAD_INPUT_C))
	{
		// �u���v�{�^����������Ă����琅���p�x���}�C�i�X����
		if (input.GetNowFrameInput() & PAD_INPUT_LEFT)
		{
			angleH -= AngleSpeed;

			// �|�P�W�O�x�ȉ��ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x�𑫂�
			if (angleH < -DX_PI_F)
			{
				angleH += DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琅���p�x���v���X����
		if (input.GetNowFrameInput() & PAD_INPUT_RIGHT)
		{
			angleH += AngleSpeed;

			// �P�W�O�x�ȏ�ɂȂ�����p�x�l���傫���Ȃ肷���Ȃ��悤�ɂR�U�O�x������
			if (angleH > DX_PI_F)
			{
				angleH -= DX_TWO_PI_F;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���}�C�i�X����
		if (input.GetNowFrameInput() & PAD_INPUT_UP)
		{
			angleV -= AngleSpeed;

			// ������p�x�ȉ��ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (angleV < -DX_PI_F * 0.5f + 0.6f)
			{
				angleV = -DX_PI_F * 0.5f + 0.6f;
			}
		}

		// �u���v�{�^����������Ă����琂���p�x���v���X����
		if (input.GetNowFrameInput() & PAD_INPUT_DOWN)
		{
			angleV += AngleSpeed;

			// ������p�x�ȏ�ɂ͂Ȃ�Ȃ��悤�ɂ���
			if (angleV > DX_PI_F * 0.5f - 0.6f)
			{
				angleV = DX_PI_F * 0.5f - 0.6f;
			}
		}
	}

	// �J�����̒����_�̓v���C���[���W����K��l���������W
	target = VAdd(player.GetPosition(), VGet(0.0f, CameraPlayerTargetHeight, 0.0f));

	// �J�����̍��W��␳����
	FixCameraPosition(stage);

	// �J�����̏������C�u�����̃J�����ɔ��f������
	SetCameraPositionAndTarget_UpVecY(position, target);
}

/// <summary>
/// �J�������W��␳����
/// </summary>
void Camera::FixCameraPosition(const Stage& stage)
{
	// ���������̉�]�͂x����]
	auto rotY = MGetRotY(angleH);

	// ���������̉�]�͂y����] )
	auto rotZ = MGetRotZ(angleV);

	// �J��������v���C���[�܂ł̏����������Z�b�g
	float cameraPlayerLength = ToPlayerLength;

	// �J�����̍��W���Z�o
	// �w���ɃJ�����ƃv���C���[�Ƃ̋����������L�т��x�N�g����
	// ����������]( �y����] )���������Ɛ���������]( �x����] )���čX��
	// �����_�̍��W�𑫂������̂��J�����̍��W
	position = VAdd(VTransform(VTransform(VGet(-cameraPlayerLength, 0.0f, 0.0f), rotZ), rotY), target);

	// ������Ȃ��ʒu�܂Ńv���C���[�ɋ߂Â�
	// �|���S���ɓ�����Ȃ��������Z�b�g
	float notHitLength = 0.0f;

	// �|���S���ɓ����鋗�����Z�b�g
	float hitLength = cameraPlayerLength;

	// �����_����J�����̍��W�܂ł̊ԂɃX�e�[�W�̃|���S�������邩���ׂ�
	do
	{
		// �����邩�ǂ����e�X�g���鋗�����Z�b�g( ������Ȃ������Ɠ����鋗���̒��� )
		float testLength = notHitLength + (hitLength - notHitLength) * 0.5f;

		// �e�X�g�p�̃J�������W���Z�o
		auto testPosition = VAdd(VTransform(VTransform(VGet(-testLength, 0.0f, 0.0f), rotZ), rotY), target);

		// �V�������W�ŕǂɓ����邩�e�X�g
		auto hitResult = MV1CollCheck_Capsule(stage.GetModelHandle(), -1, target, testPosition, CollisionSize);
		int hitNum = hitResult.HitNum;
		MV1CollResultPolyDimTerminate(hitResult);
		if (hitNum != 0)
		{
			// ���������瓖���鋗���� testLength �ɕύX����
			hitLength = testLength;

			// �J�����̍��W���Z�b�g
			position = testPosition;
		}
		else
		{
			// ������Ȃ������瓖����Ȃ������� testLength �ɕύX����
			notHitLength = testLength;
		}

		// hitLength �� NoHitLength ���\���ɋ߂Â��Ă��Ȃ������烋�[�v
	} while (hitLength - notHitLength > 0.1f);
}
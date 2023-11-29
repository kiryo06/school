#pragma once

class Input;
class Camera;
class Stage;

// �v���C���[���\����
class Player
{
public:
	// �v���C���[�̏��
	enum class State : int
	{
		STAND = 0,		// �����~�܂�
		RUN = 1,		// ����
		JUMP = 2,		// �W�����v
	};

	// �v���C���[�̃A�j���[�V�������
	enum class AnimKind : int
	{
		NONE = -1,		// �Ȃ�
		UNKNOWN = 0,	// �s��
		RUN = 1,		// ����
		JUMP = 2,		// �W�����v
		FALL = 3,		// ������
		STOP = 4,		// �����~�܂�
	};

	// �v���C���[�֌W�̒�`
	static const int MaxHitColl;	// ��������R���W�����|���S���̍ő吔

	static constexpr float	PlayAnimSpeed = 250.0f;		// �A�j���[�V�������x
	static constexpr float	MoveSpeed = 30.0f;		// �ړ����x
	static constexpr float	AnimBlendSpeed = 0.1f;		// �A�j���[�V�����̃u�����h���ω����x
	static constexpr float	AngleSpeed = 0.2f;		// �p�x�ω����x
	static constexpr float	JumpPower = 100.0f;		// �W�����v��
	static constexpr float	FallUpPower = 20.0f;		// ���𓥂݊O�������̃W�����v��
	static constexpr float	Gravity = 3.0f;		// �d��
	static constexpr float	DefaultSize = 800.0f;		// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
	static constexpr float	HitWidth = 200.0f;		// �����蔻��J�v�Z���̔��a
	static constexpr float	HitHeight = 700.0f;		// �����蔻��J�v�Z���̍���
	static constexpr int	HitTryNum = 16;		// �ǉ����o�������̍ő厎�s��
	static constexpr float	HitSlideLength = 5.0f;		// ��x�̕ǉ����o�������ŃX���C�h�����鋗��
	static constexpr float	ShadowSize = 200.0f;		// �e�̑傫��
	static constexpr float	ShadowHeight = 700.0f;		// �e�������鍂��

	void Initialize();						// ������
	void Finalize();						// ��n��
	void Update(const Input& input, const Camera& camera, const Stage& stage);						
	void Draw(const Stage& stage);

	const VECTOR& GetPosition() const { return Position; }

private:
	VECTOR		Position;				// ���W
	VECTOR		TargetMoveDirection;	// ���f���������ׂ������̃x�N�g��
	float		Angle;					// ���f���������Ă�������̊p�x
	float		CurrentJumpPower;		// �x�������̑��x
	int			ModelHandle;			// ���f���n���h��
	int			ShadowHandle;			// �e�摜�n���h��
	State		State;					// ���

	int			CurrentPlayAnim;		// �Đ����Ă���A�j���[�V�����̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		CurrentAnimCount;		// �Đ����Ă���A�j���[�V�����̍Đ�����
	int			PrevPlayAnim;			// �O�̍Đ��A�j���[�V�����̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		PrevAnimCount;			// �O�̍Đ��A�j���[�V�����̍Đ�����
	float		AnimBlendRate;			// ���݂Ɖߋ��̃A�j���[�V�����̃u�����h��


	// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
	void DisableRootFrameZMove();

	// �p�b�h���͂ɂ���Ĉړ��p�����[�^��ݒ肷��
	bool UpdateMoveParameterWithPad(const Input& input, const Camera& camera, VECTOR& UpMoveVec, VECTOR& LeftMoveVec, VECTOR& MoveVec);
	
	// �ړ�����
	void Move(const VECTOR& MoveVector, const Stage& stage);

	// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f���A�ۑ�����
	void CheckKabeAndYuka(MV1_COLL_RESULT_POLY** Kabe, MV1_COLL_RESULT_POLY** Yuka, int& KabeNum, int& YukaNum, MV1_COLL_RESULT_POLY_DIM HitDim);

	// �ǃ|���S���Ƃ̓�������`�F�b�N���A�ړ��x�N�g����␳����
	void FixNowPositionWithKabe(VECTOR& NowPos, const VECTOR& OldPos, const VECTOR& MoveVector, bool IsMove,  MV1_COLL_RESULT_POLY** Kabe, int KabeNum);
	void FixNowPositionWithKabeInternal(VECTOR& NowPos, MV1_COLL_RESULT_POLY** Kabe, int KabeNum);

	// ���|���S���Ƃ̓�������`�F�b�N���A�ړ��x�N�g����␳����
	void FixNowPositionWithYuka(VECTOR& NowPos, bool IsMove, MV1_COLL_RESULT_POLY** Yuka, int YukaNum);

	void UpdateAngle();									// ��]����
	void PlayAnim(AnimKind PlayAnim);					// �V���ȃA�j���[�V�������Đ�����
	void UpdateAnimation();								// �A�j���[�V��������
	void DrawShadow(const Stage& stage);				// �e��`��
};
#pragma once

class Input;
class Camera;
class Stage;

/// <summary>
/// �v���C���[
/// </summary>
class Player
{
public:
	// ���
	enum class State : int
	{
		Stand = 0,		// �����~�܂�
		Run = 1,		// ����
		Jump = 2,		// �W�����v
	};

	// �v���C���[�̃A�j���[�V�������
	enum class AnimKind : int
	{
		None = -1,		// �Ȃ�
		Unknown = 0,	// �s��
		Run = 1,		// ����
		Jump = 2,		// �W�����v
		Fall = 3,		// ������
		Stop = 4,		// �����~�܂�
	};

	Player();
	~Player();

	void Load();		// ���[�h
	void Unload();		// �A�����[�h
	void Update(const Input& input, const Camera& camera, Stage& stage);
	void Draw(const Stage& stage);

	void OnHitRoof();	// �V��ɓ���������
	void OnHitFloor();	// ���ɓ���������
	void OnFall();		// �������m�肵���Ƃ�

	const VECTOR& GetPosition() const { return position; }
	bool GetIsMove() const { return isMove; }
	State GetState() const { return currentState; }
	float GetJumpPower() const { return currentJumpPower; }

	// �v���C���[�֌W�̒�`
	static constexpr float	FallUpPower = 20.0f;	// ���𓥂݊O�������̃W�����v��

private:
	// �v���C���[�֌W�̒�`
	static constexpr float	PlayAnimSpeed	= 250.0f;	// �A�j���[�V�������x
	static constexpr float	MoveSpeed		= 30.0f;	// �ړ����x
	static constexpr float	AnimBlendSpeed	= 0.1f;		// �A�j���[�V�����̃u�����h���ω����x
	static constexpr float	AngleSpeed		= 0.2f;		// �p�x�ω����x
	static constexpr float	JumpPower		= 100.0f;	// �W�����v��
	static constexpr float	Gravity			= 3.0f;		// �d��
	static constexpr float	ShadowSize		= 200.0f;	// �e�̑傫��
	static constexpr float	ShadowHeight	= 700.0f;	// �e�������鍂��

	VECTOR		position;				// ���W
	VECTOR		targetMoveDirection;	// ���f���������ׂ������̃x�N�g��
	float		angle;					// ���f���������Ă�������̊p�x
	float		currentJumpPower;		// �x�������̑��x
	int			modelHandle;			// ���f���n���h��
	int			shadowHandle;			// �e�摜�n���h��
	State		currentState;			// ���

	int			currentPlayAnim;		// �Đ����Ă���A�j���[�V�����̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		currentAnimCount;		// �Đ����Ă���A�j���[�V�����̍Đ�����
	int			prevPlayAnim;			// �O�̍Đ��A�j���[�V�����̃A�^�b�`�ԍ�( -1:�����A�j���[�V�������A�^�b�`����Ă��Ȃ� )
	float		prevAnimCount;			// �O�̍Đ��A�j���[�V�����̍Đ�����
	float		animBlendRate;			// ���݂Ɖߋ��̃A�j���[�V�����̃u�����h��
	bool		isMove;					// ���̃t���[���œ��������ǂ���

	// ���[�g�t���[���̂y�������̈ړ��p�����[�^�𖳌��ɂ���
	void DisableRootFrameZMove();

	// �p�b�h���͂ɂ���Ĉړ��p�����[�^��ݒ肷��
	State UpdateMoveParameterWithPad(const Input& input, const Camera& camera, VECTOR& upMoveVec, VECTOR& leftMoveVec, VECTOR& moveVec);
	
	// �ړ�����
	void Move(const VECTOR& MoveVector, Stage& stage);

	void UpdateAnimationState(State prevState);	// �A�j���[�V�����X�e�[�g�̍X�V
	void UpdateAngle();							// ��]����
	void PlayAnim(AnimKind playAnim);			// �V���ȃA�j���[�V�������Đ�����
	void UpdateAnimation();						// �A�j���[�V��������
	void DrawShadow(const Stage& stage);		// �e��`��
};
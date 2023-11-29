#pragma once

class Input;
class Player;
class Stage;

/// <summary>
/// �J����
/// </summary>
class Camera
{
public:
	static constexpr float AngleSpeed				= 0.05f	 ;	// ���񑬓x
	static constexpr float CameraPlayerTargetHeight	= 400.0f ;	// �v���C���[���W����ǂꂾ�������ʒu�𒍎��_�Ƃ��邩
	static constexpr float ToPlayerLength			= 1600.0f;	// �v���C���[�Ƃ̋���
	static constexpr float CollisionSize			= 50.0f	 ;	// �J�����̓����蔻��T�C�Y

	Camera();
	~Camera();

	void Update(const Input& input, const Player& player, const Stage& stage);		// �J�����̏���

	const VECTOR& GetPosition() const { return position; }
	const VECTOR& GetTarget()	const { return target; }

private:
	float		angleH;					// �����p�x
	float		angleV;					// �����p�x
	VECTOR		position;				// �J�������W
	VECTOR		target;					// �����_���W

	void FixCameraPosition(const Stage& stage);
};


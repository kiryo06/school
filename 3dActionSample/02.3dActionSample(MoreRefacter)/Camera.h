#pragma once

class Input;
class Player;
class Stage;

class Camera
{
public:
	static constexpr float AngleSpeed				= 0.05f	 ;	// ���񑬓x
	static constexpr float CameraPlayerTargetHeight	= 400.0f ;	// �v���C���[���W����ǂꂾ�������ʒu�𒍎��_�Ƃ��邩
	static constexpr float ToPlayerLength			= 1600.0f;	// �v���C���[�Ƃ̋���
	static constexpr float CollisionSize			= 50.0f	 ;	// �J�����̓����蔻��T�C�Y

	void Initialize();						// �J�����̏���������
	void Update(const Input& input, const Player& player, const Stage& stage);		// �J�����̏���

	const VECTOR& GetEye() const { return Eye; }
	const VECTOR& GetTarget() const { return Target; }

private:
	float		AngleH;					// �����p�x
	float		AngleV;					// �����p�x
	VECTOR		Eye;					// �J�������W
	VECTOR		Target;					// �����_���W

	void FixCameraPosition(const Stage& stage);
};


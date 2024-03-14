#pragma once

enum class LayerType {
	BackGround,//�w�i
	Enemies,//�G
	Event,//�C�x���g
};

//�}�b�v�̍L�� * ���C���[���Ȃ̂ŁA2�̃x�N�^�[�����q�ɂ��Ă�
using MapData_t = std::vector < std::vector<unsigned char>>;


/// <summary>
/// Platinum�̃f�[�^�����[�h����
/// </summary>
class PlatinumLoader
{
public:
	/// <summary>
	/// fmf�t�@�C����ǂݍ����mapData_�ɒ��g������
	/// </summary>
	/// <param name="filePath">fmf�t�@�C���̃t�@�C���p�X</param>
	void Load(const TCHAR* filePath);
	
	/// <summary>
	/// mapData_�̒��g�𒼐ڎ擾����
	/// </summary>
	const MapData_t& GetMapData()const;
	
	/// <summary>
	/// Load�œǂݍ���MapData_t�̒��g�i�}�b�v�`�b�v�̃f�[�^�j���ʂɎ擾����
	/// </summary>
	/// <param name="layerType">�擾����}�b�v�`�b�v�̃��C���[ID</param>
	/// <param name="chipX">�擾����}�b�v�`�b�v��X���W</param>
	/// <param name="chipY">�擾����}�b�v�`�b�v��Y���W</param>
	/// <returns>�����Ŏw�肵���ꏊ�̃}�b�v�`�b�v�̃X�v���C�g�ԍ�</returns>
	const int GetChipSpriteNo(LayerType layerType,int chipX,int chipY)const;
	
	/// <summary>
	/// 
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void GetMapSize(int& width,int& height);
private:
	/// <summary>
	/// �f�[�^�̓��e��XY�]�u����
	/// </summary>
	/// <param name="layerId">���C���[ID</param>
	void TransposeMapData(int layerId);
private:

	MapData_t mapData_;
	int mapWidth_ = 0;
	int mapHeight_ = 0;
};


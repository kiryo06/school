#include "DxLib.h"
#include <vector>
#include "PlatinumLoader.h"
#include <string>
#include <cassert>

void PlatinumLoader::Load(const TCHAR* filePath)
{
	//FMF�w�b�_�[(Platinum�̃h�L�������g�ɏ����Ă���)
	struct Header {
		int8_t id[4];			//���ʎq(FMF_)			1*4�o�C�g
		uint32_t size;			//�f�[�^�T�C�Y�@		4�o�C�g
		uint32_t mapWidth;		//�}�b�v�̕�			4�o�C�g
		uint32_t mapHeight;		//�}�b�v�̍����@		4�o�C�g
		uint8_t chiphWidth;		//�`�b�v(�Z�����)�̕�					1�o�C�g
		uint8_t chpHeight;		//�`�b�v(�Z�����)�̍���				1�o�C�g
		uint8_t layerCount;		//���C���[�̐�							1�o�C�g
		uint8_t bitCount;		//�P�Z��������̃r�b�g��(��8�Ńo�C�g��)	1�o�C�g
	};//20�o�C�g

	Header header;
	int handle =  FileRead_open(filePath);
	FileRead_read(&header,sizeof(header), handle);

	std::string strId;
	strId.resize(4);
	std::copy_n(header.id, 4, strId.begin());

	if (strId != "FMF_") {
		assert(0);
	}

	mapWidth_ = header.mapWidth;
	mapHeight_ = header.mapHeight;
	int layerCount = header.layerCount;
	//���C���[1������̃T�C�Y���v�Z����
	//�}�b�v�̕����}�b�v�̍���*(�`�b�v1������̃o�C�g��)
	int layerDataSize = header.mapWidth * header.mapHeight * (header.bitCount / 8);

	mapData_.resize(layerCount);
	for (auto& layer : mapData_) {
		layer.resize(layerDataSize);
		FileRead_read(layer.data(), layerDataSize, handle);
	}

	FileRead_close(handle);

	//���s������������t���̂��߂ɁA�f�[�^�����H
	for (int layerIdx = 0; layerIdx < layerCount; ++layerIdx) {
		TransposeMapData(layerIdx);
	}

}

const MapData_t& PlatinumLoader::GetMapData() const
{
	return mapData_;
}

const int PlatinumLoader::GetChipSpriteNo(LayerType layerType, int chipX, int chipY) const
{
	assert(chipX < mapWidth_);
	assert(chipY < mapHeight_);
	auto index = chipY + chipX * mapHeight_;
	return mapData_[(int)layerType][index];
}

void PlatinumLoader::GetMapSize(int& width, int& height)
{
	width = mapWidth_;
	height = mapHeight_;
}

void PlatinumLoader::TransposeMapData(int layerId)
{

	auto temp = mapData_[layerId];//��������R�s�[���Ă���
	//���R�Ƃ��Ă̓R�s�[�����ɓ]�u���悤�Ƃ���ƌ��̃f�[�^��������
	for (int Y = 0; Y < mapHeight_; ++Y) {
		for (int X = 0; X < mapWidth_; ++X) {
			//�ʏ��XY�w��̏ꍇ
			//Y*mapwidht_+x�Ƃ������ɂȂ�
			//����ɁA��200�c15�͈ێ�����K�v������B
			//�c�ɕ��ׂĂ�������
			//0�ׂ̗�1�i���ɂ�����
			int idxSrc = Y * mapWidth_ + X;	//Source Index
			int idxDst = Y + mapHeight_ * X;	//Destination Index
			mapData_[layerId][idxDst] = temp[idxSrc];
		}
	}

}
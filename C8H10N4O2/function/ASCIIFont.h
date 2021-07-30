#pragma once

#include "SpriteBatch.h"
#include <vector>
#include <string>

class ASCII
{
public:
	typedef int Handle;
	struct SprData
	{
		std::shared_ptr<SpriteBatch>	spr;
		std::wstring					file_name;
		FLOAT2							size_per_char;
		SprData() :spr(nullptr), file_name(), size_per_char() {}
	};
private:
	friend std::unique_ptr<ASCII> std::make_unique<ASCII>();
	ASCII() = default;
	std::vector<SprData>		datas;
	static std::unique_ptr<ASCII> instance;
public:
	/// <summary>
	/// �t�H���g�f�[�^��ǉ����܂��B
	/// </summary>
	/// <param name="device">�L���ȃf�o�C�X</param>
	/// <param name="file_name">�t�H���g�e�N�X�`���̃t�@�C���A�h���X</param>
	/// <param name="size_per_char">1����������̐؂���T�C�Y</param>
	/// <returns>�o�^�����n���h��(�o�^�Ɏ��s�����ꍇ-1)</returns>
	Handle addFont(ID3D11Device* device, const wchar_t* file_name, FLOAT2 size_per_char = { 16.0f,16.0f });

	/// <summary>
	/// �������`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="handle">�t�H���g�̃n���h��</param>
	/// <param name="str">�`�悷�镶����</param>
	/// <param name="pos">�\���ʒu</param>
	/// <param name="scale">�X�P�[��</param>
	/// <param name="color">�F</param>
	/// <returns>�\���I�����̉E���̍��W</returns>
	FLOAT2 print(ID3D11DeviceContext* immediate_context,
		Handle handle,
		std::string str,
		FLOAT2 pos,
		FLOAT2 scale = { 1.0f,1.0f },
		FLOAT4 color = { 1,1,1,1 });

	//�C���X�^���X���쐬���܂�
	//�y�Ԃ�l�z�C���X�^���X�̍쐬�ɐ��������ꍇtrue��Ԃ��܂��B
	static bool createInstance();

	//�C���X�^���X���擾���܂��B
	static ASCII* getInstance()
	{
		if(instance == nullptr)createInstance();
		return instance.get();
	}

	//�C���X�^���X��j�����܂��B
	//�y�Ԃ�l�z�C���X�^���X�̔j���ɐ��������ꍇtrue��Ԃ��܂��B
	static bool destroyInstance();
	~ASCII();
};

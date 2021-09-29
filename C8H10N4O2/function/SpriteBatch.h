#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include <vector>
#include "Arithmetic.h"
#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

class SpriteBatch final
{
public:
	struct vertex
	{
		FLOAT3 position;
		FLOAT2 texcoord;
		FLOAT4 color;
	};
private:
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	FLOAT2								textrue_size;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	const size_t						max_vertices;
	std::vector<vertex>					vertices;
	SpriteBatch(SpriteBatch&) = delete;
	SpriteBatch& operator =(SpriteBatch&) = delete;
public:
	SpriteBatch(ID3D11Device* device, size_t max_sprites, const wchar_t* file_name = nullptr);
	SpriteBatch(ID3D11Device* device, size_t max_sprites, ID3D11ShaderResourceView* new_shader_resource_view);

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[�����т��܂�
	/// </summary>
	/// <param name="new_shader_resource_view">�V�F�[�_�[���\�[�X�r���[</param>
	/// <returns>���������true</returns>
	bool attach(ID3D11ShaderResourceView* new_shader_resource_view);

	/// <summary>
	/// �V�F�[�_�[���\�[�X�r���[���擾���܂��B
	/// </summary>
	ID3D11ShaderResourceView* getShaderResourceView() { return shader_resource_view.Get(); }

	/// <summary>
	/// �`��̊J�n����
	/// �����ŃV�F�[�_�[��SRV��ݒ肵�Ă���B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="external_pixel_shader">
	/// �K�p����s�N�Z���V�F�[�_�[
	/// NULL�ŋK��̃V�F�[�_�[
	/// </param>
	void begin(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader = NULL);
	/// <summary>
	/// �`�悷��f�[�^�̏�������
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="pos">�\�����W</param>
	/// <param name="size">�\���T�C�Y</param>
	/// <param name="texpos">�e�N�X�`���̐؂���J�n�ʒu</param>
	/// <param name="texsize">�e�N�X�`���̐؂���T�C�Y</param>
	/// <param name="angle">��]�p�x(���W�A��)</param>
	/// <param name="color">�\���F</param>
	void render(ID3D11DeviceContext* immediate_context,
		FLOAT2 pos,
		FLOAT2 size,
		FLOAT2 texpos = {},
		FLOAT2 texsize = {},
		float angle = 0.0f,
		XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
	/// <summary>
	/// ���ۂɕ`�悵�Ă���ꏊ
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void end(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �e�N�X�`���̃T�C�Y���擾���܂��B
	/// </summary>
	const FLOAT2& getTextrueSize()const { return textrue_size; }

	/// <summary>
	/// �e�N�X�`�����V�F�[�_�[�ɑ��M���܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="slot">���M�X���b�g�ԍ�</param>
	/// <param name="use_vs">���_�V�F�[�_�[�Ɏg�p���邩</param>
	/// <param name="use_ps">�s�N�Z���V�F�[�_�[�Ɏg�p���邩</param>
	/// <param name="use_ds">�h���C���V�F�[�_�[�Ɏg�p���邩</param>
	/// <param name="use_hs">�n���V�F�[�_�[�Ɏg�p���邩</param>
	/// <param name="use_gs">�W�I���g���V�F�[�_�[�Ɏg�p���邩</param>
	void sendTextrue(
		ID3D11DeviceContext* immediate_context,
		UINT slot,
		bool use_vs = true,
		bool use_ps = true,
		bool use_ds = true,
		bool use_hs = true,
		bool use_gs = true)const;

	virtual ~SpriteBatch() = default;
};

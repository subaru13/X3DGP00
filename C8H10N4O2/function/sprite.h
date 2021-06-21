#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "Arithmetic.h"
#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

class Sprite
{
public:
	struct vertex
	{
		FLOAT3 position;
		FLOAT2 texcoord;
	};
private:
	ComPtr<ID3D11PixelShader>			pixel_shader;
	ComPtr<ID3D11VertexShader>			vertex_shader;
	ComPtr<ID3D11InputLayout>			input_layout;
	ComPtr<ID3D11Buffer>				vertex_buffer;
	ComPtr<ID3D11ShaderResourceView>	shader_resource_view;
	ConstantBuffer<FLOAT4>				constant_buffer;
	FLOAT2								textrue_size;

	Sprite(Sprite&) = delete;
	Sprite& operator =(Sprite&) = delete;
public:
	
	Sprite(ID3D11Device* device, const wchar_t* file_name = nullptr);
	virtual ~Sprite() = default;

	/// <summary>
	/// �X�v���C�g��\�����܂��B
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
		FLOAT4 color = { 1.0f,1.0f,1.0f,1.0f });
	
	/// <summary>
	/// �e�N�X�`���̑傫�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	const FLOAT2& getTextrueSize()const { return textrue_size; }
	
};
#pragma once
#include "Arithmetic.h"
#include <d3d11.h>
#include <wrl.h>
#include <wchar.h>
#include <functional>
#include <memory>

using namespace Microsoft::WRL;

class Particle
{
public:
	struct innovation_vertex
	{
		FLOAT3	position = {};
		FLOAT2	size = {};
		FLOAT3	velocity = {};
		FLOAT3	acceleration = {};
		FLOAT	alpha = 0.0f;
		FLOAT	attenuation_alpha = 0.0f;
		FLOAT	timer = 0.0f;
		FLOAT	animation_timer = 0.0f;
		INT		chip_num = -1;
	};

private:
	struct vertex
	{
		FLOAT4	position;	//	xyz	->	world_position	w	->	no used
		FLOAT4	color;		//	xy	->	scale	z	->	type	w	->	alpha
	};

	struct textrue_data
	{
		FLOAT2	size;
		FLOAT2	per_chip;
	};

	struct conversion_material
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};

	ComPtr<ID3D11DepthStencilState>			depth_stencil_state;
	ComPtr<ID3D11RasterizerState>			rasterizer_state;
	ComPtr<ID3D11SamplerState>				sampler_state;
	ComPtr<ID3D11BlendState>				blend_state;

	ComPtr<ID3D11PixelShader>				pixel_shader;
	ComPtr<ID3D11GeometryShader>			geometry_shader;
	ComPtr<ID3D11VertexShader>				vertex_shader;
	ComPtr<ID3D11InputLayout>				input_layout;

	const size_t							max_vertex;
	std::shared_ptr<innovation_vertex[]>	vertex_data;
	ComPtr<ID3D11Buffer>					vertex_buffer;

	textrue_data							tex_data;
	ComPtr<ID3D11Buffer>					textrue_constant;

	conversion_material						conversion_data;
	ComPtr<ID3D11Buffer>					conversion_constant;

	ComPtr<ID3D11ShaderResourceView>		shader_resource_view;
	D3D11_TEXTURE2D_DESC					texture2d_desc;


public:

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="device">�L���ȃf�o�C�X</param>
	/// <param name="textrue_name">�e�N�X�`���̃p�X</param>
	/// <param name="_max_vertex">���_�̍ő吔</param>
	Particle(ID3D11Device* device, const wchar_t* textrue_name, size_t _max_vertex);

	/// <summary>
	/// �e�N�X�`����1�`�b�v������̐؂���T�C�Y��ݒ肵�܂��B
	/// </summary>
	/// <param name="per_chip">1�`�b�v������̐؂���T�C�Y</param>
	void setTexturePerChip(FLOAT2 per_chip) { tex_data.per_chip = per_chip; }

	/// <summary>
	/// �e�N�X�`����1�`�b�v������̐؂���T�C�Y���擾���܂��B
	/// </summary>
	const FLOAT2& getTexturePerChip()const { return tex_data.per_chip; }

	/// <summary>
	/// �e�N�X�`���̑傫�����擾���܂��B
	/// </summary>
	const FLOAT2& getTextrueSize()const { return tex_data.size; }


	/// <summary>
	/// �r���[�s���ݒ肵�܂��B
	/// </summary>
	/// <param name="view">�ݒ肷��r���[�s��</param>
	void setView(FLOAT4X4 view) { conversion_data.view = view; }

	/// <summary>
	/// �v���W�F�N�V�����s���ݒ肵�܂��B
	/// </summary>
	/// <param name="projection">�ݒ肷��v���W�F�N�V�����s��</param>
	void setProjection(FLOAT4X4 projection) { conversion_data.projection = projection; }

	/// <summary>
	/// �r���[�s����擾���܂��B
	/// </summary>
	const FLOAT4X4& getView()const { return conversion_data.view; }

	/// <summary>
	/// �v���W�F�N�V�����s����擾���܂��B
	/// </summary>
	const FLOAT4X4& getProjection()const { return conversion_data.projection; }

	/// <summary>
	/// �ő咸�_�����擾���܂��B
	/// </summary>
	const size_t& getMaxVertex()const { return max_vertex; }

	/// <summary>
	/// ���_�̍X�V�����܂��B
	/// </summary>
	/// <param name="dt">�o�ߎ���</param>
	void update(FLOAT dt);

	/// <summary>
	/// �A�j���V���������s���܂��B
	/// </summary>
	/// <param name="dt">�o�ߎ���</param>
	/// <param name="speed">�`�b�v�؂�ւ��̑��x</param>
	/// <param name="stop_chip_count">�`�b�v�̍ő吔</param>
	void animation(FLOAT dt, FLOAT speed, INT stop_chip_count);

	/// <summary>
	/// ���_�f�[�^��ݒ肵�܂��B
	/// </summary>
	/// <param name="position">�������W</param>
	/// <param name="size">�傫��</param>
	/// <param name="velocity">�������x</param>
	/// <param name="acceleration">�����x</param>
	/// <param name="alpha">���������x</param>
	/// <param name="attenuation_alpha">�����x�̌�����</param>
	/// <param name="timer">�����^�C�}�[</param>
	/// <param name="chip_num">�g�p����`�b�v�̔ԍ� 0�X�^�[�g</param>
	/// <returns>�o�^�ł�����true</returns>
	bool setVertex(innovation_vertex data);

	/// <summary>
	/// ���_�f�[�^��ݒ肵�܂��B
	/// </summary>
	/// <param name="position">�������W</param>
	/// <param name="size">�傫��</param>
	/// <param name="velocity">�������x</param>
	/// <param name="acceleration">�����x</param>
	/// <param name="alpha">���������x</param>
	/// <param name="attenuation_alpha">�����x�̌�����</param>
	/// <param name="timer">�����^�C�}�[</param>
	/// <param name="chip_num">�g�p����`�b�v�̔ԍ� 0�X�^�[�g</param>
	/// <returns>�o�^�ł�����true</returns>
	size_t setVertexs(size_t num, std::function<void(innovation_vertex&)> setter);


	/// <summary>
	/// �`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void render(ID3D11DeviceContext* immediate_context);

};

using InnovationVertex = Particle::innovation_vertex;
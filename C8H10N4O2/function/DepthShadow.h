#pragma once

#include "OffScreen.h"
#include "SceneConstantBuffer.h"
#include <functional>

class DepthShadow
{
public:

	enum FETCH_PIXEL_SHADER_ID { FPSI_GEOMETRY_PRIMITIVE, FPSI_SKINNED_MESH };

	struct Data
	{
		FLOAT4X4	view_orthographic{};
		FLOAT3		color{ 0,0,0 };
		float		bias = 0.0008f;
	};
private:
	FrameBuffer depth_map;
	SceneConstant scene_constant;
	ConstantBuffer<Data> depth_shadow_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[2];
public:
	DepthShadow(ID3D11Device* device, UINT width, UINT height, FLOAT zfar = 200.0f);
	/// <summary>
	/// �[�x�������� + �e�`��
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="drawing_process">�`�揈��</param>
	/// <param name="light_direction">���̌���</param>
	/// <param name="distance">���_��������܂ł̋���</param>
	void blit(ID3D11DeviceContext* immediate_context,
		std::function<void(ID3D11PixelShader** [2])> drawing_process,
		const FLOAT3& light_direction, float distance = 100.0f);
	/// <summary>
	/// �e�̐F��ݒ�
	/// </summary>
	/// <param name="color">�e�̐F</param>
	void setShadowColor(FLOAT3 color) { depth_shadow_constant_buffer.data.color = color; }
	/// <summary>
	/// �C�ӂ̎����i�e�̂Ȃ�m���j��ݒ�
	/// </summary>
	/// <param name="bias">�C�ӂ̎���</param>
	void setBias(float bias) { depth_shadow_constant_buffer.data.bias = bias; }
	/// <summary>
	/// �e�̐F���擾
	/// </summary>
	const FLOAT3& getShadowColor()const { return depth_shadow_constant_buffer.data.color; }
	/// <summary>
	/// �C�ӂ̎����i�e�̂Ȃ�m���j���擾
	/// </summary>
	const float& getBias()const { return depth_shadow_constant_buffer.data.bias; }
	virtual ~DepthShadow() = default;
};


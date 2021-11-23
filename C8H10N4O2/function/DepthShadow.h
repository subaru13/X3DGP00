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
	/// 深度書き込み + 影描画
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="drawing_process">描画処理</param>
	/// <param name="light_direction">光の向き</param>
	/// <param name="distance">原点から光源までの距離</param>
	void blit(ID3D11DeviceContext* immediate_context,
		std::function<void(ID3D11PixelShader** [2])> drawing_process,
		const FLOAT3& light_direction, float distance = 100.0f);
	/// <summary>
	/// 影の色を設定
	/// </summary>
	/// <param name="color">影の色</param>
	void setShadowColor(FLOAT3 color) { depth_shadow_constant_buffer.data.color = color; }
	/// <summary>
	/// 任意の実数（影のなる確率）を設定
	/// </summary>
	/// <param name="bias">任意の実数</param>
	void setBias(float bias) { depth_shadow_constant_buffer.data.bias = bias; }
	/// <summary>
	/// 影の色を取得
	/// </summary>
	const FLOAT3& getShadowColor()const { return depth_shadow_constant_buffer.data.color; }
	/// <summary>
	/// 任意の実数（影のなる確率）を取得
	/// </summary>
	const float& getBias()const { return depth_shadow_constant_buffer.data.bias; }
	virtual ~DepthShadow() = default;
};


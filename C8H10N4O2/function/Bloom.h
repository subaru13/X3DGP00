#pragma once

#include "GaussianBlur.h"

class BloomRenderer
{
private:
	struct Parameters
	{
		float threshold = 0.5f;
		FLOAT3 dummy;
	};
	GaussianFilter								blur_filter;
	OffScreen									screen_buffer;
	OffScreen									luminance_buffer;
	ConstantBuffer<Parameters>					constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	luminance_extraction;
	Microsoft::WRL::ComPtr<ID3D11BlendState>	additive_synthesis;
public:
	BloomRenderer(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	void setThreshold(float threshold) { constant_buffer.data.threshold = threshold; }
	const float& getThreshold()const { return constant_buffer.data.threshold; }

	/// <summary>
	/// ブルームをかける前の書き込みの開始処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void beginWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// ブルームをかける前の書き込みの終了処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void endWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// ブルームをかけて画面全体に描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="kernel_size">カーネルサイズ</param>
	/// <param name="sigma">シグマ</param>
	void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50);
};


#pragma once

#include "GaussianBlur.h"

class BloomRenderer:public GaussianFilter
{
private:
	struct LuminanceParameters
	{
		float threshold = 0.5f;
		FLOAT3 dummy;
	};
	std::shared_ptr<FrameBuffer>				screen_buffer;
	std::shared_ptr<FrameBuffer>				luminance_buffer;
	ConstantBuffer<LuminanceParameters>			luminance_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	luminance_extraction;
	Microsoft::WRL::ComPtr<ID3D11BlendState>	additive_synthesis;
public:
	BloomRenderer(ID3D11Device* device, UINT w, UINT h, UINT shrink = 8, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	/// <summary>
	/// 閾値を設定します。
	/// </summary>
	/// <param name="threshold">新しい閾値</param>
	void setThreshold(float threshold) { luminance_constant_buffer.data.threshold = threshold; }

	/// <summary>
	/// 閾値を取得します。
	/// </summary>
	const float& getThreshold()const { return luminance_constant_buffer.data.threshold; }

	/// <summary>
	/// ブルームをかける前の書き込みの開始処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void beginWriting(ID3D11DeviceContext* immediate_context)override;

	/// <summary>
	/// ブルームをかける前の書き込みの終了処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	void endWriting(ID3D11DeviceContext* immediate_context)override;

	/// <summary>
	/// ブルームをかけて画面全体に描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="kernel_size">カーネルサイズ</param>
	/// <param name="sigma">シグマ</param>
	void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50)override;
};


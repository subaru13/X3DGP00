#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <functional>
#include "Arithmetic.h"
#include "OffScreen.h"
#include "ConstantBuffer.h"

#define GAUSSIAN_BLUR_WEIGHT_SIZE 256

class GaussianFilter
	:public Peinter
{
protected:
	struct BlurParameters
	{
		FLOAT4 weight[GAUSSIAN_BLUR_WEIGHT_SIZE] = {};
		int kernel_size = 10;
		FLOAT2 texcel = {};
		float dummy = 0;
	};
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	blur_pixel_shader;
	ConstantBuffer<BlurParameters>				blur_constant_buffer;
	std::shared_ptr<FrameBuffer>				render_target;
	void getWeight(FLOAT4* array, int kernel_size, float sigma)const;
	//継承先用
	void _quad(ID3D11DeviceContext* immediate_context, int kernel_size, float sigma);
public:
	GaussianFilter(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	/// <summary>
	/// ブラーをかける前の書き込みの開始処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	virtual void beginWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// ブラーをかける前の書き込みの終了処理
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	virtual void endWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// ブラーをかけて画面全体に描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="kernel_size">カーネルサイズ</param>
	/// <param name="sigma">シグマ</param>
	virtual void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50);
};


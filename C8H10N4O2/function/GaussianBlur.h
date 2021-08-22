#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <functional>
#include "Arithmetic.h"
#include "OffScreen.h"

#define GAUSSIAN_BLUR_WEIGHT_SIZE 256

class GaussianFilter
{
private:
	struct Parameters
	{
		FLOAT4 weight[GAUSSIAN_BLUR_WEIGHT_SIZE] = {};
		int kernel_size = 10;
		FLOAT2 texcel = {};
		float dummy = 0;
	};
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixel_shader;
	ConstantBuffer<Parameters>					constant_buffer;
	OffScreen									render_traget;
	void getWeight(FLOAT4* array, int kernel_size, float sigma)const;
public:
	GaussianFilter(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	/// <summary>
	/// �u���[��������O�̏������݂̊J�n����
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void beginWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �u���[��������O�̏������݂̏I������
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void endWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �u���[�������ĉ�ʑS�̂ɕ`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="kernel_size">�J�[�l���T�C�Y</param>
	/// <param name="sigma">�V�O�}</param>
	void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50);
};


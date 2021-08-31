#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <functional>
#include "Arithmetic.h"
#include "OffScreen.h"
#include "ConstantBuffer.h"

#define GAUSSIAN_BLUR_WEIGHT_SIZE 256

class GaussianFilter
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
	std::shared_ptr<OffScreen>					render_traget;
	std::shared_ptr<FullScreenQuad>				renderer;
	void getWeight(FLOAT4* array, int kernel_size, float sigma)const;
	//�p����p
	void _quad(ID3D11DeviceContext* immediate_context, int kernel_size, float sigma);
public:
	GaussianFilter(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	/// <summary>
	/// �u���[��������O�̏������݂̊J�n����
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	virtual void beginWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �u���[��������O�̏������݂̏I������
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	virtual void endWriting(ID3D11DeviceContext* immediate_context);

	/// <summary>
	/// �u���[�������ĉ�ʑS�̂ɕ`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="kernel_size">�J�[�l���T�C�Y</param>
	/// <param name="sigma">�V�O�}</param>
	virtual void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50);
};


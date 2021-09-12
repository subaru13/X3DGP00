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
	std::shared_ptr<FrameBuffer>					screen_buffer;
	std::shared_ptr<FrameBuffer>					luminance_buffer;
	ConstantBuffer<LuminanceParameters>			luminance_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	luminance_extraction;
	Microsoft::WRL::ComPtr<ID3D11BlendState>	additive_synthesis;
public:
	BloomRenderer(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	/// <summary>
	/// 臒l��ݒ肵�܂��B
	/// </summary>
	/// <param name="threshold">�V����臒l</param>
	void setThreshold(float threshold) { luminance_constant_buffer.data.threshold = threshold; }

	/// <summary>
	/// 臒l���擾���܂��B
	/// </summary>
	const float& getThreshold()const { return luminance_constant_buffer.data.threshold; }

	/// <summary>
	/// �u���[����������O�̏������݂̊J�n����
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void beginWriting(ID3D11DeviceContext* immediate_context)override;

	/// <summary>
	/// �u���[����������O�̏������݂̏I������
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	void endWriting(ID3D11DeviceContext* immediate_context)override;

	/// <summary>
	/// �u���[���������ĉ�ʑS�̂ɕ`�悵�܂��B
	/// </summary>
	/// <param name="immediate_context">�L���ȃR���e�L�X�g</param>
	/// <param name="kernel_size">�J�[�l���T�C�Y</param>
	/// <param name="sigma">�V�O�}</param>
	void quad(ID3D11DeviceContext* immediate_context,
		int kernel_size = 10,
		float sigma = 50)override;
};


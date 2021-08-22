#include "Bloom.h"
#include "MyHandy.h"
#include "Misc.h"
#include "CreateComObjectHelpar.h"
#include "../FrameworkConfig.h"

BloomRenderer::BloomRenderer(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format)
	:blur_filter(device, w / 8, h / 8, format),
	screen_buffer(device, OFFSCREEN_LINK::RENDER_TARGET, w, h, format),
	luminance_buffer(device, OFFSCREEN_LINK::RENDER_TARGET, w, h, format),
	constant_buffer(device)
{
	assert(device && "The device is invalid.");
	HRESULT hr = S_OK;
	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "luminance_extraction_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, luminance_extraction.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"cbuffer Color : register(b0)\n"
			"{\n"
			"	float4 color;\n"
			"};\n"
			"cbuffer constant : register(b1)\n"
			"{\n"
			"	float Threshold;\n"
			"	float3 dummy;\n"
			"};\n"
			"float getBrightness(float3 color)\n"
			"{\n"
			"	float3 y = { 0.299, 0.587, 0.114 };\n"
			"	float bright = dot(y, color);\n"
			"	return bright;\n"
			"}\n"
			"Texture2D diffuse_map : register(t0);\n"
			"SamplerState diffuse_map_sampler_state : register(s0);\n"
			"float4 main(VS_OUT input) : SV_TARGET0\n"
			"{\n"
			"	float4 result = (float4)0;\n"
			"	result = diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord) * color;\n"
			"	float brightness = getBrightness(result.rgb);\n"
			"	float contribution = max(0, brightness - Threshold);\n"
			"	contribution /= brightness;\n"
			"	result.rgb *= contribution;\n"
			"	return result;\n"
			"}\n";

		hr = createPixelShader(device, ps, luminance_extraction.ReleaseAndGetAddressOf());
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_BLEND_DESC blend_desc = {};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, additive_synthesis.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void BloomRenderer::beginWriting(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	screen_buffer.clear(immediate_context);
	screen_buffer.active(immediate_context);
}

void BloomRenderer::endWriting(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	screen_buffer.deactive(immediate_context);
}

void BloomRenderer::quad(ID3D11DeviceContext* immediate_context, int kernel_size, float sigma)
{
	assert(immediate_context && "The context is invalid.");
	assert(kernel_size >= 0 && "The kernel_size is invalid.");
	luminance_buffer.clear(immediate_context);
	luminance_buffer.active(immediate_context);
	constant_buffer.send(immediate_context, 1, false, true);
	screen_buffer.quad(immediate_context, luminance_extraction.GetAddressOf());
	luminance_buffer.deactive(immediate_context);
	blur_filter.beginWriting(immediate_context);
	luminance_buffer.quad(immediate_context, NULL);
	blur_filter.endWriting(immediate_context);
	screen_buffer.quad(immediate_context);
	Microsoft::WRL::ComPtr<ID3D11BlendState>	cached_blend_state;
	FLOAT blend_factor[4] = { 1,1,1,1 };
	UINT sample_mask = 0xffffffff;
	immediate_context->OMGetBlendState(cached_blend_state.ReleaseAndGetAddressOf(), blend_factor, &sample_mask);
	immediate_context->OMSetBlendState(additive_synthesis.Get(), nullptr, 0xffffffff);
	blur_filter.quad(immediate_context, kernel_size, sigma);
	immediate_context->OMSetBlendState(cached_blend_state.Get(), blend_factor, sample_mask);
}

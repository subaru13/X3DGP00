#include "GaussianBlur.h"
#include "MyHandy.h"
#include "Misc.h"
#include "CreateComObjectHelpar.h"
#include "../FrameworkConfig.h"

void GaussianFilter::getWeight(FLOAT4* array, int kernel_size, float sigma) const
{
	float sum = 0.0f;
	int id = 0;
	float sigmag = (sigma * sigma);
	float sigmag2 = sigmag * 2.0f;
	float coefficient = (1.0f / ((Pi * 2.0f) * sigmag));
	int haif_kernel_size = kernel_size / 2;
	for (int y = -haif_kernel_size; y <= haif_kernel_size; y++)
	{
		for (int x = -haif_kernel_size; x <= haif_kernel_size; x++)
		{
			array[id].x = (float)x;
			array[id].y = (float)y;
			//ŽQl::https://www.eng.kagawa-u.ac.jp/~tishii/Lab/Etc/gauss.html
			array[id].z = coefficient * std::expf(-((x * x + y * y) / sigmag2));
			sum += array[id].z;
			id++;
		}
	}
	int pow2_kernel_size = kernel_size * kernel_size;
	for (int i = 0; i < pow2_kernel_size; i++)
		array[i].z /= sum;
}

GaussianFilter::GaussianFilter(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format)
	:constant_buffer(device),
	render_traget(device, OFFSCREEN_LINK::RENDER_TARGET, w, h, format)
{
	assert(device && "The device is invalid.");
	HRESULT hr = S_OK;
	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "gaussian_blur_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"struct VS_OUT\n"
			"{\n"
			"    float4 position : SV_POSITION;\n"
			"    float2 texcoord : TEXCOORD;\n"
			"};\n"
			"cbuffer Color : register(b0)\n"
			"{\n"
			"    float4 color;\n"
			"};\n"
			"#define BUFFER_SIZE 256\n"
			"cbuffer Parameters : register(b1)\n"
			"{\n"
			"    float4 Weight[BUFFER_SIZE];\n"
			"    int KernelSize;\n"
			"    float2 texcel;\n"
			"    float dummy;\n"
			"};\n"
			"Texture2D diffuse_map : register(t0);\n"
			"SamplerState diffuse_map_sampler_state : register(s0);\n"
			"float4 main(VS_OUT input) : SV_TARGET\n"
			"{\n"
			"    float4 result = (float4)0;\n"
			"    result.a = color.a;\n"
			"    for (int i = 0; i < KernelSize * KernelSize; i++)\n"
			"    {\n"
			"        float2 offset = texcel * Weight[i].xy;\n"
			"        float weight = Weight[i].z;\n"
			"        result.rgb += diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord + offset).rgb * weight;\n"
			"    }\n"
			"    return result;\n"
			"};\n";

		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	constant_buffer.data.texcel.x = 1.0f / static_cast<float>(w);
	constant_buffer.data.texcel.y = 1.0f / static_cast<float>(h);
}

void GaussianFilter::beginWriting(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	render_traget.clear(immediate_context);
	render_traget.active(immediate_context);
}

void GaussianFilter::endWriting(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	render_traget.deactive(immediate_context);
}

void GaussianFilter::quad(ID3D11DeviceContext* immediate_context, int kernel_size, float sigma)
{
	assert(immediate_context && "The context is invalid.");
	assert(kernel_size >= 0 && "The kernel_size is invalid.");
	constant_buffer.data.kernel_size = kernel_size;
	FLOAT4 weight[GAUSSIAN_BLUR_WEIGHT_SIZE] = {};
	getWeight(weight, kernel_size, sigma);
	memcpy(constant_buffer.data.weight, weight, sizeof(FLOAT4) * GAUSSIAN_BLUR_WEIGHT_SIZE);
	constant_buffer.send(immediate_context, 1, false, true);
	render_traget.quad(immediate_context, pixel_shader.GetAddressOf());
}
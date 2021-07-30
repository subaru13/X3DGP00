#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
#include "Misc.h"

enum class SAMPLER_STATE { SS_POINT, SS_LINEAR, SS_ANISOTROPIC };

class SamplerStates final
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> states[3] = { nullptr };
public:
	SamplerStates(ID3D11Device* device)
	{
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = device->CreateSamplerState(&sampler_desc, states[static_cast<int>(SAMPLER_STATE::SS_POINT)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device->CreateSamplerState(&sampler_desc, states[static_cast<int>(SAMPLER_STATE::SS_LINEAR)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device->CreateSamplerState(&sampler_desc, states[static_cast<int>(SAMPLER_STATE::SS_ANISOTROPIC)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	ID3D11SamplerState** at(SAMPLER_STATE type) { return states[static_cast<int>(type)].GetAddressOf(); }

	~SamplerStates() {}
};

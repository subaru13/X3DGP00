#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
#include "Misc.h"
enum class RASTERIZER_FILL { BACK_SOLID, FRONT_SOLID, WIREFRAME };
class RasterizerStates
{
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  states[3][2];
public:
	RasterizerStates(ID3D11Device* device)
	{
		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		HRESULT hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::BACK_SOLID)][FALSE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = TRUE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::BACK_SOLID)][TRUE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_FRONT;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::FRONT_SOLID)][FALSE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_FRONT;
		rasterizer_desc.FrontCounterClockwise = TRUE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::FRONT_SOLID)][TRUE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::WIREFRAME)][FALSE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.FrontCounterClockwise = TRUE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc, states[static_cast<int>(RASTERIZER_FILL::WIREFRAME)][TRUE].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	ID3D11RasterizerState* at(RASTERIZER_FILL type, BOOL clockwise) { return states[static_cast<int>(type)][clockwise==TRUE].Get(); }
	virtual ~RasterizerStates() {}
};

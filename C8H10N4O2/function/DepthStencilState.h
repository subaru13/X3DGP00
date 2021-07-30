#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <assert.h>
#include "Misc.h"

enum class DEPTH_STENCIL_STATE { DS_TRUE, DS_FALSE, DS_WRITE_TRUE, DS_WRITE_FALSE };

class DepthStencilStates
{
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> states[4];
public:
	DepthStencilStates(ID3D11Device* device)
	{
		assert(device && "The device is invalid.");
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		//デプスステンシルステート
		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, states[static_cast<int>(DEPTH_STENCIL_STATE::DS_TRUE)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		depth_stencil_desc.DepthEnable = FALSE;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, states[static_cast<int>(DEPTH_STENCIL_STATE::DS_FALSE)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, states[static_cast<int>(DEPTH_STENCIL_STATE::DS_WRITE_TRUE)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, states[static_cast<int>(DEPTH_STENCIL_STATE::DS_WRITE_FALSE)].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}
	ID3D11DepthStencilState* at(DEPTH_STENCIL_STATE type) { return states[static_cast<int>(type)].Get(); }
	virtual ~DepthStencilStates() {}
};
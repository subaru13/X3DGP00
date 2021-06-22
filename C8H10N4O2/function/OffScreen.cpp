#include "OffScreen.h"

OffScreen::OffScreen(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format, bool need_renderer)
{
	assert(device && "The device is invalid.");
	ComPtr<ID3D11Texture2D> Texture2D;
	HRESULT hr = S_OK;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = w;
	texture2d_desc.Height = h;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, Texture2D.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(Texture2D.Get(), &rtvd, render_traget_view.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(Texture2D.Get(), &srvd, associated_shader_resource_view.ReleaseAndGetAddressOf());
	assert(SUCCEEDED(hr));
	if (need_renderer)
	{
		renderer = std::make_shared<Sprite>(device, associated_shader_resource_view.Get());
	}
}

void OffScreen::clear(ID3D11DeviceContext* immediate_context, FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->ClearRenderTargetView(render_traget_view.Get(), &color.x);
}

void OffScreen::active(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->OMGetRenderTargets(1, original_render_traget_view.ReleaseAndGetAddressOf(),
		original_depth_stencil_view.ReleaseAndGetAddressOf());
	immediate_context->OMSetRenderTargets(1, render_traget_view.GetAddressOf(), nullptr);
}

void OffScreen::deactive(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->OMSetRenderTargets(1, original_render_traget_view.GetAddressOf(), original_depth_stencil_view.Get());
}

void OffScreen::quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader)
{
	assert(immediate_context && "The context is invalid.");
	if (renderer)renderer->quad(immediate_context, external_pixel_shader);
}


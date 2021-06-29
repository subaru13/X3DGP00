#include "OffScreen.h"
#include "misc.h"

void OffScreen::_render_traget(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;
	{
		ComPtr<ID3D11Texture2D> render_traget_buffer;
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

		hr = device->CreateTexture2D(&texture2d_desc, NULL, render_traget_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hr = device->CreateRenderTargetView(render_traget_buffer.Get(), &rtvd, render_traget_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(render_traget_buffer.Get(), &srvd, associated_shader_resource_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = w;
		texture2d_desc.Height = h;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = texture2d_desc.Format;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void OffScreen::_depth_stencil(ID3D11Device* device, UINT w, UINT h, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;
	{
		ComPtr<ID3D11Texture2D> render_traget_buffer;
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
		texture2d_desc.Width = w;
		texture2d_desc.Height = h;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET;

		hr = device->CreateTexture2D(&texture2d_desc, NULL, render_traget_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = texture2d_desc.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hr = device->CreateRenderTargetView(render_traget_buffer.Get(), &rtvd, render_traget_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	{
		ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		//	テクスチャ作成
		ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
		texture2d_desc.Width = w;
		texture2d_desc.Height = h;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = format;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// 深度ステンシルビュー設定
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvd, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//	シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &srvd, associated_shader_resource_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

OffScreen::OffScreen(ID3D11Device* device, LINK_DESTINATION _link_destination, UINT w, UINT h, DXGI_FORMAT format, bool need_renderer)
	:link_destination(_link_destination), associated_shader_resource_view(nullptr), render_traget_view(nullptr), depth_stencil_view(nullptr),
	original_render_traget_view(nullptr), original_depth_stencil_view(nullptr),renderer(nullptr),
	viewport(),original_viewport(),num_views(ARRAYSIZE(original_viewport))
{
	assert(device && "The device is invalid.");

	switch (link_destination)
	{
	case OffScreen::LINK_DESTINATION::RENDER_TARGET:
		_render_traget(device, w, h, format);
		break;
	case OffScreen::LINK_DESTINATION::DEPTH_STENCIL:
		_depth_stencil(device, w, h, format);
		break;
	default:
		break;
	}

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(w);
	viewport.Height = static_cast<float>(h);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	if (need_renderer)
	{
		renderer = std::make_shared<Sprite>(device, associated_shader_resource_view.Get());
	}
}

void OffScreen::clear(ID3D11DeviceContext* immediate_context, FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->ClearRenderTargetView(render_traget_view.Get(), &color.x);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void OffScreen::active(ID3D11DeviceContext* immediate_context, bool usu_depth)
{
	assert(immediate_context && "The context is invalid.");
	num_views = ARRAYSIZE(original_viewport);
	immediate_context->RSGetViewports(&num_views, original_viewport);
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMGetRenderTargets(1, original_render_traget_view.ReleaseAndGetAddressOf(),
		original_depth_stencil_view.ReleaseAndGetAddressOf());
	immediate_context->OMSetRenderTargets(1, render_traget_view.GetAddressOf(), usu_depth ? depth_stencil_view.Get() : nullptr);
}

void OffScreen::deactive(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->OMSetRenderTargets(1, original_render_traget_view.GetAddressOf(), original_depth_stencil_view.Get());
	immediate_context->RSSetViewports(num_views, original_viewport);
}

void OffScreen::quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader)
{
	assert(immediate_context && "The context is invalid.");
	if (renderer)renderer->quad(immediate_context, external_pixel_shader);
}


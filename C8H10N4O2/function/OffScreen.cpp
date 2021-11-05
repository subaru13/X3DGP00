#include "OffScreen.h"
#include "Misc.h"
#include "CreateComObjectHelpar.h"
#include "MyHandy.h"
#include "../FrameworkConfig.h"
#include <assert.h>

void FrameBuffer::createRenderTarget(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = config.render_target_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, render_target_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = config.render_target_format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	hr = device->CreateRenderTargetView(render_target_buffer.Get(), &rtvd, render_target_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = config.render_target_format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(render_target_buffer.Get(), &srvd, render_target_shader_resource_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void FrameBuffer::createDepthStencil(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = config.depth_stencil_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &srvd, depth_stencil_shader_resource_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void FrameBuffer::createRenderTargetCube(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = config.render_target_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateTexture2D(&texture2d_desc, NULL, render_target_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvd.Texture2DArray.FirstArraySlice = 0;
	rtvd.Texture2DArray.MipSlice = 0;
	rtvd.Texture2DArray.ArraySize = 6;
	hr = device->CreateRenderTargetView(render_target_buffer.Get(), &rtvd, render_target_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = config.render_target_format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = 1;
	hr = device->CreateShaderResourceView(render_target_buffer.Get(), &srvd, render_target_shader_resource_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void FrameBuffer::createDepthStencilCube(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = config.depth_stencil_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = config.depth_stencil_format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), NULL, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = config.depth_stencil_format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = 1;
	hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &srvd, depth_stencil_shader_resource_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

FrameBuffer::FrameBuffer(ID3D11Device* device, const CONFIG& _config)
	:render_target_shader_resource_view(nullptr),
	depth_stencil_shader_resource_view(nullptr),
	render_target_view(nullptr),
	depth_stencil_view(nullptr),
	original_render_target_view(nullptr),
	original_depth_stencil_view(nullptr),
	viewport(),
	original_viewport(),
	num_views(ARRAYSIZE(original_viewport)),
	config(_config)
{
	assert(device && "The device is invalid.");
	if (config.is_cube_map)
	{
		config.width = config.height;
		config.depth_stencil_format = DXGI_FORMAT_D32_FLOAT;
		createRenderTargetCube(device);
		createDepthStencilCube(device);
	}
	else
	{
		createRenderTarget(device);
		createDepthStencil(device);
	}

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(config.width);
	viewport.Height = static_cast<float>(config.height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void FrameBuffer::clear(ID3D11DeviceContext* immediate_context,FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->ClearRenderTargetView(render_target_view.Get(), &color.x);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FrameBuffer::active(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	num_views = ARRAYSIZE(original_viewport);
	immediate_context->RSGetViewports(&num_views, original_viewport);
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMGetRenderTargets(1, original_render_target_view.ReleaseAndGetAddressOf(),
		original_depth_stencil_view.ReleaseAndGetAddressOf());
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
}

void FrameBuffer::deactive(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->OMSetRenderTargets(1, original_render_target_view.GetAddressOf(), original_depth_stencil_view.Get());
	immediate_context->RSSetViewports(num_views, original_viewport);
}

void GeometryBuffer::createRenderTargets(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	for (UINT i = 0; i < SCREEN_COUNT; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
		texture2d_desc.Width = config.width;
		texture2d_desc.Height = config.height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = config.render_target_formats[i];
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		hr = device->CreateTexture2D(&texture2d_desc, NULL, render_target_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.Format = config.render_target_formats[i];
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), &rtvd, render_target_views[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = config.render_target_formats[i];
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(render_target_buffer.Get(), &srvd, render_target_shader_resource_views[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}
}

void GeometryBuffer::createDepthStencil(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = config.depth_stencil_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvd, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//	シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &srvd, depth_stencil_shader_resource_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void GeometryBuffer::createRenderTargetsCube(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	for (UINT i = 0; i < SCREEN_COUNT; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
		texture2d_desc.Width = config.width;
		texture2d_desc.Height = config.height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 6;
		texture2d_desc.Format = config.render_target_formats[i];
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, render_target_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		ZeroMemory(&rtvd, sizeof(rtvd));
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvd.Texture2DArray.FirstArraySlice = 0;
		rtvd.Texture2DArray.MipSlice = 0;
		rtvd.Texture2DArray.ArraySize = 6;
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), &rtvd, render_target_views[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = config.render_target_formats[i];
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvd.TextureCube.MostDetailedMip = 0;
		srvd.TextureCube.MipLevels = 1;
		hr = device->CreateShaderResourceView(render_target_buffer.Get(), &srvd, render_target_shader_resource_views[i].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}
}

void GeometryBuffer::createDepthStencilCube(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	//	テクスチャ作成
	ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
	texture2d_desc.Width = config.width;
	texture2d_desc.Height = config.height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 6;
	texture2d_desc.Format = config.depth_stencil_format;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//	デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = config.depth_stencil_format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvd.Texture2DArray.FirstArraySlice = 0;
	dsvd.Texture2DArray.ArraySize = 6;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), NULL, depth_stencil_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = config.depth_stencil_format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MostDetailedMip = 0;
	srvd.TextureCube.MipLevels = 1;
	hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &srvd, depth_stencil_shader_resource_view.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

GeometryBuffer::GeometryBuffer(ID3D11Device* device, const CONFIG& _config)
	:render_target_shader_resource_views(),
	depth_stencil_shader_resource_view(nullptr),
	render_target_views(),
	depth_stencil_view(nullptr),
	original_render_target_view(nullptr),
	original_depth_stencil_view(nullptr),
	viewport(),
	original_viewport(),
	num_views(ARRAYSIZE(original_viewport)),
	config(_config)
{
	assert(device && "The device is invalid.");

	if (config.is_cube_map)
	{
		config.width = config.height;
		config.depth_stencil_format = DXGI_FORMAT_D32_FLOAT;
		createRenderTargetsCube(device);
		createDepthStencilCube(device);
	}
	else
	{
		createRenderTargets(device);
		createDepthStencil(device);
	}

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(config.width);
	viewport.Height = static_cast<float>(config.height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void GeometryBuffer::clear(ID3D11DeviceContext* immediate_context, FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");
	for (auto rtv : render_target_views)
	{
		immediate_context->ClearRenderTargetView(rtv.Get(), &color.x);
	}
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GeometryBuffer::active(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	num_views = ARRAYSIZE(original_viewport);
	immediate_context->RSGetViewports(&num_views, original_viewport);
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMGetRenderTargets(1, original_render_target_view.ReleaseAndGetAddressOf(),
		original_depth_stencil_view.ReleaseAndGetAddressOf());
	immediate_context->OMSetRenderTargets(SCREEN_COUNT, getRenderTargetViews().data(), depth_stencil_view.Get());
}

void GeometryBuffer::deactive(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The context is invalid.");
	immediate_context->OMSetRenderTargets(1, original_render_target_view.GetAddressOf(), original_depth_stencil_view.Get());
	immediate_context->RSSetViewports(num_views, original_viewport);
}



Peinter::Peinter(ID3D11Device* device, const char* ps_filename)
	: pixel_shader(nullptr), vertex_shader(nullptr)
{
	assert(device && "The device is invalid.");
	HRESULT hr = S_OK;
	std::string cso_pass = ps_filename ? ps_filename : "";
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	}
	else if (isExistFileA(cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "full_screen_quad_ps.cso")))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"Texture2D texture_map : register(t0);\n"
			"SamplerState texture_map_sampler_state : register(s0);\n"
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	return texture_map.Sample(texture_map_sampler_state, pin.texcoord);\n"
			"}\n";

		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "full_screen_quad_vs.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadVertexShader(device, cso_pass, vertex_shader.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string vs =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"VS_OUT main(in uint vertex_id : SV_VERTEXID)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.texcoord = float2((vertex_id << 1) & 2, vertex_id & 2);\n"
			"	vout.position = float4(vout.texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);\n"
			"	return vout;\n"
			"}\n";
		hr = createVertexShader(device, vs, vertex_shader.ReleaseAndGetAddressOf());
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void Peinter::blit(ID3D11DeviceContext* immediate_context,
	ID3D11ShaderResourceView* shader_resource_view,
	ID3D11PixelShader** external_pixel_shader) const
{
	blit(immediate_context, &shader_resource_view, 1, external_pixel_shader);
}

void Peinter::blit(ID3D11DeviceContext* immediate_context,
	ID3D11ShaderResourceView** shader_resource_views,
	UINT shader_resource_views_count,
	ID3D11PixelShader** external_pixel_shader) const
{
	immediate_context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(nullptr);

	if (external_pixel_shader)
	{
		immediate_context->PSSetShader((*external_pixel_shader), nullptr, 0);
	}
	else
	{
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	}
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);

	immediate_context->PSSetShaderResources(0, shader_resource_views_count, shader_resource_views);

	immediate_context->Draw(4, 0);
}
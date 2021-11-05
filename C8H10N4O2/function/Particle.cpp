#include "particle.h"
#include "CreateComObjectHelpar.h"
#include "MyHandy.h"
#include "../FrameworkConfig.h"
#include <assert.h>

Particle::Particle(ID3D11Device* device, const wchar_t* textrue_name, size_t _max_vertex) :
	max_vertex(_max_vertex), vertex_data(new innovation_vertex[max_vertex]), pixel_shader(nullptr),
	vertex_shader(nullptr), geometry_shader(nullptr), input_layout(nullptr), vertex_buffer(nullptr),
	conversion_data(), conversion_constant(nullptr), tex_data(), textrue_constant(nullptr),
	shader_resource_view(nullptr), texture2d_desc(), sampler_state(nullptr), depth_stencil_state(nullptr),
	blend_state(nullptr), rasterizer_state(nullptr)
{
	assert(device && "The device is invalid.");
	assert(_max_vertex != 0 && "0 cannot be set for the number of vertices.");
	HRESULT hr = S_OK;

	{
		std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertex) };
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * max_vertex);
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = vertices.get();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}


	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0,	 DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "COLOR",		0,	 DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};


	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "particle_vs.cso");
	loadVertexShader(device,
		cso_pass,
		vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(),
		input_element_desc,
		ARRAYSIZE(input_element_desc));
	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "particle_ps.cso");
	loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "particle_gs.cso");
	loadGeometryShader(device, cso_pass, geometry_shader.ReleaseAndGetAddressOf());

	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(textrue_data);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = device->CreateBuffer(&bd, NULL, textrue_constant.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}

	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(conversion_material);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = device->CreateBuffer(&bd, NULL, conversion_constant.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}

	{
		D3D11_SAMPLER_DESC sd{};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sd, sampler_state.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}

	{
		D3D11_DEPTH_STENCIL_DESC dd{};
		dd.DepthEnable = TRUE;
		dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dd.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&dd, depth_stencil_state.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}

	{
		D3D11_BLEND_DESC bd{};
		bd.IndependentBlendEnable = FALSE;
		bd.AlphaToCoverageEnable = FALSE;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&bd, blend_state.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}

	{
		D3D11_RASTERIZER_DESC rd{};
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_BACK;
		rd.FrontCounterClockwise = TRUE;
		rd.DepthBias = 0;
		rd.DepthBiasClamp = 0;
		rd.SlopeScaledDepthBias = 0;
		rd.DepthClipEnable = TRUE;
		rd.ScissorEnable = FALSE;
		rd.MultisampleEnable = FALSE;
		rd.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rd, rasterizer_state.ReleaseAndGetAddressOf());
		assert(hr == S_OK);
	}
	if (textrue_name)
	{
		loadTextureFromFile(device, textrue_name, shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);
	}
	else
	{
		hr = makeDummyTexture(device, shader_resource_view.ReleaseAndGetAddressOf());
		texture2d_desc.Width = 1;
		texture2d_desc.Height = 1;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
	}

	tex_data.size.x = static_cast<float>(texture2d_desc.Width);
	tex_data.size.y = static_cast<float>(texture2d_desc.Height);
	tex_data.per_chip = tex_data.size;

	DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f) };
	DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
	DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
	DirectX::XMMATRIX v{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
	DirectX::XMStoreFloat4x4(&conversion_data.view, v);
	DirectX::XMStoreFloat4x4(&conversion_data.projection,
		DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30.0f), 1.77f, 0.1f, 1000.0f));
}

void Particle::update(FLOAT dt)
{
	for (size_t i = 0; i < max_vertex; i++)
	{
		if (vertex_data[i].chip_num < 0)continue;
		vertex_data[i].position.x += vertex_data[i].velocity.x * dt;
		vertex_data[i].position.y += vertex_data[i].velocity.y * dt;
		vertex_data[i].position.z += vertex_data[i].velocity.z * dt;

		vertex_data[i].velocity.x += vertex_data[i].acceleration.x * dt;
		vertex_data[i].velocity.y += vertex_data[i].acceleration.y * dt;
		vertex_data[i].velocity.z += vertex_data[i].acceleration.z * dt;

		vertex_data[i].alpha -= vertex_data[i].attenuation_alpha * dt;

		vertex_data[i].timer -= dt;

		if (vertex_data[i].timer <= 0)
		{
			vertex_data[i].chip_num = -1;
		}
	}
}

void Particle::animation(FLOAT dt,FLOAT speed, INT stop_chip_count)
{
	for (int i = 0; i < max_vertex; i++)
	{
		if (vertex_data[i].chip_num < 0) continue;
		if (vertex_data[i].animation_timer > speed)
		{
			vertex_data[i].animation_timer = 0.0f;
			++vertex_data[i].chip_num;
			vertex_data[i].chip_num %= stop_chip_count;
		}
		vertex_data[i].animation_timer += dt;
	}
}

bool Particle::setVertex(innovation_vertex data)
{
	if (data.chip_num < 0 || data.timer <= 0.0f)return false;
	for (size_t i = 0; i < max_vertex; i++)
	{
		if (vertex_data[i].chip_num >= 0)continue;
		vertex_data[i] = data;
		return true;
	}
	return false;
}


size_t Particle::setVertexs(size_t num, std::function<void(innovation_vertex&)> setter)
{
	for (size_t i = 0, e = num; i < e; i++)
	{
		if (vertex_data[i].chip_num >= 0)continue;
		innovation_vertex data;
		setter(data);
		vertex_data[i] = data;
		if ((--num) == 0)break;
	}
	return num;
}

void Particle::render(ID3D11DeviceContext* immediate_context)
{
	assert(immediate_context && "The device is invalid.");

	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	HRESULT hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	int n = 0;
	for (int i = 0; i < max_vertex; i++)
	{
		if (vertex_data[i].chip_num < 0) continue;
		vertices[i].position.x = vertex_data[i].position.x;
		vertices[i].position.y = vertex_data[i].position.y;
		vertices[i].position.z = vertex_data[i].position.z;
		vertices[i].position.w = 1.0f;

		vertices[i].color.x = vertex_data[i].size.x;
		vertices[i].color.y = vertex_data[i].size.y;
		vertices[i].color.z = static_cast<float>(vertex_data[i].chip_num);
		vertices[i].color.w = vertex_data[i].alpha;
		++n;
	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);

	if (n <= 0)return;

	immediate_context->UpdateSubresource(textrue_constant.Get(), 0u, nullptr, &tex_data, 0u, 0u);
	immediate_context->GSSetConstantBuffers(0, 1, textrue_constant.GetAddressOf());
	immediate_context->UpdateSubresource(conversion_constant.Get(), 0u, nullptr, &conversion_data, 0u, 0u);
	immediate_context->GSSetConstantBuffers(1, 1, conversion_constant.GetAddressOf());

	D3D11_PRIMITIVE_TOPOLOGY default_topology;
	immediate_context->IAGetPrimitiveTopology(&default_topology);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	ComPtr<ID3D11SamplerState>				default_sampler_state;
	ComPtr<ID3D11BlendState>				default_blend_state;
	ComPtr<ID3D11DepthStencilState>			default_depth_stencil_state;
	ComPtr<ID3D11RasterizerState>			default_rasterizer_state;
	FLOAT									blend_facter[4]{};
	UINT									blend_mask;

	immediate_context->OMGetBlendState(default_blend_state.ReleaseAndGetAddressOf(), blend_facter, &blend_mask);
	immediate_context->RSGetState(default_rasterizer_state.ReleaseAndGetAddressOf());
	immediate_context->OMGetDepthStencilState(default_depth_stencil_state.ReleaseAndGetAddressOf(), 0);
	immediate_context->PSGetSamplers(0, 1, default_sampler_state.ReleaseAndGetAddressOf());

	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	immediate_context->RSSetState(rasterizer_state.Get());
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);
	immediate_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

	UINT stride = sizeof(vertex);
	UINT offset = 0;

	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	immediate_context->HSSetShader(nullptr, nullptr, 0);
	immediate_context->DSSetShader(nullptr, nullptr, 0);

	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->GSSetShader(geometry_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	immediate_context->Draw(n, 0);

	immediate_context->IASetPrimitiveTopology(default_topology);
	immediate_context->OMSetBlendState(default_blend_state.Get(), blend_facter, blend_mask);
	immediate_context->RSSetState(default_rasterizer_state.Get());
	immediate_context->OMSetDepthStencilState(default_depth_stencil_state.Get(), 1);
	immediate_context->PSSetSamplers(0, 1, default_sampler_state.GetAddressOf());

	immediate_context->IASetInputLayout(nullptr);
	immediate_context->VSSetShader(nullptr, nullptr, 0);
	immediate_context->GSSetShader(nullptr, nullptr, 0);
	immediate_context->PSSetShader(nullptr, nullptr, 0);

}

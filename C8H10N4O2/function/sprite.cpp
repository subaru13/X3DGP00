#include "Sprite.h"
#include "../FrameworkConfig.h"
#include "CreateComObjectHelpar.h"
#include "Misc.h"
#include "MyHandy.h"
#include <assert.h>

Sprite::Sprite(ID3D11Device* device, const wchar_t* file_name)
	:vertex_buffer(nullptr), pixel_shader(nullptr),
	input_layout(nullptr), vertex_shader(nullptr),
	shader_resource_view(nullptr), textrue_size(0, 0),
	constant_buffer(device)
{
	assert(device && "The device is invalid.");
	vertex vertices[]
	{
	 { { -1.0f, +1.0f, 0.0f },{0,0} },
	 { { +1.0f, +1.0f, 0.0f },{1,0} },
	 { { -1.0f, -1.0f, 0.0f },{0,1} },
	 { { +1.0f, -1.0f, 0.0f },{1,1} },
	};

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "sprite_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
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
			"    float4 color;\n"
			"};\n"
			"Texture2D diffuse_map : register(t0);\n"
			"SamplerState diffuse_map_sampler_state : register(s0);\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * color;\n"
			"}\n";

		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0,	 DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0,	 DXGI_FORMAT_R32G32_FLOAT,			0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};

	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "sprite_vs.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadVertexShader(device, cso_pass,
			vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	else
	{
		std::string vs =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"VS_OUT main(float4 position : POSITION,float2 texcoord : TEXCOORD)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.position = position;\n"
			"	vout.texcoord = texcoord;\n"
			"	return vout;\n"
			"}\n";
		hr = createVertexShader(device, vs,
			vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	if (file_name == nullptr)
	{
		hr = makeDummyTexture(device, shader_resource_view.ReleaseAndGetAddressOf());
		textrue_size = { 1.0f,1.0f };
	}
	else
	{
		bool isDummy = !isExistFileW(file_name);
		if (isDummy)
		{
			hr = makeDummyTexture(device, shader_resource_view.ReleaseAndGetAddressOf());
			textrue_size = { 1.0f,1.0f };
		}
		else
		{
			D3D11_TEXTURE2D_DESC texture2d_desc{};
			hr = loadTextureFromFile(device, file_name, shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);
			textrue_size = { static_cast<float>(texture2d_desc.Width),static_cast<float>(texture2d_desc.Height) };
		}
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

Sprite::Sprite(ID3D11Device* device, ID3D11ShaderResourceView* new_shader_resource_view)
	:vertex_buffer(nullptr), pixel_shader(nullptr),
	input_layout(nullptr), vertex_shader(nullptr),
	shader_resource_view(new_shader_resource_view), textrue_size(0, 0),
	constant_buffer(device)
{
	assert(device && "The device is invalid.");
	vertex vertices[]
	{
	 { { -1.0f, +1.0f, 0.0f },{0,0} },
	 { { +1.0f, +1.0f, 0.0f },{1,0} },
	 { { -1.0f, -1.0f, 0.0f },{0,1} },
	 { { +1.0f, -1.0f, 0.0f },{1,1} },
	};

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(vertices);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "sprite_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
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
			"    float4 color;\n"
			"};\n"
			"Texture2D diffuse_map : register(t0);\n"
			"SamplerState diffuse_map_sampler_state : register(s0);\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * color;\n"
			"}\n";

		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0,	 DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "TEXCOORD",	0,	 DXGI_FORMAT_R32G32_FLOAT,			0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};

	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "sprite_vs.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadVertexShader(device, cso_pass,
			vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	else
	{
		std::string vs =
			"struct VS_OUT\n"
			"{\n"
			"	float4 position : SV_POSITION;\n"
			"	float2 texcoord : TEXCOORD;\n"
			"};\n"
			"VS_OUT main(float4 position : POSITION,float2 texcoord : TEXCOORD)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.position = position;\n"
			"	vout.texcoord = texcoord;\n"
			"	return vout;\n"
			"}\n";
		hr = createVertexShader(device, vs,
			vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	if (shader_resource_view == nullptr)
	{
		hr = makeDummyTexture(device, shader_resource_view.ReleaseAndGetAddressOf());
		textrue_size = { 1.0f,1.0f };
	}
	else
	{
		HRESULT hr = S_OK;
		ComPtr<ID3D11Resource> resource;
		shader_resource_view->GetResource(&resource);
		ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d->GetDesc(&texture2d_desc);
		textrue_size = { static_cast<float>(texture2d_desc.Width),static_cast<float>(texture2d_desc.Height) };
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

bool Sprite::attach(ID3D11ShaderResourceView* new_shader_resource_view)
{
	if (new_shader_resource_view)
	{
		shader_resource_view.Attach(new_shader_resource_view);
		HRESULT hr = S_OK;
		ComPtr<ID3D11Resource> resource;
		shader_resource_view->GetResource(&resource);
		ComPtr<ID3D11Texture2D> texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d->GetDesc(&texture2d_desc);
		textrue_size = { static_cast<float>(texture2d_desc.Width),static_cast<float>(texture2d_desc.Height) };
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		return true;
	}
	return false;
}

void Sprite::render(ID3D11DeviceContext* immediate_context,
	ID3D11PixelShader** external_pixel_shader,
	FLOAT2 pos,
	FLOAT2 size,
	FLOAT2 texpos,
	FLOAT2 texsize,
	float angle,
	FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");

	if ((size.x * size.y) == 0.0f)
	{
		return;
	}

	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	if ((texsize.x * texsize.y) == 0.0f)
	{
		texsize = textrue_size;
	}

	// left-top
	float x0{ pos.x };
	float y0{ pos.y };
	// right-top
	float x1{ pos.x + size.x };
	float y1{ pos.y };
	// left-bottom
	float x2{ pos.x };
	float y2{ pos.y + size.y };
	// right-bottom
	float x3{ pos.x + size.x };
	float y3{ pos.y + size.y };

	//回転の中心を矩形の中心点にした場合
	float cx = pos.x + size.x * 0.5f;
	float cy = pos.y + size.y * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	HRESULT hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };

		vertices[0].texcoord = { texpos.x,texpos.y };
		vertices[1].texcoord = { texpos.x + texsize.x,texpos.y };
		vertices[2].texcoord = { texpos.x,texpos.y + texsize.y };
		vertices[3].texcoord = { texpos.x + texsize.x,texpos.y + texsize.y };

		vertices[0].texcoord /= textrue_size;
		vertices[1].texcoord /= textrue_size;
		vertices[2].texcoord /= textrue_size;
		vertices[3].texcoord /= textrue_size;
	}
	immediate_context->Unmap(vertex_buffer.Get(), 0);
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	if (external_pixel_shader)
	{
		immediate_context->PSSetShader((*external_pixel_shader), nullptr, 0);
	}
	else
	{
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	}
	constant_buffer.data = color;
	constant_buffer.send(immediate_context, 0, 0, 1);
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
	immediate_context->Draw(4, 0);
}

void Sprite::quad(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader, FLOAT2 texpos, FLOAT2 texsize, FLOAT4 color)
{
	assert(immediate_context && "The context is invalid.");
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);
	render(immediate_context, external_pixel_shader, { 0.0f,0.0f }, { viewport.Width,viewport.Height }, texpos, texsize, 0.0f, color);
}

void Sprite::sendTextrue(ID3D11DeviceContext* immediate_context,
	UINT slot,
	bool use_vs,
	bool use_ps,
	bool use_ds,
	bool use_hs,
	bool use_gs) const
{
	assert(immediate_context && "The context is invalid.");
	if (use_vs)immediate_context->VSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
	if (use_ps)immediate_context->PSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
	if (use_ds)immediate_context->DSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
	if (use_hs)immediate_context->HSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
	if (use_gs)immediate_context->GSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
}
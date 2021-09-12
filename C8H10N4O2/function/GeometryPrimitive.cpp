#include "GeometryPrimitive.h"
#include "CreateComObjectHelpar.h"
#include "MyHandy.h"
#include "Misc.h"
#include "../FrameworkConfig.h"

GeometryPrimitive::GeometryPrimitive(ID3D11Device* device, GP_CONFIG config) :
	constant_buffer(device), vertex_buffer(nullptr), index_buffer(nullptr),
	vertex_shader(nullptr), pixel_shader(nullptr), input_layout(nullptr), primitive_config(config)
{
	assert(device && "The device is invalid.");
	HRESULT hr = S_OK;
	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "geometry_primitive_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shader.ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"struct VS_OUT\n"
			"{\n"
			"	float4 sv_position : SV_POSITION;\n"
			"	float4 position : POSITION;\n"
			"	float4 world_position : WORLD_POSITION; \n"
			"	float4 normal    : NORMAL;\n"
			"};\n"
			"cbuffer Constant : register(b0) \n"
			"{\n"
			"	row_major float4x4 world;\n"
			"	float4 material_color;\n"
			"};\n"
			"cbuffer Scene : register(b7) \n"
			"{\n"
			"	row_major float4x4 view_projection;\n"
			"	float4 light_direction;\n"
			"	float4 camera_position;\n"
			"};\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	float3 L = normalize(-light_direction.xyz);\n"
			"	float3 N = normalize(pin.normal.xyz);\n"
			"	float D = dot(L,N);\n"
			"	return float4(material_color.rgb * max(max(0,light_direction.w),D),material_color.a);\n"
			"}\n";
		hr = createPixelShader(device, ps, pixel_shader.ReleaseAndGetAddressOf());
	}

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA,	0 },
	};

	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "geometry_primitive_vs.cso");
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
			"	float4 sv_position : SV_POSITION;\n"
			"	float4 position : POSITION;\n"
			"	float4 world_position : WORLD_POSITION; \n"
			"	float4 normal    : NORMAL;\n"
			"};\n"
			"cbuffer Constant : register(b0) \n"
			"{\n"
			"	row_major float4x4 world;\n"
			"	float4 material_color;\n"
			"};\n"
			"cbuffer Scene : register(b7) \n"
			"{\n"
			"	row_major float4x4 view_projection;\n"
			"	float4 light_direction;\n"
			"	float4 camera_position;\n"
			"};\n"
			"VS_OUT main(float4 position : POSITION,float4 normal : NORMAL)\n"
			"{\n"
			"	VS_OUT vout;\n"
			"	vout.sv_position = mul(position,mul(world,view_projection));\n"
			"	vout.position = vout.sv_position;\n"
			"	vout.world_position = mul(position, world);\n"
			"	normal.w = 0;\n"
			"	vout.normal = mul(normal,world);\n"
			"	return vout;\n"
			"}\n";

		hr = createVertexShader(device, vs,
			vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;

	switch (primitive_config.shape)
	{
	case GP_SHAPE::GP_CUBE:
		create_cube(vertices, indices);
		break;
	case GP_SHAPE::GP_SPHERE:
		assert(primitive_config.division > 0 && "The number of divisions is too small");
		create_sphere(primitive_config.division, vertices, indices);
		break;
	case GP_SHAPE::GP_CYLINDER:
		assert(primitive_config.division > 0 && "The number of divisions is too small");
		create_cylinder(primitive_config.division, vertices, indices);
		break;
	}
	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void GeometryPrimitive::render(ID3D11DeviceContext* immediate_context, ID3D11PixelShader** external_pixel_shader, const FLOAT4X4& world, const FLOAT4& material_color)
{
	assert(immediate_context && "The context is invalid.");
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

	constants data{ world, material_color };
	constant_buffer = data;
	constant_buffer.send(immediate_context, 0, true, true);

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);
	immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

void GeometryPrimitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC buffer_desc{};
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void GeometryPrimitive::create_cube(std::vector<vertex>& vertices, std::vector<uint32_t>& indices)
{
	vertices.resize(24);
	indices.resize(36);

	int face;

	// top-side
	// 0---------1
	// |         |
	// |   -Y    |
	// |         |
	// 2---------3
	face = 0;
	vertices[face * 4 + 0].position = FLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 1].position = FLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = FLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 3].position = FLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 1].normal = FLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 2].normal = FLOAT3(+0.0f, +1.0f, +0.0f);
	vertices[face * 4 + 3].normal = FLOAT3(+0.0f, +1.0f, +0.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// bottom-side
	// 0---------1
	// |         |
	// |   -Y    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = FLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 1].position = FLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 2].position = FLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = FLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 1].normal = FLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 2].normal = FLOAT3(+0.0f, -1.0f, +0.0f);
	vertices[face * 4 + 3].normal = FLOAT3(+0.0f, -1.0f, +0.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// front-side
	// 0---------1
	// |         |
	// |   +Z    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = FLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = FLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 2].position = FLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = FLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(+0.0f, +0.0f, -1.0f);
	vertices[face * 4 + 1].normal = FLOAT3(+0.0f, +0.0f, -1.0f);
	vertices[face * 4 + 2].normal = FLOAT3(+0.0f, +0.0f, -1.0f);
	vertices[face * 4 + 3].normal = FLOAT3(+0.0f, +0.0f, -1.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// back-side
	// 0---------1
	// |         |
	// |   +Z    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = FLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 1].position = FLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = FLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 3].position = FLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(+0.0f, +0.0f, +1.0f);
	vertices[face * 4 + 1].normal = FLOAT3(+0.0f, +0.0f, +1.0f);
	vertices[face * 4 + 2].normal = FLOAT3(+0.0f, +0.0f, +1.0f);
	vertices[face * 4 + 3].normal = FLOAT3(+0.0f, +0.0f, +1.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;

	// right-side
	// 0---------1
	// |         |
	// |   -X    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = FLOAT3(+0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = FLOAT3(+0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = FLOAT3(+0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = FLOAT3(+0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(+1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 1].normal = FLOAT3(+1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 2].normal = FLOAT3(+1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 3].normal = FLOAT3(+1.0f, +0.0f, +0.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 1;
	indices[face * 6 + 2] = face * 4 + 2;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 3;
	indices[face * 6 + 5] = face * 4 + 2;

	// left-side
	// 0---------1
	// |         |
	// |   -X    |
	// |         |
	// 2---------3
	face += 1;
	vertices[face * 4 + 0].position = FLOAT3(-0.5f, +0.5f, -0.5f);
	vertices[face * 4 + 1].position = FLOAT3(-0.5f, +0.5f, +0.5f);
	vertices[face * 4 + 2].position = FLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[face * 4 + 3].position = FLOAT3(-0.5f, -0.5f, +0.5f);
	vertices[face * 4 + 0].normal = FLOAT3(-1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 1].normal = FLOAT3(-1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 2].normal = FLOAT3(-1.0f, +0.0f, +0.0f);
	vertices[face * 4 + 3].normal = FLOAT3(-1.0f, +0.0f, +0.0f);
	indices[face * 6 + 0] = face * 4 + 0;
	indices[face * 6 + 1] = face * 4 + 2;
	indices[face * 6 + 2] = face * 4 + 1;
	indices[face * 6 + 3] = face * 4 + 1;
	indices[face * 6 + 4] = face * 4 + 2;
	indices[face * 6 + 5] = face * 4 + 3;
}

void GeometryPrimitive::create_sphere(u_int division, std::vector<vertex>& vertices, std::vector<uint32_t>& indices)
{
	float r = 0.5f;
	vertex top_vertex;
	top_vertex.position = FLOAT3(0.0f, +r, 0.0f);
	top_vertex.normal = FLOAT3(0.0f, +1.0f, 0.0f);
	vertex bottom_vertex;
	bottom_vertex.position = FLOAT3(0.0f, -r, 0.0f);
	bottom_vertex.normal = FLOAT3(0.0f, -1.0f, 0.0f);
	vertices.push_back(top_vertex);
	float phi_step = DirectX::XM_PI / division;
	float theta_step = 2.0f * DirectX::XM_PI / division;
	for (u_int i = 1; i <= division - 1; ++i)
	{
		float phi = i * phi_step;
		for (u_int j = 0; j <= division; ++j)
		{
			float theta = j * theta_step;
			vertex v;
			v.position.x = r * sinf(phi) * cosf(theta);
			v.position.y = r * cosf(phi);
			v.position.z = r * sinf(phi) * sinf(theta);
			DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&v.position);
			DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));
			vertices.push_back(v);
		}
	}
	vertices.push_back(bottom_vertex);
	for (UINT i = 1; i <= division; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}
	u_int base_index = 1;
	u_int ring_vertex_count = division + 1;
	for (u_int i = 0; i < division - 2; ++i)
	{
		for (u_int j = 0; j < division; ++j)
		{
			indices.push_back(base_index + i * ring_vertex_count + j);
			indices.push_back(base_index + i * ring_vertex_count + j + 1);
			indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

			indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
			indices.push_back(base_index + i * ring_vertex_count + j + 1);
			indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
		}
	}
	u_int south_pole_index = (u_int)vertices.size() - 1;
	base_index = south_pole_index - ring_vertex_count;
	for (u_int i = 0; i < division; ++i)
	{
		indices.push_back(south_pole_index);
		indices.push_back(base_index + i);
		indices.push_back(base_index + i + 1);
	}
}

void GeometryPrimitive::create_cylinder(u_int division, std::vector<vertex>& vertices, std::vector<uint32_t>& indices)
{
	float d = 2.0f * DirectX::XM_PI / division;
	float r = 0.5f;
	vertex vertex;
	u_int base_index = 0;
	vertex.position = FLOAT3(0.0f, +0.5f, 0.0f);
	vertex.normal = FLOAT3(0.0f, +1.0f, 0.0f);
	vertices.push_back(vertex);
	for (u_int i = 0; i < division; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = FLOAT3(x, +0.5f, z);
		vertex.normal = FLOAT3(0.0f, +1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = 0;
	for (u_int i = 0; i < division - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 2);
		indices.push_back(base_index + i + 1);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + 1);
	indices.push_back(base_index + division);
	vertex.position = FLOAT3(0.0f, -0.5f, 0.0f);
	vertex.normal = FLOAT3(0.0f, -1.0f, 0.0f);
	vertices.push_back(vertex);
	for (u_int i = 0; i < division; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);
		vertex.position = FLOAT3(x, -0.5f, z);
		vertex.normal = FLOAT3(0.0f, -1.0f, 0.0f);
		vertices.push_back(vertex);
	}
	base_index = division + 1;
	for (u_int i = 0; i < division - 1; ++i)
	{
		indices.push_back(base_index + 0);
		indices.push_back(base_index + i + 1);
		indices.push_back(base_index + i + 2);
	}
	indices.push_back(base_index + 0);
	indices.push_back(base_index + (division - 1) + 1);
	indices.push_back(base_index + (0) + 1);
	for (u_int i = 0; i < division; ++i)
	{
		float x = r * cosf(i * d);
		float z = r * sinf(i * d);

		vertex.position = FLOAT3(x, +0.5f, z);
		vertex.normal = FLOAT3(x, 0.0f, z);
		vertices.push_back(vertex);

		vertex.position = FLOAT3(x, -0.5f, z);
		vertex.normal = FLOAT3(x, 0.0f, z);
		vertices.push_back(vertex);
	}
	base_index = division * 2 + 2;
	for (u_int i = 0; i < division - 1; ++i)
	{
		indices.push_back(base_index + i * 2 + 0);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 1);

		indices.push_back(base_index + i * 2 + 1);
		indices.push_back(base_index + i * 2 + 2);
		indices.push_back(base_index + i * 2 + 3);
	}
	indices.push_back(base_index + (division - 1) * 2 + 0);
	indices.push_back(base_index);
	indices.push_back(base_index + (division - 1) * 2 + 1);
	indices.push_back(base_index + (division - 1) * 2 + 1);
	indices.push_back(base_index);
	indices.push_back(base_index + 1);
}
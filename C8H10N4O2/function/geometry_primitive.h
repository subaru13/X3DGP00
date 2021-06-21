#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <memory>
#include "Arithmetic.h"
#include "ConstantBuffer.h"

using namespace Microsoft::WRL;

enum GP_SHAPE
{
	GP_CUBE,GP_SPHERE,GP_CYLINDER
};

struct GP_CONFIG
{
	const GP_SHAPE	shape;
	const UINT		division;
	GP_CONFIG(GP_SHAPE shape, UINT division = 32) :shape(shape), division(division) {}
};

class GeometryPrimitive
{
public:

	 struct vertex
	{
		FLOAT3 position;
		FLOAT3 normal;
	};
	struct constants
	{
		FLOAT4X4	world;
		FLOAT4		material_color;
	};
private:
	ComPtr<ID3D11Buffer>		vertex_buffer;
	ComPtr<ID3D11Buffer>		index_buffer;
	ComPtr<ID3D11VertexShader>	vertex_shader;
	ComPtr<ID3D11PixelShader>	pixel_shader;
	ComPtr<ID3D11InputLayout>	input_layout;
	ConstantBuffer<constants>	constant_buffer;
	const GP_CONFIG				primitive_config;
public:
	GeometryPrimitive(ID3D11Device* device, GP_CONFIG config);
	virtual ~GeometryPrimitive() = default;

	/// <summary>
	/// ジオメトリを描画します。
	/// </summary>
	/// <param name="immediate_context">有効なコンテキスト</param>
	/// <param name="world">ワールド変換行列</param>
	/// <param name="material_color">色</param>
	virtual void render(ID3D11DeviceContext* immediate_context,
		const FLOAT4X4& world, 
		const FLOAT4& material_color = {1,1,1,1});

	/// <summary>
	/// ジオメトリの構成を取得します。
	/// </summary>
	const GP_CONFIG& getConfig()const { return primitive_config; }
protected:
	void create_com_buffers(ID3D11Device * device, vertex * vertices, size_t vertex_count,
		uint32_t * indices, size_t index_count);

	void create_cube(std::vector<vertex>& vertices, std::vector<uint32_t>& indices);
	void create_sphere(u_int division,std::vector<vertex>& vertices, std::vector<uint32_t>& indices);
	void create_cylinder(u_int division, std::vector<vertex>& vertices, std::vector<uint32_t>& indices);
};

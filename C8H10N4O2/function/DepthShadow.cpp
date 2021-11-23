#include "DepthShadow.h"
#include "CreateComObjectHelpar.h"
#include "CameraControl.h"
#include "Misc.h"
#include "MyHandy.h"
#include "../FrameworkConfig.h"


#include <algorithm>

DepthShadow::DepthShadow(ID3D11Device* device, UINT width, UINT height, FLOAT zfar)
	:depth_map(device, FB_CONFIG{ width ,height,DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R24G8_TYPELESS,FALSE }),
	scene_constant(device), depth_shadow_constant_buffer(device), sampler_state(nullptr),pixel_shaders{ nullptr,nullptr }
{
	assert(device && "The device is invalid.");
	CameraControl camera_control;
	(*camera_control.getWidth()) = (float)width;
	(*camera_control.getHeight()) = (float)height;
	(*camera_control.getFar()) = zfar;
	scene_constant.setProjection(camera_control.getOrthographic());
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, sampler_state.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	std::string cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "geometry_primitive_fetch_depth_shadow_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shaders[FPSI_GEOMETRY_PRIMITIVE].ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"cbuffer DepthShadowConstantBuffer : register(b8) \n"
			"{ \n"
			"	row_major float4x4 view_orthographic; \n"
			"	float3 color; \n"
			"	float bias; \n"
			"}; \n"
			"Texture2D		depth_map_texture2d : register(t10); \n"
			"SamplerState	depth_map_sampler_state : register(s10); \n"
			"float3 GetShadowTex(float3 pos) \n"
			"{ \n"
			"	float4 wvp_pos; \n"
			"	wvp_pos.xyz = pos; \n"
			"	wvp_pos.w = 1; \n"
			"	wvp_pos = mul(view_orthographic, wvp_pos); \n"
			"	wvp_pos /= wvp_pos.w; \n"
			"	wvp_pos.y = -wvp_pos.y; \n"
			"	wvp_pos.xy = 0.5f * wvp_pos.xy + 0.5f; \n"
			"	return wvp_pos.xyz; \n"
			"} \n"
			"float3 GetShadow(float3 pos) \n"
			"{ \n"
			"	float3 tex = GetShadowTex(pos); \n"
			"	float d = depth_map_texture2d.Sample(depth_map_sampler_state, tex.xy).r; \n"
			"	float3 scolor = (tex.z - d > bias) ? color : float3(1, 1, 1); \n"
			"	return scolor; \n"
			"} \n"
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
			"	return float4(material_color.rgb * max(max(0,light_direction.w),D)* GetShadow(pin.world_position.xyz),material_color.a);\n"
			"}\n";
		hr = createPixelShader(device, ps, pixel_shaders[FPSI_GEOMETRY_PRIMITIVE].ReleaseAndGetAddressOf());
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	cso_pass = combinePathsA(CSO_FILE_DIRECTORY, "skinned_mesh_fetch_depth_shadow_ps.cso");
	if (isExistFileA(cso_pass))
	{
		hr = loadPixelShader(device, cso_pass, pixel_shaders[FPSI_SKINNED_MESH].ReleaseAndGetAddressOf());
	}
	else
	{
		std::string ps =
			"cbuffer DepthShadowConstantBuffer : register(b8) \n"
			"{ \n"
			"	row_major float4x4 view_orthographic; \n"
			"	float3 color; \n"
			"	float bias; \n"
			"}; \n"
			"Texture2D		depth_map_texture2d : register(t10); \n"
			"SamplerState	depth_map_sampler_state : register(s10); \n"
			"float3 GetShadowTex(float3 pos) \n"
			"{ \n"
			"	float4 wvp_pos; \n"
			"	wvp_pos.xyz = pos; \n"
			"	wvp_pos.w = 1; \n"
			"	wvp_pos = mul(view_orthographic, wvp_pos); \n"
			"	wvp_pos /= wvp_pos.w; \n"
			"	wvp_pos.y = -wvp_pos.y; \n"
			"	wvp_pos.xy = 0.5f * wvp_pos.xy + 0.5f; \n"
			"	return wvp_pos.xyz; \n"
			"} \n"
			"float3 GetShadow(float3 pos) \n"
			"{ \n"
			"	float3 tex = GetShadowTex(pos); \n"
			"	float d = depth_map_texture2d.Sample(depth_map_sampler_state, tex.xy).r; \n"
			"	float3 scolor = (tex.z - d > bias) ? color : float3(1, 1, 1); \n"
			"	return scolor; \n"
			"} \n"
			"struct VS_IN \n"
			"{ \n"
			"	float4 position : POSITION; \n"
			"	float4 normal : NORMAL; \n"
			"	float4 tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 bone_weights : WEIGHTS; \n"
			"	uint4 bone_indices : BONES; \n"
			"}; \n"
			"struct VS_OUT \n"
			"{ \n"
			"	float4 sv_position : SV_POSITION; \n"
			"	float4 position : POSITION; \n"
			"	float4 world_position : WORLD_POSITION; \n"
			"	float4 world_normal : NORMAL; \n"
			"	float4 world_tangent : TANGENT; \n"
			"	float2 texcoord : TEXCOORD; \n"
			"	float4 color : COLOR; \n"
			"}; \n"
			"static const int MAX_BONES = 256; \n"
			"cbuffer object_constant : register(b0) \n"
			"{ \n"
			"	row_major float4x4 world; \n"
			"	float4 material_color; \n"
			"	row_major float4x4 bone_transforms[MAX_BONES]; \n"
			"}; \n"
			"cbuffer Scene : register(b7) \n"
			"{\n"
			"	row_major float4x4 view_projection;\n"
			"	float4 light_direction;\n"
			"	float4 camera_position;\n"
			"};\n"
			"SamplerState samplerstate : register(s0);\n"
			"Texture2D texture_maps[4] : register(t0);\n"
			"float4 main(VS_OUT pin) : SV_TARGET\n"
			"{\n"
			"	float4 color = texture_maps[0].Sample(samplerstate, pin.texcoord);\n"
			"	float3 N = normalize(pin.world_normal.xyz);\n"
			"	float3 L = normalize(-light_direction.xyz);\n"
			"	float3 diffuse = color.rgb * max(max(0, light_direction.w), dot(N, L));\n"
			"	return float4(diffuse * GetShadow(pin.world_position.xyz) , color.a) * pin.color;\n"
			"}\n";
		hr = createPixelShader(device, ps, pixel_shaders[FPSI_SKINNED_MESH].ReleaseAndGetAddressOf());
	}
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void DepthShadow::blit(ID3D11DeviceContext* immediate_context,
	std::function<void(ID3D11PixelShader**[2])> drawing_process,
	const FLOAT3& light_direction, float distance)
{
	assert(immediate_context && "The context is invalid.");

	float ld_length = vec3Length(light_direction);
	if (ld_length <= 0.0f || distance <= 0.0f)
	{
		ID3D11PixelShader** apply_pixel_shaders[2]{ nullptr,nullptr };
		drawing_process(apply_pixel_shaders);
		return;
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> cached_constant_buffers[6]{ nullptr };
	immediate_context->VSGetConstantBuffers(7, 1,cached_constant_buffers[0].GetAddressOf());
	immediate_context->PSGetConstantBuffers(7, 1,cached_constant_buffers[1].GetAddressOf());
	immediate_context->GSGetConstantBuffers(7, 1,cached_constant_buffers[2].GetAddressOf());
	immediate_context->DSGetConstantBuffers(7, 1,cached_constant_buffers[3].GetAddressOf());
	immediate_context->HSGetConstantBuffers(7, 1,cached_constant_buffers[4].GetAddressOf());
	CameraControl camera_control;
	VECTOR3 nlight_direction = light_direction / ld_length;
	(*camera_control.getPos()) = -nlight_direction * distance;
	(*camera_control.getTarget()) = FLOAT3{ 0.0f,0.0f,0.0f };
	const FLOAT3 world_axis_y{ 0.0f,1.0f,0.0f };

	float dot = std::clamp<float>(vec3Dot(world_axis_y, nlight_direction), -1.0f, 1.0f);

	if (fabsf(dot) >= 1.0f)
	{
		(*camera_control.getUp()) = (dot > 0.0f) ? VECTOR3(0.0f, 0.0f, -1.0f) : VECTOR3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		(*camera_control.getUp()) = world_axis_y;
	}

	scene_constant.setView(camera_control.getView());
	scene_constant.send(immediate_context);
	XMStoreFloat4x4(&depth_shadow_constant_buffer.data.view_orthographic,
		XMLoadFloat4x4(&scene_constant.getView()) * XMLoadFloat4x4(&scene_constant.getProjection()));
	ID3D11PixelShader* null_pixel_shader = nullptr;
	ID3D11PixelShader** apply_pixel_shaders[2]{ &null_pixel_shader,&null_pixel_shader };
	depth_map.clear(immediate_context, { 1.0f,1.0f,1.0f,1.0f });
	depth_map.active(immediate_context);
	drawing_process( apply_pixel_shaders);
	depth_map.deactive(immediate_context);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cached_shader_resource_view{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11SamplerState> cached_sampler_state{ nullptr };
	immediate_context->VSSetConstantBuffers(7, 1, cached_constant_buffers[0].GetAddressOf());
	immediate_context->PSSetConstantBuffers(7, 1, cached_constant_buffers[1].GetAddressOf());
	immediate_context->GSSetConstantBuffers(7, 1, cached_constant_buffers[2].GetAddressOf());
	immediate_context->DSSetConstantBuffers(7, 1, cached_constant_buffers[3].GetAddressOf());
	immediate_context->HSSetConstantBuffers(7, 1, cached_constant_buffers[4].GetAddressOf());
	immediate_context->PSGetConstantBuffers(8, 1, cached_constant_buffers[5].GetAddressOf());
	immediate_context->PSGetSamplers(10, 1, cached_sampler_state.GetAddressOf());
	immediate_context->PSGetShaderResources(10, 1, cached_shader_resource_view.GetAddressOf());
	depth_shadow_constant_buffer.send(immediate_context, 8, false, true, false, false, false);
	apply_pixel_shaders[FPSI_GEOMETRY_PRIMITIVE] = pixel_shaders[FPSI_GEOMETRY_PRIMITIVE].GetAddressOf();
	apply_pixel_shaders[FPSI_SKINNED_MESH] = pixel_shaders[FPSI_SKINNED_MESH].GetAddressOf();
	ID3D11ShaderResourceView* apply_shader_resource_view = depth_map.getDepthStencilShaderResourceView();
	immediate_context->PSSetSamplers(10, 1, sampler_state.GetAddressOf());
	immediate_context->PSSetShaderResources(10, 1, &apply_shader_resource_view);
	drawing_process( apply_pixel_shaders);
	immediate_context->PSSetSamplers(10, 1, cached_sampler_state.GetAddressOf());
	immediate_context->PSSetShaderResources(10, 1, cached_shader_resource_view.GetAddressOf());
	immediate_context->PSSetConstantBuffers(8, 1, cached_constant_buffers[5].GetAddressOf());
}



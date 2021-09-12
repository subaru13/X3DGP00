#include "scene_constant.hlsli"
struct VS_OUT
{
	float4 sv_position : SV_POSITION;
	float4 position : POSITION;
	float4 world_position : WORLD_POSITION;
	float4 normal : NORMAL;
};
cbuffer object_constant : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
};

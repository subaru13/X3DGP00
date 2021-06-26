#include "scene_constant.hlsli"
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 world_position : POSITION;
	float4 normal    : NORMAL;
};
cbuffer object_constant : register(b0)
{
	row_major float4x4 world;
	float4 matrial_color;
};

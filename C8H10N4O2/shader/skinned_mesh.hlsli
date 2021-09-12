#include "scene_constant.hlsli"
struct VS_IN
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texcoord : TEXCOORD;
	float4 bone_weights : WEIGHTS;
	uint4 bone_indices : BONES;
};
struct VS_OUT
{
	float4 sv_position		: SV_POSITION;
	float4 position			: POSITION;
	float4 world_position	: WORLD_POSITION;
	float4 world_normal		: NORMAL;
	float4 world_tangent	: TANGENT;
	float2 texcoord			: TEXCOORD;
	float4 color			: COLOR;
};
static const int MAX_BONES = 256;
cbuffer object_constant : register(b0)
{
	row_major float4x4 world;
	float4 material_color;
	row_major float4x4 bone_transforms[MAX_BONES];
};
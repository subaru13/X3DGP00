#include "geometry_primitive.hlsli"

VS_OUT main(float4 position : POSITION,float4 normal : NORMAL)
{
	VS_OUT vout;
	vout.sv_position = mul(position, mul(world, view_projection));
	vout.position = vout.sv_position;
	vout.world_position = mul(position, world);
	normal.w = 0;
	vout.normal = mul(normal,world);
	return vout;
}
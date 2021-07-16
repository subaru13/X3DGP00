#include "geometry_primitive.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
	float3 L = normalize(-light_direction.xyz);
	float3 N = normalize(pin.normal.xyz);
	float D = dot(L,N);
	return float4(material_color.rgb * max(max(0, light_direction.w), D), material_color.a);
}
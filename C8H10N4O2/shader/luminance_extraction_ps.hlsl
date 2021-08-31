struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer Parameters : register(b1)
{
	float Threshold;
	float3 dummy;
};

float getBrightness(float3 color)
{
	float3 y = { 0.299, 0.587, 0.114 };
	float bright = dot(y, color);
	return bright;
}

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT input) : SV_TARGET0
{
	float4 result = (float4) 0;
	result = diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord);
	float brightness = getBrightness(result.rgb);
	float contribution = max(0, brightness - Threshold);
	contribution /= brightness;
	result.rgb *= contribution;
	return result;
}
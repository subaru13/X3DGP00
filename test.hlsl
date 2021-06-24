struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer Color : register(b0)
{
	float4 color;
};

cbuffer DEBUG_CONSTANT_BUFFER : register(b7)
{
	float4	options;
}

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
#if 0
	float2 texcoord = pin.texcoord;
#else
	float2 division = options.x * options.y;
	float2 texcoord = ceil(pin.texcoord * division) / division;
#endif

	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, texcoord);
	float alpha = color.w;

	float exposure = options.z;
	color.rgb = 1 - exp(-color.rgb * exposure);

	const float GAMMA = 2.2;
	return float4(pow(color.rgb, options.w / GAMMA), alpha);
};
Texture2D texture_map : register(t0);
SamplerState texture_map_sampler_state : register(s0);
struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};
float4 main(VS_OUT pin) : SV_TARGET
{
	return texture_map.Sample(texture_map_sampler_state, pin.texcoord);
}
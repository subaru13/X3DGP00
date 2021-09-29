#include "particle.hlsli"

Texture2D diff_texture : register(t0);
SamplerState diff_sampler : register(s0);
float4 main(GS_OUT pin) : SV_TARGET
{
	return diff_texture.Sample(diff_sampler, pin.tex_code) * pin.color;
}
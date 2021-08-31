struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

#define BUFFER_SIZE 256
cbuffer Parameters : register(b1)
{
	float4 Weight[BUFFER_SIZE];
	int KernelSize;
	float2 texcel;
	float alpha;
};

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(VS_OUT input) : SV_TARGET
{
	float4 result = (float4) 0;
	result.a = 1.0f;
	for (int i = 0; i < KernelSize * KernelSize; i++)
	{
		float2 offset = texcel * Weight[i].xy;
		float weight = Weight[i].z;
		result.rgb += diffuse_map.Sample(diffuse_map_sampler_state, input.texcoord + offset).rgb * weight;
	}
	return result;
}
cbuffer DepthShadowConstantBuffer : register(b8)
{
	row_major float4x4 view_orthographic;
	float3 shadow_color;
	float bias;
};
Texture2D		depth_map_texture2d : register(t10);
SamplerState	depth_map_sampler_state : register(s10);
float3 GetShadowTex(float3 pos)
{
	float4 wvp_pos;
	wvp_pos.xyz = pos;
	wvp_pos.w = 1;
	wvp_pos = mul(view_orthographic,wvp_pos);
	wvp_pos /= wvp_pos.w;
	wvp_pos.y = -wvp_pos.y;
	wvp_pos.xy = 0.5f * wvp_pos.xy + 0.5f;
	return wvp_pos.xyz;
}
float3 GetShadow(float3 pos)
{
	float3 tex = GetShadowTex(pos);
	float d = depth_map_texture2d.Sample(depth_map_sampler_state, tex.xy).r;
	float3 scolor = (tex.z - d > bias) ? shadow_color : float3(1, 1, 1);
	return scolor;
}
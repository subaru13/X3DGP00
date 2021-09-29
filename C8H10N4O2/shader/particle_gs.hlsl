#include "particle.hlsli"

[maxvertexcount(4)]
void main(point VS_OUT gin[1], inout TriangleStream<GS_OUT> output)
{
	float4 pos = mul(view, float4(gin[0].position.xyz, 1.0));

	float w = gin[0].color.x;
	float h = gin[0].color.y;

	float4 pos_left_top = pos + float4(-w, h, 0.0, 0.0);
	float4 pos_left_bottom = pos + float4(-w, -h, 0.0, 0.0);
	float4 pos_right_top = pos + float4(w, h, 0.0, 0.0);
	float4 pos_right_bottom = pos + float4(w, -h, 0.0, 0.0);

	float side_count = tex_size.x / per_chip.x;

	float us = int(gin[0].color.z % side_count) * per_chip.x;
	float vs = int(gin[0].color.z / side_count) * per_chip.y;

	float uw = per_chip.x;
	float vh = per_chip.y;

	GS_OUT gout;
	gout.color = float4(1, 1, 1, gin[0].color.a);

	gout.position = mul(projection, pos_left_top);
	gout.tex_code = float2(us, vs) / tex_size;
	output.Append(gout);

	gout.position = mul(projection, pos_left_bottom);
	gout.tex_code = float2(us, vs + vh) / tex_size;
	output.Append(gout);

	gout.position = mul(projection, pos_right_top);
	gout.tex_code = float2(us + uw, vs) / tex_size;
	output.Append(gout);

	gout.position = mul(projection, pos_right_bottom);
	gout.tex_code = float2(us + uw, vs + vh) / tex_size;
	output.Append(gout);

	output.RestartStrip();
}
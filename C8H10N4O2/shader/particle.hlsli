cbuffer textrue_data : register(b0)
{
	float2	tex_size;
	float2	per_chip;
};

cbuffer conversion : register(b1)
{
	matrix	view;
	matrix	projection;
};

struct VS_OUT
{
	float4	position	:	SV_POSITION;
	float4	color		:	COLOR;
};

struct GS_OUT
{
	float4	position	:	SV_POSITION;
	float2	tex_code	:	TEX_CODE;
	float4	color		:	COLOR;
};
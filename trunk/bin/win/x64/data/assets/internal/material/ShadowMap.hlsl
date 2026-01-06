#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	matrix LIGHT_MATRIX_MVP;
END_CONSTANTS

struct appdata
{
	float4 vertex : POSITION;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
};

v2f vs_main(appdata v)
{
	v2f o;
	o.pos = mul(LIGHT_MATRIX_MVP, v.vertex);
	o.depth.xy = o.pos.zw;
	return o;
}

float4 ps_main(v2f i) : SV_Target
{
	float r = i.depth.x / i.depth.y;
	return r;
}
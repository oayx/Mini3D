#include "CGInclude.hlsli"

struct v2f
{
	float4 pos	: SV_POSITION;
	float4 color: TEXCOORD0;
};
v2f vs_main( float4 Pos : POSITION, float4 color : COLOR0)
{
	v2f o;
	o.pos = mul(MATRIX_MVP,Pos);
	o.color = color;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = i.color;
	return LinearToGammaSpace(final_col);
}
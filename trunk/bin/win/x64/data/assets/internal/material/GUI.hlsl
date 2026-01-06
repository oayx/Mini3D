#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4x4 ProjectionMatrix;
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct v2f
{
	float4 pos	: SV_POSITION;
	float4 color: TEXCOORD0;
	float2 uv	: TEXCOORD1;
};
v2f vs_main( float4 Pos : POSITION, float4 color : COLOR0, float2 uv : TEXCOORD0)
{
	v2f o;
	o.pos = mul(ProjectionMatrix, float4(Pos.x, Pos.y,0,1));
	o.color = color;
	o.uv = uv;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv) * i.color;
	return final_col;
}
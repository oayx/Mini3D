#include "CGInclude.hlsli"

void vs_main(float4 Pos : POSITION, out float4 oPos : SV_POSITION, out float oDepth : TEXCOORD0)
{
	oPos = mul(MATRIX_MVP,Pos);
	oDepth = mul(MATRIX_MV,Pos).z * _ProjectionParams.w;
}
float4 ps_main(float4 Pos : SV_POSITION, float Depth : TEXCOORD0):SV_Target
{
	float r = Depth;
	return r;
}
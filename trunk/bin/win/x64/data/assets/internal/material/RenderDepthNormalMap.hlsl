#include "CGInclude.hlsli"

void vs_main(float4 Pos : POSITION, float3 Normal : NORMAL, out float4 oPos : SV_POSITION, out float oDepth : TEXCOORD0, out float3 oNormal : TEXCOORD1)
{
	oPos = mul(MATRIX_MVP,Pos);
	oDepth = mul(MATRIX_MV,Pos).z * _ProjectionParams.w;
	oNormal = ObjectToViewNormal(Normal);
}
float4 ps_main(float4 Pos : SV_POSITION, float Depth : TEXCOORD0, float3 Normal : TEXCOORD1):SV_Target
{
	return float4(Normal,Depth);
}
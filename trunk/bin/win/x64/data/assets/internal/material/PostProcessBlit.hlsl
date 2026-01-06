#include "CGInclude.hlsli"

DECLARE_TEXTURE(_MainTex, 0);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};
v2f vs_main( float3 Pos : POSITION, float2 uv : TEXCOORD0)
{
	v2f o;
	o.pos = float4(Pos, 1);
	o.uv = uv;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	return SAMPLE_TEXTURE(_MainTex, i.uv);
}
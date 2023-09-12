#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_CUBEMAP(_ReflectionProbe, 1);

struct v2f
{
	float4 pos	: SV_POSITION;
	float2 uv	: TEXCOORD0;
	float3 uv1	: TEXCOORD1;
};

v2f vs_main(appdata_img v)
{
	v2f o;
	o.pos = mul(MATRIX_MVP,v.vertex);
	o.uv = TRANSFORM_TEX(v.uv,_MainTex); 
	o.uv1 = mul(MATRIX_M,v.vertex).xyz;
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	float4 reflect = SAMPLE_CUBEMAP(_ReflectionProbe, i.uv1);
	float4 final_col = lerp(albedo, reflect, 1);
	return final_col;
}
#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
END_CONSTANTS

// 纹理采样器
DECLARE_TEXTURE(_MainTex, 0);
DECLARE_CUBEMAP(_SkyBox, 1);

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 world_pos : TEXCOORD1;
	float3 world_normal : TEXCOORD2;
};

v2f vs_main(appdata_img v)
{
	v2f o;
	o.pos = mul(MATRIX_MVP,v.vertex);
	o.uv = TRANSFORM_TEX(v.uv,_MainTex); 
	o.world_pos = mul(MATRIX_M, v.vertex).xyz;
	o.world_normal = ObjectToWorldNormal(v.normal);
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	//反射
	float3 I = normalize(i.world_pos - _WorldSpaceCameraPos.xyz);
	float3 R = reflect(I, normalize(i.world_normal));
	
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	float4 reflect = SAMPLE_CUBEMAP(_SkyBox, R);
	float4 final_col = lerp(albedo, reflect, 0.5);
	return final_col;
}
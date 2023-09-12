#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
	float4 _TintColor;
	float _Speed = 1;
	float _Distort = 1;
	float _Intensity = 1;
	float _Alpha = 1;
	float4 _Time;
	float _FresnelIndensity = 5;
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);
DECLARE_CUBEMAP(_SkyBox, 1);

struct appdata
{
    float4 vertex : POSITION;
	float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct v2f
{
    float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 world_pos : TEXCOORD1;
	float3 world_normal : TEXCOORD2;
	float3 world_view : TEXCOORD3;
	FOG_COORDS(4)
};

v2f vs_main( appdata v )
{
    v2f o;
    o.pos = mul(MATRIX_MVP,v.vertex);
	o.uv = v.uv;
	o.world_pos = mul(MATRIX_M, v.vertex).xyz;
	o.world_normal = ObjectToWorldNormal(v.normal);
	o.world_view = WorldSpaceViewDir(v.vertex.xyz);

	TRANSFER_FOG(o, o.pos);
    return o;
}

float4 ps_main( v2f i ) : SV_Target
{
	float time = _Time.x * _Speed;

	float2 st_0 = (i.uv + time * float2(0, 0.05));
	float4 color_0 = SAMPLE_TEXTURE(_MainTex, TRANSFORM_TEX(st_0, _MainTex));

	float2 st_1 = (i.uv + time * float2(-0.01, 0.05));
	float4 color_1 = SAMPLE_TEXTURE(_MainTex, TRANSFORM_TEX(st_1, _MainTex));

	float2 st_2 = i.uv + time * float2(0.03, 0.03) + color_0.rg * color_1.rg * _Distort;
	float4 color_2 = SAMPLE_TEXTURE(_MainTex, TRANSFORM_TEX(st_2, _MainTex));

	float3 color_3 = _TintColor.rgb * color_2.rgb * _Intensity;
	float4 albedo = float4(color_3, (color_2.b * _Alpha));
	
	//反射
	float3 I = normalize(i.world_pos - _WorldSpaceCameraPos.xyz);
	float3 R = reflect(I, normalize(i.world_normal));
	float4 reflect_col = SAMPLE_CUBEMAP(_SkyBox, R);
	
	float fresnel = 1 - saturate(pow(max(0, dot(i.world_normal, i.world_view)), _FresnelIndensity));
	float4 final_col = float4(lerp(albedo.rgb, reflect_col.rgb, fresnel), 1);
	
	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}
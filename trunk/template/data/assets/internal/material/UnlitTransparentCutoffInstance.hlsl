#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	matrix LIGHT_MATRIX_MVP;
	float4 _MainTex_ST;
	float4 _Color = float4(1, 1, 1, 1);
	float _Cutoff;
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
	float4 vertex : POSITION;
	float4 color : COLOR0;
	float2 uv : TEXCOORD0;
	matrix offset : POSITION1;
};
struct v2f
{
	float4 pos : SV_POSITION;
	float4 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
	FOG_COORDS(2)
};
struct v2f_shadow
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
};

v2f vs_main(appdata v)
{
	v2f o;
	float4 pos = mul(v.offset, v.vertex);
	o.pos = mul(MATRIX_MVP, pos);
	o.color = v.color * _Color;
	o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	TRANSFER_FOG(o, o.pos);
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv) * i.color;
	clip(final_col.a - _Cutoff);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}

v2f_shadow vs_shadow(appdata v)
{
	v2f_shadow o;
	float4 pos = mul(v.offset, v.vertex);
	o.pos = mul(LIGHT_MATRIX_MVP, pos);
	o.depth.xy = o.pos.zw;
	return o;
}
float4 ps_shadow(v2f_shadow i) : SV_Target
{
	float r = i.depth.x / i.depth.y;
	return r;
}
#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _Color = float4(1, 1, 1, 1);
	float4 _MainTex_ST;
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
	float4 vertex : POSITION;
	float4 color : COLOR0;
	float2 uv : TEXCOORD0;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float4 color : TEXCOORD0;
	float2 uv : TEXCOORD1;
	FOG_COORDS(2)
};

v2f vs_main(appdata v)
{
	v2f o;
	o.pos = mul(MATRIX_MVP, v.vertex);
	o.color = v.color * _Color;
	o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	TRANSFER_FOG(o, o.pos);
	return o;
}

float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv) * i.color;

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}
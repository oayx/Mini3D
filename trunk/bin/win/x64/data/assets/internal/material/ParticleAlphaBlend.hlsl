#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_ST;
	float4 _Time;
	float4 _TintColor;
	float _USpeed;
	float _VSpeed;
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
    float4 vertex		: POSITION;
    float4 color		: COLOR;
    float2 uv			: TEXCOORD0;
};

struct v2f
{
    float4 pos			: SV_POSITION;
	float4 color		: COLOR;
	float2 uv			: TEXCOORD0;
	FOG_COORDS(1)
};

v2f vs_main( appdata v )
{
    v2f o;
    o.pos = mul(MATRIX_MVP,v.vertex);
	o.color = v.color;

	float2 tmp_uv = v.uv;
	tmp_uv.x += _Time.y*_USpeed;
	tmp_uv.y += _Time.y*_VSpeed;
    o.uv = TRANSFORM_TEX(tmp_uv,_MainTex); 

	TRANSFER_FOG(o, o.pos);
    return o;
}

float4 ps_main( v2f i ) : SV_Target
{
	float4 final_col = i.color * _TintColor * SAMPLE_TEXTURE(_MainTex, i.uv);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}
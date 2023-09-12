#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _Color = float4(1, 1, 1, 1);
END_CONSTANTS

struct appdata
{
	float4 vertex		: POSITION;
	float4 color		: COLOR0;
};

struct v2f
{
    float4 pos			: SV_POSITION;
	float4 color		: TEXCOORD0;
	FOG_COORDS(1)
};

v2f vs_main( appdata v )
{
    v2f o;
    o.pos = mul(MATRIX_MVP,v.vertex);
	o.color = v.color * _Color;

	TRANSFER_FOG(o, o.pos);
    return o;
}

float4 ps_main( v2f i ) : SV_Target
{
	float4 final_col = i.color;

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}

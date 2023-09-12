#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _Color = float4(1, 1, 1, 1);
END_CONSTANTS

struct appdata
{
	float4 vertex	: POSITION;
	float4 color	: COLOR0;
};

struct appdata_gs
{
	float4 pos		: POSITION;
	float4 color	: COLOR0;
};

struct v2f
{
	float4 pos		: SV_POSITION;
	float4 color	: TEXCOORD0;
};

appdata_gs vs_main( appdata v )
{
	appdata_gs o;
	o.pos = mul(MATRIX_MVP,v.vertex);
	o.color = v.color * _Color;
	return o;
}

[maxvertexcount(24)]
void gs_main( point appdata_gs input[1], inout PointStream<v2f> stream )
{
	// Compute screen coordinates
	v2f output;
	output.pos = input[0].pos;
	output.color = input[0].color;
	stream.Append( output );
}

float4 ps_main( v2f i ) : SV_Target
{
    return i.color;
}

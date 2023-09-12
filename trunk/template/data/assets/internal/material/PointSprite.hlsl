#include "CGInclude.hlsli"

static const float2 QuadUVs[4] =
{
	float2(0.0f, 1.0f), // v0, lower-left
	float2(0.0f, 0.0f), // v1, upper-left
	float2(1.0f, 1.0f), // v2, lower-right
	float2(1.0f, 0.0f)	// v3, upper-right
};

BEGIN_CONSTANTS(CustomBuffer, 0)
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata_vs
{
	float4 vertex		: POSITION;
	float size			: POINT_SIZE;
};
struct appdata_gs
{
	float4 world_pos	: POSITION;
	float size			: POINT_SIZE;
};
struct v2f
{
    float4 pos			: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

appdata_gs vs_main(appdata_vs v)
{
	appdata_gs o;
	o.world_pos = mul(MATRIX_M, v.vertex); //转到世界坐标
	o.size = v.size;
    return o;
}

[maxvertexcount(4)]
void gs_main(point appdata_gs input[1], inout TriangleStream<v2f> stream)
{ 
	float4 v[4];
	float4 center = input[0].world_pos;
	float half_width = 0.5f * input[0].size;
	float half_height = 0.5f * input[0].size;
	v[0] = center + float4(-half_width, -half_height, 0.0f, 0.0f);
	v[1] = center + float4(-half_width, half_height, 0.0f, 0.0f);
	v[2] = center + float4(half_width, -half_height, 0.0f, 0.0f);
	v[3] = center + float4(half_width, half_height, 0.0f, 0.0f);

    // 对顶点位置进行矩阵变换，并以TriangleStrip形式输出
	v2f o;
    [unroll]
	for (int i = 0; i < 4; ++i)
	{
		o.pos = mul(MATRIX_VP, v[i]);
		o.uv = QuadUVs[i];
		stream.Append(o);
	}
}

float4 ps_main( v2f i ) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv);
	return LinearToGammaSpace(final_col);
}
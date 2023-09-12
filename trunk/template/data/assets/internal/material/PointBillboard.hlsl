#include "CGInclude.hlsli"

static const float2 QuadUVs[4] =
{
	float2(0.0f, 1.0f), // v0, lower-left
	float2(0.0f, 0.0f), // v1, upper-left
	float2(1.0f, 1.0f), // v2, lower-right
	float2(1.0f, 0.0f) // v3, upper-right
};

BEGIN_CONSTANTS(CustomBuffer, 0)
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata_vs
{
	float4 vertex : POSITION;
	float2 size : SIZE;
};
struct appdata_gs
{
	float4 world_pos : POSITION;
	float2 size : SIZE;
};
struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
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
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = WorldSpaceViewDir(input[0].world_pos.xyz);
	look.y = 0.0f; // look向量只取投影到xz平面的向量
	look = normalize(look);
	float3 right = cross(up, look);

    // 计算出公告板矩形的四个顶点
	float4 v[4];
	float3 center = input[0].world_pos.xyz;
	float half_width = 0.5f * input[0].size.x;
	float half_height = 0.5f * input[0].size.y;
	v[0] = float4(center + half_width * right - half_height * up, 1.0f);
	v[1] = float4(center + half_width * right + half_height * up, 1.0f);
	v[2] = float4(center - half_width * right - half_height * up, 1.0f);
	v[3] = float4(center - half_width * right + half_height * up, 1.0f);

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

float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv);
	return LinearToGammaSpace(final_col);
}
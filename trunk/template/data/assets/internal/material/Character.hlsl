#include "CGInclude.hlsli"

#if defined(SM_1_0) || defined(SM_2_0) || defined(SM_3_0)
	#define MAX_BONE 60	//SM3.0只支持最大256个常量寄存器
#else
	#define MAX_BONE 200 //SM4.0之后支持65535个常量寄存器
#endif

BEGIN_CONSTANTS(CustomBuffer, 0)
	matrix LIGHT_MATRIX_MVP;
	float4 _MainTex_ST;
	matrix BoneMatrixArray[MAX_BONE]; //骨骼调色板矩阵,OPENGLES2.0编译会报错
END_CONSTANTS

DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
	float4 vertex : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 uv : TEXCOORD0;
	float4 indices : BLENDINDICES;
	float4 weights : BLENDWEIGHT;
};

struct v2f
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	FOG_COORDS(1)
};
struct v2f_shadow
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD0;
};

v2f vs_main(appdata v)
{
	v2f o;
	float4 pos = float4(0, 0, 0, 0);
	float3 normal = float3(0, 0, 0);
    //计算前NumBones-1个骨骼对于该顶点位置及法向量的影响
	[unroll]
	for (int iBone = 0; iBone < 4; iBone++)
	{
		pos += mul(BoneMatrixArray[v.indices[iBone]], v.vertex) * v.weights[iBone];
	}
	o.pos = mul(MATRIX_MVP, float4(pos.xyz, 1));
	o.uv = TRANSFORM_TEX(v.uv, _MainTex);

	TRANSFER_FOG(o, o.pos);
	return o;
}
float4 ps_main(v2f i) : SV_Target
{
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv);

	APPLY_FOG(i.fogCoord, final_col);
	return LinearToGammaSpace(final_col);
}

v2f_shadow vs_shadow(appdata v)
{
	v2f_shadow o;
	float4 pos = float4(0, 0, 0, 0);
	float3 normal = float3(0, 0, 0);
    //计算前NumBones-1个骨骼对于该顶点位置及法向量的影响
	[unroll]
	for (int iBone = 0; iBone < 4; iBone++)
	{
		pos += mul(BoneMatrixArray[v.indices[iBone]], v.vertex) * v.weights[iBone];
	}
	o.pos = mul(LIGHT_MATRIX_MVP, pos);
	o.depth.xy = o.pos.zw;
	return o;
}
float4 ps_shadow(v2f_shadow i) : SV_Target
{
	float r = i.depth.x / i.depth.y;
	return r;
}
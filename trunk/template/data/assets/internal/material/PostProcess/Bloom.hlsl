#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
float4 _MainTex_TexelSize;
//高亮部分提取阀值
float4 _ColorThreshold;
//高斯模糊方向
float4 _BlurOffsets;
//最后图像叠加
float4 _BloomColor;
float _BloomFactor;
END_CONSTANTS

// 基础贴图
DECLARE_TEXTURE(_MainTex, 0);
//模糊后的贴图
DECLARE_TEXTURE(_BlurTex, 1);

struct appdata
{
	float3 vertex	: POSITION;
	float2 uv		: TEXCOORD0;
};

//用于阈值提取高亮部分
struct v2f_threshold
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD0;
};

//用于blur
struct v2f_blur
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float4 uv01		: TEXCOORD1;
	float4 uv23		: TEXCOORD2;
	float4 uv45		: TEXCOORD3;
};
 
//用于bloom
struct v2f_bloom
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float2 uv1		: TEXCOORD1;
};

//高亮部分提取shader
v2f_threshold vert_threshold(appdata v)
{
	v2f_threshold o;
	o.pos = float4(v.vertex, 1);
	o.uv = v.uv;
	return o;
}
float4 frag_threshold(v2f_threshold i) : SV_Target
{
	float4 color = SAMPLE_TEXTURE(_MainTex, i.uv);
	//仅当color大于设置的阈值的时候才输出(saturate(x)的作用是如果x取值小于0，则返回值为0。如果x取值大于1，则返回值为1。若x在0到1之间，则直接返回x的值)
	return saturate(color - _ColorThreshold);
}

//高斯模糊
v2f_blur vert_blur(appdata v)
{
	v2f_blur o;
	float4 offsets = _BlurOffsets * _MainTex_TexelSize.xyxy;
	o.pos = float4(v.vertex, 1);
	o.uv = v.uv.xy;
 
	o.uv01 = v.uv.xyxy + offsets.xyxy * float4(1, 1, -1, -1);
	o.uv23 = v.uv.xyxy + offsets.xyxy * float4(1, 1, -1, -1) * 2.0;
	o.uv45 = v.uv.xyxy + offsets.xyxy * float4(1, 1, -1, -1) * 3.0;
 
	return o;
}
float4 frag_blur(v2f_blur i) : SV_Target
{
	float4 color = float4(0,0,0,0);
	color += 0.40 * SAMPLE_TEXTURE(_MainTex, i.uv);
	color += 0.15 * SAMPLE_TEXTURE(_MainTex, i.uv01.xy);
	color += 0.15 * SAMPLE_TEXTURE(_MainTex, i.uv01.zw);
	color += 0.10 * SAMPLE_TEXTURE(_MainTex, i.uv23.xy);
	color += 0.10 * SAMPLE_TEXTURE(_MainTex, i.uv23.zw);
	color += 0.05 * SAMPLE_TEXTURE(_MainTex, i.uv45.xy);
	color += 0.05 * SAMPLE_TEXTURE(_MainTex, i.uv45.zw);
	return color;
}

//Bloom效果 vertex shader
v2f_bloom vert_bloom(appdata v)
{
	v2f_bloom o;
	o.pos = float4(v.vertex, 1);
	o.uv = v.uv;
	o.uv1 = o.uv;
#if !defined(UV_STARTS_AT_TOP)
	o.uv.y = 1.0 - o.uv.y;
	o.uv1.y = 1.0 - o.uv1.y;
#endif
	return o;
}
float4 frag_bloom(v2f_bloom i) : SV_Target
{
	//取原始清晰图片进行uv采样
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	//取模糊图片进行uv采样
	float4 blur = SAMPLE_TEXTURE(_BlurTex, i.uv);
	//输出= 原始图像，叠加bloom权值*bloom颜色*泛光颜色
	float4 final = albedo + _BloomColor*blur*_BloomFactor;
	return final;
}
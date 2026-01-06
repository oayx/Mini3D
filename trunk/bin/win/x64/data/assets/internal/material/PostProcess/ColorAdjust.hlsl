#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float _Brightness = 1;
	float _Saturation = 1;
	float _Contrast = 1;
END_CONSTANTS

// 基础贴图
DECLARE_TEXTURE(_MainTex, 0);

struct appdata
{
	float3 vertex		: POSITION;
	float2 uv			: TEXCOORD0;
};

struct v2f
{
    float4 pos			: SV_POSITION;
    float2 uv  			: TEXCOORD0;
};

// 顶点渲染器主函数
v2f vs_main(appdata v)
{
	v2f o;

    //顶点位置 
	o.pos = float4(v.vertex, 1);
    //纹理
	o.uv = v.uv;
#if !defined(UV_STARTS_AT_TOP)
	o.uv.y = 1.0 - o.uv.y;
#endif

    return o;
}

// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv);
	//brigtness亮度直接乘以一个系数，也就是RGB整体缩放，调整亮度
	float3 finalColor = albedo.rgb * _Brightness;
	//saturation饱和度：首先根据公式计算同等亮度情况下饱和度最低的值：
	float gray = 0.2125 * albedo.r + 0.7154 * albedo.g + 0.0721 * albedo.b;
	float3 grayColor = float3(gray, gray, gray);
	//根据Saturation在饱和度最低的图像和原图之间差值
	finalColor = lerp(grayColor, finalColor, _Saturation);
	//contrast对比度：首先计算对比度最低的值
	float3 avgColor = float3(0.5, 0.5, 0.5);
	//根据Contrast在对比度最低的图像和原图之间差值
	finalColor = lerp(avgColor, finalColor, _Contrast);
    return float4(finalColor.rgb, albedo.a);
}
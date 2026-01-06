#include "CGInclude.hlsli"

// 深度图
DECLARE_TEXTURE(_CameraDepthTexture, 0);

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
    return o;
}

// 像素渲染器主函数
float4 ps_main(v2f i) : SV_Target
{
	float4 color = SAMPLE_TEXTURE(_CameraDepthTexture, i.uv);
	return float4(color.rgb, 1);
}
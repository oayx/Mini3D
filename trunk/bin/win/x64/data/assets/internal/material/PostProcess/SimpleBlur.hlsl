#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
float4 _MainTex_TexelSize;
float _BlurRadius = 1;
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
	float4 color = float4(0,0,0,0);
		
	//计算uv上下左右四个点对于blur半径下的uv坐标
	float2 uv1 = i.uv + _BlurRadius * _MainTex_TexelSize.xy * float2(1, 1);
	float2 uv2 = i.uv + _BlurRadius * _MainTex_TexelSize.xy * float2(-1, 1);
	float2 uv3 = i.uv + _BlurRadius * _MainTex_TexelSize.xy * float2(-1, -1);
	float2 uv4 = i.uv + _BlurRadius * _MainTex_TexelSize.xy * float2(1, -1);

	color += SAMPLE_TEXTURE(_MainTex, i.uv);
	color += SAMPLE_TEXTURE(_MainTex, uv1);
	color += SAMPLE_TEXTURE(_MainTex, uv2);
	color += SAMPLE_TEXTURE(_MainTex, uv3);
	color += SAMPLE_TEXTURE(_MainTex, uv4);
	color = color * 0.2;
	return float4(color.rgb, 1);
}
#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
	float4 _MainTex_TexelSize;
	float4 _Intensity;
	matrix _PRE_MATRIX_VP;
END_CONSTANTS

// 基础贴图
DECLARE_TEXTURE(_MainTex, 0);
//深度图
DECLARE_TEXTURE(_CameraDepthTexture, 1);

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
	float4 final_col = SAMPLE_TEXTURE(_MainTex, i.uv);

	float depth = SAMPLE_TEXTURE(_CameraDepthTexture, i.uv).x;
#if !defined(SHADER_API_DIRECTX)
	depth = depth * 2 - 1;//dx是[0,1]，opengl是[-1,1]
#endif
	float4 H = float4(i.uv.x * 2 - 1, i.uv.y * 2 - 1, depth, 1); //NDC坐标
	float4 D = mul(MATRIX_I_VP, H);
	float4 W = D / D.w; //将齐次坐标w分量变1得到世界坐标

	float4 currentPos = H;
	float4 lastPos = mul(_PRE_MATRIX_VP, W);
	lastPos /= lastPos.w;
				
	//采样两点所在直线上的点，进行模糊
	float2 velocity = (currentPos.xy - lastPos.xy) / 2.0 * _Intensity.xy;
	float2 uv = i.uv;
	uv += velocity;
	int numSamples = 3;
	for (int index = 1; index < numSamples; index++, uv += velocity)
	{
		final_col += SAMPLE_TEXTURE(_MainTex, uv);
	}
	final_col /= numSamples;
	
	return final_col;
}
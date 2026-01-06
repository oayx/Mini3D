#include "CGInclude.hlsli"

BEGIN_CONSTANTS(CustomBuffer, 0)
float4 _MainTex_TexelSize;
//高斯模糊方向
float4 _BlurOffsets;
float _FocalDistance;
float _FocalLength;
float _NearBlurScale;
float _FarBlurScale;
END_CONSTANTS

//基础贴图
DECLARE_TEXTURE(_MainTex, 0);
//模糊后的贴图
DECLARE_TEXTURE(_BlurTex, 1);
//深度图
DECLARE_TEXTURE(_CameraDepthTexture, 2);

struct appdata
{
	float3 vertex	: POSITION;
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
struct v2f_dof
{
	float4 pos		: SV_POSITION;
	float2 uv		: TEXCOORD0;
	float2 uv1		: TEXCOORD1;
};

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
v2f_dof vert_dof(appdata v)
{
	v2f_dof o;
	o.pos = float4(v.vertex, 1);
	o.uv = v.uv;
	o.uv1 = o.uv;
#if !defined(UV_STARTS_AT_TOP)
	o.uv.y = 1.0 - o.uv.y;
	o.uv1.y = 1.0 - o.uv1.y;
#endif
	return o;
}
float4 frag_dof(v2f_dof i) : SV_Target
{
	//取原始清晰图片进行uv采样
	float4 albedo = SAMPLE_TEXTURE(_MainTex, i.uv1);
	//取模糊普片进行uv采样
	float4 blur = SAMPLE_TEXTURE(_BlurTex, i.uv);
	//取当位置对应的深度值
	float depth = SAMPLE_TEXTURE(_CameraDepthTexture, i.uv).x;
	//depth = LinearEyeDepth(depth);
		
	//如果depth小于焦点的物体，那么使用原始清晰图像，否则使用模糊的图像与清晰图像的差值，通过差值避免模糊和清晰之间明显的边界，结果为远景模糊效果
	float4 final = (depth <= _FocalDistance + _FocalLength) ? albedo : lerp(albedo, blur, clamp((depth - (_FocalDistance + _FocalLength)) * _FarBlurScale, 0, 1));
	//上面的结果，再进行一次计算，如果depth大于焦点的物体，使用上面的结果和模糊图像差值，得到近景模糊效果
	final = (depth > _FocalDistance - _FocalLength) ? final : lerp(albedo, blur, clamp(((_FocalDistance - _FocalLength) - depth) * _NearBlurScale, 0, 1));
	//焦点位置是清晰的图像，两边分别用当前像素深度距离焦点的距离进行差值，这样就达到原理焦点位置模糊的效果

	return final;
}
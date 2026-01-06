#ifndef CG_INCLUDE_H
#define CG_INCLUDE_H

#if HLSL_VERSION_5_1
	#define BEGIN_CONSTANTS(Name, index)	cbuffer Name : register(b##index) {
	#define END_CONSTANTS					};

	#define DECLARE_TEXTURE(Name, index) \
		Texture2D Name : register(t##index, space1); \
		sampler Name##Sampler : register(s##index, space2)

	#define DECLARE_CUBEMAP(Name, index) \
		TextureCube Name : register(t##index, space1); \
		sampler Name##Sampler : register(s##index, space2)

	#define SAMPLE_TEXTURE(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)
	#define SAMPLE_CUBEMAP(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)

	#define POINT_SIZE SIZE
#elif HLSL_VERSION_3_0
	#define BEGIN_CONSTANTS(Name, slot)		
	#define END_CONSTANTS	
	
	#define DECLARE_TEXTURE(Name, index) \
		texture2D Name; \
		sampler Name##Sampler : register(s##index) = sampler_state { Texture = (Name); };

	#define DECLARE_CUBEMAP(Name, index) \
		textureCUBE Name; \
		sampler Name##Sampler : register(s##index) = sampler_state { Texture = (Name); };

	#define SAMPLE_TEXTURE(Name, texCoord)  tex2D(Name##Sampler, texCoord)
	#define SAMPLE_CUBEMAP(Name, texCoord)  texCUBE(Name##Sampler, texCoord)

	#define POINT_SIZE PSIZE
#else
	#define BEGIN_CONSTANTS(Name, index)	cbuffer Name : register(b##index) {
	#define END_CONSTANTS					};

	#define DECLARE_TEXTURE(Name, index) \
		Texture2D Name : register(t##index); \
		sampler Name##Sampler : register(s##index)

	#define DECLARE_CUBEMAP(Name, index) \
		TextureCube Name : register(t##index); \
		sampler Name##Sampler : register(s##index)

	#define SAMPLE_TEXTURE(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)
	#define SAMPLE_CUBEMAP(Name, texCoord)  Name.Sample(Name##Sampler, texCoord)

	#define POINT_SIZE SIZE
#endif


//角度与弧度转换
#define Deg2Rad 0.0174533
#define Rad2Deg 57.2958

//阴影
#define FilterSize 1

//灯光信息
struct Light
{
	float3 Dir;
	int	   Type;
	float3 Color;
	float3 Attenuation;
	float  Range;
	float3 Position;
	float  SpotAngle;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//对象位置改变更新
BEGIN_CONSTANTS(TransformBuffer, 10)
	matrix MATRIX_MVP;
	matrix MATRIX_M;
	matrix MATRIX_MV;
	matrix MATRIX_I_M;
	matrix MATRIX_T_M;
	matrix MATRIX_T_MV;
	matrix MATRIX_IT_M;
	matrix MATRIX_IT_MV;
END_CONSTANTS
//相机信息改变更新
BEGIN_CONSTANTS(CameraBuffer, 11)
	matrix MATRIX_V;
	matrix MATRIX_P;
	matrix MATRIX_VP;
	matrix MATRIX_I_VP;
	float3 _WorldSpaceCameraPos;
	float3 _WorldSpaceCameraDir;
	float4 _ProjectionParams;
	float4 _ScreenParams;
	float4 _ZBufferParams;
END_CONSTANTS
//灯光信息改变更新
BEGIN_CONSTANTS(LightBuffer, 12)
	int _LightCount;
	float3 _LightDir0;
	float3 _LightColor0;
	Light  _Light[8];
END_CONSTANTS
//不经常变化的
BEGIN_CONSTANTS(CommbonRarelyBuffer, 13)
	float3 _LIGHTMODEL_AMBIENT;
	int _FogMode;
	float4 _FogColor;
	float _FogStart;
	float _FogEnd;
	float _FogDensity;
END_CONSTANTS

//////////////////////////////////////////////////////////////////////////////////////////////////////
struct appdata_base 
{
    float4 vertex : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct appdata_img
{
    float4 vertex : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct appdata_tan 
{
    float4 vertex : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
};

struct appdata_full 
{
    float4 vertex : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

//Transforms 2D UV by scale/bias property
#define TRANSFORM_TEX(tex,name) (tex.xy * name##_ST.xy + name##_ST.zw)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//光线方向转化到模型空间(灯光方向是从顶点指向外部的)
inline float3 ObjSpaceLightDir(float3 v)
{
	return normalize(mul((float3x3) MATRIX_I_M, -_LightDir0));
}
//世间坐标下的光方向(指向光源)
inline float3 WorldSpaceLightDir(float3 w_pos)
{
	if (_Light[0].Type == 0)//方向光
		return -_LightDir0;
	else
		return normalize(_Light[0].Position - w_pos);
}
//世界空间观察方向
inline float3 WorldSpaceViewDir(float3 w_pos)
{
	return normalize(_WorldSpaceCameraPos.xyz - w_pos.xyz);
}
//本地空间观察方向
inline float3 ObjSpaceViewDir(float3 w_pos)
{
	float3 camera_pos = mul(MATRIX_I_M,float4(_WorldSpaceCameraPos.xyz, 1)).xyz;
	return normalize(camera_pos - w_pos.xyz);
}
//将顶点从本地坐标变换到相机坐标
inline float3 ObjectToViewPos(float3 pos)
{
	float4 w_pos = mul(MATRIX_M,float4(pos.xyz, 1.0));
	return mul(w_pos, MATRIX_V).xyz;
}
//将顶点从世界坐标变换到相机坐标
inline float3 WorldToViewPos(float3 w_pos)
{
	return mul(MATRIX_V,float4(w_pos.xyz, 1.0)).xyz;
}
//将方向从本地坐标变换到相机坐标
inline float3 ObjectToWorldDir(float3 dir)
{
	return normalize(mul((float3x3) MATRIX_M, dir.xyz));
}
//将方向从世界坐标变换到本地坐标
inline float3 WorldToObjectDir(float3 dir)
{
	return normalize(mul((float3x3) MATRIX_I_M, dir.xyz));
}
//将法线从本地坐标变换到世界坐标
inline float3 ObjectToWorldNormal(float3 n)
{
	return normalize(mul((float3x3) MATRIX_IT_M, n.xyz));
}
//将法线从本地坐标变换到观察坐标
inline float3 ObjectToViewNormal(float3 n)
{
	return normalize(mul((float3x3) MATRIX_IT_MV, n.xyz));
}
//法线贴图保存的法线方向是[0,1]，这里转换成[-1,1]
inline float3 UnpackNormal(float4 packednormal)
{
	return packednormal.xyz * 2 - 1;
}
//构建切线空间
inline float3x3 GetTangentSpace(float3 normal, float4 tangent)
{
	float3 N = normalize(normal);
	float3 T = normalize(tangent.xyz);
	float3 B = cross(N, T) * tangent.w;
	return float3x3(T, B, N);
}
// Z buffer to linear 0..1 depth，范围(near/far,1)
inline float Linear01Depth(float z)
{
	return (1.0 / (_ZBufferParams.x * z + _ZBufferParams.y));
}
// Z buffer to linear depth，范围(near,far)
inline float LinearEyeDepth(float z)
{
	return (1.0 / (_ZBufferParams.z * z + _ZBufferParams.w));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//计算屏幕空间位置[0,1]， 输入参数pos是经过MVP矩阵变换后在裁剪空间中的顶点坐标
inline float4 ComputeScreenPos(float4 pos)
{
    float4 o = pos * 0.5f;
    o.xy = float2(o.x, o.y) + o.w;
    o.zw = pos.zw;
    return o;
}
// 灰度
inline float Luminance(float3 c)
{
	return dot(c, float3(0.22, 0.707, 0.071));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//gamma校正
inline float4 GammaToLinearSpace(float4 color)
{
#if COLORSPACE_LINEAR
	return float4(color.rgb * (color.rgb * (color.rgb * 0.305306011h + 0.682171111h) + 0.012522878h), color.a);
#else
	return color;
#endif
}
inline float4 LinearToGammaSpace(float4 color)
{
#if COLORSPACE_LINEAR
	float3 rgb = max(color.rgb, float3(0.h, 0.h, 0.h));
	return float4(max(1.055h * pow(rgb, 0.416666667h) - 0.055h, 0.h), color.a);
#else
	return color;
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//fog
//#if defined(FOG_LINEAR) || defined(FOG_EXP) || defined(FOG_EXP2)
	#define FOG_COORDS(idx) float fogCoord : TEXCOORD##idx;
	#define TRANSFER_FOG(o,outpos) o.fogCoord = outpos.z
//#else
//	#define FOG_COORDS(idx)
//	#define TRANSFER_FOG(o,outpos)
//#endif
#if defined(FOG_LINEAR)
	#define FOG_FACTOR(fogCoord) clamp((_FogEnd-fogCoord)/(_FogEnd-_FogStart), 0.0, 1.0) //雾化因子线性变化
#elif defined(FOG_EXP)
	#define FOG_FACTOR(fogCoord) exp(-(_FogDensity*fogCoord)) //指数雾
#elif defined(FOG_EXP2)
	#define FOG_FACTOR(fogCoord) exp(-_FogDensity*_FogDensity*fogCoord*fogCoord) //指数平方雾
#endif

#define APPLY_FOG(fogCoord,color) float fog_factor = 1.0; \
			if(_FogMode == 1) \
				fog_factor = clamp((_FogEnd - fogCoord) / (_FogEnd - _FogStart), 0.0, 1.0); \
			else if(_FogMode == 2) \
				fog_factor = exp(-(_FogDensity*fogCoord)); \
			else if (_FogMode == 3) \
				fog_factor = exp(-_FogDensity * _FogDensity*fogCoord*fogCoord); \
			color.rgb = lerp(_FogColor.rgb, color.rgb, fog_factor) \

//////////////////////////////////////////////////////////////////////////////////////////////////////
//深度+法线图
// Encoding/decoding [0..1) floats into 8 bit/channel RG. Note that 1.0 will not be encoded properly.
inline float2 EncodeFloatRG(float v)
{
	float2 kEncodeMul = float2(1.0, 255.0);
	float kEncodeBit = 1.0 / 255.0;
	float2 enc = kEncodeMul * v;
	enc = frac(enc);
	enc.x -= enc.y * kEncodeBit;
	return enc;
}
inline float DecodeFloatRG(float2 enc)
{
	float2 kDecodeDot = float2(1.0, 1 / 255.0);
	return dot(enc, kDecodeDot);
}
// Encoding/decoding view space normals into 2D 0..1 vector
inline float2 EncodeViewNormalStereo(float3 n)
{
	float kScale = 1.7777;
	float2 enc;
	enc = n.xy / (n.z + 1);
	enc /= kScale;
	enc = enc * 0.5 + 0.5;
	return enc;
}
inline float3 DecodeViewNormalStereo(float4 enc4)
{
	float kScale = 1.7777;
	float3 nn = enc4.xyz * float3(2 * kScale, 2 * kScale, 0) + float3(-kScale, -kScale, 1);
	float g = 2.0 / dot(nn.xyz, nn.xyz);
	float3 n;
	n.xy = g * nn.xy;
	n.z = g - 1;
	return n;
}
inline float4 EncodeDepthNormal(float depth, float3 normal)
{
	float4 enc;
	enc.xy = EncodeViewNormalStereo(normal);
	enc.zw = EncodeFloatRG(depth);
	return enc;
}
inline void DecodeDepthNormal(float4 enc, out float depth, out float3 normal)
{
	depth = DecodeFloatRG(enc.zw);
	normal = DecodeViewNormalStereo(enc);
}

#endif
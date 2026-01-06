 
/*****************************************************************************
* Author： hannibal
* Date：2020/3/30
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include <comdef.h>
#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE

/********************************************************************/
inline const char* HLSLDefineVersion[int(ShaderModel::Max)] = { "HLSL_VERSION_1_0", "HLSL_VERSION_2_0", "HLSL_VERSION_3_0", "HLSL_VERSION_4_0", "HLSL_VERSION_5_0", "HLSL_VERSION_5_1" };

/********************************************************************/
//获得顶点输入语义
inline const char* DXGetInputSemantic(VertexSemantic value)
{
	switch (value)
	{
	case VertexSemantic::Position:return "POSITION";
	case VertexSemantic::Diffuse:return "COLOR";
	case VertexSemantic::Normal:return "NORMAL";
	case VertexSemantic::Tangent:return "TANGENT";
	case VertexSemantic::Coordinates1:return "TEXCOORD";
	case VertexSemantic::Coordinates2:return "TEXCOORD";
	case VertexSemantic::Weights:return "BLENDWEIGHT";
	case VertexSemantic::Indices:return "BLENDINDICES";
	case VertexSemantic::Size:return "SIZE";
	case VertexSemantic::InstanceOffset:return "POSITION";
	default:AssertEx(0, "%d", value); return "TEXCOORD";
	}
}
inline VertexSemantic DXGetVertexSemantic(int index, const String& value)
{
	if (value == "POSITION")
	{
		switch (index)
		{
		case 0:return VertexSemantic::Position;
		case 1:return VertexSemantic::InstanceOffset;
		case 2:return VertexSemantic::InstanceOffset;
		case 3:return VertexSemantic::InstanceOffset;
		case 4:return VertexSemantic::InstanceOffset;
		}
	}
	else if (value == "COLOR")return VertexSemantic::Diffuse;
	else if (value == "NORMAL")return VertexSemantic::Normal;
	else if (value == "TANGENT")return VertexSemantic::Tangent;
	else if (value == "BLENDWEIGHT")return VertexSemantic::Weights;
	else if (value == "BLENDINDICES")return VertexSemantic::Indices;
	else if (value == "SIZE")return VertexSemantic::Size;
	else if (value == "TEXCOORD")
	{
		switch (index)
		{
		case 0:return VertexSemantic::Coordinates1;
		case 1:return VertexSemantic::Coordinates2;
		}
	}
	else AssertEx(0, "%s", value.c_str()); return (VertexSemantic)0;
}
//获得shader入口
inline String DXGetShaderTarget(ShaderType type, const String& version)
{
	switch (type)
	{
	case ShaderType::Vertex:	return String("vs_") + version;
	case ShaderType::Hull:		return String("hs_") + version;
	case ShaderType::Domain:	return String("ds_") + version;
	case ShaderType::Geometry:	return String("gs_") + version;
	case ShaderType::Pixel:		return String("ps_") + version;
	case ShaderType::Compute:	return String("cs_") + version;
	default:AssertEx(false, ""); return "";
	}
}

#if DC_DEBUG
	#define DX_ERROR(hr) do { AssertEx(SUCCEEDED(hr), "DXError:%s", Encoding::WCharToUtf8(_com_error(hr).ErrorMessage()).c_str()); } while(0)
#else
	#define DX_ERROR(hr) UNUSED(hr)
#endif

DC_END_NAMESPACE

/*
常用标量类型如下：
类型	描述
bool	32位整数值用于存放逻辑值true和false
int		32位有符号整数
uint	32位无符号整数
half	16位浮点数(仅提供用于向后兼容)
float	32位浮点数
double	64位浮点数
*/

/*
变量的修饰符
关键字	含义
static	该着色器变量将不会暴露给C++应用层，需要在HLSL中自己初始化，否则使用默认初始化
extern	与static相反，该着色器变量将会暴露给C++应用层
uniform	该着色器变量允许在C++应用层被改变，但在着色器执行的过程中，其值始终保持不变（运行前可变，运行时不变）。着色器程序中的全局变量默认为既uniform又extern
const	和C++中的含义相同，它是一个常量，需要被初始化且不可以被修改
*/

/*
顶点着色器语义
输入				描述					类型
BINORMAL[n]			副法线（副切线）向量	float4
BLENDINDICES[n]		混合索引				uint
BLENDWEIGHT[n]		混合权重				float
COLOR[n]			漫反射/镜面反射颜色		float4
NORMAL[n]			法向量					float4
POSITION[n]			物体坐标系下的顶点坐标	float4
POSITIONT			变换后的顶点坐标		float4
PSIZE[n]			点的大小				float
TANGENT[n]			切线向量				float4
TEXCOORD[n]			纹理坐标				float4
Output				仅描述输出				Type
FOG					顶点雾					float

像素着色器语义
输入				描述					类型
COLOR[n]			漫反射/镜面反射颜色		float4
TEXCOORD[n]			纹理坐标				float4
Output				仅描述输出				Type
DEPTH[n]			深度值					float

系统值语义
所有的系统值都包含前缀SV_。这些系统值将用于某些着色器的特定用途（并未全部列出）

系统值				描述																											类型
SV_Depth			深度缓冲区数据，可以被任何着色器写入/读取																		float
SV_InstanceID		每个实例都会在运行期间自动生成一个ID。在任何着色器阶段都能读取													uint
SV_IsFrontFace		指定该三角形是否为正面。可以被几何着色器写入，以及可以被像素着色器读取											bool
SV_POSITION			说明该顶点的位置在从顶点着色器输出后，后续的着色器都不能改变它的值，作为光栅化时最终确定的像素位置				float4
SV_PrimitiveID		每个原始拓扑都会在运行期间自动生成一个ID。可用在几何/像素着色器中写入，也可以在像素/几何/外壳/域着色器中读取	uint
SV_StencilRef		代表当前像素着色器的模板引用值。只可以被像素着色器写入															uint
SV_VertexID			每个实例都会在运行期间自动生成一个ID。仅允许作为顶点着色器的输入												uint
SV_Target			说明输出的颜色值将会直接保存到渲染目标视图的后备缓冲区对应位置
*/

/*
几何着色器输入模式	顶点着色器输入									顶点最少个数
point				POINTS											1
line				LINES, LINE_LOOP, LINE_STRIP					2
triangle			TRIANGLES, TRIANGLE_FAN, TRIANGLE_STRIP			3
lines_adjacency		LINES_ADJACENCY，LINE_STRIP_ADJACENCY			4
triangles_adjacency	TRIANGLES_ADJACENCY,TRIANGLE_STRIP_ADJACENCY	6
*/

/*
DirectX版本与SM对应关系
Direct3D	Shader Model		Note
8.0			1.0 – 1.3	 
8.1			1.4(PS)/1.1(VS) 
9.0			2.0	 
9.0c		3.0	 
10.0		4.0					added Geometry Shaders
10.1		4.1	 
11.0		5.0					added Domain & Hull Shaders
12.0		5.1
*/

/*
Shader Model 和 DirectX 的关系：			常量寄存器			临时寄存器				循环嵌套深度		指令槽的最大数目
DirectX 8.0  - Shader Model 1.0 & 1.1																		128					
DirectX 8.0a - Shader Model 1.3
DirectX 8.1  - Shader Model 1.4
DirectX 9.0  - Shader Model 2.0																				256
DirectX 9.0a - Shader Model 2.0a
DirectX 9.0b - Shader Model 2.0b
DirectX 9.0c - Shader Model 3.0				256					32						4					>=512
DirectX 10.0 - Shader Model 4.0				65535
DirectX 10.1 - Shader Model 4.1
DirectX 11.0 - Shader Model 5.0
DirectX 12.0 - Shader Model 5.1
*/

/* HLSL内置函数
函数名		使用						说明
abs			abs(x)						计算输入值的绝对值。
acos		acos(x)						返回输入值反余弦值。
all			all(x)						测试非0值。
any			any(x)						测试输入值中的任何非零值。
asin		asin(x)						返回输入值的反正弦值。
atan		atan(x)						返回输入值的反正切值。
atan2		atan2(y, x)					返回y/x的反正切值。
ceil		ceil(x)						返回大于或等于输入值的最小整数。
clamp		clamp(x, min, max)			把输入值限制在[min, max]范围内。
clip		clip(x)						如果输入向量中的任何元素小于0，则丢弃当前像素。
cos			cos(x)						返回输入值的余弦。
cosh		cosh(x)						返回输入值的双曲余弦。
cross		cross(x, y)					返回两个3D向量的叉积。
ddx			ddx(x)						返回关于屏幕坐标x轴的偏导数。
ddy			ddy(x)						返回关于屏幕坐标y轴的偏导数。
degrees		degrees(x)					弧度到角度的转换
determinant	determinant(m)				返回输入矩阵的值。
distance	distance(x, y)				返回两个输入点间的距离。
dot			dot(x, y)					返回两个向量的点积。
exp			exp(x)						返回以e为底数，输入值为指数的指数函数值。
exp2		exp2(x)						返回以2为底数，输入值为指数的指数函数值。
faceforward	faceforward(n, i, ng)		检测多边形是否位于正面。
floor		floor(x)					返回小于等于x的最大整数。
fmod		fmod(x, y)					返回a / b的浮点余数。
frac		frac(x)						返回输入值的小数部分。
frexp		frexp(x, exp)				返回输入值的尾数和指数
fwidth		fwidth(x)					返回 abs ( ddx (x) + abs ( ddy(x))。
isfinite	isfinite(x)					如果输入值为有限值则返回true，否则返回false。
isinf		isinf(x)					如何输入值为无限的则返回true。
isnan		isnan(x)					如果输入值为NAN或QNAN则返回true。
ldexp		ldexp(x, exp)				frexp的逆运算，返回 x * 2 ^ exp。
len / lenth	length(v)					返回输入向量的长度。
lerp		lerp(x, y, s)				对输入值进行插值计算。
lit			lit(n • l, n • h, m)		返回光照向量（环境光，漫反射光，镜面高光，1）。
log			log(x)						返回以e为底的对数。
log10		log10(x)					返回以10为底的对数。
log2		log2(x)						返回以2为底的对数。
max			max(x, y)					返回两个输入值中较大的一个。
min			min(x, y)					返回两个输入值中较小的一个。
modf		modf(x, out ip)				把输入值分解为整数和小数部分。
mul			mul(x, y)					返回输入矩阵相乘的积。
noise		noise(x)					Generates a random value using the Perlin-noise algorithm.
normalize	normalize(x)				返回规范化的向量，定义为 x / length(x)。
pow			pow(x, y)					返回输入值的指定次幂。
radians		radians(x)					角度到弧度的转换。
reflect		reflect(i, n)				返回入射光线i对表面法线n的反射光线。
refract		refract(i, n, R)			返回在入射光线i，表面法线n，折射率为eta下的折射光线v。
round		round(x)					返回最接近于输入值的整数。
rsqrt		rsqrt(x)					返回输入值平方根的倒数。
saturate	saturate(x)					把输入值限制到[0, 1]之间。
sign		sign(x)						计算输入值的符号。
sin			sin(x)						计算输入值的正弦值。
sincos		sincos(x, out s, out c)		返回输入值的正弦和余弦值。
sinh		sinh(x)						返回x的双曲正弦。
smoothstep	smoothstep(min, max, x)		返回一个在输入值之间平稳变化的插值。
sqrt		sqrt(x)						返回输入值的平方根。
step		step(a, x)					返回（x >= a）? 1 : 0。
tan			tan(x)						返回输入值的正切值。
fanh		tanh(x)						返回输入值的双曲线切线。
transpose	transpose(m)				返回输入矩阵的转置。
tex1D		tex1D(s, t)					1D texture lookup.
tex1Dbias	tex1Dbias(s, t)				1D texture lookup with bias.
tex1Dgrad	tex1Dgrad(s, t, ddx, ddy)	1D texture lookup with a gradient.
tex1Dlod	tex1Dlod(s, t)				1D texture lookup with LOD.
tex1Dproj	tex1Dproj(s, t)				1D texture lookup with projective divide.
tex2D		tex2D(s, t)					2D texture lookup.
tex2Dbias	tex2Dbias(s, t)				2D texture lookup with bias.
tex2Dgrad	tex2Dgrad(s, t, ddx, ddy)	2D texture lookup with a gradient.
tex2Dlod	tex2Dlod(s, t)				2D texture lookup with LOD.
tex2Dproj	tex2Dproj(s, t)				2D texture lookup with projective divide.
tex3D		tex3D(s, t)					3D texture lookup.
tex3Dbias	tex3Dbias(s, t)				3D texture lookup with bias.
tex3Dgrad	tex3Dgrad(s, t, ddx, ddy)	3D texture lookup with a gradient.
tex3Dlod	tex3Dlod(s, t)				3D texture lookup with LOD.
tex3Dproj	tex3Dproj(s, t)				3D texture lookup with projective divide.
texCUBE		texCUBE(s, t)				Cube texture lookup.
texCUBEbias	texCUBEbias(s, t)			Cube texture lookup with bias.
texCUBEgrad	texCUBEgrad(s, t, ddx, ddy)	Cube texture lookup with a gradient.
texCUBElod	tex3Dlod(s, t)				Cube texture lookup with LOD.
texCUBEproj	texCUBEproj(s, t)			Cube texture lookup with projective divide.
*/
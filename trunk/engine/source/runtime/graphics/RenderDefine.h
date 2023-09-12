 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/25
* Description： 定义渲染相关的数据类型
*****************************************************************************/
#pragma once

#include "core/Object.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
//默认shader入口
#define Default_VS_Enter "vs_main"
#define Default_HS_Enter "hs_main"
#define Default_DS_Enter "ds_main"
#define Default_GS_Enter "gs_main"
#define Default_PS_Enter "ps_main"
#define Default_CS_Enter "cs_main"

//事件
inline const String PreCommandEvt = "PreRender";
inline const String PostCommandEvt = "PostRender";

/********************************************************************/
// shader类型
enum class ShaderType : byte
{
	Vertex = 0,			//顶点着色器
	Hull,				//外壳(opengl叫Tessellation Control Shader)
	Domain,				//域着色器(opengl叫Tessellation Evaluation Shader)
	Geometry,			//几何着色器
	Pixel,				//像素着色器
	Compute,			//计算着色器,ComputeShader不属于图形渲染管线的任何阶段，但它可以计算图形渲染管线的通用数据，加快整个渲染流程
	Max,
};
DECLARE_ENUM_OPERATORS(ShaderType);

// shader语言
enum class ShaderLanguage : byte
{
	Dxil = 0,
	SpirV,

	Hlsl,
	Glsl,
	Essl,
	Msl_macOS,
	Msl_iOS,
};
DECLARE_ENUM_OPERATORS(ShaderLanguage);

enum class ShaderModel : byte
{
	SM_1_0 = 0,
	SM_2_0,
	SM_3_0,
	SM_4_0,
	SM_5_0,
	SM_5_1,
	Max,
};
DECLARE_ENUM_OPERATORS(ShaderModel);

inline const char* ShaderVersion[int(ShaderModel::Max)] = { "SM_1_0", "SM_2_0", "SM_3_0", "SM_4_0", "SM_5_0", "SM_5_1" };
inline const char* ShaderEnterEnum[int(ShaderType::Max)] = { "vs_main", "hs_main", "ds_main", "gs_main", "ps_main" , "cs_main" };
inline const char* ShaderFileExtEnum[int(ShaderType::Max)] = { ".vs",".hs",".ds",".gs",".ps",".cs" };
inline ShaderType ShaderStageEnum[int(ShaderType::Max)] = { ShaderType::Vertex,ShaderType::Hull,ShaderType::Domain,ShaderType::Geometry,ShaderType::Pixel,ShaderType::Compute };

//获得语言对应的文件名称
inline String ShadingLanguageFileName()
{
#if defined(DC_GRAPHICS_API_DX9) || defined(DC_GRAPHICS_API_DX11)
	return "hlsl";
#elif defined(DC_GRAPHICS_API_OPENGL)
	return "glsl";
#elif defined(DC_GRAPHICS_API_OPENGLES3)
	return "essl";
#else
#error "Unknown RendererAPI"
#endif
}
/********************************************************************/
// 颜色空间
enum class ColorSpace : byte
{
	Linear = 1,
	SRGB,
};
DECLARE_ENUM_OPERATORS(ColorSpace);
/********************************************************************/
// 渲染模式
enum class RenderPass : byte
{
	Forward = 0,
	Deferred,
};
DECLARE_ENUM_OPERATORS(RenderPass);
enum class RenderMode : byte
{
	Normal = 0,
	Reflect,
	ShadowMap,
	Blit,
	Depth,
	DepthNormal,
};
DECLARE_ENUM_OPERATORS(RenderMode);
/********************************************************************/
// 填充模式，对应D3DFILLMODE
enum class FillMode : byte
{
	Point = 1,			//点
	Wire, 
	Solid, 
};
DECLARE_ENUM_OPERATORS(FillMode);

/********************************************************************/
// 着色模式，对应D3DSHADEMODE
enum class ShadeMode : byte
{
	Flat = 1,			//平面着色
	Gouraud, 
};
DECLARE_ENUM_OPERATORS(ShadeMode);

/********************************************************************/
// 剔除模式，对应D3DCULL(默认的剔除方式是D3DCULL_CCW)
enum class CullMode : byte
{
	Off= 1,				//不剔除任何面
	Front,				//剔除顺时针
	Back,				//剔除逆时针
};
DECLARE_ENUM_OPERATORS(CullMode);

/********************************************************************/
// 图元类型
enum class PrimitiveType : byte
{
	PointList = 1,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	TriangleFun,
};
DECLARE_ENUM_OPERATORS(PrimitiveType);

/********************************************************************/
// 图元顶点布局
enum class VertexLayout : byte
{
	Variable,					//可变，默认
	VertexColor,
	VertexColorTexcoord,
};
DECLARE_ENUM_OPERATORS(VertexLayout);

//顶点+颜色
#pragma pack(push,4)
struct VertexColorLayout
{
	Vector3 pos;
	uint color;
};
#pragma pack(pop)
static_assert(sizeof(VertexColorLayout) == sizeof(Vector3) + sizeof(uint), "invalid bytes");

//顶点+颜色+uv
#pragma pack(push,4)
struct VertexColorUVLayout
{
	Vector3 pos;
	uint color;
	Vector2 uv;
};
#pragma pack(pop)
static_assert(sizeof(VertexColorUVLayout) == sizeof(Vector3) + sizeof(uint) + sizeof(Vector2), "invalid bytes");

/********************************************************************/
// 深度/Alpha比较函数
enum class StencilCmp : byte
{
	Never = 1,			//深度测试函数总是返回false
	Less,				//深度测试值小于深度缓冲区相应值时返回true
	Equal,				//相等
	LessEqual,			//小于等于
	Greater,			//大于
	NotEqual,			//不等于
	GreaterEqual,		//大于等于
	Always,				//一直  
};
DECLARE_ENUM_OPERATORS(StencilCmp);

/********************************************************************/
// 纹理过滤方式,对应D3DTEXTUREFILTERTYPE
// RenderTargetKey 设置TextureFilter filter_mode : 2，限制了不能超过3个枚举
enum class TextureFilter : byte
{
	Nearest = 0,		//最近点采样
	Bilinear,			//双线性过滤
	Trilinear,			//三线性过滤
};
DECLARE_ENUM_OPERATORS(TextureFilter);

/********************************************************************/
// 纹理寻址模式,对应D3DTEXTUREADDRESS
// RenderTargetKey 设置TextureAddress wrap_mode : 2，限制了不能超过3个枚举
enum class TextureAddress : byte
{
	Wrap = 0,			//重叠纹理寻址模式 
	Mirror,				//镜像纹理寻址模式
	Clamp,				//夹取纹理寻址模式,用边界颜色填充剩余区域
	Border,				//边框纹理寻址模式,需要配合边框颜色
};
DECLARE_ENUM_OPERATORS(TextureAddress);
/********************************************************************/
// 混合系数,对应D3DBLEND
enum class AlphaBlend : byte
{
	Zero = 1,			//混合系数为0
	One,				//混合系数为1
	SrcColor,			//混合系数为当前绘制像素的color值
	OneMinusSrcColor,	//混合系数为1 - 当前绘制像素的color值
	SrcAlpha,
	OneMinusSrcAlpha,
	DestAlpha,			//混合系数为颜色缓冲区中像素的Alpha值
	OneMinusDestAlpha,
	DestColor,
	OneMinusDestColor,
};
DECLARE_ENUM_OPERATORS(AlphaBlend);

/********************************************************************/
// 纹理种类
enum class TextureGroup : byte
{
	Default = 0,
	Normal,
	SpriteAtlas,	//图集
};
DECLARE_ENUM_OPERATORS(TextureGroup);
// 纹理类型
enum class TextureType : byte
{
	None = 0,
	D2,
	D3,
	Cube,
	D2Array,
};
DECLARE_ENUM_OPERATORS(TextureType);

/********************************************************************/
// 索引类型
enum class IndexType : byte
{
	B16 = 0,
	B32 = 1,
};
DECLARE_ENUM_OPERATORS(IndexType);

/********************************************************************/
//顶点stream数量
enum
{
	MAX_STREAM_COUNT = 2,			//stream最大数量
	INSTANCE_STREAM_INDEX = 1,		//instance在stream的索引
};

// 顶点buffer描述
enum class VertexSemantic : ushort
{
	None			= 0x0,
	Position		= 0x1,
	Diffuse			= 0x2,
	Normal			= 0x4,
	Tangent			= 0x8,
	Coordinates1	= 0x10,
	Coordinates2	= 0x20,
	Weights			= 0x40,
	Indices			= 0x80,
	Size			= 0x100,
	InstanceOffset	= 0x200,
	Max
};
DECLARE_ENUM_OPERATORS(VertexSemantic);

// 顶点buffer类型
enum class VertexSize : byte
{
	Float1 = 0,
	Float2,
	Float3,
	Float4,
	Color,
	Short1,
	Short2,
	Short3,
	Short4,
	Byte4,
};
DECLARE_ENUM_OPERATORS(VertexSize);

// 顶点属性
struct VertexAttrib
{
	byte index = 0;
	String name = "";
	VertexSize type = VertexSize::Byte4;
};

/********************************************************************/
// 资源的存储方式
enum class GPUResourcePool : byte
{
	Default = 0,
	Manager = 1,
	System = 2,
	Scratch = 3,
};
DECLARE_ENUM_OPERATORS(GPUResourcePool);

/********************************************************************/
// 资源的锁住类型
/*
CPU 和GPU是可以并行工作的，如果GPU正在使用一块内存上的数据绘制，而CPU要修改这块数据，就会出现CPU等待GPU或GPU等待CPU的情况，造成资源浪费。
D3DLOCK_NOOVERWRITE 向Buffer里面再添加数据，并且能在修改数据的时候继续渲染
D3DLOCK_DISCARD 使用对象是Dynamic，像Dynamic Texture,Dynamic Vertex buffer等等，且在修改数据时，GPU会分配一块新内存，能使用原来的数据进行渲染。当修改里面数据完成时，调用Unlock后，就以新的buffer数据进行渲染
如果不使用任何标志进行Lock的话，将会打断CPU和GPU的并行流水线，使GPU处于Idle状态，直到调用Unlock才会使其（GPU）继续工作
lock和unlock是为了CPU和GPU的同步设计的
*/
enum class GPUResourceLock : byte
{
	Discard = 0,	//丢弃原来的
	NoOverWrite = 1,//添加
	ReadOnly = 2,	//只读
};
DECLARE_ENUM_OPERATORS(GPUResourceLock);

/********************************************************************/
// 资源的使用方式
enum class GPUResourceUsage : byte
{
	Static = 0,
	Dynamic,
	Staging,
};
DECLARE_ENUM_OPERATORS(GPUResourceUsage);

/********************************************************************/
// 模板测试操作函数
enum class StencilOp : byte
{
	Keep = 0,		//保持当前值
	Zero,			//用0替换
	Replace,		//用参考值替换
	Incr,			//加1，如果超过最大值，保持为最大值
	Incr_Wrap,		//加1，如果超过最大值，变成0
	Decr,			//减1，如果小于0，保持为0
	Decr_Wrap,		//减1，如果小于0，变成允许的最大值
};
DECLARE_ENUM_OPERATORS(StencilOp);

/********************************************************************/
// fog类型
enum class FogMode : byte
{
	Linear,			//线性，计算公式为f = (end - d)/(end - start)，其中d为计算点到观察者的距离
	Exp,
	Exp2,
};
DECLARE_ENUM_OPERATORS(FogMode);
/********************************************************************/
//纹理标记
enum class TextureFlag : ushort
{
	NONE		= 0x0,			// No buffer selected.
	COLOR		= 0x1,			// Color buffer selected.
	DEPTH		= 0x2,			// Depth buffer selected.
	STENCIL		= 0x4,			// Stencil buffer selected.
	CUBE		= 0x8,			// Dynamic Cube
	MipMap		= 0x10,			// mipmap
	MSAA		= 0x20,			// msaa
	sRGB		= 0x40,
	ShadowMap	= 0x80,			//shadowmap
	COLOR_AND_DEPTH = COLOR | DEPTH,
	COLOR_AND_STENCIL = COLOR | STENCIL,
	DEPTH_AND_STENCIL = DEPTH | STENCIL,
	COLOR_DEPTH_AND_STENCIL = COLOR | DEPTH | STENCIL,
};
DECLARE_ENUM_OPERATORS(TextureFlag);

enum
{
	kMaxSupportedTextureUnits = 8,
	kMaxSupportedTextureCoords = 8,

	kMaxSupportedVertexLights = 8,

	kMaxSupportedRenderTargets = 4,
	kMaxSupportedConstantBuffers = 16,
	kMaxSupportedComputeResources = 16,
};

/********************************************************************/
//msaa,默认不进行多重采样，则两个数值分别为1和0
struct alignas(4) MSAALevel
{
	int Count = 1;//有效值：1,2,4,8,16
	uint Quality = 0;
};

enum class MSAAType : byte
{
	Disabled = 1,
	Level_2 = 2,
	Level_4 = 4,
	Level_8 = 8,
};
DECLARE_ENUM_OPERATORS(MSAAType);

/********************************************************************/
//反射变量
struct sShaderReflectVariable
{
	String cb_name = "";
	String var_name = "";
	String var_fullname ="";//包括结构体名的全名称
	uint offset = 0;
	uint size = 0;
	ShaderType stage = ShaderType::Max;	//使用阶段
};
typedef Map<String, sShaderReflectVariable> ShaderReflectVariables;

//cbuffer
struct sShaderReflectCBuffer
{
	String name;
	byte slot = 0;
	uint size = 0;
	uint stage = 0;			//使用阶段
	ShaderReflectVariables variables;
};
typedef Map<String, sShaderReflectCBuffer> ShaderReflectCBuffers;

//纹理
struct sShaderReflectTexture
{
	String name = "";
	byte slot = 0;
	uint stage = 0;			//使用阶段
	TextureType type = TextureType::D2;
};
typedef Map<String, sShaderReflectTexture> ShaderReflectTextures;

/********************************************************************/
// shader字段类型
enum class ShaderVariableType : byte
{
	Int = 0,
	Float,
	Bool,
	Slider,
	Color,
	Vector,
	Rect,
	D2,
	D3,
	Cube,
};
DECLARE_ENUM_OPERATORS(ShaderVariableType);

class ShaderVariable
{
	DECLARE_SERIALIZE(ShaderVariable);
public:
	ShaderVariableType Type = ShaderVariableType::Float;
	String	Name = "";
	String	Alias = "";
	String	Value = "";
	Vector2 Range = Vector2::zero;
	bool	HideInspector = false;
};
typedef Vector<ShaderVariable> VecShaderVariable;

class ShaderTexture
{
	DECLARE_SERIALIZE(ShaderTexture);
public:
	String Name = "_MainTex";
	TextureType Type = TextureType::D2;
	String File = "";
	Vector2	Scale = Vector2::one;					//纹理坐标缩放
	Vector2	Offset = Vector2::zero;					//纹理坐标偏移
};
typedef Vector<ShaderTexture> VecShaderTexture;

class ShaderDesc
{
public:
	int					PassIdx = 0;
	VecString			ShaderFile;					//shader文件
	VecString			Enter;						//入口
	VecString			Target;						//版本

	VecString			ShaderDefines;				//常量

	VertexSemantic			SRVertexSemantic = VertexSemantic::None;//反射语义
	ShaderReflectCBuffers	SRBuffers;				//反射cbuffer
	ShaderReflectVariables	SRVariables;			//反射变量
	ShaderReflectTextures	SRTextures;				//反射纹理

public:
	ShaderDesc()
	{
		ShaderFile.Resize(int(ShaderType::Max));
		Enter.Resize(int(ShaderType::Max));
		Target.Resize(int(ShaderType::Max));
	}
};

/********************************************************************/
// 获得类型大小
inline uint GetSizeByVertex(VertexSize etype)
{
	switch (etype)
	{
	case VertexSize::Color:
		return sizeof(uint);
	case VertexSize::Float1:
		return sizeof(float);
	case VertexSize::Float2:
		return sizeof(float) * 2;
	case VertexSize::Float3:
		return sizeof(float) * 3;
	case VertexSize::Float4:
		return sizeof(float) * 4;
	case VertexSize::Short1:
		return sizeof(short);
	case VertexSize::Short2:
		return sizeof(short) * 2;
	case VertexSize::Short3:
		return sizeof(short) * 3;
	case VertexSize::Short4:
		return sizeof(short) * 4;
	case VertexSize::Byte4:
		return sizeof(byte) * 4;
	}
	return 0;
}
inline bool IsInstanceVertexSemantic(VertexSemantic type)
{
	if (type == VertexSemantic::InstanceOffset)
		return true;
	return false;
}
/********************************************************************/
//雾
struct FogDesc
{
	DECLARE_SERIALIZE(FogDesc);
public:
	bool enable = false;
	FogMode mode = FogMode::Linear;
	Color color = Color::White;
	float start = 0.0f;			//start+end：线性雾
	float end = 0.0f;
	float density = 0.01f;		//指数雾
};
//锁纹理
struct alignas(4) TextureLockDesc
{
	uint level = 0;	//mipmap
	GPUResourceLock flags = GPUResourceLock::Discard;
	uint pitch = 0;//行字节数

	TextureLockDesc() = default;
	TextureLockDesc(uint lv) : level(lv)
	{
	}
	TextureLockDesc(GPUResourceLock f) : flags(f)
	{
	}
	TextureLockDesc(uint lv, GPUResourceLock f) : level(lv), flags(f)
	{
	}
};
// 视口
struct alignas(4) ViewPortDesc
{
	DECLARE_SERIALIZE(ViewPortDesc);
public:
	float	x = 0.0f;
	float	y = 0.0f;
	float	w = 0.0f;
	float	h = 0.0f;
	float   z_near = 0.0f;		//近裁剪面，默认设置0
	float   z_far = 1.0f;		//远裁剪面，默认设置1

public:
	ViewPortDesc() = default;
	ViewPortDesc(float w, float h)
	{
		this->w = w; this->h = h;
	}
	ViewPortDesc(float x, float y, float w, float h)
	{
		this->x = x; this->y = y;
		this->w = w; this->h = h;
	}
	ViewPortDesc(float x, float y, float w, float h, float min_z, float max_z)
	{
		this->x = x; this->y = y;
		this->w = w; this->h = h;
		z_near = min_z; z_far = max_z;
	}
	void Set(float w, float h)
	{
		this->w = w; this->h = h;
	}
	void Set(float x, float y, float w, float h)
	{
		this->x = x; this->y = y;
		this->w = w; this->h = h;
	}
	void Set(float x, float y, float w, float h, float min_z, float max_z)
	{
		this->x = x; this->y = y;
		this->w = w; this->h = h;
		z_near = min_z; z_far = max_z;
	}
};

//VBO
struct alignas(4) VertexBufferDesc
{
	byte stream = 0;
	uint num_vertex = 0;
	GPUResourcePool pool = GPUResourcePool::Default;
	GPUResourceUsage usage = GPUResourceUsage::Static;
	VertexBufferDesc() {}
	VertexBufferDesc(byte index, uint num, GPUResourceUsage s) : stream(index), num_vertex(num), usage(s) {}
	VertexBufferDesc(byte index, uint num, GPUResourcePool p, GPUResourceUsage s) : stream(index), num_vertex(num), pool(p), usage(s) {}
};

//IBO
struct alignas(4) IndexBufferDesc
{
	IndexType index_type = IndexType::B16;
	uint num_index = 0;
	GPUResourcePool pool = GPUResourcePool::Default;
	GPUResourceUsage usage = GPUResourceUsage::Static;
	IndexBufferDesc() {}
	IndexBufferDesc(IndexType type, uint num, GPUResourceUsage s) :index_type(type), num_index(num), usage(s) {}
	IndexBufferDesc(IndexType type, uint num, GPUResourcePool p, GPUResourceUsage s) :index_type(type), num_index(num), pool(p), usage(s) {}
};

//纹理
struct alignas(4) TextureDesc
{
	int				width = 0;
	int				height = 0;
	TextureType		type = TextureType::D2;
	ColorFormat		format = ColorFormat::R8G8B8A8;
	Vector2			scale = Vector2::one;
	Vector2			offset = Vector2::zero;
	TextureAddress	address = TextureAddress::Clamp;
	Color			border = Color::Black;
	TextureFilter	filter = TextureFilter::Bilinear;
	uint			anisotropy = 1;
	byte			antiAlias = 1;	//抗锯齿
	GPUResourceUsage usage = GPUResourceUsage::Static;
	TextureFlag		flags = TextureFlag::NONE;
};

//Primitive数据提交
struct alignas(4) PrimitiveUploadDesc
{
	GPUResourceUsage usage = GPUResourceUsage::Static;
	PrimitiveUploadDesc() {}
	PrimitiveUploadDesc(GPUResourceUsage u) : usage(u) {}
};
DC_END_NAMESPACE


#if 0 //Pool
D3D RUTIME的内存类型，分为3种，VIDEO MEMORY（VM）、AGP MEMORY（AM）和SYSTEM MEMORY（SM），
所有D3D资源都创建在这3种内存之中，在创建资源时，我们可以指定如下存储标志，
D3DPOOL_DEFAULT、D3DPOOL_MANAGED、D3DPOOL_SYSTEMMEM和D3DPOOL_SCRATCH。
VM就是位于显卡上的显存，CPU只能通过AGP或PCI-E总线访问到，读写速度都是非常慢的，
CPU连续写VM稍微快于读，因为CPU写VM时会在CACHE中分配32或64个字节（取决于CACHE LINE长度）
的写缓冲，当缓冲满后会一次性写入VM；SM就是系统内存，CPU读写都非常快，
因为SM是被CACHE到2级缓冲的，但GPU却不能直接访问到系统缓冲，所以创建在SM中的资源，
GPU是不能直接使用的；AM是最麻烦的一个类型，AM实际也存在于系统内存中，
但这部分MEM不会被CPU CACHE，意味着CPU读写AM都会写来个CACHE MISSING然后才通过内存总线访问AM，
所以CPU读写AM相比SM会比较慢，但连续的写会稍微快于读，
原因就是CPU写AM使用了“write combining”，而且GPU可以直接通过AGP或PCI-E总线访问AM。

如果我们使用D3DPOOL_DEFAULT来创建资源，则表示让D3D RUNTIME根据我们指定的资源使用
方法来自动使用存储类型，一般是VM或AM，系统不会在其他地方进行额外备份，当设备丢失后，
这些资源内容也会被丢失掉。但系统并不会在创建的时候使用D3DPOOL_SYSTEMMEM或D3DPOOL_MANAGED
来替换它，注意他们是完全不同的POOL类型，创建到D3DPOOL_DEFAULT中的纹理是不能被CPU LOCK的，
除非是动态纹理。但创建在D3DPOOL_DEFAULT中的VB IB RENDERTARGET BACK BUFFERS可以被LOCK。
当你用D3DPOOL_DEFAULT创建资源时，如果显存已经使用完毕，则托管资源会被换出显存来释放足够的空间。 
D3DPOOL_SYSTEMMEM和D3DPOOL_SCRATCH都是位于SM中的，其差别是使用D3DPOOL_SYSTEMMEM时，
资源格式受限于Device性能，因为资源很可能会被更新到AM或VM中去供图形系统使用，
但SCRATCH只受RUNTIME限制，所以这种资源无法被图形系统使用。 
D3DRUNTIME会优化D3DUSAGE_DYNAMIC 资源，一般将其放置于AM中，但不敢完全保证。
另外为什么静态纹理不能被LOCK，动态纹理却可以，都关系到D3D RUNTIME的设计，
在后面D3DLOCK说明中会叙述。

D3DPOOL_MANAGED表示让D3D RUNTIME来管理资源，被创建的资源会有2份拷贝，一份在SM中，
一份在VM/AM中，创建的时候被放置L在SM，在GPU需要使用资源时D3D RUNTIME自动将数据拷贝到
VM中去，当资源被GPU修改后，RUNTIME在必要时自动将其更新到SM中来，
而在SM中修改后也会被UPDATE到VM去中。所以被CPU或者GPU频发修改的数据，
一定不要使用托管类型，这样会产生非常昂贵的同步负担。当LOST DEVICE发生后，
RESET时RUNTIME会自动利用SM中的COPY来恢复VM中的数据

#endif

#if 0 //Lock
如果LOCK DEFAULT资源会发生什么情况呢？DEFAULT资源可能在VM或AM中，如果在VM中，
必须在系统内容中开辟一个临时缓冲返回给数据，当应用程序将数据填充到临时缓冲后，
UNLOCK的时候，RUNTIME会将临时缓冲的数据传回到VM中去，如果资源D3DUSAGE属性不是WRITEONLY的，
则系统还需要先从VM里拷贝一份原始数据到临时缓冲区，这就是为什么不指定WRITEONLY会降低
程序性能的原因。CPU写AM也有需要注意的地方，因为CPU写AM一般是WRITE COMBINING，
也就是说将写缓冲到一个CACHE LINE上，当CACHE LINE满了之后才FLUSH到AM中去，
第一个要注意的就是写数据必须是WEAK ORDER的（图形数据一般都满足这个要求），
据说D3DRUNTIME和NV DIRVER有点小BUG，就是在CPU没有FLUSH到AM时，GPU就开始绘制相关资源产生
的错误，这时请使用SFENCE等指令FLUSH CACHE LINE。第二请尽量一次写满一个CACHE LINE，
否则会有额外延迟，因为CPU每次必须FLUSH整个CACHE LINE到目标，但如果我们只写了LINE中部分字节，
CPU必须先从AM中读取整个LINE长数据COMBINE后重新FLUSH。第三尽可能顺序写，
随机写会让WRITE COMBINING反而变成累赘，如果是随机写资源，不要使用D3DUSAGE_DYNAMIC创建，
请使用D3DPOOL_MANAGED，这样写会完全在SM中完成。

普通纹理（D3DPOOL_DEFAULT）是不能被锁定的，因为其位于VM中，只能通过UPDATESURFACE和
UPDATETEXTURE来访问，为什么D3D不让我们锁定静态纹理，却让我们锁定静态VB IB呢？我猜测可能
有2个方面的原因，第一就是纹理矩阵一般十分庞大，且纹理在GPU内部已二维方式存储；
第二是纹理在GPU内部是以NATIVE FORMAT方式存储的，并不是明文RGBA格式。动态纹理因为表明
这个纹理需要经常修改，所以D3D会特别存储对待，高频率修改的动态纹理不适合用动态属性创建，
在此分两种情况说明，一种是GPU写入的RENDERTARGET，一种是CPU写入的TEXTURE VIDEO，
我们知道动态资源一般是放置在AM中的，GPU访问AM需要经过AGP/PCI-E总线，速度较VM慢许多，
而CPU访问AM又较SM慢很多，如果资源为动态属性，意味着GPU和CPU访问资源会持续的延迟，
所以此类资源最好以D3DPOOL_DEFAULT和D3DPOOL_SYSTEMMEM各创建一份，自己手动进行双向更新更好。
千万别 RENDERTARGET以D3DPOOL_MANAGED 属性创建，这样效率极低，原因自己分析。
而对于改动不太频繁的资源则推荐使用DEFAULT创建，自己手动更新，因为一次更新的效率损
失远比GPU持续访问AM带来的损失要小。 

不合理的LOCK会严重影响程序性能，因为一般LOCK需要等待COMMAND BUFFER前面的绘制指令全部
执行完毕才能返回，否则很可能修改正在使用的资源，从LOCK返回到修改完毕UNLOCK这段时间GPU
全部处于空闲状态，没有合理使用GPU和CPU的并行性，DX8.0引进了一个新的LOCK标志D3DLOCK_DISCARD，
表示不会读取资源，只会全写资源，这样驱动和RUNTIME配合来了个瞒天过海，立即返回给应用程序
另外块VM地址指针，而原指针在本次UNLOCK之后被丢弃不再使用，这样CPU LOCK无需等待GPU使用
资源完毕，能继续操作图形资源（顶点缓冲和索引缓冲），这技术叫VB IB换名（renaming）。
#endif

/*
OpenGL版本与DirectX版本对应关系
OpenGL			Direct3D
2.1				9.0
3.0-3.1			9.0c
3.2-3.3			10.0
4.2-4.3			11.0
*/
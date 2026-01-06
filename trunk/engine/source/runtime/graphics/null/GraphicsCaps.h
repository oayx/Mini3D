
/*****************************************************************************
* Author： hannibal
* Date：2021年4月5日
* Description： 设备能力
*****************************************************************************/
#pragma once

#include "runtime/graphics/RenderDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GraphicsCaps : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(GraphicsCaps, Object)
	END_REFECTION_TYPE;

public:
	virtual void Initialize() {}
	
	virtual bool IsGenerateMipsSupported(TextureType type, ColorFormat format) { return true; }
	virtual bool CheckTextureFormat(uint usage, ColorFormat format) { return true; }
	virtual bool CheckCubeTextureFormat(uint usage, ColorFormat format) { return true; }
	virtual bool CheckRenderTextureFormat(ColorFormat format) { return true; }
	virtual bool CheckSRGBTextureReadFormat(ColorFormat format) { return true; }
	virtual bool CheckSRGBTextureWriteFormat(ColorFormat format) { return true; }

public:
	int		maxVSyncInterval = 1;		// 同步等待时间
	int		maxLights = 8;				// 灯光数量

	int		maxTextureSize = 1024;
	int		maxCubeMapSize = 512;
	int		maxRenderTextureSize = 128;	// 最大渲染到纹理
	int		maxRenderTargets = 4;		// 渲染到目标数量
	int		maxRenderBufferSize = 2048;	// 渲染目标最大像素

	int		maxTexCoords = 4;			// 纹理坐标数量
	int		maxTexUnits = 4;			// 纹理单元数量
	int		maxTextureRepeat = 4096;	// 纹理平铺次数

	int		maxPrimitiveCount = 1048575;// 最大图元数量
	int		maxVertexCount = 1048575;	// 最大顶点数
	int		maxIndexCount = 1048575;	// 最大索引数
	int		maxVertexAttribs = 16;		// 顶点属性数量
	int		maxVertexShaderUniform = 256;//vertex shader统一变量最大数量(vec4)

	int		maxPointSize = 1;			// 无抗锯齿点大小
	int		maxLineWidth = 1;			// 无抗锯齿线宽

	bool	hasAnisoFilter = false;		// 各项异性
	int		maxAnisoLevel = 1;			// 最大各向异性

	bool	hasAutoMipMapGen = false;	// 自动计算mipmap
	bool	hasMipLevelBias = false;	// mipmap是否可指定bias
	bool	hasMipMaxLevel = false;		// 是否可以指定最大mipmap

	bool	has3DTexture = false;
	bool	hasMipTexture = true;		// mip纹理是否提供
	bool	hasCubeTexture = true;		// 立方体纹理是否提供
	bool	hasCubeMipTexture = true;	// 立方体mip是否提供
	bool	hasRenderToTexture = true;
	bool	hasRenderToCubemap = true;
	bool	hasRenderTo3D = false;

	bool	hasDXT1Texture = false;
	bool	hasDXT3Texture = false;
	bool	hasDXT5Texture = false;
	bool	hasETC1Texture = false;
	bool	hasETC2Texture = false;
	bool	hasPVRTCTexture = false;
	bool	hasASTCTexture = false;

	bool	hasStencil = true;
	bool	hasMultiSample = true;
	bool    hasTimerQuery = true;
	bool	hasFrameBufferBlit = false;	// Framebuffer blit
	bool	hasMapbuffer = false;
	bool	hasMapbufferRange = false;
	bool	hasSRGBReadWrite = false;
	bool	hasInstancing = false;

	bool	hasGeometryShader = false;
	bool	hasHullShader = false;
	bool	hasDomainShader = false;
	bool	hasComputeShader = false;

	bool	useProgramBinary = false;	//使用编译后的shader
};
DC_END_NAMESPACE
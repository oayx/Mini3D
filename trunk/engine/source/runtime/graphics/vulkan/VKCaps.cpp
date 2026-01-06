#include "VKCaps.h"
#include "VKDevice.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(VKCaps, GraphicsCaps);
void VKCaps::Initialize()
{
	maxVSyncInterval = 4;
	maxLights = 8;

	// 纹理大小
	maxTextureSize = 16384;
	maxCubeMapSize = 16384;
	maxRenderTextureSize = maxTextureSize;
	maxRenderTargets = kMaxSupportedRenderTargets;

	maxTexUnits = kMaxSupportedTextureUnits;
	maxTexCoords = 8;

	maxPrimitiveCount = 1048575;
	maxVertexCount = 1048575;
	maxIndexCount = 1048575;

	hasAnisoFilter = true;
	maxAnisoLevel = 16;

	hasAutoMipMapGen = true;
	hasMipLevelBias = true;

	has3DTexture = true;
	hasMipTexture = true;
	hasCubeTexture = true;
	hasRenderToTexture = true;
	hasRenderToCubemap = true;
	hasRenderTo3D = true;
	hasStencil = true;

	hasMultiSample = true;
	hasSRGBReadWrite = true;

	hasComputeShader = true;
	hasInstancing = true;

	hasDXT1Texture = false;
	hasDXT3Texture = false;
	hasDXT5Texture = false;
}
DC_END_NAMESPACE
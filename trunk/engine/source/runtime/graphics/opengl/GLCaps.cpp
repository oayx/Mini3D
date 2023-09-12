#include "GLCaps.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLCaps, GraphicsCaps);
static bool FindGLExtension(const char* extensions, const char* ext)
{
	// Extension names should not have spaces, be NULL or empty
	if (!ext || ext[0] == 0)
		return false;
	if (strchr(ext, ' '))
		return false;

	AssertEx(extensions != NULL, "");
	if (!extensions)
		return false;

	const char* start = extensions;
	for (;;)
	{
		const char* where = strstr(start, ext);
		if (!where)
			break;
		const char* terminator = where + strlen(ext);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		}
		start = terminator;
	}
	return false;
}
bool GL_QueryExtension(const char* ext)
{
	static const char* extensions = NULL;
	if (!extensions)
	{
		extensions = (const char*)glGetString(GL_EXTENSIONS);
		Debuger::Log("%s", extensions);
	}
	return FindGLExtension(extensions, ext);
}
void GLCaps::Initialize()
{
	const char* rendererString = (const char*)glGetString(GL_RENDERER);
	const char* vendorString = (const char*)glGetString(GL_VENDOR);
	const char* version = (const char*)glGetString(GL_VERSION);
	const char* shader_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
	Debuger::Log("GL_RENDERER:%s", rendererString);
	Debuger::Log("GL_VENDOR:%s", vendorString);
	Debuger::Log("GL_VERSION:%s", version);
	Debuger::Log("GL_SHADING_LANGUAGE_VERSION:%s", shader_version);
	Debuger::Log("GL_EXTENSIONS:%s", extensions);

#if defined(GL_MAX_LIGHTS)
	glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapSize);
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderTextureSize);
#if defined(GL_MAX_DRAW_BUFFERS)
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxRenderTargets);
#endif
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize);

#if defined(GL_MAX_TEXTURE_COORDS)
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTexCoords);
#endif
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnits);

#if defined(GL_MAX_ELEMENTS_VERTICES)
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount);
#endif
#if defined(GL_MAX_ELEMENTS_INDICES)
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount);
#endif
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	maxPrimitiveCount = maxVertexCount;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexShaderUniform);

	Size size; 
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, size.p); //两个字段分别为【最小，最大】
	maxPointSize = size.height;
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, size.p);
	maxLineWidth = size.height;

	hasAnisoFilter = GL_QueryExtension("GL_EXT_texture_filter_anisotropic");
#if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	if (hasAnisoFilter)
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint *)&maxAnisoLevel);
#endif

	hasAutoMipMapGen = GL_QueryExtension("GL_SGIS_generate_mipmap");

	has3DTexture = GL_QueryExtension("GL_EXT_texture3D");
	hasRenderToTexture = GL_QueryExtension("GL_EXT_framebuffer_object");
	hasRenderToCubemap = hasRenderToTexture;
	hasRenderTo3D = has3DTexture && hasRenderToTexture;

	hasDXT1Texture = GL_QueryExtension("GL_EXT_texture_compression_s3tc") || GL_QueryExtension("GL_EXT_texture_compression_dxt1");
	hasDXT3Texture = GL_QueryExtension("GL_EXT_texture_compression_s3tc") || GL_QueryExtension("GL_EXT_texture_compression_dxt3");
	hasDXT5Texture = GL_QueryExtension("GL_EXT_texture_compression_s3tc") || GL_QueryExtension("GL_EXT_texture_compression_dxt5");
	hasPVRTCTexture = GL_QueryExtension("GL_IMG_texture_compression_pvrtc");
	hasASTCTexture = GL_QueryExtension("GL_KHR_texture_compression_astc_ldr");
#if defined(DC_GRAPHICS_API_OPENGLES3)
	hasETC1Texture = true;
	hasETC2Texture = true;
#else
	hasETC1Texture = GL_QueryExtension("GL_OES_compressed_ETC1_RGB8_texture");
	hasETC2Texture = GL_QueryExtension("GL_OES_compressed_ETC2_RGB8_texture");
#endif
	Debuger::Log("hasDXT1Texture:%d,hasDXT3Texture:%d,hasDXT5Texture:%d,hasETC1Texture:%d,hasETC2Texture:%d,hasPVRTCTexture:%d,hasASTCTexture:%d",
		hasDXT1Texture, hasDXT3Texture, hasDXT5Texture, hasETC1Texture, hasETC2Texture, hasPVRTCTexture, hasASTCTexture);

	hasMultiSample = GL_QueryExtension("GL_ARB_multisample");
	hasMultiSample &= GL_QueryExtension("GL_EXT_framebuffer_multisample");
	hasMapbuffer = GL_QueryExtension("GL_OES_mapbuffer");
	hasMapbufferRange = GL_QueryExtension("GL_EXT_map_buffer_range");
	hasFrameBufferBlit = GL_QueryExtension("GL_EXT_framebuffer_blit");
	Debuger::Log("hasMapbuffer:%d,hasMapbufferRange:%d,hasFrameBufferBlit:%d", hasMapbuffer, hasMapbufferRange, hasFrameBufferBlit);

	hasGeometryShader = true;
	hasHullShader = true;
	hasDomainShader = true;
	hasComputeShader = true;
	hasInstancing = true;

#if !defined(DC_DEBUG)
	int binFormatCount = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &binFormatCount);
	useProgramBinary = binFormatCount > 0;
#endif
}
DC_END_NAMESPACE
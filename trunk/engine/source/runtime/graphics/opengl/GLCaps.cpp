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
bool GLCaps::QueryExtension(const char* ext)
{
	return _extensions.Contains(ext);
}
void GLCaps::Initialize()
{
	const char* rendererString = (const char*)glGetString(GL_RENDERER); GL_CHECK_ERROR();
	const char* vendorString = (const char*)glGetString(GL_VENDOR); GL_CHECK_ERROR();
	const char* version = (const char*)glGetString(GL_VERSION); GL_CHECK_ERROR();
	const char* shader_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION); GL_CHECK_ERROR();
	Debuger::Log("GL_RENDERER:%s", rendererString);
	Debuger::Log("GL_VENDOR:%s", vendorString);
	Debuger::Log("GL_VERSION:%s", version);
	Debuger::Log("GL_SHADING_LANGUAGE_VERSION:%s", shader_version);

	GLint numExtensions = 0;
	GL_ERROR(glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions));
	Debuger::Log("GL_EXTENSIONS nums:%d", numExtensions);
	for (GLint i = 0; i < numExtensions; i++) 
	{
		const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i); GL_CHECK_ERROR();
		_extensions.Add(extension);
		Debuger::Log("------Extension:%d : %s", i, extension);
	}

#if defined(GL_MAX_LIGHTS) && !defined(DC_PLATFORM_MAC)
	GL_ERROR(glGetIntegerv(GL_MAX_LIGHTS, &maxLights));
#endif

	GL_ERROR(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));
	GL_ERROR(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapSize));
	GL_ERROR(glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderTextureSize));
#if defined(GL_MAX_DRAW_BUFFERS)
	GL_ERROR(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxRenderTargets));
#endif
	GL_ERROR(glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize));

#if defined(GL_MAX_TEXTURE_COORDS) && !defined(DC_PLATFORM_MAC)
	GL_ERROR(glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTexCoords));
#endif
	GL_ERROR(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexUnits));

#if defined(GL_MAX_ELEMENTS_VERTICES)
	GL_ERROR(glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVertexCount));
#endif
#if defined(GL_MAX_ELEMENTS_INDICES)
	GL_ERROR(glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount));
#endif
	GL_ERROR(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs));
	maxPrimitiveCount = maxVertexCount;
	GL_ERROR(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexShaderUniform));

#if !defined(DC_PLATFORM_MAC)
	Size size; 
	GL_ERROR(glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, size.p)); //两个字段分别为【最小，最大】
	maxPointSize = size.height;
	GL_ERROR(glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, size.p));
	maxLineWidth = size.height;
#endif

	hasAnisoFilter = QueryExtension("GL_EXT_texture_filter_anisotropic");
#if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
	if (hasAnisoFilter)
		GL_ERROR(glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint *)&maxAnisoLevel));
#endif

	hasAutoMipMapGen = QueryExtension("GL_SGIS_generate_mipmap");

	has3DTexture = QueryExtension("GL_EXT_texture3D");
	hasRenderToTexture = QueryExtension("GL_EXT_framebuffer_object");
	hasRenderToCubemap = hasRenderToTexture;
	hasRenderTo3D = has3DTexture && hasRenderToTexture;

	hasDXT1Texture = QueryExtension("GL_EXT_texture_compression_s3tc") || QueryExtension("GL_EXT_texture_compression_dxt1");
	hasDXT3Texture = QueryExtension("GL_EXT_texture_compression_s3tc") || QueryExtension("GL_EXT_texture_compression_dxt3");
	hasDXT5Texture = QueryExtension("GL_EXT_texture_compression_s3tc") || QueryExtension("GL_EXT_texture_compression_dxt5");
	hasPVRTCTexture = QueryExtension("GL_IMG_texture_compression_pvrtc");
	hasASTCTexture = QueryExtension("GL_KHR_texture_compression_astc_ldr");
#if defined(DC_GRAPHICS_API_OPENGLES3)
	hasETC1Texture = true;
	hasETC2Texture = true;
#else
	hasETC1Texture = QueryExtension("GL_OES_compressed_ETC1_RGB8_texture");
	hasETC2Texture = QueryExtension("GL_OES_compressed_ETC2_RGB8_texture");
#endif
	Debuger::Log("hasDXT1Texture:%d,hasDXT3Texture:%d,hasDXT5Texture:%d,hasETC1Texture:%d,hasETC2Texture:%d,hasPVRTCTexture:%d,hasASTCTexture:%d",
		hasDXT1Texture, hasDXT3Texture, hasDXT5Texture, hasETC1Texture, hasETC2Texture, hasPVRTCTexture, hasASTCTexture);

	hasMultiSample = QueryExtension("GL_ARB_multisample");
	hasMultiSample &= QueryExtension("GL_EXT_framebuffer_multisample");
	hasMapbuffer = QueryExtension("GL_OES_mapbuffer");
	hasMapbufferRange = QueryExtension("GL_EXT_map_buffer_range");
	hasFrameBufferBlit = QueryExtension("GL_EXT_framebuffer_blit");
	Debuger::Log("hasMapbuffer:%d,hasMapbufferRange:%d,hasFrameBufferBlit:%d", hasMapbuffer, hasMapbufferRange, hasFrameBufferBlit);

	hasGeometryShader = true;
	hasHullShader = true;
	hasDomainShader = true;
	hasComputeShader = true;
	hasInstancing = true;

#if !defined(DC_DEBUG)
	int binFormatCount = 0;
	GL_ERROR(glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &binFormatCount));
	useProgramBinary = binFormatCount > 0;
#endif
}
DC_END_NAMESPACE
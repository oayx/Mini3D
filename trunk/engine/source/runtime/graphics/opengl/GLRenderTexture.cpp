#include "GLRenderTexture.h"
#include "core/image/Image.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
// 验证是否准备就绪
static void GL_CheckFramebuffer()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_UNDEFINED:
			Debuger::Error("glCheckFramebufferStatus error:GL_FRAMEBUFFER_UNDEFINED");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			Debuger::Error("glCheckFramebufferStatus error:GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			Debuger::Error("glCheckFramebufferStatus error:GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			Debuger::Error("glCheckFramebufferStatus error:GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		default:
			Debuger::Error("glCheckFramebufferStatus error:%d", status);
			break;
		}
	}
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLRenderTexture, RenderTexture);
GLRenderTexture::GLRenderTexture(const TextureDesc& desc)
: base(desc)
{
	// 创建FBO对象
	GL_ERROR(glGenFramebuffers(1, &_frameBuffer));
	GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer));

	if (_flags & TextureFlag::COLOR)
	{
		this->CreateRenderTarget();
	}
	if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		this->CreateDepthStencil();
	}
	//立方体在使用时才会赋予纹理，这里先不验证
	if (_textureType != TextureType::Cube)
	{
		GL_CheckFramebuffer();
	}
	GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
GLRenderTexture::~GLRenderTexture()
{
	if (_colorTexture > 0)
	{
		GL_ERROR(glDeleteTextures(1, &_colorTexture));
		_colorTexture = 0;
	}
	if (_depthTexture > 0)
	{
		GL_ERROR(glDeleteTextures(1, &_depthTexture));
		_depthTexture = 0;
	}
	if (_frameBuffer > 0)
	{
		GL_ERROR(glDeleteFramebuffers(1, &_frameBuffer));
		_frameBuffer = 0;
	}
}
void GLRenderTexture::PreRender()
{
	GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer));
}
void GLRenderTexture::BeginFrame(RenderFrameDesc& desc)
{
	AssertEx(desc.face < (_textureType == TextureType::Cube ? 6 : 1), "");

	if (_flags & TextureFlag::COLOR && _textureType == TextureType::Cube)
	{
		const GLenum attachments = GL_GetColorAttachments(_flags);
		const GLuint texture_type = GL_GetTextureType(_textureType, _antiAlias > 1);
		GL_ERROR(glBindTexture(texture_type, _colorTexture));
		const GLenum tex_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + desc.face;
		//把纹理附加到帧缓冲
		GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, attachments, tex_target, _colorTexture, 0));
		GL_ERROR(glBindTexture(texture_type, 0));
	}
	desc.view_port.Set((float)_imageWidth, (float)_imageHeight);
	base::BeginFrame(desc);
}
void GLRenderTexture::PostRender()
{
	//绑定到名称0将会解除当前绑定的FBO，并把渲染重新定向到窗口的帧缓冲区
	GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
bool GLRenderTexture::GetData(MemoryDataStream& stream)
{
	AssertEx(_flags & TextureFlag::COLOR, "");//TODO:如果读取深度缓存，会报错
	if (_textureType != TextureType::D2)
		return false;

	stream.Resize(this->GetBytes());
#if defined(GL_READ_FRAMEBUFFER)
	GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, _frameBuffer));
	{//每次需要重新读取
		GLint read_type, read_format;
		GL_ERROR(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &read_type));
		GL_ERROR(glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &read_format));
		GL_ERROR(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0));
		GL_ERROR(glReadBuffer(GL_COLOR_ATTACHMENT0));
		GL_ERROR(glReadPixels(0, 0, this->GetWidth(), this->GetHeight(), read_format, read_type, stream.data()));
	}
	GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	return true;
#else
	return false;
#endif
}
bool GLRenderTexture::Copy(Texture* dst)
{
	AssertEx(_flags & TextureFlag::COLOR, "");//TODO:如果读取深度缓存，会报错
	AssertEx(dst->GetWidth() == this->GetWidth(), "");
	AssertEx(dst->GetHeight() == this->GetHeight(), "");
	AssertEx(dst->GetFormat() == this->GetFormat(), "");

#if defined(GL_READ_FRAMEBUFFER)
	GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, _frameBuffer));
	{
		const GLuint texture_type = GL_GetTextureType(_textureType, _antiAlias > 1);
		GLenum internal_format = GL_GetInternalFormat(_imageFormat, false);
		GL_ERROR(glReadBuffer(GL_COLOR_ATTACHMENT0));
		GL_ERROR(glBindTexture(texture_type, *((GLuint*)dst->GetTexture())));
		GL_ERROR(glCopyTexImage2D(texture_type, 0, internal_format, 0, 0, _imageWidth, _imageHeight, 0));
		GL_ERROR(glBindTexture(texture_type, 0));
	}
	GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	return true;
#else
	return false;
#endif
}
void* GLRenderTexture::GetTexture()
{
	if (_flags & TextureFlag::COLOR && (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL))
	{
		return &_colorTexture;
	}
	else if (_flags & TextureFlag::COLOR)
	{
		return &_colorTexture;
	}
	else if (_flags & TextureFlag::DEPTH || _flags & TextureFlag::STENCIL)
	{
		return &_depthTexture;
	}
	return &_colorTexture;
}
void GLRenderTexture::SaveToFile(const String& name, ImageType type)
{
	Image* image = Image::Create(_imageFormat, iSize(_imageWidth, _imageHeight), false);
	MemoryDataStream stream;
	if (this->GetData(stream) && image->GetSize() == stream.Size())
	{
		image->Fill(stream.data(), stream.Size());
		image->SaveToFile(name);
	}
}
/*纹理附件
当把一个纹理附加到帧缓冲上的时候，所有渲染命令会写入到纹理上，就像它是一个普通的颜色/深度或者模板缓冲一样。
使用纹理的好处是，所有渲染操作的结果都会被储存为一个纹理图像，这样我们就可以简单的在着色器中使用了。
*/
void GLRenderTexture::CreateRenderTarget()
{
	GLuint texture_type = GL_GetTextureType(_textureType, _antiAlias > 1);
	GL_ERROR(glGenTextures(1, &_colorTexture));
	GL_ERROR(glBindTexture(texture_type, _colorTexture));
	GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, GL_GetMagTextureFilter(_filterType)));
	GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, GL_GetMinTextureFilter(_filterType)));
	GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, GL_GetTextureAddress(_addressMode)));
	GL_ERROR(glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, GL_GetTextureAddress(_addressMode)));
	{
		GLenum color_format = GL_GetTextureFormat(_imageFormat);
		GLenum internal_format = GL_GetInternalFormat(_imageFormat, false);
		GLenum format_type = GL_GetTextureFormatType(_imageFormat);
		if (_textureType == TextureType::Cube)
		{
			for (int i = 0; i < 6; ++i)
			{
				GL_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, _imageWidth, _imageHeight, 0, color_format, format_type, NULL));
			}
		}
		else
		{
			//if (_antiAlias > 1)
			//{
			//	GL_ERROR(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _antiAlias, internal_format, _imageWidth, _imageHeight, GL_TRUE));
			//}
			//else
			{
				GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, _imageWidth, _imageHeight, 0, color_format, format_type, NULL));
			}
		}
	}
	if (_enableMips)
	{
		GL_ERROR(glGenerateMipmap(_colorTexture));
	}
	if(_textureType != TextureType::Cube)
	{//立方体需要实时绑定
		const GLenum tex_target = GL_TEXTURE_2D;
		//把纹理附加到帧缓冲
		GLenum attachment_type = GL_GetColorAttachments(_flags);
		GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, tex_target, _colorTexture, 0));
	}
	GL_ERROR(glBindTexture(texture_type, 0));
}
void GLRenderTexture::CreateDepthStencil()
{
	GL_ERROR(glGenTextures(1, &_depthTexture));
	GL_ERROR(glBindTexture(GL_TEXTURE_2D, _depthTexture));
	GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_GetMagTextureFilter(_filterType)));
	GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_GetMinTextureFilter(_filterType)));
	GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_GetTextureAddress(_addressMode)));
	GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_GetTextureAddress(_addressMode)));
	
	if (_flags & TextureFlag::ShadowMap)
	{
		//linux 下会出现不显示阴影问题  && (defined(DC_PLATFORM_WIN32) || defined(DC_PLATFORM_ANDROID))
		//百分比渐进过滤(PCF)，可以控制纹理过滤发生在深度比较之后执行
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
	}
	{
		GLenum internal_format = GL_GetInternalDepthFormat(_imageFormat, _flags);
		GLenum texture_format = GL_GetTextureDepthFormat(_imageFormat, _flags);
		GLenum type_format = GL_GetTextureDepthFormatType(_imageFormat, _flags);
		GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, _imageWidth, _imageHeight, 0, texture_format, type_format, nullptr));
	}
	{
		GLenum attachment_type = GL_GetDepthAttachments(_flags);
		GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, _depthTexture, 0));
	}
	GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
}
DC_END_NAMESPACE
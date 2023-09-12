#include "GLDevice.h"
#include "GLCaps.h"
#include "GLHardwareVertexBuffer.h"
#include "GLHardwareIndexBuffer.h"
#include "GLPostProcess.h"
#include "GLRenderContent.h"
#include "GLTexture.h"
#include "GLRenderTexture.h"
#include "GLProgram.h"
#include "runtime/Application.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLDevice, GraphicsDevice);
GLDevice* GetGLDevice()
{
	return (GLDevice*)Application::GetGraphics();
}
GLDevice::GLDevice()
{
	_graphicsCaps = GLCaps::Create();
}
GLDevice::~GLDevice()
{
}
bool GLDevice::CreateDevice(RenderWindow* window)
{
	_graphicsCaps->Initialize();

#if defined(DC_GRAPHICS_API_OPENGLES3)
	char const* version = (char const*)glGetString(GL_VERSION);
	if (version != nullptr)
	{
		Debuger::Log("gl version:%s", version);

		std::string ver_str = version;
		if (ver_str.find("OpenGL ES 1.0") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_10;
		}
		else if (ver_str.find("OpenGL ES 1.1") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_11;
		}
		else if (ver_str.find("OpenGL ES 2.0") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_20; 
			_shaderVersion = "100";
		}
		else if (ver_str.find("OpenGL ES 3.0") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_30;
			_shaderVersion = "300";
		}
		else if (ver_str.find("OpenGL ES 3.1") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_31;
			_shaderVersion = "310";
		}
		else if (ver_str.find("OpenGL ES 3.2") != std::string::npos)
		{
			_shaderModel = GLShaderModel::SM_32;
			_shaderVersion = "320";
		}
	}
	else
	{
#if defined(DC_PLATFORM_WIN32)
		_shaderModel = GLShaderModel::SM_30;
		_shaderVersion = "300";
#endif
	}
#elif defined(DC_GRAPHICS_API_OPENGL)
	GLint major, minor;
	GL_ERROR(glGetIntegerv(GL_MAJOR_VERSION, &major));
	GL_ERROR(glGetIntegerv(GL_MINOR_VERSION, &minor));
	switch (major)
	{
	case 1:
		switch (minor)
		{
		case 0:_shaderModel = GLShaderModel::SM_10; break;
		case 1:_shaderModel = GLShaderModel::SM_11; break;
		case 2:_shaderModel = GLShaderModel::SM_12; break;
		case 3:_shaderModel = GLShaderModel::SM_13; break;
		case 4:_shaderModel = GLShaderModel::SM_14; break;
		default:_shaderModel = GLShaderModel::SM_15; break;
		}
		break;
	case 2:
		switch (minor)
		{
		case 0:_shaderModel = GLShaderModel::SM_20; 
			_shaderVersion = "110";
			break;
		default:_shaderModel = GLShaderModel::SM_21;
			_shaderVersion = "120";
			break;
		}
		break;
	case 3:
		switch (minor)
		{
		case 0:_shaderModel = GLShaderModel::SM_30;
			_shaderVersion = "130";
			break;
		case 1:_shaderModel = GLShaderModel::SM_31; 
			_shaderVersion = "140";
			break;
		case 2:_shaderModel = GLShaderModel::SM_32; 
			_shaderVersion = "150";
			break;
		default:_shaderModel = GLShaderModel::SM_33; 
			_shaderVersion = "330";
			break;
		}
		break;
	case 4:
		switch (minor)
		{
		case 0:
			_shaderModel = GLShaderModel::SM_40; 
			_shaderVersion = "400";
			break;
		case 1:
			_shaderModel = GLShaderModel::SM_41; 
			_shaderVersion = "410";
			break;
		case 2:
			_shaderModel = GLShaderModel::SM_42; 
			_shaderVersion = "420";
			break;
		case 3:
			_shaderModel = GLShaderModel::SM_43; 
			_shaderVersion = "430";
			break;
		case 4:
			_shaderModel = GLShaderModel::SM_44; 
			_shaderVersion = "440";
			break;
		case 5:
			_shaderModel = GLShaderModel::SM_45; 
			_shaderVersion = "450";
			break;
		default:
			_shaderModel = GLShaderModel::SM_46; 
			_shaderVersion = "460";
			break;
		}
		break;
	case 5:
		switch (minor)
		{
		case 0:
			_shaderModel = GLShaderModel::SM_50; 
			_shaderVersion = "500";
			break;
		default:
			_shaderModel = GLShaderModel::SM_50; 
			_shaderVersion = "500";
			break;
		}
		break;
	}
#endif
	Debuger::Log("gl shader model:%d", (int)_shaderModel);

	std::string glsl_version = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (!glsl_version.empty())
	{
		Debuger::Log("shader version:%s", glsl_version.c_str());
	}
	else
	{
		AssertEx(false, "Get glsl shader version error");
	}

	//由于OpenGL默认是逆时针为正面，与D3D相反(顺时针)，这里修改OpenGL正面的定义
	GL_ERROR(glFrontFace(GL_CW));
	//纹理数据对齐方式
	GL_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

	//开启MSAA
#if defined(DC_GRAPHICS_API_OPENGL)
	if (_graphicsCaps->hasMultiSample)
	{
		GL_ERROR(glEnable(GL_MULTISAMPLE));
	}
#endif

	return true;
}
bool GLDevice::CreateSwapChain(RenderWindow* window)
{
	CHECK_RETURN_PTR_FALSE(window);
	GLSwapChain* swap_chain = GLSwapChain::Create();
	swap_chain->CreateSwapChain(window);
	_swapChains[int(window->GetTargetDisplay())] = swap_chain;
	return true;
}
void GLDevice::CreateRenderContent()
{
	RenderContent* content = GLRenderContent::Create();
	content->Initialize();
	_renderContent = content;
}
void GLDevice::Resize(const WindowResizeDesc& desc)
{
	base::Resize(desc);
}
HardwareVertexBuffer* GLDevice::CreateVertexBuffer()
{
	return GLHardwareVertexBuffer::Create();
}
HardwareIndexBuffer* GLDevice::CreateIndexBuffer()
{
	return GLHardwareIndexBuffer::Create();
}
CGProgram* GLDevice::CreateShader()
{
	return GLProgram::Create();
}
Texture* GLDevice::CreateTexture(const TextureDesc& desc)
{
	return GLTexture::Create(desc);
}
RenderTexture* GLDevice::CreateRenderTexture(const TextureDesc& desc)
{
	return GLRenderTexture::Create(desc);
}
FinalProcess* GLDevice::CreateFinalProcess()
{
	return GLFinalProcess::Create();
}
DC_END_NAMESPACE
#include "GLSwapChain.h"
#include "GLDevice.h"
#include "runtime/graphics/null/Texture.h"
#include "runtime/Application.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLSwapChain, SwapChain);
GLSwapChain::GLSwapChain()
{

}
GLSwapChain::~GLSwapChain()
{
}
bool GLSwapChain::CreateSwapChain(RenderWindow* window) 
{ 
	_msaa.Count = window->GetAntiAlias();
//	if (_msaa.Count > 1)
//	{
//		glEnable(GL_MULTISAMPLE);
//	}
//	else
//	{
//		glDisable(GL_MULTISAMPLE);
//	}
	return base::CreateSwapChain(window);
}
void GLSwapChain::Resize(const WindowResizeDesc& desc)
{ 
	base::Resize(desc);
}
void GLSwapChain::BeginFrame(RenderFrameDesc& desc)
{
	GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
//#if defined(DC_GRAPHICS_API_OPENGL) || defined(DC_GRAPHICS_API_OPENGLES3)
//	const GLenum draw_attachments = GL_BACK;
//	GL_ERROR(glDrawBuffers(1, &draw_attachments));
//#endif
	base::BeginFrame(desc);
}
void GLSwapChain::Present(uint sync)
{
	GL_ERROR(glFlush());
}
bool GLSwapChain::Copy(Texture* texture)const
{
	GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	GL_ERROR(glReadBuffer(GL_BACK));

	TextureLockDesc lock_desc;
	byte* by = texture->Lock(lock_desc);
	//第一、二个参数表示了矩形的左下角横、纵坐标，坐标以窗口最左下角为零，右上角为最大值；第三、四个参数表示了矩形的宽度和高度
	//glReadPixels实际上是从缓冲区中读取数据，如果使用了双缓冲区，则默认是从正在显示的缓冲（即前缓冲）中读取，而绘制工作是默认绘制到后缓冲区的。
	GL_ERROR(glReadPixels(0, 0, texture->GetWidth(), texture->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, by));
	{//上下颠倒
		int row_pitch = texture->GetPitch();
		int total_size = texture->GetBytes();
		byte* new_by = NewArray<byte>(total_size);
		Memory::Copy(new_by, by, total_size);//保证by指针不变
		for (int row = 0; row < (int)texture->GetHeight(); ++row)
		{
			Memory::Copy(by + row_pitch * row, new_by + row_pitch * ((int)texture->GetHeight() - row - 1), row_pitch);
		}
		DeleteArray(new_by);
	}
	texture->Unlock(lock_desc);
	return true;
}
DC_END_NAMESPACE
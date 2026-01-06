#include "GLHardwareIndexBuffer.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLHardwareIndexBuffer, HardwareIndexBuffer);
GLHardwareIndexBuffer::GLHardwareIndexBuffer()
{
	GL_ERROR(glGenBuffers(1, &_nBufferId));
}
GLHardwareIndexBuffer::~GLHardwareIndexBuffer()
{
	if (_nBufferId != 0)
	{
		GL_ERROR(glDeleteBuffers(1, &_nBufferId));
		_nBufferId = 0;
	}
}
void* GLHardwareIndexBuffer::Lock(const IndexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (_indexCount == 0)
		return nullptr;
	
	if(rebuild)
	{
		DWORD gl_usage = 0;
		switch (_usage)
		{
		case GPUResourceUsage::Staging:gl_usage = GL_STATIC_DRAW; break;
		case GPUResourceUsage::Dynamic:gl_usage = GL_DYNAMIC_DRAW; break;
		case GPUResourceUsage::Static:gl_usage = GL_STATIC_DRAW; break;
		}
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _nBufferId));
		GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->GetBufferCapacity(), nullptr, gl_usage));//为现在绑定到target上面的缓冲区对象，创建一个新的存储，将会删除之前存在的所有数据存储
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	if (!_bufferData || rebuild)
	{
		Memory::DeleteArray(_bufferData);
		_bufferData = Memory::NewArray<byte>(this->GetBufferCapacity());
		::memset(_bufferData, 0, this->GetBufferCapacity());
	}
	return _bufferData;
}
void  GLHardwareIndexBuffer::Unlock()
{
	base::Unlock();
	if (_indexCount == 0)
		return;

	GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _nBufferId));
	GL_ERROR(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, this->GetBufferSize(), _bufferData));
	GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
void GLHardwareIndexBuffer::Render()
{
	GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _nBufferId));
}
DC_END_NAMESPACE
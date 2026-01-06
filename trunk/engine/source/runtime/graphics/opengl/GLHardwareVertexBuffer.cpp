#include "GLHardwareVertexBuffer.h"
#include "GLProgram.h"
#include "runtime/graphics/null/GraphicsDevice.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLHardwareVertexBuffer, HardwareVertexBuffer);
GLHardwareVertexBuffer::GLHardwareVertexBuffer()
{
}
GLHardwareVertexBuffer::~GLHardwareVertexBuffer()
{
	for (int i = 0; i < MAX_STREAM_COUNT; ++i)
	{
		if (_nBufferId[i] != 0)
		{
			GL_ERROR(glDeleteBuffers(1, &_nBufferId[i]));
			_nBufferId[i] = 0;
		}
	}
	if (_vao != 0)
	{
		GL_ERROR(glDeleteVertexArrays(1, &_vao));
		_vao = 0;
	}
}
void* GLHardwareVertexBuffer::Lock(const VertexBufferDesc& desc)
{
	bool rebuild = this->NeedRebuild(desc);

	base::Lock(desc);
	if (desc.num_vertex == 0)
		return nullptr;

	if (_nBufferId[desc.stream] == 0)
	{
		GL_ERROR(glGenBuffers(1, &_nBufferId[desc.stream]));
	}

	if (rebuild && _vao != 0)
	{
		GL_ERROR(glDeleteVertexArrays(1, &_vao));
		_vao = 0;
	}

	if (rebuild)
	{
		//Usage只是对OpengGL的一个提示，而不是保证
		DWORD gl_usage = 0;
		switch (_usage)
		{
		case GPUResourceUsage::Staging:gl_usage = GL_STATIC_DRAW; break;
		case GPUResourceUsage::Dynamic:gl_usage = GL_DYNAMIC_DRAW; break;
		case GPUResourceUsage::Static:gl_usage = GL_STATIC_DRAW; break;
		}
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _nBufferId[desc.stream]));
		GL_ERROR(glBufferData(GL_ARRAY_BUFFER, this->GetBufferCapacity(desc.stream), nullptr, gl_usage));//分配内存,再次调用可以修改内存大小，旧内存销毁；与glBufferStorage区别是不可修改大小
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	if (!_bufferData[desc.stream] || rebuild)
	{
		Memory::DeleteArray(_bufferData[desc.stream]);
		_bufferData[desc.stream] = Memory::NewArray<float>(this->GetBufferCapacity(desc.stream));
		::memset(_bufferData[desc.stream], 0, this->GetBufferCapacity(desc.stream));
	}
	return _bufferData[desc.stream];
}
void  GLHardwareVertexBuffer::Unlock(const VertexBufferDesc& desc)
{
	base::Unlock(desc);
	if (desc.num_vertex == 0)
		return;
	
	GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _nBufferId[desc.stream]));
	GL_ERROR(glBufferSubData(GL_ARRAY_BUFFER, 0, this->GetBufferSize(desc.stream), _bufferData[desc.stream]));//用来更新一个已有缓冲区对象中的一部分数据
	GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
void GLHardwareVertexBuffer::Render(CGProgram* shader)
{
	DC_PROFILE_FUNCTION;
	GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _nBufferId[0]));
	this->BuildInputLayout(shader);
	GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
void GLHardwareVertexBuffer::Clear()
{
	base::Clear();
	if (_vao != 0)
	{
		GL_ERROR(glDeleteVertexArrays(1, &_vao));
		_vao = 0;
	}
}
/*
	glVertexAttribPointer 指定了渲染时索引值为 index 的顶点属性数组的数据格式和位置。
	void glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,const GLvoid * pointer);
	参数：
	(1)index
	指定要修改的顶点属性的索引值
	(2)size
	指定每个顶点属性的组件数量。必须为1、2、3或者4。初始值为4。（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a））
	(4)type
	指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
	(5)normalized
	指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）。
	(6)stride
	指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0。
	(7)pointer
	指定第一个组件在数组的第一个顶点属性中的偏移量。该数组与GL_ARRAY_BUFFER绑定，储存于缓冲区中。初始值为0；
*/
void GLHardwareVertexBuffer::BuildInputLayout(CGProgram* shader)
{
	AssertEx((int)this->GetElementCount(0) <= GetGraphicsCaps()->maxVertexAttribs, "");

	if (_vao == 0 || IsVertexSemanticChange(shader))
	{
		_shaderVertexSemantic = shader->GetInputVertexSemantic();
		GL_ERROR(glGenVertexArrays(1, &_vao));
		GL_ERROR(glBindVertexArray(_vao));

		for (GLuint i = 0; i < 16; ++i)
		{
			GL_ERROR(glDisableVertexAttribArray(i));
		}

		int index = 0;
		{
			GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _nBufferId[0]));
			GLuint total_size = this->GetVertexSize(0);
			for (byte i = 0; i < this->GetElementCount(0); ++i)
			{
				const VertexElement* element = this->GetElement(0, i);
				if (!shader->HasInputVertexSemantic(element->GetSemantic()))continue;
				switch (element->GetSemantic())
				{
				case VertexSemantic::Position:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 3, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Diffuse:
					GL_ERROR(glEnableVertexAttribArray(index));
					//GL_TRUE:指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）
					GL_ERROR(glVertexAttribPointer(index++, 4, GL_UNSIGNED_BYTE, GL_TRUE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Normal:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 3, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Tangent:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 4, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Size:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 1, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Coordinates1:
				case VertexSemantic::Coordinates2:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 2, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Indices:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 4, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				case VertexSemantic::Weights:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index++, 4, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					break;
				default:
					AssertEx(0, "%d", element->GetSemantic());
					break;
				}
			}
			GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}
		if (_vertexCount[INSTANCE_STREAM_INDEX] > 0)
		{
			GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _nBufferId[INSTANCE_STREAM_INDEX]));
			GLuint total_size = this->GetVertexSize(INSTANCE_STREAM_INDEX);
			for (byte i = 0; i < this->GetElementCount(INSTANCE_STREAM_INDEX); ++i)
			{
				const VertexElement* element = this->GetElement(INSTANCE_STREAM_INDEX, i);
				if (!shader->HasInputVertexSemantic(element->GetSemantic()))continue;
				switch (element->GetSemantic())
				{
				case VertexSemantic::InstanceOffset:
					GL_ERROR(glEnableVertexAttribArray(index));
					GL_ERROR(glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, total_size, GL_BUFFER_OFFSET(element->GetOffset())));
					//第一个参数是顶点属性索引，第二个参数是属性除数（attribute divisor）。默认属性除数是0，告诉OpenGL在顶点着色器的每次迭代更新顶点属性的内容。
					//把这个属性设置为1，我们就是告诉OpenGL我们打算在开始渲染一个新的实例的时候更新顶点属性的内容
					GL_ERROR(glVertexAttribDivisor(index++, 1));
					break;
				default:break;
				}
			}
			GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}
	}
	if (_vao)
	{
		GL_ERROR(glBindVertexArray(_vao));
	}
}
DC_END_NAMESPACE

/* glMapBufferRange
GL_MAP_READ_BIT		指示返回的指针可用于读取缓冲区对象数据。如果使用指针查询不包含该标志的映射，则不会产生GL错误，但是结果是不确定的，并且可能发生系统错误（可能包括程序终止）。
GL_MAP_WRITE_BIT	指示返回的指针可用于修改缓冲区对象数据。如果使用指针修改不包含该标志的映射，则不会产生GL错误，但是结果是不确定的，并且可能发生系统错误（可能包括程序终止）。

此外，访问中的以下可选标志位可用于修改映射：
GL_MAP_INVALIDATE_RANGE_BIT		表示指定范围的先前内容可能会被丢弃。除随后写入的数据外，未定义此范围内的数据。如果后续的GL操作访问未写入的数据，则不会产生GL错误，但是结果是不确定的，并且可能发生系统错误（可能包括程序终止）。该标志不能与GL_MAP_READ_BIT结合使用。
GL_MAP_INVALIDATE_BUFFER_BIT	指示可以丢弃整个缓冲区的先前内容。除随后写入的数据外，整个缓冲区内的数据均未定义。如果后续的GL操作访问未写入的数据，则不会产生GL错误，但是结果是不确定的，并且可能发生系统错误（可能包括程序终止）。该标志不能与GL_MAP_READ_BIT结合使用。
GL_MAP_FLUSH_EXPLICIT_BIT		指示可以修改映射的一个或多个离散子范围。设置此标志后，必须通过调用glFlushMappedBufferRange显式清除对每个子范围的修改。如果修改了映射的子范围并且未刷新，则不会设置GL错误，但是未定义缓冲区相应子范围内的数据。该标志只能与GL_MAP_WRITE_BIT一起使用。当选择此选项时，刷新将严格限于取消映射之前对glFlushMappedBufferRange的调用明确指示的区域；如果未选择此选项，则glUnmapBuffer会在调用时自动刷新整个映射范围。
GL_MAP_UNSYNCHRONIZED_BIT		指示GL在从glMapBufferRange返回之前，不应尝试对缓冲区上的未决操作进行同步。如果源或修改缓冲区的挂起操作与映射区域重叠，但是未定义此类先前操作和任何后续操作的结果，则不会生成GL错误。
*/
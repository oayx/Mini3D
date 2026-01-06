#include "GLPostProcess.h"
#include "GLProgram.h"
#include "GLDevice.h"
#include "runtime/graphics/Material.h"
#include "runtime/graphics/null/SwapChain.h"
#include "runtime/renderable/Renderer.h"
#include "runtime/scene/Camera.h"
#include "runtime/Application.h"
#include "editor/main/EMain_GameView.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(GLFinalProcess, FinalProcess);
GLFinalProcess::GLFinalProcess()
{
	//顶点buffer
	{
		const float triangle[] =
		{
			-1, 1,	0,	0,0,
			1,	1,	0,	1,0,
			1,	-1, 0,	1,1,
			-1, -1, 0,	0,1,
		};
		GL_ERROR(glGenBuffers(1, &_vertexVBO));
		//顶点buffer
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO));
		//将顶点数据绑定至当前默认的缓存中
		GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW));
	}
	GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//索引buffer
	{
		ushort indexes[] = { 0, 1, 2, 0, 2, 3 };
		GL_ERROR(glGenBuffers(1, &_indexVBO));
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO));
		GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW));
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	//顶点着色器和片段着色器源码
	const char* vertex_shader_source = R"(
		#version 100
		attribute vec3 a_position;
		attribute vec2 a_texCoord;
		varying vec2 v_texCoord;
		void main()
		{
			gl_Position = vec4(a_position,1.0);
			v_texCoord = a_texCoord;
		}
	)";
	const char* fragment_shader_source = R"(
		#version 100
		precision mediump float;
		uniform sampler2D _MainTex;
		varying vec2 v_texCoord;
		void main()
		{
			gl_FragColor = texture2D(_MainTex, v_texCoord);
		}
	)";
	VecString codes = { vertex_shader_source, "", "", "", fragment_shader_source, "" };
	String name = "internal/finalprocess";
#if defined(DC_GRAPHICS_API_OPENGLES3)
	name += "_elsl";
#else
	name += "_glsl";
#endif
	name += "_";
	name += GetGLDevice()->GetShaderVersion();
	_shadeProgram = GL_CompileShader(name, codes);
}
GLFinalProcess::~GLFinalProcess()
{
	GL_ERROR(glDeleteBuffers(1, &_vertexVBO));
	GL_ERROR(glDeleteBuffers(1, &_indexVBO));
	GL_ERROR(glDeleteProgram(_shadeProgram));
}
void GLFinalProcess::Render(Camera* camera, RenderTexture* dest)
{
	RenderFrameDesc desc;
	desc.viewPort = camera->GetViewPort();
	desc.clearFlag = camera->GetClearFlag();
	desc.clearColor = camera->GetClearColor();
	if (camera->GetRenderTexture())
	{//拷贝到纹理
		camera->GetRenderTexture()->PreRender();
		camera->GetRenderTexture()->BeginFrame(desc);
	}
	else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
	{
		EMain_GameView::GetRenderTexture()->PreRender();
		EMain_GameView::GetRenderTexture()->BeginFrame(desc);
	}
	else
	{//拷贝到默认缓冲区
		Application::GetGraphics()->GetSwapChain()->BeginFrame(desc);//防止深度缓存开启写入不进去
	}

	{
		//使用着色器程序
		GL_ERROR(glUseProgram(_shadeProgram));

		//纹理
		uint registerId = glGetUniformLocation(_shadeProgram, "_MainTex");
		if (glGetError() == 0 && registerId != -1)
		{
			//激活纹理
			GL_ERROR(glActiveTexture(GL_TEXTURE0 + 0));

			//设置纹理
			GL_ERROR(glBindTexture(GL_TEXTURE_2D, *(GLuint*)dest->GetTexture()));

			GL_ERROR(glUniform1i(registerId, 0));

			//纹理寻址模式
			GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_GetTextureAddress(TextureAddress::Clamp)));
			GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_GetTextureAddress(TextureAddress::Clamp)));

			//纹理过滤
			GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_GetMinTextureFilter(TextureFilter::Bilinear)));
			GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		}

		//顶点buffer
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _vertexVBO));

		//设置顶点属性指针
		GL_ERROR(glEnableVertexAttribArray(0));
		GL_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 2 * sizeof(float), GL_BUFFER_OFFSET(0)));
		GL_ERROR(glEnableVertexAttribArray(1));
		GL_ERROR(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 2 * sizeof(float), GL_BUFFER_OFFSET(12)));

		//索引
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexVBO));

		//显示
		GL_ERROR(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void *)0));
	}

	if (camera->GetRenderTexture())
	{
		camera->GetRenderTexture()->EndFrame();
		camera->GetRenderTexture()->PostRender();
	}
	else if (Application::IsEditor() && EMain_GameView::GetRenderTexture())
	{
		EMain_GameView::GetRenderTexture()->EndFrame();
		EMain_GameView::GetRenderTexture()->PostRender();
	}
	else
	{
		Application::GetGraphics()->GetSwapChain()->EndFrame();
	}

	//统计
	SceneManager::AddDrawCall(1);
	SceneManager::AddRenderBatch(1);
	SceneManager::AddSetPassCall(1);
	SceneManager::AddDrawTriangles(2);
}
DC_END_NAMESPACE
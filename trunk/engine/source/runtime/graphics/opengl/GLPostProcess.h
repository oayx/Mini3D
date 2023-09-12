 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/16
* Description：后处理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"
#include "GLDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class GLFinalProcess Final : public FinalProcess
{
	friend class GLDevice;
	DEFAULT_CREATE(GLFinalProcess);
	FRIEND_CONSTRUCT_DESTRUCT(GLFinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(GLFinalProcess, FinalProcess)
	END_DERIVED_REFECTION_TYPE;

protected:
	GLFinalProcess();
	~GLFinalProcess();

protected:
	virtual void Render(Camera* camera, RenderTexture* dest)override;

private:
	GLuint _vertexVBO;
	GLuint _indexVBO;
	GLuint _shadeProgram;
};
DC_END_NAMESPACE

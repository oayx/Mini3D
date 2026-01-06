 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/16
* Description：后处理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"
#include "VKDefine.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 最后显示到屏幕
class VKFinalProcess final : public FinalProcess
{
	friend class VKDevice;
	DEFAULT_CREATE(VKFinalProcess);
	FRIEND_CONSTRUCT_DESTRUCT(VKFinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(VKFinalProcess, FinalProcess)
	END_REFECTION_TYPE;

protected:
	VKFinalProcess();
	~VKFinalProcess();

protected:
	virtual void Render(Camera* camera, RenderTexture* dest)override;

private:
};
DC_END_NAMESPACE

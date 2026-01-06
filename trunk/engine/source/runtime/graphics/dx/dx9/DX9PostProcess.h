
/*****************************************************************************
* Author： hannibal
* Date：2020年6月16日
* Description：后处理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 最后显示到屏幕
class DX9FinalProcess final : public FinalProcess
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9FinalProcess);
	FRIEND_CONSTRUCT_DESTRUCT(DX9FinalProcess);
	DISALLOW_COPY_ASSIGN(DX9FinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(DX9FinalProcess, FinalProcess)
	END_REFECTION_TYPE;

	DX9FinalProcess();
	~DX9FinalProcess();

public:
	virtual void Render(Camera* camera, RenderTexture* dest)override;

private:
	IDirect3DVertexDeclaration9* _vertDecl = nullptr;
	ID3DXEffect* _effect = nullptr;
};
DC_END_NAMESPACE

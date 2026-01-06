 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/16
* Description：后处理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"
#include "DX12Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class DX12FinalProcess final : public FinalProcess
{
	friend class DX12Device;
	DEFAULT_CREATE(DX12FinalProcess);
	FRIEND_CONSTRUCT_DESTRUCT(DX12FinalProcess);
	DISALLOW_COPY_ASSIGN(DX12FinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(DX12FinalProcess, FinalProcess)
	END_REFECTION_TYPE;

	DX12FinalProcess();
	~DX12FinalProcess();

public:
	virtual void Render(Camera* camera, RenderTexture* dest)override;

private:
	ID3D12Resource* _vertexBuffer = nullptr;
	ID3D12Resource* _indexBuffer = nullptr;
	ID3D12PipelineState* _pipelineState = nullptr;
};
DC_END_NAMESPACE


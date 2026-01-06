 
/*****************************************************************************
* Author： hannibal
* Date：2020/6/16
* Description：后处理
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/PostProcess.h"
#include "DX11Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// 最后显示到屏幕
class DX11FinalProcess final : public FinalProcess
{
	friend class DX11Device;
	DEFAULT_CREATE(DX11FinalProcess);
	FRIEND_CONSTRUCT_DESTRUCT(DX11FinalProcess);
	DISALLOW_COPY_ASSIGN(DX11FinalProcess);
	BEGIN_DERIVED_REFECTION_TYPE(DX11FinalProcess, FinalProcess)
	END_REFECTION_TYPE;

	DX11FinalProcess();
	~DX11FinalProcess();

public:
	virtual void Render(Camera* camera, RenderTexture* dest)override;

private:
	ID3D11InputLayout* _vertexLayout = nullptr;

	ID3D11Buffer* _vertexBuffer = nullptr;
	ID3D11Buffer* _indexBuffer = nullptr;

	ID3D11VertexShader* _vertexShader = nullptr;
	ID3D11PixelShader* _pixelShader = nullptr;

	ID3D11SamplerState* _samplerState = nullptr;
};
DC_END_NAMESPACE

/*****************************************************************************
* Author： hannibal
* Date：2020年3月18日
* Description：
*****************************************************************************/
#pragma once

#include "runtime/graphics/null/CGProgram.h"
#include "DX9Define.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// d3d9 shader程序
class DX9Program final : public CGProgram
{
	friend class DX9Device;
	DEFAULT_CREATE(DX9Program);
	FRIEND_CONSTRUCT_DESTRUCT(DX9Program);
	DISALLOW_COPY_ASSIGN(DX9Program);
	BEGIN_DERIVED_REFECTION_TYPE(DX9Program, CGProgram)
	END_REFECTION_TYPE;

	DX9Program() = default;
	~DX9Program();

public:
	virtual bool LoadFromFile(const String& file)override;
	virtual bool LoadFromDesc(const ShaderDesc& info)override;
	virtual bool LoadFromMemory(const String& name, const VecString& codes, const VecString& defines)override;

	virtual void PreRender()override;
	virtual void PostRender()override;

public:
	virtual bool SetVariable(const String& name, const Matrix4& mat)override;
	virtual bool SetVariable(const String& name, const Matrix4* mats, int count)override;
	virtual bool SetVariable(const String& name, const float* floats, int count)override;
	virtual bool SetVariable(const String& name, const Vector4* vecs, int count)override;
	virtual bool SetVariable(const String& name, const int* ints, int count)override;
	virtual bool SetVariable(const String& name, const bool bools)override;
	virtual bool SetVariable(const String& name, const ShaderLight& light, int index)override;
	virtual bool SetVariable(byte index, TextureUnit* tex_unit)override;
	virtual bool SetVariable(byte index, const String& name, Texture* texture)override;
	virtual bool SetPassVariable(byte index, const String& name, Texture* texture)override;

private:
	bool SetVertexShader();
	bool SetPixelShader();

private:
	IDirect3DVertexShader9* _pVertexShader = nullptr;
	IDirect3DPixelShader9*	_pPixelShader = nullptr;
	ID3DXConstantTable*     _constantTable[(int)ShaderType::Max] = {};
};
bool DX9CompileShaderFromFile(const String& file, const VecString& defines, const String& enter_point, const String& compile_target, ID3DXConstantTable* &constTable, ID3DXBuffer* &codeBuffer);
bool DX9CompileShaderFromMemory(const String& code, const VecString& defines, const String& enter_point, const String& compile_target, ID3DXConstantTable* &constTable, ID3DXBuffer* &codeBuffer);
DC_END_NAMESPACE

/*VS:语义
输入:
BINORMAL[n]		Binormal						float4
BLENDINDICES[n]	Blend indices					uint
BLENDWEIGHT[n]	Blend weights					float
COLOR[n]		Diffuse and specular color		float4
NORMAL[n]		Normal vector					float4
POSITION[n]		Vertex position in object space float4
POSITIONT		Transformed vertex position.	float4
PSIZE[n]		Point size						float
TANGENT[n]		Tangent							float4
TEXCOORD[n]

输出
COLOR[n]		Diffuse or specular color		float4
FOG				Vertex fog						float
POSITION[n]		Position of a vertex in homogenous space.Compute position in screen - space by dividing(x, y, z) by w.Every vertex shader must write out a parameter with this semantic.float4
PSIZE			Point size						float
TESSFACTOR[n]	Tessellation factor				float
TEXCOORD[n]		Texture coordinates				float4
*/

/*PS:语义
输入：
COLOR[n]		Diffuse or specular color.		float4
TEXCOORD[n]		Texture coordinates				float4
VFACE			Floating-point scalar that indicates a back-facing primitive. A negative value faces backwards, while a positive value faces the camera.	float
VPOS			The pixel location (x,y) in screen space. To convert a Direct3D 9 shader (that uses this semantic) to a Direct3D 10 shader, seeDirect3D 9 VPOS and Direct3D 10 SV_Position)	float2

输出：
COLOR[n]		Output color					float4
DEPTH[n]		Output depth					float
*/
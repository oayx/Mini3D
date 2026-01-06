
/*****************************************************************************
* Author： hannibal
* Description：
*****************************************************************************/
#pragma once

#include "Pass.h"
#include "TextureUnit.h"
#include "RenderDefine.h"
#include "runtime/resources/Resources.h"
#include "runtime/scene/SceneUtils.h"

DC_BEGIN_NAMESPACE
class Material;
class CGProgram;
/********************************************************************/
class ENGINE_DLL Shader final : public Resource
{
	friend class Material;
	typedef Vector<Pass*> Passes;
	FRIEND_CONSTRUCT_DESTRUCT(Shader);
	DISALLOW_COPY_ASSIGN(Shader);
	BEGIN_DERIVED_REFECTION_TYPE(Shader, Resource)
	END_REFECTION_TYPE;
	Shader(Material* material);
	~Shader();

public:
	static Shader* Create(Material* material);

public:
	Material* GetMaterial()const { return _material; }
	Pass* CreatePass(int pass = 0);
	Pass* GetPass(int pass = 0)const;
	uint  GetPassCount()const { return (uint)_passes.Size(); }
	const String& GetGroupInspector() { return _groupInspector; }

	void  SetTexture(const ShaderTexture& info);

public:
	int  GetRenderQueue()const { return _nRenderQueue; }
	void SetRenderQueue(int quque) { _nRenderQueue = quque; }

	void SetScissorEnable(bool b) { _scissorEnable = b; }
	bool IsScissorEnable()const { return _scissorEnable; }

	bool  IsCastShadow()const { return _castShadow; }
	bool  IsReceiveShadow()const { return _receiveShadow; }
	bool  EnableLight()const { return _enableLight; }

	void  SetDepthOffset(float factor, float units) { _enbaleDepthOffset = true; _depthOffsetFactor = factor; _depthOffsetUnits = units; }
	bool  IsDepthOffsetEnable()const { return _enbaleDepthOffset; }
	float GetDepthOffsetFactor()const { return _depthOffsetFactor; }
	float GetDepthOffsetUnits()const { return _depthOffsetUnits; }

	int	  GetVariableCount()const { return _shaderVariables.Size(); }
	const ShaderVariable& GetVariable(int index) { return _shaderVariables[index]; }
	const VecShaderVariable& GetVariables() { return _shaderVariables; }

	int	  GetTextureCount()const { return _shaderTextures.Size(); }
	const ShaderTexture& GetTexture(int index) { return _shaderTextures[index]; }
	const VecShaderTexture& GetTextures() { return _shaderTextures; }

private:
	bool ParseFromFile(const String& file);
	virtual bool LoadFromFile(const String& file)override;

private:
	Material* _material = nullptr;
	Passes	_passes;

	String  _groupInspector = "";

	bool	_castShadow = true;			//是否产生阴影
	bool	_receiveShadow = false;		//是否接受阴影
	String	_renderType = "Opaque";		//渲染类型，可以自定义，默认的有：Background,Opaque,Transparent,Overlay
	int		_nRenderQueue = (int)eRenderQueue::Geometry;

	bool	_scissorEnable = false;		//裁剪是否开启

	bool	_enbaleDepthOffset = false;	//光栅化深度偏移
	float	_depthOffsetFactor = 0.2f;
	float	_depthOffsetUnits = 0.1f;

	bool	_enableLight = false;			//受灯光影响

	VecShaderVariable	_shaderVariables;	//变量列表
	VecShaderTexture	_shaderTextures;	//纹理
};
/********************************************************************/
class ShaderSerialize : public object
{
	DEFAULT_CREATE(ShaderSerialize);
	FRIEND_CONSTRUCT_DESTRUCT(ShaderSerialize);
public:
	typedef Vector<PassSerialize> PassSerializes;
	void	Serialize(const String& file);

public:
	String  GroupInspector = "";		//分组：在shader筛选中有用到
	bool	HideInspector = false;		//在筛选中是否隐藏

	bool	CastShadow = true;			//是否产生阴影
	bool	ReceiveShadow = false;		//是否接受阴影
	String	RenderType = "Opaque";		//渲染类型，可以自定义，默认的有：Background,Opaque,Transparent,Overlay
	int		RenderQueue = (int)eRenderQueue::Geometry;

	bool	EnableLight = false;		//受灯光影响
	bool	ScissorEnable = false;		//裁剪是否开启

	VecShaderVariable	ShaderVariables;//变量列表
	VecShaderTexture	ShaderTextures;	//纹理

	PassSerializes		mPassSerializes;
};
class ShaderSerializePools
{
	friend class Shader;
	friend class Application;
	typedef Map<String, ShaderSerialize*> ShaderSerializes;

	static void Destroy();
	static void Serialize(const String& file);

public:
	static ShaderSerialize* Get(const String& file);

private:
	inline static ShaderSerializes _shaderSerializes;
};
DC_END_NAMESPACE
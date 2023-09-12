
/*****************************************************************************
* Author： hannibal
* Date：2020/12/19
* Description：地形
*****************************************************************************/
#pragma once

#include "runtime/renderable/Renderer.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL Terrain Final : public Renderer
{
	friend class GameObject;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(Terrain);
	DISALLOW_COPY_ASSIGN(Terrain);
	FRIEND_CONSTRUCT_DESTRUCT(Terrain);
	BEGIN_DERIVED_REFECTION_TYPE(Terrain, Renderer)
		CTORS(DEFAULT_CTOR(Terrain))
		ADD_EDITOR_COMPONENT("Terrain/Terrain")
	END_DERIVED_REFECTION_TYPE;

	Terrain() = default;
	~Terrain();

protected:
	virtual bool CanRemove()override;
	virtual void OnDrawEditor()override;

public:
	void	SetHeightMap(const String& file, float height_limit = 50.0f, int smooth_iterator = 2, int smooth_range = 2);
	float	GetHeight(int row, int col);
	void	SetHeight(int row, int col, float height);
	float	GetMinHeight()const { return _minHeight; }
	float	GetMaxHeight()const { return _maxHeight; }

	int		GetRow(float z);
	int		GetCol(float x);
	float	GetPositionX(int col);
	float	GetPositionZ(int row);
	float	GetHeight(float x, float z);

	const	float* GetHeightData()const { return _heightData; }
	int		GetTileRows()const { return _tileRows; }
	int		GetTileCols()const { return _tileCols; }

private:
	void	Build();
	void	LoadHeightMap();
	void	SmoothHeight();			//平滑处理
	void	BuildTile();
	void	Clear();

private:
	int		_tileRows = 0;
	int		_tileCols = 0;
	float*	_heightData = nullptr;
	float	_minHeight = 0.0f;
	float	_maxHeight = 0.0f;

	String	_heightMapFile = "";	//高度图
	float	_heightLimit = 50.0f;	//高度限制
	int		_smoothIterator = 2;	//平滑处理迭代次数
	int		_smoothRange = 2;		//平滑处理范围
};
DC_END_NAMESPACE

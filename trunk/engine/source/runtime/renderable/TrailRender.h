
/*****************************************************************************
* Author： hannibal
* Date：2021/5/24
* Description：拖尾
*****************************************************************************/
#pragma once

#include "Renderer.h"
#include "core/color/Color.h"

DC_BEGIN_NAMESPACE
class Camera;
/********************************************************************/
class ENGINE_DLL TrailRender final : public Renderer
{
	friend class GameObject;
	typedef std::deque<Vector3> Positions;
	typedef std::deque<float> TimeStamps;
	DECLARE_OBJECT_CLONE;
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(TrailRender);
	FRIEND_CONSTRUCT_DESTRUCT(TrailRender);
	BEGIN_DERIVED_REFECTION_TYPE(TrailRender, Renderer)
		CTORS(DEFAULT_CTOR(TrailRender))
		ADD_EDITOR_COMPONENT("Effects/TrailRender")
	END_REFECTION_TYPE;

protected:
	virtual void Awake()override;
	virtual void OnTransformChanged()override;
	virtual void OnPreRender(Camera* camera)override;

public:
	void 	SetColor(const Color& color) { _color = color; }
	void 	SetWidth(float start_width, float end_width) { _widthRange.Set(start_width, end_width); }
	void 	SetDurationTime(float time) { _durationTime = time; }
	void 	SetMinVertexDistance(float distance) { _minVertexDistance = distance; }

private:
	void	Build3DLine(VariablePrimitive* prim, Camera* camera);

private:
	Color	_color = Color::White;
	Size	_widthRange;
	float	_durationTime = 5.0f;
	float	_minVertexDistance = 0.1f;
	bool	_transformChanged = true;
	Positions  _positions;
	TimeStamps _timeStamps;
};
DC_END_NAMESPACE
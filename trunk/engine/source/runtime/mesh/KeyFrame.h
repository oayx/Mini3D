
/*****************************************************************************
* Author： hannibal
* Date：2020/8/19
* Description：
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
class ENGINE_DLL KeyFrame Final : public Object
{
	friend class AnimationNode;
	DEFAULT_CREATE(KeyFrame);
	FRIEND_CONSTRUCT_DESTRUCT(KeyFrame);
	DISALLOW_COPY_ASSIGN(KeyFrame);
	BEGIN_DERIVED_REFECTION_TYPE(KeyFrame, Object)
	END_DERIVED_REFECTION_TYPE;

	KeyFrame(uint time) : Time(time){}
	~KeyFrame() {}

private:
	void Copy(KeyFrame* frame);

public:
	uint		Time;		//第几帧
	Vector3		Position;
	Quaternion	Rotate;
	Vector3		Scale;
};
DC_END_NAMESPACE

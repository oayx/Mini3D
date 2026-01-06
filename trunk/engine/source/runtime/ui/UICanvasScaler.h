
/*****************************************************************************
* Author： hannibal
* Date：2020/9/1
* Description：scaler ui
*****************************************************************************/
#pragma once

#include "runtime/component/Component.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
enum class UIScaleMode
{
	ConstantPixelSize,		//连续的像素大小，这也就意味着，你布局的UI元素 都是实际的大小
	ScaleWithScreenSize,	//和屏幕大小一起缩放， 这个非常常用，这样就确保在不同分辨率下，当前UI 不会出现大的偏差
	ConstantPhysicalSize,	//连续的物理大小，与第一条相对，在不同像素的设备上都会有良好的表现，比如 2cm * 2cm, 在不同设备上都会这么大
};
DECLARE_ENUM_OPERATORS(UIScaleMode);

enum class UIScaleMatchMode
{
	MatchWidthOrHeight,		//匹配宽或高
	Expand,					//扩张:如果视口比 小于 参考分辨率， 以5：4为例，那么视图就会将1600 * 900扩张到1600 * 1280， 也就是宽扩张，多余的部分用蓝色填充
	Shrink,					//收缩
};
DECLARE_ENUM_OPERATORS(UIScaleMatchMode);

class ENGINE_DLL UICanvasScaler final : public Component
{
	friend class GameObject;
	friend class UICanvas;
	DECLARE_OBJECT_CLONE;
	DECLARE_OBJECT_SERIALIZE(UICanvasScaler);
	DISALLOW_CONSTRUCTOR_COPY_ASSIGN(UICanvasScaler);
	FRIEND_CONSTRUCT_DESTRUCT(UICanvasScaler);
	BEGIN_DERIVED_REFECTION_TYPE(UICanvasScaler, Component)
		CTORS(DEFAULT_CTOR(UICanvasScaler))
		ADD_EDITOR_COMPONENT("UI/CanvasScaler")
	END_REFECTION_TYPE;

protected:
	virtual bool		CanRemove()override;
	virtual void		OnDrawEditor()override;

public:
	void				SetReferenceResolution(const iSize& size) { _referenceResolution = size;  Refresh(); }
	const iSize&		GetReferenceResolution()const { return _referenceResolution; }
	UIScaleMode			GetScaleMode()const { return _scaleMode; }
	UIScaleMatchMode	GetScaleMatchMode()const { return _scaleMatchMode; }
	void				SetScaleMatchMode(UIScaleMatchMode mode) { _scaleMatchMode = mode;   Refresh(); }
	void				SetMatchWidth(bool b) { _matchWidth = b;   Refresh(); }
	bool				IsMatchWidth()const { return _matchWidth; }

	const iSize&		GetRealResolution()const { return _realResolution; }

	void				Refresh();

private:
	iSize				_referenceResolution;
	UIScaleMode			_scaleMode = UIScaleMode::ScaleWithScreenSize;
	UIScaleMatchMode	_scaleMatchMode = UIScaleMatchMode::MatchWidthOrHeight;
	bool				_matchWidth = true;//false->height

	iSize				_realResolution;
};
DC_END_NAMESPACE

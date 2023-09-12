#include "KeyFrame.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(KeyFrame, Object);
void KeyFrame::Copy(KeyFrame* frame)
{
	this->Position = frame->Position;
	this->Rotate = frame->Rotate;
	this->Scale = frame->Scale;
}
DC_END_NAMESPACE

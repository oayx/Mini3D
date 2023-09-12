#include "ManualLine.h"

BEGING_NAMESPACE_DC
/********************************************************************/
ManualLine::ManualLine(const String &name, const VecVector3& lines)
: base(OT_LINE_STRIP, name)
{
	AssertEx(!lines.empty(), "line size is empty");
	m_bUseIndexData = false;

	SetVertex(lines);
}
END_NAMESPACE_DC
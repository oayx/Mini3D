#include "SerializeRead.h"
#include "core/stream/DataStream.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SerializeRead, SerializeBase);
SerializeRead::SerializeRead(const String& file)
	: base(file)
{
	FileDataStream stream(_filePath, "rb+");
	tinyxml2::XMLError error = _docment.LoadFile(stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("SerializeRead::SerializeRead -  open XML error (%s)", file.c_str());
		return;
	}
	else
	{
		_isOpen = true;
		_rootNode = _docment.RootElement();
		_currNode = _rootNode;
	}
}
void SerializeRead::Reset()
{
	_currNode = _rootNode;
	_nodeStack.Clear();
}
DC_END_NAMESPACE
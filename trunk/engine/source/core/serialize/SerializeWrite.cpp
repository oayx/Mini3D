#include "SerializeWrite.h"
#include "core/stream/DataStream.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(SerializeWrite, SerializeBase);
SerializeWrite::SerializeWrite(const String& file)
	: base(file)
{
	_isOpen = true;
	_rootNode = _docment.NewElement("Root");
	_docment.InsertEndChild(_rootNode);
	_currNode = _rootNode;
}
SerializeWrite::~SerializeWrite() 
{
	if (!_isSaved)
	{
		this->Save();
	}
}
void SerializeWrite::Save()
{
	if (!File::Exist(_filePath))
	{
		File::Create(_filePath);
	}
	FileDataStream stream(_filePath, "wb+");
	tinyxml2::XMLError error = _docment.SaveFile(stream.GetHandle());
	if (error != tinyxml2::XML_SUCCESS)
	{
		Debuger::Error("SerializeWrite::Save -  save XML error (%s)", _filePath.c_str());
		return;
	}
	_isSaved = true;
}
DC_END_NAMESPACE
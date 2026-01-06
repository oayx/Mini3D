#include "CommandLine.h"
 
DC_BEGIN_NAMESPACE
/********************************************************************/
IMPL_REFECTION_TYPE(CommandLine);
void CommandLine::Pause(int argc, char** argv)
{
	_commands.Reserve(argc);
	for (int i = 0; i < argc; ++i)
	{
		Debuger::Log("argc %d:%s", i, argv[i]);
		_commands.Add(argv[i]);
	}
}
DC_END_NAMESPACE
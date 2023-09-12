#pragma once
#include "String.h"

#include <external/format/format.h>

DC_BEGIN_NAMESPACE
template <typename... Args>
static String String::Format(const char* format, const Args & ... args)
{ 
	return fmt::format(format, args...);
}
DC_END_NAMESPACE
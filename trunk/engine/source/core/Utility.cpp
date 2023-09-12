#include "Utility.h" 

DC_BEGIN_NAMESPACE
/********************************************************************/
void Utility::StretchMemoryCopy(const byte* src_data, const iPoint& src_offset, int src_pitch, byte* dst_data, const iPoint& dst_offset, int dst_pitch, const iSize& size, int per_bytes)
{
	for (int row = 0; row < size.height; ++row)
	{
		Memory::Copy(dst_data + (row + dst_offset.y) * dst_pitch + dst_offset.x * per_bytes,
			src_data + (row + src_offset.y) * src_pitch + src_offset.x * per_bytes,
			size.width * per_bytes);
	}
}
DC_END_NAMESPACE
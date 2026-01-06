#include "Utility.h" 

DC_BEGIN_NAMESPACE
/********************************************************************/
void Utility::StretchMemoryCopy(const byte* srcData, const iPoint& srcOffset, int srcPitch, byte* dstData, const iPoint& dstOffset, int dstPitch, const iSize& size, int perBytes)
{
	for (int row = 0; row < size.height; ++row)
	{
		Memory::Copy(dstData + (row + dstOffset.y) * dstPitch + dstOffset.x * perBytes,
			srcData + (row + srcOffset.y) * srcPitch + srcOffset.x * perBytes,
			size.width * perBytes);
	}
}
DC_END_NAMESPACE
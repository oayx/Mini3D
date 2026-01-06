#include "DataStream.h"
#include "core/Debuger.h"
#include "platform/PlatformDefine.h"
#if !defined(DC_PLATFORM_WIN32)
#include <unistd.h>
#endif

DC_BEGIN_NAMESPACE
#define STREAM_TEMP_SIZE 512
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DataStream, Object);
IMPL_DERIVED_REFECTION_TYPE(MemoryDataStream, DataStream);
MemoryDataStream::MemoryDataStream(uint size)
{
	mSize = size;
	if (mSize > 0)
		mData = Memory::NewArray<byte>(mSize);
}
MemoryDataStream::MemoryDataStream(byte* buffer, uint size)
: base(), mData(buffer)
{
	mSize = size;
	if (mSize > 0)
	{
		mData = Memory::NewArray<byte>(mSize);
		::memcpy(mData, buffer, (size_t)mSize);
	}
}
MemoryDataStream::MemoryDataStream(const MemoryDataStream& other)
: DataStream(other)
{
	if (mSize > 0)
	{
		mData = Memory::NewArray<byte>(mSize);
		::memcpy(mData, other.Buffer(), (size_t)mSize);
	}
}
MemoryDataStream::MemoryDataStream(MemoryDataStream&& other)
: DataStream(other)
{
	mData = std::move(other.mData);
	other.mData = nullptr;
}
MemoryDataStream::~MemoryDataStream()
{
	Close();
}
MemoryDataStream& MemoryDataStream::operator=(const MemoryDataStream& other) noexcept
{
	if (mData)
	{
		Memory::DeleteArray(mData);
		mData = nullptr;
	}
	mSize = other.Size();
	if (mSize > 0)
	{
		mData = Memory::NewArray<byte>(mSize);
		::memcpy(mData, other.Buffer(), (size_t)mSize);
	}
	return *this;
}
MemoryDataStream& MemoryDataStream::operator=(MemoryDataStream&& other) noexcept
{
	mSize = other.Size();
	mData = std::move(other.mData);
	other.mData = nullptr;
	return *this;
}
int MemoryDataStream::Read(void* buf, int count) noexcept
{
	int cnt = count;
	if (mPos + cnt > mSize)
		cnt = mSize - mPos;
	if (cnt <= 0 || mPos + cnt > mSize)
		return 0;

	if(buf != nullptr)::memcpy(buf, mData + mPos, (size_t)cnt);
	mPos += cnt;
	return cnt;
}
int MemoryDataStream::Write(const void* buf, uint offset, int count) noexcept
{
	if (count == 0)
		return 0;

	if (mPos + count > mSize)
	{//长度不够，申请新的内存，并且复制旧的到新的内存
		byte* new_data = Memory::NewArray<byte>(mPos + count);
		if (mData != nullptr)::memcpy(new_data, mData, mSize);
		Memory::DeleteArray(mData);
		mData = new_data;
		mSize = mPos + count;
	}
	::memcpy(mData + mPos, &((byte*)buf)[offset], (size_t)count);
	return count;
}
void MemoryDataStream::Skip(int count) noexcept
{
	if (mPos + count > mSize)
		mPos = mSize;
	else
		mPos += count;
}
void MemoryDataStream::Seek(int pos, bool relative) noexcept
{
	if (relative)
	{
		if (mPos + pos > mSize)
			mPos = mSize;
		else
			mPos += pos;
	}
	else
	{
		if(pos > mSize)
			mPos = mSize;
		else
			mPos = pos;
	}
}
uint MemoryDataStream::Position(void) noexcept
{
	return mPos;
}
bool MemoryDataStream::eof(void) noexcept
{
	return mPos >= mSize;
}
void MemoryDataStream::Close(void)noexcept
{
	if (mData)
	{
		Memory::DeleteArray(mData);
		mData = nullptr;
	}
	mSize = 0;
	mPos = 0;
}
void MemoryDataStream::Resize(uint size) noexcept
{
	if (mData)
	{//需要复制数据
		byte* new_data = nullptr;
		if (size > 0)
		{
			new_data = Memory::NewArray<byte>(size);
			::memcpy(new_data, mData, size >= mSize ? mSize : size);
		}
		Memory::DeleteArray(mData);
		mData = new_data;
	}
	else if (size > 0)
	{
		mData = Memory::NewArray<byte>(size);
	}

	mPos = 0;
	mSize = size;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FileDataStream, DataStream);
FileDataStream::FileDataStream(FILE* handle)
: base(), mFileHandle(handle)
{
	DC_PROFILE_FUNCTION;
	fseek(mFileHandle, 0, SEEK_END);
	mSize = ftell(mFileHandle);
	fseek(mFileHandle, 0, SEEK_SET);
}
FileDataStream::FileDataStream(const String& path, const String& mode)
{
	DC_PROFILE_FUNCTION;
#if defined(DC_PLATFORM_WIN32)
	std::wstring wPath = Encoding::Utf8ToWChar(path.c_str(), path.Size());
	std::wstring w_mode = Encoding::Utf8ToWChar(mode.c_str(), mode.Size());
	mFileHandle = ::_wfopen(wPath.c_str(), w_mode.c_str());
#else
	mFileHandle = fopen(path.c_str(), mode.c_str());
#endif

	if (!mFileHandle)
	{
#if DC_DEBUG
		Debuger::Error("open file error:%s", path.c_str());
#endif
		this->Close();
	}
	else
	{
		fseek(mFileHandle, 0, SEEK_END);
		mSize = ftell(mFileHandle);
		fseek(mFileHandle, 0, SEEK_SET);
	}
}
FileDataStream::~FileDataStream()
{
	Close();
}
bool FileDataStream::IsOpen() noexcept
{
	if (mFileHandle == nullptr)return false;
	return true;
}
int FileDataStream::Read(void* buf, int count) noexcept
{
	if (!IsOpen())return 0;
	return (int)fread(buf, 1, (size_t)count, mFileHandle);
}
int FileDataStream::Write(const void* buf, uint offset, int count) noexcept
{
	if (!IsOpen())return 0;
	return (int)fwrite(&((byte*)buf)[offset], 1, (size_t)count, mFileHandle);
}
void FileDataStream::Skip(int count) noexcept
{
	if (!IsOpen())return;
	fseek(mFileHandle, (long)count, SEEK_CUR);
}
void FileDataStream::Seek(int pos, bool relative) noexcept
{
	if (!IsOpen())return;
	fseek(mFileHandle, (long)pos, relative ? SEEK_CUR : SEEK_SET);
}
uint FileDataStream::Position(void) noexcept
{
	if (!IsOpen())return 0;
	return ftell(mFileHandle);
}
bool FileDataStream::eof(void) noexcept
{
	if (!IsOpen())return false;
	return feof(mFileHandle) != 0;
}
void FileDataStream::Flush() noexcept
{
	if (!IsOpen())return;
	fflush(mFileHandle);
#if !defined(DC_PLATFORM_WIN32)
	int fd = fileno(mFileHandle);  //由文件流获取文件描述符
	if(fd != -1)fsync(fd);
#endif
}
void FileDataStream::Close(void) noexcept
{
	if (mFileHandle != nullptr)
	{
		fclose(mFileHandle);
		mFileHandle = nullptr;
	}
}
DC_END_NAMESPACE

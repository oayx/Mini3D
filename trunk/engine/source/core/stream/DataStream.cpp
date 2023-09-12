#include "DataStream.h"
#include "core/Debuger.h"
#include "platform/PlatformDefine.h"
 
DC_BEGIN_NAMESPACE
#define STREAM_TEMP_SIZE 512
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(DataStream, Object);
IMPL_DERIVED_REFECTION_TYPE(MemoryDataStream, DataStream);
MemoryDataStream::MemoryDataStream(uint size)
{
	mSize = size;
	if (mSize > 0)
		mData = NewArray<byte>(mSize);
}
MemoryDataStream::MemoryDataStream(byte* buffer, uint size)
: base(), mData(buffer)
{
	mSize = size;
	if (mSize > 0)
	{
		mData = NewArray<byte>(mSize);
		::memcpy(mData, buffer, (size_t)mSize);
	}
}
MemoryDataStream::MemoryDataStream(const MemoryDataStream& buffer)
{
	mSize = buffer.Size();
	if (mSize > 0)
	{
		mData = NewArray<byte>(mSize);
		::memcpy(mData, buffer.data(), (size_t)mSize);
	}
}
MemoryDataStream::~MemoryDataStream()
{
	Close();
}
MemoryDataStream& MemoryDataStream::operator=(const MemoryDataStream& buffer)
{
	if (mData)
	{
		DeleteArray(mData);
		mData = nullptr;
	}
	mSize = buffer.Size();
	if (mSize > 0)
	{
		mData = NewArray<byte>(mSize);
		::memcpy(mData, buffer.data(), (size_t)mSize);
	}
	return *this;
}
int MemoryDataStream::Read(void* buf, int count)
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
int MemoryDataStream::Write(const void* buf, uint offset, int count)
{
	if (count == 0)
		return 0;

	if (mPos + count > mSize)
	{//长度不够，申请新的内存，并且复制旧的到新的内存
		byte* new_data = NewArray<byte>(mPos + count);
		if (mData != nullptr)::memcpy(new_data, mData, mSize);
		DeleteArray(mData);
		mData = new_data;
		mSize = mPos + count;
	}
	::memcpy(mData + mPos, &((byte*)buf)[offset], (size_t)count);
	return count;
}
void MemoryDataStream::Skip(int count)
{
	if (mPos + count > mSize)
		mPos = mSize;
	else
		mPos += count;
}
void MemoryDataStream::Seek(int pos, bool relative)
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
uint MemoryDataStream::Tell(void)
{
	return mPos;
}
bool MemoryDataStream::eof(void)
{
	return mPos >= mSize;
}
void MemoryDataStream::Close(void)    
{
	if (mData)
	{
		DeleteArray(mData);
		mData = nullptr;
	}
	mSize = 0;
	mPos = 0;
}
void MemoryDataStream::Resize(uint size)
{
	if (mData)
	{//需要复制数据
		byte* new_data = nullptr;
		if (size > 0)
		{
			new_data = NewArray<byte>(size);
			::memcpy(new_data, mData, size >= mSize ? mSize : size);
		}
		DeleteArray(mData);
		mData = new_data;
	}
	else if (size > 0)
	{
		mData = NewArray<byte>(size);
	}

	mPos = 0;
	mSize = size;
}
/********************************************************************/
IMPL_DERIVED_REFECTION_TYPE(FileDataStream, DataStream);
FileDataStream::FileDataStream(FILE* handle)
: base(), mFileHandle(handle)
{
	DC_PROFILE_FUNCTION();
	fseek(mFileHandle, 0, SEEK_END);
	mSize = ftell(mFileHandle);
	fseek(mFileHandle, 0, SEEK_SET);
}
FileDataStream::FileDataStream(const String& path, const String& mode)
{
	DC_PROFILE_FUNCTION();
#if defined(DC_PLATFORM_WIN32)
	std::wstring w_path = Encoding::Utf8ToWChar(path.c_str(), path.Size());
	std::wstring w_mode = Encoding::Utf8ToWChar(mode.c_str(), mode.Size());
	mFileHandle = ::_wfopen(w_path.c_str(), w_mode.c_str());
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
bool FileDataStream::IsOpen()
{
	if (mFileHandle == nullptr)return false;
	return true;
}
int FileDataStream::Read(void* buf, int count)
{
	if (!IsOpen())return 0;
	return (int)fread(buf, 1, (size_t)count, mFileHandle);
}
int FileDataStream::Write(const void* buf, uint offset, int count)
{
	if (!IsOpen())return 0;
	return (int)fwrite(&((byte*)buf)[offset], 1, (size_t)count, mFileHandle);
}
void FileDataStream::Skip(int count)
{
	if (!IsOpen())return;
	fseek(mFileHandle, (long)count, SEEK_CUR);
}
void FileDataStream::Seek(int pos, bool relative)
{
	if (!IsOpen())return;
	fseek(mFileHandle, (long)pos, relative ? SEEK_CUR : SEEK_SET);
}
uint FileDataStream::Tell(void)
{
	if (!IsOpen())return 0;
	return ftell(mFileHandle);
}
bool FileDataStream::eof(void)
{
	if (!IsOpen())return false;
	return feof(mFileHandle) != 0;
}
void FileDataStream::Flush()
{
	if (!IsOpen())return;
	fflush(mFileHandle);
#if defined(DC_PLATFORM_LINUX) || defined(DC_PLATFORM_ANDROID) || defined(DC_PLATFORM_MAC) || defined(DC_PLATFORM_IOS)
	int fd = fileno(mFileHandle);  //由文件流获取文件描述符
	if(fd != -1)fsync(fd);
#endif
}
void FileDataStream::Close(void)
{
	if (mFileHandle != nullptr)
	{
		fclose(mFileHandle);
		mFileHandle = nullptr;
	}
}
DC_END_NAMESPACE

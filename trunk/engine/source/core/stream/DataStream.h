 
/*****************************************************************************
* Author： hannibal
* Date：2009/11/22
* Description： 数据流
*****************************************************************************/
#pragma once

#include "core/Object.h"

DC_BEGIN_NAMESPACE
/********************************************************************/
// ClassName：DataStream
// Description：数据流基类
class ENGINE_DLL DataStream : public Object
{
	BEGIN_DERIVED_REFECTION_TYPE(DataStream, Object)
	END_REFECTION_TYPE;

public:
	DataStream() : mSize(0) {}
	DataStream(const DataStream& other) :mSize(other.mSize) {}
	DataStream(DataStream&& other) :mSize(other.mSize) {}
	virtual ~DataStream() {}

public:
	//- Description：读取指定数量的字节到buf中
	// @par：buf - 存储读取的字符
	// @par：buf - 读取数量
	// @return：实际读取的字节数
	virtual int Read(void* buf, int count) noexcept = 0;

	//- Description：写入指定数量的字节到文件中
	// @par：buf - 存储字符
	// @par：buf - 写入数量
	// @return：实际写入的字节数
	virtual int Write(const void* buf, uint offset, int count) noexcept = 0;

	//- Description：跳过指定字节
	// @par：count - 跳过的字节数，可以为负值
	// @return：
	virtual void Skip(int count) noexcept = 0;

	//- Description：定位到指定的位置
	// @par：
	// @return：
	virtual void Seek(int pos, bool relative = false) noexcept = 0;

	//- Description：返回当前流的位置
	// @par：
	// @return：
	virtual uint Position(void) noexcept = 0;

	//- Description：是否在文件未
	// @par：
	// @return：
	virtual bool eof(void) noexcept = 0;

	//- Description：清空流
	// @par：
	// @return：
	virtual void Flush() noexcept = 0;

	//- Description：关闭流
	// @par：
	// @return：
	virtual void Close(void) noexcept = 0;

public:
	uint Size(void) const noexcept { return mSize; }

protected:
	uint mSize = 0;
};//DataStream

/********************************************************************/
// ClassName：MemoryDataStream
// Description：处理已经存在的内存
class ENGINE_DLL MemoryDataStream final : public DataStream
{
	DEFAULT_CREATE(MemoryDataStream);
	FRIEND_CONSTRUCT_DESTRUCT(MemoryDataStream);
	BEGIN_DERIVED_REFECTION_TYPE(MemoryDataStream, DataStream)
	END_REFECTION_TYPE;

public:
	explicit MemoryDataStream(uint size = 0);
	MemoryDataStream(byte* buffer, uint size);
	MemoryDataStream(const MemoryDataStream& other);
	MemoryDataStream(MemoryDataStream&& other);
	~MemoryDataStream();

	MemoryDataStream& operator =(const MemoryDataStream& other) noexcept;
	MemoryDataStream& operator =(MemoryDataStream&& other) noexcept;
	byte& operator [](int index) noexcept { return mData[index]; }
	const byte& operator [](int index) const noexcept { return mData[index]; }

public:
	virtual int Read(void* buf, int count) noexcept override;
	virtual int Write(const void* buf, uint offset, int count) noexcept override;
	virtual void Skip(int count) noexcept override;
	virtual void Seek(int pos, bool relative = false) noexcept override;
	virtual uint Position(void) noexcept override;
	virtual bool eof(void) noexcept override;
	virtual void Flush() noexcept override {}
	virtual void Close(void) noexcept override;

public:
	template<class T>
	T Read() noexcept
	{
		T t;
		Read((void*)&t, sizeof(T));
		return t;
	}
	String ReadString(int cnt) noexcept
	{
		if (mPos + cnt > mSize)
			cnt = mSize - mPos;
		if (cnt <= 0)
			return "";
		char* span = reinterpret_cast<char*>(mData + mPos);
		mPos += cnt;
		return String(span, cnt);
	}

	template<class T>
	void Write(const T& t) noexcept
	{
		Write((void*)&t, 0, sizeof(T));
	}

	byte* Buffer()const noexcept { return mData; }
	byte* CurrBuffer()const noexcept { return mData + mPos; }

	void Resize(uint size) noexcept;

protected:
	byte* mData = nullptr;		//开始位置
	uint mPos = 0;				//当前位置
};//MemoryDataStream

/********************************************************************/
// ClassName：FileDataStream
// Description：文件处理
class ENGINE_DLL FileDataStream final : public DataStream
{
	DEFAULT_CREATE(FileDataStream);
	FRIEND_CONSTRUCT_DESTRUCT(FileDataStream);
	BEGIN_DERIVED_REFECTION_TYPE(FileDataStream, DataStream)
	END_REFECTION_TYPE;

public:
	explicit FileDataStream(FILE* handle);
	//r		以只读方式打开文件，该文件必须存在。
	//r+	以可读写方式打开文件，该文件必须存在。
	//rb+	读写打开一个二进制文件，只允许读写数据。
	//rt+	读写打开一个文本文件，允许读和写。
	//w		打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
	//w+	打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
	//a		以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
	//a+	以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）
	//wb	只写打开或新建一个二进制文件；只允许写数据。
	//wb+	读写打开或建立一个二进制文件，允许读和写。
	//wt+	读写打开或着建立一个文本文件；允许读写。
	//at+	读写打开一个文本文件，允许读或在文本末追加数据。
	//ab+	读写打开一个二进制文件，允许读或在文件末追加数据。
	FileDataStream(const String& path, const String& mode);
	FileDataStream(const FileDataStream& other) :DataStream(other) {}
	FileDataStream(FileDataStream&& other) :DataStream(other) {}
	~FileDataStream();

public:
	/*
	linux对IO文件的操作分为：
	不带缓存：open  read。posix标准，在用户空间没有缓冲，在内核空间还是进行了缓存的。数据-----内核缓存区----磁盘
		假设内核缓存区长度为100字节，你调用 ssize_twrite (int fd,const void * buf,size_tcount);
		写操作时，设每次写入count=10字节，那么你要调用10次这个函数才能把这个缓存区写满，没写满时数据还是在内核缓冲区中，
		并没有写入到磁盘中，内核缓存区满了之后或者执行了fsync（强制写入硬盘）之后，才进行实际的IO操作，吧数据写入磁盘上。
	带缓存区：fopen fwrite fget等，是c标准库中定义的。数据-----流缓存区-----内核缓存区----磁盘。
		假设流缓存区长度为50字节，内核缓存区100字节，我们用标准c库函数fwrite()将数据写入到这个流缓存中，每次写10字节，
		需要写5次流缓存区满后调用write()(或 调用 fflush() )，将数据写到内核缓存区，直到内核缓存区满了之后或者执行了fsync（强制写入硬盘）之后，
		才进行实际的IO操作，把数据写入磁盘上。标准IO操作fwrite()最后还是要调用无缓存IO操作write
	*/
	/*
	以fgetc / fputc 为例,当用户程序第一次调用fgetc 读一个字节时,fgetc 函数可能通过系统调用进入内核读1K字节到I/O缓冲区中,然后返回I/O缓冲区中的第一个字节给用户,
		把读写位置指向I/O缓冲区中的第二个字符,以后用户再调fgetc ,就直接从I/O缓冲区中读取,而不需要进内核了,
		当用户把这1K字节都读完之后,再次调用fgetc 时,fgetc 函数会再次进入内核读1K字节到I/O缓冲区中。
		在这个场景中用户程序、C标准库和内核之间的关系就像在“Memory Hierarchy”中CPU、Cache和内存之间的关系一样,C标准库之所以会从内核预读一些数据放在I/O缓冲区中,
		是希望用户程序随后要用到这些数据,C标准库的I/O缓冲区也在用户空间,直接从用户空间读取数据比进内核读数据要快得多。
	另一方面,用户程序调用fputc 通常只是写到I/O缓 冲区中,这样fputc函数可以很快地返回,如果I/O缓冲区写满了,fputc 就通过系统调用把I/O缓冲区中的数据传给内核,内核最终把数据写回磁盘或设备。
		有时候用户程序希望把I/O缓冲区中的数据立刻传给内核,让内核写回设备或磁盘,这称为Flush操作,对应的库函数是fflush,fclose函数在关闭文件之前也会做Flush操作。
	*/
	virtual int Read(void* buf, int count) noexcept override;
	virtual int Write(const void* buf, uint offset, int count) noexcept override;

	virtual void Skip(int count) noexcept override;
	virtual void Seek(int pos, bool relative = false) noexcept override;
	virtual uint Position(void) noexcept override;
	virtual bool eof(void) noexcept override;
	/*
		传统的UNIX实现在内核中设有缓冲区、高速缓存或页面高速缓存，大多数磁盘I/O都通过缓冲进行。
	当将数据写入文件时，内核通常先将该数据复制到其中一个缓冲区中，如果该缓冲区尚未写满，则并不将其排入输出队列，
	而是等待其写满或者当内核需要重用该缓冲区以便存放其他磁盘块数据时，再将该缓冲排入输出队列，然后待其到达队首时，才进行实际的I/O操作。
	假设内核一直没重用该缓存，且缓存一直没写满，是不是就存在一个大大大的BUG呢？显然这种情况是不允许的，为了解决这个问题，系统有一个叫update守护进程定期会执行sync
		延迟写减少了磁盘读写次数，但是却降低了文件内容的更新速度，使得欲写到文件中的数据在一段时间内并没有写到磁盘上。
	当系统发生故障时，这种延迟可能造成文件更新内容的丢失。为了保证磁盘上实际文件系统与缓冲区高速缓存中内容的一致性，UNIX系统提供了 sync、fsync和fdatasync三个函数。
	（1）sync  函数只是将所有修改过的块缓冲区排入写队列，然后就返回，它并不等待实际写磁盘操作结束。通常称为update的系统守护进程会周期性地（一般每隔30秒）调用sync函数。这就保证了定期冲洗内核的块缓冲区。命令sync(1)也调用sync函数。
	（2）fsync  函数是系统提供的系统调用。只对由文件描述符filedes指定的单一文件起作用，并且等待写磁盘操作结束，然后返回。fsync可用于数据库这样的应用程序，这种应用程序需要确保将修改过的块立即写到磁盘上。接受的时一个Int型的文件描述符。
	（3）fdatasync  函数类似于fsync，但它只影响文件的数据部分。而除数据外，fsync还会同步更新文件的属性。
	（4）fflush：标准I / O函数（如：fread，fwrite）会在内存建立缓冲，该函数刷新内存缓冲，将内容写入内核缓冲，要想将其写入磁盘，还需要调用fsync（先调用fflush后调用fsync，否则不起作用）。fflush接受一个参数FILE * 。
	c库缓冲----- fflush-------〉内核缓冲--------fsync-----〉磁盘
	*/
	virtual void Flush() noexcept override;
	virtual void Close(void) noexcept override;

	template<class T>
	T Read() noexcept
	{
		T t;
		Read((void*)&t, sizeof(T));
		return t;
	}

public:
	bool IsOpen() noexcept;
	FILE* GetHandle()const noexcept { return mFileHandle; }

protected:
	FILE* mFileHandle = nullptr;
};//FileDataStream
DC_END_NAMESPACE

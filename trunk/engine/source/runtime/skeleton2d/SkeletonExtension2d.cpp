#include "SkeletonExtension2d.h"
#include "runtime/graphics/null/Texture.h"
USING_NAMESPACE_DC;
namespace spine
{
	class SP_API SpineAllocExtension : public SpineExtension {
	public:
		SpineAllocExtension() : SpineExtension() {}
		virtual ~SpineAllocExtension() {}

	protected:
		virtual void *_alloc(size_t size, const char *file, int line)override
		{
			SP_UNUSED(file);
			SP_UNUSED(line);

			if (size == 0)
				return 0;
			void *ptr = Memory::Alloc(size);
			return ptr;
		}
		virtual void *_calloc(size_t size, const char *file, int line)override
		{
			SP_UNUSED(file);
			SP_UNUSED(line);

			if (size == 0)
				return 0;

			void *ptr = Memory::Alloc(size);
			if (ptr) {
				memset(ptr, 0, size);
			}
			return ptr;
		}
		virtual void *_realloc(void *ptr, size_t size, const char *file, int line)override
		{
			SP_UNUSED(file);
			SP_UNUSED(line);

			void *mem = NULL;
			if (size == 0)
				return 0;
			if (ptr == NULL)
				mem = Memory::Alloc(size);
			else
				mem = Memory::Realloc(ptr, size);
			return mem;
		}
		virtual void _free(void *mem, const char *file, int line)override
		{
			SP_UNUSED(file);
			SP_UNUSED(line);

			Memory::Free(mem);
		}
		virtual char *_readFile(const String &path, int *length)override
		{
			char *data;
			FILE *file = fopen(path.buffer(), "rb");
			if (!file) return 0;

			fseek(file, 0, SEEK_END);
			*length = (int)ftell(file);
			fseek(file, 0, SEEK_SET);

			data = SpineExtension::Alloc<char>(*length, __FILE__, __LINE__);
			fread(data, 1, *length, file);
			fclose(file);

			return data;
		}
	};

	static SpineAllocExtension extension;
	SpineExtension* getDefaultExtension()
	{
		return &extension;
	}
} 
DC_BEGIN_NAMESPACE
/********************************************************************/
static TextureFilter GetFilter(spine::TextureFilter filter)
{
	switch (filter) 
	{
	case spine::TextureFilter_Unknown:
		return TextureFilter::Bilinear;
	case spine::TextureFilter_Nearest:
		return TextureFilter::Nearest;
	case spine::TextureFilter_Linear:
		return TextureFilter::Bilinear;
	case spine::TextureFilter_MipMap:
	case spine::TextureFilter_MipMapNearestNearest:
	case spine::TextureFilter_MipMapLinearNearest:
	case spine::TextureFilter_MipMapNearestLinear:
	case spine::TextureFilter_MipMapLinearLinear:
		return TextureFilter::Trilinear;
	default:
		return TextureFilter::Bilinear;
	}
}
static TextureAddress GetAddress(spine::TextureWrap wrap)
{
	switch (wrap)
	{
	case spine::TextureWrap_MirroredRepeat:
		return TextureAddress::Mirror;
	case spine::TextureWrap_ClampToEdge:
		return TextureAddress::Clamp;
	case spine::TextureWrap_Repeat:
		return TextureAddress::Wrap;
	default:
		return TextureAddress::Clamp;
	}
}
void SpineTextureLoader::load(spine::AtlasPage& page, const spine::String& path)
{
	String filePath = String(path.buffer());
	filePath = Resource::GetRelativePath(filePath);
	TextureDesc desc;
	desc.address = GetAddress(page.uWrap); desc.filter = GetFilter(page.minFilter);
	desc.flags |= TextureFlag::sRGB;
	Texture* tex = Texture::Create2D(filePath, desc);
	tex->Retain();
	page.setRendererObject(tex);
	page.width = tex->GetWidth();
	page.height = tex->GetHeight();
}
void SpineTextureLoader::unload(void* texture)
{
	Texture* tex = reinterpret_cast<Texture*>(texture);
	SAFE_RELEASE(tex);
}
DC_END_NAMESPACE

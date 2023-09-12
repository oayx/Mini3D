
include_directories(source/external/directxtex)

file(GLOB_RECURSE ENGINE_EXTERNAL_DDSCONV_SRC "source/external/ddsconv/*.cpp")

set(external_ddsconv "source/external/ddsconv/*")
source_group("external/ddsconv" REGULAR_EXPRESSION "${external_ddsconv}" )

file(GLOB_RECURSE ENGINE_EXTERNAL_ICONV_SRC "source/external/iconv/win_iconv.c")

set(external_iconv "source/external/iconv/*")
source_group("external/iconv" REGULAR_EXPRESSION "${external_iconv}" )
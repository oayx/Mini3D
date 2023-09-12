include_directories(
	${PROJECT_SOURCE_DIR}/source/external/assimp
)
file(GLOB_RECURSE ENGINE_EXTERNAL_ASSIMP_SRC "source/external/assimp/*.cpp")

set(external_assimp "source/external/assimp/*")
source_group("external/assimp" REGULAR_EXPRESSION "${external_assimp}" )
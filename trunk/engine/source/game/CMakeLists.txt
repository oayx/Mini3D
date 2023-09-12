
file(GLOB_RECURSE ENGINE_GAME_SRC
	source/game/*.cpp
)

set(runtime "source/game/")
source_group("game" REGULAR_EXPRESSION "${game}" )
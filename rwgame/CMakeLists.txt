find_package(Boost COMPONENTS filesystem REQUIRED)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)

set(RWGAME_SOURCES
	GitSHA1.h
	"${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp"

	main.cpp

	RWGame.cpp

	GameConfig.cpp
	GameWindow.cpp

	State.cpp

	states/LoadingState.hpp
	states/LoadingState.cpp
	states/IngameState.hpp
	states/IngameState.cpp
	states/PauseState.hpp
	states/PauseState.cpp
	states/MenuState.hpp
	states/MenuState.cpp
	states/DebugState.hpp
	states/DebugState.cpp
	states/BenchmarkState.hpp
	states/BenchmarkState.cpp
	
	DrawUI.cpp
)

add_executable(rwgame
	${RWGAME_SOURCES})

include_directories(SYSTEM
	${BULLET_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIR}
	)
include_directories(
	"${CMAKE_SOURCE_DIR}/rwgame"
	"${Boost_INCLUDE_DIRS}"
)
include_directories(
	"${CMAKE_SOURCE_DIR}/rwengine/include"
)

target_link_libraries(rwgame
	rwengine
	inih
	${OPENGL_LIBRARIES}
	${BULLET_LIBRARIES}
	${SDL2MAIN_LIBRARY}
	${SDL2_LIBRARY}
	${Boost_LIBRARIES}
	)

if(MSVC) # Check if we are using the Visual Studio compiler
  set_target_properties(rwgame PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif()

if(MINGW)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
	add_definitions(-D _USE_MATH_DEFINES)
	target_link_libraries(rwgame
	iconv
	mman)
endif()

install(TARGETS rwgame RUNTIME DESTINATION "${BIN_DIR}")

set(CMAKE_SYSTEM_NAME Emscripten)

set(CMAKE_CROSSCOMPILING TRUE)

if (WIN32)
	message("It's a windows")
	set(COMPILER_SUFFIX .bat)
elseif(UNIX)
	message("It's a unix")
	set(COMPILER_SUFFIX .sh)
endif()


#set(WIN32)
#set(UNIX 1)

set(CMAKE_C_COMPILER "emcc${COMPILER_SUFFIX}")
set(CMAKE_CXX_COMPILER "em++${COMPILER_SUFFIX}")

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" Clang)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" Clang)

set(CMAKE_C_COMPILER "emcc${COMPILER_SUFFIX}")
set(CMAKE_CXX_COMPILER "em++${COMPILER_SUFFIX}")

message(${CMAKE_C_COMPILER})
message(${CMAKE_CXX_COMPILER})
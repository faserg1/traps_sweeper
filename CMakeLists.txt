cmake_minimum_required(VERSION 3.8)
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
project(App NONE)
else()
project(App C CXX)
endif()
set (CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE source_list  "src/*.cpp")  

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_executable(App main.cpp ${source_list}) 
else()
	add_executable(App WIN32 main.cpp ${source_list}) 
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
target_compile_options(App PRIVATE
	-Wno-unused-variable
	-Wno-unused-private-field
)
endif()

     
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
add_subdirectory(extern/fmt)
include_directories(App SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmt/include")
target_link_libraries(App fmt::fmt-header-only)

target_compile_options(App PRIVATE "-std=c++17")
target_compile_options(App PRIVATE "-s\ " "WASM=1")
target_compile_options(App PRIVATE "-s\  " "USE_SDL=2")
target_compile_options(App PRIVATE "-s\   " "USE_SDL_IMAGE=2")
target_compile_options(App PRIVATE "-s\    " "USE_SDL_TTF=2")
target_compile_options(App PRIVATE "-s\     " "USE_FREETYPE=1")
target_compile_options(App PRIVATE "-s\      " "SDL2_IMAGE_FORMATS=[\"png\",\"jpg\"]")
target_compile_options(App PRIVATE "-s\       " "USE_WEBGL2=1")
set(USE_LINKER_OPTS "-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_FREETYPE=1 -s USE_WEBGL2=1 -s SDL2_IMAGE_FORMATS=[\"png\",\"jpg\"]")
set(DEBUG_GL_OPTS "-s GL_DEBUG=1 -s GL_ASSERTIONS=1")
set(DEBUG_OPTS "-g4 --source-map-base /")
set(CMAKE_EXE_LINKER_FLAGS "${USE_LINKER_OPTS} ${DEBUG_OPTS} ${DEBUG_GL_OPTS} -v")
else()
find_package(Freetype REQUIRED)
list(APPEND EXTERN_INCLUDES ${FREETYPE_INCLUDE_DIRS})
include_directories(App SYSTEM ${FREETYPE_INCLUDE_DIRS})  
target_link_libraries(App ${FREETYPE_LIBRARIES})   
 
find_package(SDL2 REQUIRED)   
list(APPEND EXTERN_INCLUDES ${SDL2_INCLUDE_DIRS})
include_directories(App SYSTEM ${SDL2_INCLUDE_DIRS})  
target_link_libraries(App ${SDL2_LIBRARIES})  

 
find_package(SDL2_image REQUIRED)  
list(APPEND EXTERN_INCLUDES ${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(App SYSTEM ${SDL2_IMAGE_INCLUDE_DIRS}) 
target_link_libraries(App ${SDL2_IMAGE_LIBRARIES})   
 
find_package(OPENGL REQUIRED) 
list(APPEND EXTERN_INCLUDES ${OPENGL_INCLUDE_DIRS})
include_directories(App SYSTEM ${OPENGL_INCLUDE_DIRS}) 
target_link_libraries(App ${OPENGL_LIBRARIES})
 
find_package(GLEW REQUIRED) 
list(APPEND EXTERN_INCLUDES ${GLEW_INCLUDE_DIR})
include_directories(App SYSTEM ${GLEW_INCLUDE_DIR}) 
target_link_libraries(App ${GLEW_LIBRARY})



add_subdirectory(extern/fmt)
list(APPEND EXTERN_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmt/include")
include_directories(App SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/extern/fmt/include") 
target_link_libraries(App fmt::fmt-header-only)
endif()

#include_directories(App EXTERN_INCLUDES)

# 
if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
# Nothing to do
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
target_compile_options(App PRIVATE
	/imsvc C:/Users/admin/Dropbox/programing/c++/cpp_engine/extern/SDL2/include #unfortunately this only works for a single directory, not a bunch of them -.-
)
endif()

foreach(dir ${EXTERN_INCLUDES})
message(STATUS "extern dir='${dir}'")

endforeach()

include_directories("${PROJECT_SOURCE_DIR}")

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
set_target_properties(App PROPERTIES SUFFIX ".html")
else()
get_filename_component(SDL_LIB_PATH ${PROJECT_SOURCE_DIR}/extern/SDL2 ABSOLUTE)
add_custom_command(TARGET App POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."  
	"${SDL_LIB_PATH}/lib/x64/SDL2.dll"# <--this is in-file
    $<TARGET_FILE_DIR:App>      # <--this is out-file path
)                 

get_filename_component(SDL2_IMAGE_DLLS_PATH "${SDL2_IMAGE_LIBRARIES}" DIRECTORY)
file(GLOB SDL2_IMAGE_DLLS "${SDL2_IMAGE_DLLS_PATH}/*.dll")
foreach(file ${SDL2_IMAGE_DLLS})
add_custom_command(TARGET App POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	${file}
    $<TARGET_FILE_DIR:App>      
)      
endforeach() 

get_filename_component(FREETYPE_DLLS_PATH "${FREETYPE_LIBRARIES}" DIRECTORY)
file(GLOB FREETYPE_DLLS "${FREETYPE_DLLS_PATH}/*.dll")
foreach(file ${FREETYPE_DLLS})
add_custom_command(TARGET App POST_BUILD        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
	${file}
    $<TARGET_FILE_DIR:App>      
)      
endforeach() 

add_custom_command(TARGET App
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
    -Dsrc_dir="${CMAKE_SOURCE_DIR}/assets"
    -Ddst_dir=$<TARGET_FILE_DIR:App>/assets
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/update_directory.cmake"
)
endif()
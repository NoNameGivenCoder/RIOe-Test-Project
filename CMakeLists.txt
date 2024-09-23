cmake_minimum_required(VERSION 3.10)
project(helloworld_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(GLEW_INCLUDE_DIR "glew/include")
set(GLEW_LIB_DIR "glew/lib/Release/x64")

set(GLFW_INCLUDE_DIR "glfw/include")
set(GLFW_LIB_DIR "glfw/lib-vc2022")

include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIR})

include_directories(lib/rio/include)
include_directories(include)
include_directories(lib/rio-e/include)
include_directories(lib/yaml-cpp/include)
include_directories(lib/tinygltf)

link_directories(${GLEW_LIB_DIR})
link_directories(${GLFW_LIB_DIR})

file(GLOB_RECURSE SOURCES src/main.cpp src/*.cpp include/*.h)
file(GLOB_RECURSE RIO_SOURCES lib/rio/src/*.cpp lib/rio/include/*.h)
file(GLOB_RECURSE RIOE_SOURCES lib/rio-e/src/*.cpp lib/rio-e/include/*.h)
file(GLOB_RECURSE YAML_SOURCES lib/yaml-cpp/src/*.cpp lib/yaml-cpp/include/*.h)

add_definitions(-DRIO_DEBUG -DYAML_CPP_STATIC_DEFINE -D_WIN32)

add_executable(helloworld_cpp ${RIO_SOURCES} ${SOURCES} ${RIOE_SOURCES} ${YAML_SOURCES} ${GLTF_SOURCES})

target_link_libraries(helloworld_cpp
    opengl32
    glew32
    glfw3
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET helloworld_cpp PROPERTY CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
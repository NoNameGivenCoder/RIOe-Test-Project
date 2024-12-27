cmake_minimum_required(VERSION 3.22)
project(rioe_testapp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

option(build_wiiu "Building for Wii U" OFF)

if(build_wiiu)
    include(${CMAKE_SOURCE_DIR}/WiiUBuildConfig.cmake)

    message(STATUS "Building for Wii U")

    message(STATUS "Title Name: ${LONG_NAME}")
    message(STATUS "Product Code: ${PRODUCT_CODE}")
    message(STATUS "Version: ${VERSION}")
    message(STATUS "Title ID: ${TITLE_ID}")

    add_definitions(-D__WIIU__ -D__WUT__ -D__powerpc__ -DRIO_CAFE_MAIN_FILE_DEVICE_AS_CONTENT)

    # curl + zlib include directories
    include_directories("${DEVKITPRO}/portlibs/ppc/include")
    include_directories("${DEVKITPRO}/portlibs/wiiu/include")
else()
    include(${CMAKE_SOURCE_DIR}/PCBuildConfig.cmake)

    message(STATUS "Building for PC")

    message(STATUS "Window Name: ${WINDOW_NAME}")
    message(STATUS "FFL MLC Path: ${FFL_MLC_PATH}")

    # Configure the target binary to build outside of the build directory.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../") # Main source directory.

    if(MSVC) # MSVC/Visual Studio needs a specific tweak to emit to the source dir
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES}) # Iterate through each config type (Debug, Release)
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG_UPPER)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG_UPPER} "${CMAKE_SOURCE_DIR}")
        endforeach()
    endif()

    link_directories(${GLEW_LIB_DIR})
    link_directories(${GLFW_LIB_DIR})

    include_directories(${GLFW_INCLUDE_DIR})
    include_directories(${GLEW_INCLUDE_DIR})

    add_definitions(-DFFL_NO_DATABASE_FILE)

    add_compile_definitions(FFL_MLC_PATH="${FFL_MLC_PATH}")
    add_compile_definitions(WINDOW_NAME="${WINDOW_NAME}")

    set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Disable Zlib Examples" FORCE)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib-1.3.1")

    set(ZLIB_LIBRARY "zlibstatic")
    set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/zlib-1.3.1")
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE RIO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/rio/src/*.cpp")
file(GLOB_RECURSE RIOE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/rio-e/src/*.cpp")
file(GLOB_RECURSE FFL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/ffl/src/*.cpp")

if (build_wiiu)
    file(GLOB_RECURSE IMGUI_SOURCES lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/backends/wiiu/imgui_impl_gx2.cpp lib/imgui/backends/wiiu/imgui_impl_wiiu.cpp)
else()
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/ninTexUtils/include")
    file(GLOB_RECURSE NINTEXUTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/ninTexUtils/src/ninTexUtils/gx2/gx2Surface.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/lib/ninTexUtils/src/ninTexUtils/gx2/tcl/addrlib.cpp")

    file(GLOB_RECURSE IMGUI_SOURCES lib/imgui/imgui.cpp lib/imgui/imgui_demo.cpp lib/imgui/imgui_draw.cpp lib/imgui/imgui_tables.cpp lib/imgui/imgui_widgets.cpp lib/imgui/backends/imgui_impl_opengl3.cpp lib/imgui/backends/imgui_impl_glfw.cpp)
endif()

add_executable(rioe_testapp ${SOURCES} ${RIO_SOURCES} ${RIOE_SOURCES} ${FFL_SOURCES} ${IMGUI_SOURCES} ${NINTEXUTILS_SOURCES})

add_definitions(-DRIO_DEBUG -DNOMINMAX)

target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/rio/include")
target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/rio-e/include")
target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui")
target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/ffl/include")
target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb")

set(TINYGLTF_HEADER_ONLY ON CACHE BOOL "Enable Header Only" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "Turning off loader examples" FORCE)
set(TINYGLTF_INSTALL OFF CACHE BOOL "Turning off install" FORCE)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/tinygltf")
target_link_libraries(rioe_testapp PRIVATE tinygltf)

# set(KTX_FEATURE_GL_UPLOAD OFF CACHE BOOL "No GL Upload" FORCE)
# set(KTX_FEATURE_VK_UPLOAD OFF CACHE BOOL "No VK Upload" FORCE)
# set(KTX_FEATURE_TESTS OFF CACHE BOOL "No Tests" FORCE)
# set(KTX_FEATURE_STATIC_LIBRARY ON CACHE BOOL "")
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/KTX-Software")
# target_link_libraries(rioe_testapp PRIVATE ktx_read)
# target_include_directories(rioe_testapp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/KTX-Software/include")

if (build_wiiu)
    target_link_libraries(rioe_testapp PRIVATE libcurl.a libz.a) 

    wut_create_rpx(rioe_testapp)

    # [app_name].rpx
    add_custom_command(TARGET rioe_testapp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/fs/code"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/rioe_testapp.rpx" "${CMAKE_BINARY_DIR}/fs/code/"
    )

    # iconTex.tga
    add_custom_command(TARGET rioe_testapp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/fs/meta"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/wiiu_config/iconTex.tga" "${CMAKE_BINARY_DIR}/fs/meta/"
    )

    # bootDrcTex.tga
    add_custom_command(TARGET rioe_testapp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/fs/meta"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/wiiu_config/bootDrcTex.tga" "${CMAKE_BINARY_DIR}/fs/meta/"
    )

    # bootTvTex.tga
    add_custom_command(TARGET rioe_testapp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/fs/meta"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/wiiu_config/bootTvTex.tga" "${CMAKE_BINARY_DIR}/fs/meta/"
    )

    # meta.xml
    configure_file(
        ${CMAKE_SOURCE_DIR}/wiiu_config/meta.config.xml
        ${CMAKE_BINARY_DIR}/fs/meta/meta.xml
        @ONLY
    )

    # app.xml
    configure_file(
        ${CMAKE_SOURCE_DIR}/wiiu_config/app.config.xml
        ${CMAKE_BINARY_DIR}/fs/code/app.xml
        @ONLY
    )

    # cos.xml
    configure_file(
        ${CMAKE_SOURCE_DIR}/wiiu_config/cos.config.xml
        ${CMAKE_BINARY_DIR}/fs/code/cos.xml
        @ONLY
    )
else()
    target_link_libraries(rioe_testapp PRIVATE opengl32 glew32 glfw3 zlibstatic)

    if (CMAKE_VERSION VERSION_GREATER 3.12)
        set_property(TARGET rioe_testapp PROPERTY CXX_STANDARD 20)
    else()
        set(CMAKE_CXX_STANDARD 20)
    endif()
endif()
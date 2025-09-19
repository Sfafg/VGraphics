cmake_minimum_required(VERSION 3.10.0)
project(VGraphics)

option(VGRAPHICS_BUILD_TESTS "Build tests" OFF)

set(TESTS_ROOT "${PROJECT_SOURCE_DIR}/tests")
set(SRC_ROOT "${PROJECT_SOURCE_DIR}/src")
set(RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/resources")
set(RESOURCES_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
set(SHADER_ROOT "${SRC_ROOT}/shaders")
set(SHADER_OUTPUT_DIR "${RESOURCES_OUTPUT_DIR}/shaders")

add_compile_options(-static-libstdc++)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer -g")
# set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

file(MAKE_DIRECTORY ${TESTS_ROOT} ${SRC_ROOT} ${RESOURCES_ROOT} ${RESOURCES_OUTPUT_DIR} ${SHADER_ROOT} ${SHADER_OUTPUT_DIR})

# Library
find_package(Vulkan REQUIRED)
file(GLOB_RECURSE SRC "${SRC_ROOT}/*.cpp")

list(REMOVE_ITEM SRC
    ${SRC_ROOT}/Instance.cpp
    ${SRC_ROOT}/Device.cpp
)
list(PREPEND SRC
    ${SRC_ROOT}/Instance.cpp
    ${SRC_ROOT}/Device.cpp
)
add_library(VGraphics STATIC)
target_link_libraries(VGraphics PUBLIC Vulkan::Vulkan)
target_sources(VGraphics
    PRIVATE
        ${SRC}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS src
        FILES src/VG/VG.h
)
target_include_directories(VGraphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

if(VGRAPHICS_BUILD_TESTS)
# SHADERS
    find_program(GLSLC_EXECUTABLE glslc)
    if(NOT GLSLC_EXECUTABLE)
        message(FATAL_ERROR "glslc not found. Set GLSLC_EXECUTABLE manually or add to PATH.")
    endif()

    file(GLOB_RECURSE SHADERS "${SHADER_ROOT}/*.vert" "${SHADER_ROOT}/*.frag" "${SHADER_ROOT}/*.comp")
    foreach(SHADER ${SHADERS})
        string(REPLACE "${SHADER_ROOT}/" "" SHADER_NAME "${SHADER}")
        add_custom_command(
            OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv"
            COMMAND "${GLSLC_EXECUTABLE}" "${SHADER}" -o "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv"
            DEPENDS "${SHADER}"
            COMMENT "Compiling Shader: ${SHADER_NAME}.spv"
        )
        list(APPEND SHADER_DEPEND "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")
    endforeach()

    add_custom_target(VGRAPHICS_Shaders ALL DEPENDS ${SHADER_DEPEND})

# RESOURCES
    file(GLOB_RECURSE RESOURCES "${RESOURCES_ROOT}/*.*")

    foreach(RESOURCE ${RESOURCES})
        string(REPLACE "${RESOURCES_ROOT}/" "" RESOURCE_NAME "${RESOURCE}")
        add_custom_command(
            OUTPUT "${RESOURCES_OUTPUT_DIR}/${RESOURCE_NAME}"
            COMMAND ${CMAKE_COMMAND} -E copy "${RESOURCE}" "${RESOURCES_OUTPUT_DIR}/${RESOURCE_NAME}"
            DEPENDS "${RESOURCE}"
            COMMENT "Copying resource: ${RESOURCE_NAME}"
        )
        list(APPEND RESOURCE_DEPEND "${RESOURCES_OUTPUT_DIR}/${RESOURCE_NAME}")
    endforeach()

    add_custom_target(VGRAPHICS_Resources ALL DEPENDS ${RESOURCE_DEPEND})

# GLFW
    include(FetchContent)
    FetchContent_Declare(
      glfw
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG master
      CMAKE_CACHE_ARGS
        -DGLFW_BUILD_WAYLAND:BOOL=ON
        -DGLFW_BUILD_X11:BOOL=OFF
        -DGLFW_BUILD_DOCS:BOOL=OFF
    )
    FetchContent_MakeAvailable(glfw)

    FetchContent_Declare(
      stb
      GIT_REPOSITORY https://github.com/nothings/stb.git
      GIT_TAG master
    )
    FetchContent_MakeAvailable(stb)
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE
      ${stb_SOURCE_DIR}
    )

    FetchContent_Declare(
      glm
      GIT_REPOSITORY https://github.com/g-truc/glm.git
      GIT_TAG master
    )
    FetchContent_MakeAvailable(glm)

# TEST
    add_executable(VGRAPHICS_VGTest ${TESTS_ROOT}/Source.cpp)
    target_link_libraries(VGRAPHICS_VGTest PRIVATE VGraphics glfw glm stb_image)
    add_dependencies(VGRAPHICS_VGTest VGRAPHICS_Shaders VGraphics VGRAPHICS_Resources)

# WARIACJE
    add_executable(VGRAPHICS_Wariacje ${TESTS_ROOT}/Wariacje.cpp)
    target_link_libraries(VGRAPHICS_Wariacje PRIVATE VGraphics glfw glm stb_image)
    add_dependencies(VGRAPHICS_Wariacje VGRAPHICS_Shaders VGraphics)
endif()

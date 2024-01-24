cmake_minimum_required (VERSION 3.26.4)
project (VGraphics)
set(CMAKE_CXX_STANDARD 20)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON) 

add_subdirectory("External/glfw-3.3.8/")
set(TESTS_ROOT ${PROJECT_SOURCE_DIR}/tests)
set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(SHADER_ROOT ${SRC_ROOT}/shaders)
set(SHADER_COMPILER_DIR C:/VulkanSDK/1.3.261.1/Bin/glslc.exe)

set(SRC
    ${TESTS_ROOT}/Source.cpp
    ${SRC_ROOT}/Instance.cpp
    ${SRC_ROOT}/Device.cpp
    ${SRC_ROOT}/Queue.cpp
    ${SRC_ROOT}/Surface.cpp
    ${SRC_ROOT}/Swapchain.cpp
    ${SRC_ROOT}/Shader.cpp
    ${SRC_ROOT}/GraphicsPipeline.cpp
    ${SRC_ROOT}/Subpass.cpp
    ${SRC_ROOT}/RenderPass.cpp
    ${SRC_ROOT}/Attachment.cpp
    ${SRC_ROOT}/Framebuffer.cpp
    ${SRC_ROOT}/CommandBuffer.cpp
    ${SRC_ROOT}/MySemaphore.cpp
    ${SRC_ROOT}/Fence.cpp
    ${SRC_ROOT}/Surface.cpp
)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options("-O3")
    add_compile_options("-mwindows")
endif()
add_compile_options("-pthread")

add_executable (VGraphics ${SRC})
# target_link_options(VGraphics PRIVATE "-mwindows")
target_include_directories(VGraphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_include_directories(VGraphics PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_include_directories(VGraphics PRIVATE "C:/VulkanSDK/1.3.261.1/include/")
target_link_directories(VGraphics PRIVATE "C:/VulkanSDK/1.3.261.1/lib/" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
target_link_libraries(VGraphics PRIVATE glfw3 vulkan-1)
cmake_minimum_required (VERSION 3.23.3)
project (VGraphics)
set(CMAKE_CXX_STANDARD 23)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
 
add_subdirectory("External/glfw-3.3.8/")
set(TESTS_ROOT ${PROJECT_SOURCE_DIR}/tests)
set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(SHADER_ROOT ${SRC_ROOT}/shaders)
set(SHADER_COMPILER_DIR C:/VulkanSDK/1.3.261.1/Bin/glslc.exe)

set(SRC
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
    ${SRC_ROOT}/Synchronization.cpp
    ${SRC_ROOT}/Buffer.cpp
    ${SRC_ROOT}/Image.cpp
    ${SRC_ROOT}/MemoryManager.cpp
    ${SRC_ROOT}/Window.cpp
    ${SRC_ROOT}/PipelineLayout.cpp
    ${SRC_ROOT}/DescriptorPool.cpp
    ${SRC_ROOT}/DescriptorSet.cpp
    ${SRC_ROOT}/ImageView.cpp
    ${SRC_ROOT}/Sampler.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options("-O3")
    add_compile_options("-mwindows")
endif()
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_library(VGraphics STATIC ${SRC})

target_include_directories(VGraphics PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)

target_link_directories(VGraphics INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)
    
target_link_libraries(VGraphics INTERFACE vulkan-1)

add_custom_command(TARGET VGraphics POST_BUILD
    COMMAND  lib.exe /OUT:${CMAKE_CURRENT_BINARY_DIR}/VGraphics.lib $<TARGET_FILE:VGraphics> ${CMAKE_CURRENT_BINARY_DIR}/libVGraphics.a ${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib
)

target_include_directories(VGraphics PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    "C:/VulkanSDK/1.3.261.1/include/")

add_executable (VGTest ${TESTS_ROOT}/Source.cpp)

target_link_libraries(VGTest PRIVATE VGraphics glfw3 vulkan-1 STB_Image)

add_custom_command(TARGET VGTest PRE_BUILD
    COMMAND  ../CompileShaders.bat
)

target_link_directories(VGTest PRIVATE 
    "C:/VulkanSDK/1.3.261.1/lib/"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)


add_executable (VGTest2 ${TESTS_ROOT}/VGTest.cpp)

target_link_libraries(VGTest2 PRIVATE VGraphics glfw3 vulkan-1 STB_Image)

add_custom_command(TARGET VGTest2 PRE_BUILD
    COMMAND  ../CompileShaders.bat
)

target_link_directories(VGTest2 PRIVATE 
    "C:/VulkanSDK/1.3.261.1/lib/"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)
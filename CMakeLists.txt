cmake_minimum_required (VERSION 3.30.1)
project (VGraphics)
set(CMAKE_CXX_STANDARD 23)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
set(CMAKE_EXE_LINKER_FLAGS "-static")
 
set(TESTS_ROOT "${PROJECT_SOURCE_DIR}/tests")
set(SRC_ROOT "${PROJECT_SOURCE_DIR}/src")
set(RESOURCES_ROOT "${PROJECT_SOURCE_DIR}/resources")
set(RESOURCES_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
set(SHADER_COMPILER_DIR "C:/VulkanSDK/1.3.261.1/Bin/glslc.exe")
set(SHADER_ROOT "${SRC_ROOT}/shaders")
set(SHADER_OUTPUT_DIR "${RESOURCES_OUTPUT_DIR}/shaders")
set(CMAKE_EXE_LINKER_FLAGS "-static")
 
set(SRC
    ${SRC_ROOT}/Instance.cpp
    ${SRC_ROOT}/Device.cpp
    ${SRC_ROOT}/Queue.cpp
    ${SRC_ROOT}/Surface.cpp
    ${SRC_ROOT}/Swapchain.cpp
    ${SRC_ROOT}/Shader.cpp
    ${SRC_ROOT}/GraphicsPipeline.cpp
    ${SRC_ROOT}/ComputePipeline.cpp
    ${SRC_ROOT}/Subpass.cpp
    ${SRC_ROOT}/RenderPass.cpp
    ${SRC_ROOT}/Framebuffer.cpp
    ${SRC_ROOT}/CmdBuffer.cpp
    ${SRC_ROOT}/Synchronization.cpp
    ${SRC_ROOT}/Buffer.cpp
    ${SRC_ROOT}/Image.cpp
    ${SRC_ROOT}/MemoryManager.cpp
    ${SRC_ROOT}/Window.cpp
    ${SRC_ROOT}/PipelineLayout.cpp
    ${SRC_ROOT}/PipelineCache.cpp
    ${SRC_ROOT}/DescriptorPool.cpp
    ${SRC_ROOT}/DescriptorSet.cpp
    ${SRC_ROOT}/ImageView.cpp
    ${SRC_ROOT}/Sampler.cpp
    ${SRC_ROOT}/FormatInfo.cpp
    ${SRC_ROOT}/CmdPool.cpp
)

set(SHADERS
    ${SHADER_ROOT}/shader.comp
    ${SHADER_ROOT}/shader.frag
    ${SHADER_ROOT}/shader.vert
)

set(RESOURCES
    ${RESOURCES_ROOT}/textures/texture.jpg
)

file(MAKE_DIRECTORY ${TESTS_ROOT} ${SRC_ROOT} ${RESOURCES_ROOT} ${RESOURCES_OUTPUT_DIR} ${SHADER_ROOT} ${SHADER_OUTPUT_DIR})

# GLFW 
add_subdirectory("External/glfw-3.3.8/")


# Library
add_library(VGraphics STATIC ${SRC})
target_include_directories(VGraphics PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)
target_link_directories(VGraphics INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)
target_include_directories(VGraphics PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    "C:/VulkanSDK/1.3.261.1/include/")
target_link_libraries(VGraphics INTERFACE vulkan-1)
add_custom_command(TARGET VGraphics POST_BUILD
    COMMAND  lib.exe "/OUT:${CMAKE_CURRENT_BINARY_DIR}/VGraphics.lib" "${CMAKE_CURRENT_BINARY_DIR}/libVGraphics.a" "${CMAKE_CURRENT_SOURCE_DIR}/lib/vulkan-1.lib"
    COMMENT "Joining libraries: libVGraphics.a and vulkan-1.lib"
)

# SHADERS
foreach(SHADER ${SHADERS})
    string(REPLACE "${SHADER_ROOT}/" "" SHADER_NAME "${SHADER}")
    add_custom_command(
        OUTPUT "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv"
        COMMAND "${SHADER_COMPILER_DIR}" "${SHADER}" -o "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv"
        DEPENDS "${SHADER}"
        COMMENT "Compiling Shader: ${SHADER_NAME}.spv"
    )
    list(APPEND SHADER_DEPEND "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")
endforeach()
add_custom_target(Shaders ALL DEPENDS ${SHADER_DEPEND})


# RESOURCES
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
add_custom_target(Resources ALL DEPENDS ${RESOURCE_DEPEND})


# TEST 
add_executable (VGTest ${TESTS_ROOT}/Source.cpp)
target_link_libraries(VGTest PRIVATE VGraphics glfw3 vulkan-1 STB_Image)
target_link_directories(VGTest PRIVATE 
    "C:/VulkanSDK/1.3.261.1/lib/"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
)
add_dependencies(VGTest Shaders VGraphics Resources)

# # COMPUTE SHADER 
# add_executable (ComputeShaderTest ${TESTS_ROOT}/ComputeShader.cpp)
# target_link_libraries(ComputeShaderTest PRIVATE VGraphics glfw3 vulkan-1 STB_Image)
# target_link_directories(ComputeShaderTest PRIVATE 
#     "C:/VulkanSDK/1.3.261.1/lib/"
#     "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
# )
# add_dependencies(ComputeShaderTest Shaders VGraphics Resources)
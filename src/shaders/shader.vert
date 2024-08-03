#version 450

layout( push_constant ) uniform PushConstants
{
	vec3 pos;
} pushConstants;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 texCoord;

void main() {
    vec4 worldPos = ubo.model * vec4(aPosition, 1.0) + vec4(pushConstants.pos,0);
    gl_Position = ubo.proj * ubo.view * worldPos;
    fragColor = aColor;
    texCoord = aTexCoord;
}

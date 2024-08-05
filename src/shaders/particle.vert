#version 450

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_PointSize = 1;
    gl_Position = vec4(aPosition,0, 1.0);
    fragColor = aColor;
}

#version 450

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aCol;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_PointSize = 1.0;
    gl_Position = vec4(aPosition,0, 1);
    fragColor = vec3(aCol.rgb);
}
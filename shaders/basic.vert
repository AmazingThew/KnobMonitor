#version 400

layout(location = 0) in vec3 PositionAttribute;
layout(location = 1) in vec3 ColorAttribute;
out vec3 VertexColor;

void main() {
    VertexColor = ColorAttribute;
    gl_Position = vec4(PositionAttribute, 1.0);
}
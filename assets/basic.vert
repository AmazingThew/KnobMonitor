#version 400

layout(location = 0) in vec3 PositionAttribute;
layout(location = 1) in vec3 ColorAttribute;
layout(location = 2) in vec2 UVAttribute;
out vec3 VertexColor;
out vec2 UV;

void main() {
    VertexColor = ColorAttribute;
	UV = UVAttribute;
    gl_Position = vec4(PositionAttribute, 1.0);
}
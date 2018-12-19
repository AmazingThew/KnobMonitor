#version 400

in vec3 VertexColor;
out vec4 frag_color;

void main() {
	frag_color = vec4(VertexColor, 1.0);
}
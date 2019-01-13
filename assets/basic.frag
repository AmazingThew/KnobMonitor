#version 400

in vec3 VertexColor;
in vec2 UV;
out vec4 frag_color;

void main() {
	frag_color = vec4(VertexColor, 1.0);
	//frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
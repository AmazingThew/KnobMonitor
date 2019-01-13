#version 400

in vec3 VertexColor;
in vec2 UV;
out vec4 frag_color;

uniform sampler2D texture0;

void main() {
	//vec4 tex = textureLod(texture0, UV, 4);
	vec4 tex = texture(texture0, UV);
	vec4 tint = vec4(VertexColor, 1.0);
	frag_color = tex * tint;
}
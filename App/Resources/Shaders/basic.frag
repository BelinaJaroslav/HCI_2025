#version 460 core

in vec2 o_texture_coordinates;

//uniform vec4 u_color = vec4(1.0);
uniform sampler2D tex0;

out vec4 FragColor;

void main() {
	//FragColor = u_color;
	FragColor = texture(tex0, o_texture_coordinates);
}

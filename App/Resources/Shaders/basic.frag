#version 460 core

in VS_OUT
{
    vec2 texture_coordinates;
} fs_in;

//uniform vec4 u_color = vec4(1.0);
uniform sampler2D tex0;

out vec4 FragColor;

void main() {
	//FragColor = u_color;
	FragColor = texture(tex0, fs_in.texture_coordinates);
}

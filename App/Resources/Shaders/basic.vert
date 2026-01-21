#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture_coordinates;

uniform mat4 u_projection_mx = mat4(1.0);
uniform mat4 u_model_mx = mat4(1.0);
uniform mat4 u_view_mx = mat4(1.0);

out vec2 o_texture_coordinates;
out vec3 o_fragment_position;
out vec3 o_normal;


void main()
{
    o_texture_coordinates = a_texture_coordinates;
    o_fragment_position = vec3(u_model_mx * vec4(a_position, 1.0f));
    o_normal = mat3(transpose(inverse(u_model_mx))) * a_normal;

    gl_Position = u_projection_mx * u_view_mx * u_model_mx * vec4(a_position, 1.0f);
}

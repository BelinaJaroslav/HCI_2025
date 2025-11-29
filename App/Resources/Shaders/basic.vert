#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture_coordinates;

uniform mat4 u_projection_mx = mat4(1.0);
uniform mat4 u_model_mx = mat4(1.0);
uniform mat4 u_view_mx = mat4(1.0);

out vec2 o_texture_coordinates;

void main()
{
    // Outputs the positions/coordinates of all vertices
    
    o_texture_coordinates = a_texture_coordinates;

    gl_Position = u_projection_mx * u_view_mx * u_model_mx * vec4(a_position, 1.0f);
}

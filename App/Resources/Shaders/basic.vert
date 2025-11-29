#version 460 core

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texture;

uniform mat4 u_projection_mx = mat4(1.0);
uniform mat4 u_model_mx = mat4(1.0);
uniform mat4 u_view_mx = mat4(1.0);

out VS_OUT
{
    vec2 texture_coordinates;
} vs_out;

void main()
{
    // Outputs the positions/coordinates of all vertices
    
    gl_Position = u_projection_mx * u_view_mx * u_model_mx * vec4(a_position, 1.0f);

    vs_out.texture_coordinates = a_texture;
}

#version 460 core

in vec3 a_position;

uniform mat4 u_projection_mx = mat4(1.0);
uniform mat4 u_model_mx = mat4(1.0);
uniform mat4 u_view_mx = mat4(1.0);

void main()
{
    // Outputs the positions/coordinates of all vertices
    
    gl_Position = u_projection_mx * u_view_mx * u_model_mx * vec4(a_position, 1.0f);
}

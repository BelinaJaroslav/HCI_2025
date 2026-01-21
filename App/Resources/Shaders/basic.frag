#version 460 core

in vec2 o_texture_coordinates;
in vec3 o_fragment_position;
in vec3 o_normal;

uniform sampler2D tex0;

uniform vec3 u_camera_position;

uniform vec3 u_material_ambient;
uniform vec3 u_material_specular;
uniform float u_material_shininess;

uniform vec3 u_dirlight_direction;
uniform vec3 u_dirlight_diffuse;
uniform vec3 u_dirlight_specular;

out vec4 frag_color;


void main() {
    vec3 normal = normalize(o_normal);
	vec3 frag2camera = normalize(u_camera_position - o_fragment_position);
	vec4 out_color = vec4(0.0f);

	// Ambient light
	vec4 ambient = vec4(u_material_ambient, 0.0f) * texture(tex0, o_texture_coordinates);

    // Directional light
    vec3 frag2light = normalize(-u_dirlight_direction);
    vec4 diffuse = vec4(u_dirlight_diffuse * max(dot(normal, frag2light), 0.0f), 0.0f) * texture(tex0, o_texture_coordinates);
	vec3 specular = u_dirlight_specular * u_material_specular * pow(max(dot(normal, normalize(frag2light + frag2camera)), 0.0f), u_material_shininess);
	out_color += (diffuse + vec4(specular, 0.0f));

	frag_color = ambient + out_color;
}

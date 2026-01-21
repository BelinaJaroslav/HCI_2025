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

struct Reflector {
	vec3 position;
	vec3 direction;
	float cos_inner_cone;
	float cos_outer_cone;
	vec3 diffuse;
	vec3 specular;
	int is_on;
	float constant;
	float linear;
	float exponent;
};
uniform Reflector u_reflector;

out vec4 frag_color;


void main() {
    vec3 normal = normalize(o_normal);
	vec3 frag2camera = normalize(u_camera_position - o_fragment_position);
	vec4 out_color = vec4(0.0f);

	// Ambient light
	vec4 ambient = vec4(u_material_ambient, 0.0f) * texture(tex0, o_texture_coordinates);

    // Directional light
    vec3 fragment_to_light = normalize(-u_dirlight_direction);
    vec4 diffuse = vec4(u_dirlight_diffuse * max(dot(normal, fragment_to_light), 0.0f), 0.0f) * texture(tex0, o_texture_coordinates);
	vec3 specular = u_dirlight_specular * u_material_specular * pow(max(dot(normal, normalize(fragment_to_light + frag2camera)), 0.0f), u_material_shininess);
	out_color += (diffuse + vec4(specular, 0.0f));

    // Reflector
    if (u_reflector.is_on == 1) {
        fragment_to_light = normalize(u_reflector.position - o_fragment_position);
        diffuse = vec4(u_reflector.diffuse * max(dot(normal, fragment_to_light), 0.0f), 0.0f) * texture(tex0, o_texture_coordinates);
        specular = u_reflector.specular * u_material_specular * pow(max(dot(normal, normalize(fragment_to_light + frag2camera)), 0.0f), u_material_shininess);
        float d = length(u_reflector.position - o_fragment_position);
        float attenuation = 1.0f / (u_reflector.constant + u_reflector.linear * d + u_reflector.exponent * (d * d));
        float spot_intensity = smoothstep(u_reflector.cos_outer_cone, u_reflector.cos_inner_cone, dot(-fragment_to_light, normalize(u_reflector.direction)));
        diffuse *= attenuation * spot_intensity;
        specular *= attenuation * spot_intensity;	
        out_color += (diffuse + vec4(specular, 0.0f));
    }

	frag_color = ambient + out_color;
}

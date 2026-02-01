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
uniform Reflector u_tractor;

out vec4 frag_color;

// ... Reflector struct and apply_reflector ...
// Change the return of apply_reflector to vec3 to simplify math
vec3 apply_reflector(Reflector light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if (light.is_on == 0) return vec3(0.0);

    vec3 fragToLight = normalize(light.position - fragPos);
    
    // 1. Standard Diffuse/Specular
    float diff = max(dot(normal, fragToLight), 0.0);
    vec3 halfVec = normalize(fragToLight + viewDir);
    float spec = pow(max(dot(normal, halfVec), 0.0), u_material_shininess);

    // 2. Add an Ambient term for that "inner glow"
    // This makes the beam feel like it's filled with green energy
    vec3 ambient = light.diffuse * 0.3; 

    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + light.exponent * d * d);

    float spot = smoothstep(light.cos_outer_cone, light.cos_inner_cone, dot(-fragToLight, normalize(light.direction)));

    vec3 diffuse  = light.diffuse  * diff;
    vec3 specular = light.specular * u_material_specular * spec;

    // 3. Combine - Now including the ambient glow
    return (ambient + diffuse + specular) * attenuation * spot;
}

void main() {
    vec3 normal = normalize(o_normal);
    vec3 frag2camera = normalize(u_camera_position - o_fragment_position);
    vec4 tex_color = texture(tex0, o_texture_coordinates);
    
    // 1. Directional / Global Lighting
    vec3 ambient = u_material_ambient * tex_color.rgb;
    
    vec3 fragment_to_light = normalize(-u_dirlight_direction);
    vec3 diff_dir = u_dirlight_diffuse * max(dot(normal, fragment_to_light), 0.0f) * tex_color.rgb;
    vec3 spec_dir = u_dirlight_specular * u_material_specular * pow(max(dot(normal, normalize(fragment_to_light + frag2camera)), 0.0f), u_material_shininess);
    
    vec3 combined_lighting = diff_dir + spec_dir;

    // 2. Add Reflectors (Multiply by tex_color so they illuminate the surface)
    combined_lighting += apply_reflector(u_reflector, normal, o_fragment_position, frag2camera) * tex_color.rgb;
    combined_lighting += apply_reflector(u_tractor, normal, o_fragment_position, frag2camera) * tex_color.rgb;

    frag_color = vec4(ambient + combined_lighting, 1.0f); // Force alpha to 1.0
}

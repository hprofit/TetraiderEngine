#version 430

const int MAX_LIGHTS = 20;

// UNIFORM INPUTS
layout(location = 0) uniform sampler2D baseTexture;

layout(location = 40) uniform vec3 ambient_global_color;
layout(location = 41) uniform vec3 ambient_color;
layout(location = 42) uniform vec3 diffuse_color;
layout(location = 43) uniform vec3 specular_color;
//layout(location = 44) uniform float spec_coeff;

layout(location = 47) uniform bool lit;
layout(location = 50) uniform vec4 l_pos_dist[MAX_LIGHTS];
layout(location = 70) uniform vec4 l_color[MAX_LIGHTS]; 	// 50 + MAX_LIGHTS
layout(location = 90) uniform vec2 l_ab[MAX_LIGHTS];		// 50 + MAX_LIGHTS + MAX_LIGHTS


// NON-UNIFORM INPUTS
layout(location = 2) in vec2 vtexture_coord;
layout(location = 5) in vec4 vnormal_vector;
layout(location = 6) in vec4 vtangent_vector;
layout(location = 7) in vec4 vbitangent_vector;
layout(location = 8) in vec4 vview_vector;

layout(location = 10) in vec4 vl_lightVector;


// OUTPUTS
out vec4 frag_color;


void main(void) {
	vec3 diffuse_color = vec3(.25, .25, 0);
	vec3 lightColor = vec3(0,0,0);

	vec4 L = normalize(vl_lightVector);
	vec4 m = normalize(vnormal_vector);
	lightColor = max(dot(m, L), 0) * diffuse_color;
	
	frag_color = vec4(diffuse_color, 1);
}
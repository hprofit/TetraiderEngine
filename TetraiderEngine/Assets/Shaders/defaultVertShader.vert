#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texture_coord;

layout(location = 10) uniform mat4 persp_matrix;
layout(location = 11) uniform mat4 view_matrix;
layout(location = 12) uniform mat4 model_matrix;

layout(location = 30) uniform vec4 tint_color;
layout(location = 31) uniform vec4 saturation_color;
layout(location = 32) uniform vec2 frame_offset;
layout(location = 33) uniform vec2 frame_size;

layout(location = 50) uniform vec4 l_pos_dist;
layout(location = 51) uniform vec4 l_color;

layout(location = 0) out vec4 vtint_color;
layout(location = 1) out vec4 vsaturation_color;
layout(location = 2) out vec2 vtexture_coord;
layout(location = 3) out vec2 vframe_offset;
layout(location = 4) out vec2 vframe_size;

void main() {
	vec4 P = model_matrix * position;
	gl_Position = persp_matrix * view_matrix * P;
	vtexture_coord = texture_coord;

	vframe_offset = frame_offset;
	vframe_size = frame_size;

	vtint_color = tint_color;
	vsaturation_color = saturation_color;
}
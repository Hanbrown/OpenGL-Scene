// Pranav Rao
// Assignment 8
// Fragment shader for sky
#sof
#version 430 core

in vec3 tc;
out vec4 fragColor;

uniform mat4 v_matrix, proj_matrix;

layout(binding = 0) uniform samplerCube samp;

void main(void) {
	fragColor = texture(samp, tc);
}

#eof
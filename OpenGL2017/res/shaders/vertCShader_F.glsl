// Pranav Rao
// Assignment 8
// Vertex shader for sky
#sof
#version 430 core

// Vertex attributes
layout(location = 0) in vec3 position;

out vec3 tc;

uniform mat4 v_matrix, proj_matrix;

layout(binding = 0) uniform samplerCube samp;

void main(void) {
	tc = position;	// Set texture coordinates to simply be the vertex coordinates
	mat4 vrot_matrix = mat4(mat3(v_matrix)); // Remove translation operations from view matrix
	gl_Position = proj_matrix * vrot_matrix * vec4(position, 1.0);
}

#eof
// Pranav Rao
// Assignment 8
// Vertex shader for object
///////////////////////////////////////////////  With lighting   ////////////////////////////////////////////////

#version 430 core

// Vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;

out vec3 varyingVertPos;
out vec2 tc;
out vec3 varyingNormal;
out vec3 varyingLightDir;

struct PositionalLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
	vec3 direction;
	float cutoff;
};

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix, proj_matrix, nrm_matrix;
uniform float fac;

// Check main cpp file to see that sky tex is at index 0 and Falcon tex is at index 1
layout(binding = 0) uniform samplerCube sky_map;
layout(binding = 1) uniform sampler2D tex_map;

void main(void) {
	
	varyingVertPos = (mv_matrix * vec4(position, 1.0)).xyz;
	tc = tex_coord;
	varyingNormal = (nrm_matrix * vec4(normal, 1.0)).xyz;
	varyingLightDir = light.position - varyingVertPos;

	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

#eof

///////////////////////////////////////////////  No lighting   ////////////////////////////////////////////////
#sof
#version 430 core

// Vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;

out vec3 varyingVertPos;
out vec2 tc;
out vec3 varyingNormal;

uniform mat4 mv_matrix, proj_matrix, nrm_matrix;
uniform float fac;

// Check main cpp file to see that sky tex is at index 0 and Falcon tex is at index 1
layout(binding = 0) uniform samplerCube sky_map;
layout(binding = 1) uniform sampler2D tex_map;

void main(void) {
	varyingVertPos = (mv_matrix * vec4(position, 1.0)).xyz;
	tc = tex_coord;
	varyingNormal = (nrm_matrix * vec4(normal, 1.0)).xyz;
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}

#eof
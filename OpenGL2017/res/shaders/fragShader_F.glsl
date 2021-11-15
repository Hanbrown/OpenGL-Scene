// Pranav Rao
// Assignment 8
// Fragment shader for object
///////////////////////////////////////////////  With lighting   ////////////////////////////////////////////////

#version 430 core

in vec3 varyingVertPos;
in vec2 tc;
in vec3 varyingNormal;
in vec3 varyingLightDir;

out vec4 fragColor;

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
uniform float fac, isOn;

layout(binding = 0) uniform samplerCube sky_map;
layout(binding = 1) uniform sampler2D tex_map;

void main(void) {
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);

	vec3 R = normalize(reflect(-L, N));

	float cosTheta = dot(L, N);

	float cosPhi = dot(V, R);

	// Reflect sky map so it properly reflects off the airplane
	vec3 r = -reflect(V, N);

	// Generate gloss texture and airplane diffuse (color) texture
	vec4 gloss = texture(sky_map, r);
	vec4 diffuse1 = texture(tex_map, tc);

	// Mix both textures with a tunable factor
	//fragColor = mix(gloss, diffuse1, fac);
	vec4 mixRGB = mix(gloss, diffuse1, fac);

	/*vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
	vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess);*/

	float onFac = 1.0;
	if (isOn < 0.0) { // If light is off
		onFac = 0.0;
	}

	// Implement attenuation
	float distance = length(light.position - normalize(varyingVertPos).xyz) / 70.0;
	//distance = 0.5;
	float attenuation = 1.0 / (1.0 + 0.7 * distance + 0.75 * (distance * distance));

	vec3 ambient = ((globalAmbient * mixRGB) + (onFac * light.ambient * mixRGB)).xyz;
	vec3 diffuse = (onFac * light.diffuse.xyz * mixRGB.xyz * max(cosTheta, 0.0));
	vec3 specular = (onFac * light.specular.xyz * mixRGB.xyz * pow(max(cosPhi, 0.0), 25.0f));

	/*
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	*/

	fragColor = vec4((ambient + diffuse + specular), 1.0);

	//fragColor = vec4(distance, 0.0, 0.0, 1.0);
}

#eof

///////////////////////////////////////////////  No lighting   ////////////////////////////////////////////////

#sof
#version 430 core

in vec3 varyingVertPos;
in vec2 tc;
in vec3 varyingNormal;

out vec4 fragColor;

uniform mat4 mv_matrix, proj_matrix, nrm_matrix;
uniform float fac;

layout(binding = 0) uniform samplerCube sky_map;
layout(binding = 1) uniform sampler2D tex_map;

void main(void) {
	// Reflect sky map so it properly reflects off the airplane
	vec3 r = -reflect(normalize(-varyingVertPos), normalize(varyingNormal));
	
	// Generate gloss texture and airplane diffuse (color) texture
	vec4 gloss = texture(sky_map, r);
	vec4 diffuse = texture(tex_map, tc);

	// Mix both textures with a tunable factor
	fragColor = mix(gloss, diffuse, fac);
}

#eof
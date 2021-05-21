#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;

subroutine vec3 brdfType(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y);
subroutine uniform brdfType brdfUniform;

// Lighting
uniform vec3 lightDirection;
uniform vec3 viewDirection;

// Matrices
uniform mat4 mModelViewProj;

INCLUDE BRDF

subroutine(brdfType)
vec3 noBRDF(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y)
{
	return vPosition;
}

void main()
{
	const vec3 vertexPosition = brdfUniform(lightDirection, normalize(vPosition), viewDirection, vec3(.0f), vec3(.0f));

	gl_Position = mModelViewProj * vec4(vertexPosition, 1.0f);
}
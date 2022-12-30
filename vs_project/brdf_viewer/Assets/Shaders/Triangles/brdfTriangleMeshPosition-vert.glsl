#version 450

#include <Assets/Shaders/Compute/Templates/constraints.glsl>
#include <Assets/Shaders/Compute/Templates/utilities.glsl>

layout (location = 0) in vec3 vPosition;
layout (location = 3) in vec3 vTangent; 

subroutine vec3 brdfType(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y);
subroutine uniform brdfType brdfUniform;

// Lighting
uniform vec3 lightDirection;
uniform vec3 viewDirection;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;

out vec3 position;

INCLUDE BRDF

subroutine(brdfType)
vec3 noBRDF(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y)
{
	return vPosition;
}

void main()
{
	const vec3 vertexPosition = brdfUniform(lightDirection, normalize(vPosition), viewDirection, vTangent, vec3(.0f));

	position = vec3(mModelView *  vec4(vertexPosition, 1.0f));
	gl_Position = mModelViewProj * vec4(vertexPosition, 1.0f);
}

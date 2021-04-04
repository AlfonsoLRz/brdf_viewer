#version 450

layout (location = 0) in vec4 vPosition;

subroutine vec3 brdfType(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y);
subroutine uniform brdfType brdfUniform;

uniform mat4 mModelViewProj;
uniform float pointSize;

INCLUDE BRDF

subroutine(brdfType)
vec3 noBRDF(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y)
{
	return vPosition.xyz;
}

void main() {
	const vec3 L = normalize(vec3(.0f, 1.0f, .0f));
	const vec3 V = normalize(vec3(1.0f, 1.0f, .0f));
	const vec3 vertexPosition = brdfUniform(L, normalize(vPosition.xyz), V, vec3(.0f), vec3(.0f));

	gl_PointSize = pointSize;
	gl_Position = mModelViewProj * vec4(vertexPosition, 1.0f);
}
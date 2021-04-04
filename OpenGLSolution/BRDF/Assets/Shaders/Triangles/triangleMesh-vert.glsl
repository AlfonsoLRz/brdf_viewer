#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextCoord;
layout(location = 3) in vec3 vTangent;

subroutine vec3 brdfType(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y);
subroutine uniform brdfType brdfUniform;

// Lighting
uniform vec3 lightPosition;
uniform vec3 lightDirection;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;
uniform mat4 mShadow;

// Vertex related
out vec3 worldPosition;
out vec3 position;
out vec3 normal;
out vec2 textCoord;
out vec4 shadowCoord;

INCLUDE BRDF

subroutine(brdfType)
vec3 noBRDF(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y)
{
	return vPosition;
}

void main()
{
	const vec3 L = normalize(vec3(.0f, 1.0f, .0f));
	const vec3 V = normalize(vec3(1.0f, 1.0f, .0f));
	const vec3 vertexPosition = brdfUniform(L, normalize(vPosition), V, vec3(.0f), vec3(.0f));

	worldPosition = vertexPosition;
	position = vec3(mModelView *  vec4(vertexPosition, 1.0f));
	gl_Position = mModelViewProj * vec4(vertexPosition, 1.0f);
	normal = vec3(mModelView * vec4(vNormal, 0.0f));
	shadowCoord = mShadow * vec4(vertexPosition, 1.0f);
	textCoord = vTextCoord;
}

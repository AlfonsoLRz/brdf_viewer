#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextCoord;
layout(location = 3) in vec3 vTangent;

// Lighting
uniform vec3 lightPosition;
uniform vec3 lightDirection;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;
uniform mat4 mShadow;

// Vertex related
out vec3 position;
out vec3 normal;
out vec2 textCoord;
out vec4 shadowCoord;

// ********* FUNCTIONS ************

void main()
{
	position = vec3(mModelView *  vec4(vPosition, 1.0f));
	gl_Position = mModelViewProj * vec4(vPosition, 1.0f);
	normal = vec3(mModelView * vec4(vNormal, 0.0f));
	shadowCoord = mShadow * vec4(vPosition, 1.0f);
	textCoord = vTextCoord;
}

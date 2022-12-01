#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTextCoord;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in float vBSDF;

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


void main()
{
	const vec3 L = normalize(vec3(-1.0f, -1.0f, .0f));
	const vec3 V = normalize(vec3(1.0f, 1.0f, .0f));
	const vec3 N = normalize(vPosition);

	// Calculate pos
	const vec3 v = normalize(vPosition - vec3(.0f));

	vec3 R = reflect(L,N);

    // specular
    float val = pow(max(0, dot(R,V)), 2.0f);
    //val = val / dot(N,L);

	const float intensity = max(val, 1.0f / 3.14f);
	const vec3 spherePosition = v * intensity;

	worldPosition = spherePosition;
	position = vec3(mModelView *  vec4(spherePosition, 1.0f));
	gl_Position = mModelViewProj * vec4(spherePosition, 1.0f);
	normal = vec3(mModelView * vec4(vNormal, 0.0f));
	shadowCoord = mShadow * vec4(spherePosition, 1.0f);
	textCoord = vTextCoord;
}

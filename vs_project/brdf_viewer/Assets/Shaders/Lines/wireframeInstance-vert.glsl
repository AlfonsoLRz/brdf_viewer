#version 450

layout (location = 0) in vec4 vPosition;

uniform mat4 mModelViewProj;

void main() {
	gl_Position = mModelViewProj * vec4(vertexPosition, 1.0f);
}
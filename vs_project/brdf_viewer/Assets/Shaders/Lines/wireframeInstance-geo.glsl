#version 450

layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform vec3 vector;
uniform mat4 mModelViewProj;

layout (location = 0) out vec4 fColor;
layout (location = 1) out vec4 brightColor;

void main() {
	gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + mModelViewProj * vec4(normalize(vector), 0.0f);
    EmitVertex();
    
    EndPrimitive();
}
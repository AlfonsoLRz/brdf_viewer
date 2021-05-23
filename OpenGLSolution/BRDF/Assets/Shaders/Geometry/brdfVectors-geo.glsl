#version 450

layout(points) in;
layout(line_strip, max_vertices = 4) out;

// Ray target
uniform vec3	lightDirection;
uniform vec3	lightVectorColor;
uniform vec3	viewDirection;
uniform vec3	viewVectorColor;

// Matrices
uniform mat4	mModelViewProj;

out vec3 color;

void main()
{
	vec4 origin = gl_in[0].gl_Position;

	gl_Position = origin;
	color = lightVectorColor;
	EmitVertex();

	gl_Position = mModelViewProj * vec4(lightDirection, 1.0f);
	color = lightVectorColor;
	EmitVertex();

	EndPrimitive();

	gl_Position = origin;
	color = viewVectorColor;
	EmitVertex();

	gl_Position = mModelViewProj * vec4(viewDirection, 1.0f);
	color = viewVectorColor;
	EmitVertex();

	EndPrimitive();
}
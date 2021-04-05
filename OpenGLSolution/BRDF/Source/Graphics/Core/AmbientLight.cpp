#include "stdafx.h"
#include "AmbientLight.h"

void AmbientLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shaderType)
{
	shader->setSubroutineUniform(shaderType, "lightUniform", "ambientLight");

	shader->setUniform("Ia", light->getIa());
}
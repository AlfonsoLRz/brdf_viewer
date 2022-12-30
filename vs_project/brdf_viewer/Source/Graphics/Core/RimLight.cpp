#include "stdafx.h"
#include "RimLight.h"

void RimLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shaderType)
{
	shader->setSubroutineUniform(shaderType, "lightUniform", "rimLight");

	shader->setUniform("Ia", light->getIa());
}
#include "stdafx.h"
#include "PointLight.h"

void PointLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shaderType)
{
	shader->setSubroutineUniform(shaderType, "lightUniform", "pointLight");

	shader->setUniform("lightPosition", vec3(viewMatrix * vec4(light->getPosition(), 1.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}
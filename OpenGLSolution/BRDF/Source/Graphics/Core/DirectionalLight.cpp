#include "stdafx.h"
#include "DirectionalLight.h"

void DirectionalLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shaderType)
{
	shader->setSubroutineUniform(shaderType, "lightUniform", "directionalLight");

	shader->setUniform("lightDirection", vec3(viewMatrix * vec4(light->getDirection(), 0.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}
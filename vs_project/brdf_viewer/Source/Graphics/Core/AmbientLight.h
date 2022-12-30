#pragma once

#include "Graphics/Core/LightType.h"

/**
*	@file AmbientLight.h
*	@authors Alfonso L�pez Ruiz (alr00048@red.ujaen.es)
*	@date 07/17/2019
*/

/**
*	@brief Applicator for an ambient light model.
*/
class AmbientLight: public LightType
{
public:
	/**
	*	@brief Applies the uniform variables related to an specific type of light.
	*	@param light Light from where we get the parameters.
	*	@param shader Shader where we specity the uniform variables.
	*	@param viewMatrix View matrix of currently active camera.
	*/
	virtual void applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shaderType = GL_FRAGMENT_SHADER);
};


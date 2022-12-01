#pragma once

#include "Graphics/Core/Model3D.h"

/**
*	@file BRDFSphere.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 01/04/2021
*/

/**
*	@brief 
*/
class BRDFSphere: public Model3D
{
protected:
	const static unsigned SECTORS, STACKS;			//!<

protected:
	Texture* _heightTexture;						//!< Rendering texture

protected:
	/**
	*	@brief Renders a component as a set of triangles.
	*	@param modelComp Component where the VAO is located.
	*	@param primitive Primitive we need to use to render the VAO.
	*/
	virtual void renderTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix, ModelComponent* modelComp, const GLuint primitive);

public:
	/**
	*	@brief Constructor of a BRDF sphere.
	*/
	BRDFSphere(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Deleted copy constructor.
	*/
	BRDFSphere(const BRDFSphere& plane) = delete;

	/**
	*	@brief Destructor.
	*/
	~BRDFSphere();

	/**
	*	@brief Loads the plane data into GPU.
	*	@return Success of operation.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Deleted assignment operator overriding.
	*/
	BRDFSphere& operator=(const BRDFSphere& plane) = delete;

	/**
	*	@brief 
	*/
	void updateReflectance(std::vector<float>& reflectance);
};


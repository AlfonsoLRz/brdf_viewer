#pragma once

#include "Geometry/3D/AABB.h"
#include "Graphics/Application/RenderingParameters.h"
#include "Graphics/Core/Model3D.h"

/**
*	@file PGLLPointCloud.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 09/24/2020
*/

#define DELIMITER ' '
#define INTENSITY_HEADER "#format:"

/**
*	@brief 
*/
class PGLLPointCloud : public Model3D
{
protected:
	const static std::string BINARY_EXTENSION;						//!< File extension for binary models

protected:
	std::string			_filename;									//!<
	bool				_useBinary;									//!<

	// Spatial information
	std::vector<vec4>	_points;									//!<
	AABB				_aabb;										//!<		

protected:
	/**
	*	@brief Computes a triangle mesh buffer composed only by indices.
	*/
	void computeCloudData();

	/**
	*	@brief Fills the content of model component with binary file data.
	*/
	bool loadModelFromBinaryFile();

	/**
	*	@brief Generates geometry via GPU.
	*/
	bool loadModelFromPGLL(const mat4& modelMatrix);

	/**
	*	@brief Loads the PLY point cloud from a binary file, if possible.
	*/
	virtual bool readBinary(const std::string& filename, const std::vector<Model3D::ModelComponent*>& modelComp);

	/**
	*	@brief Communicates the model structure to GPU for rendering purposes.
	*/
	virtual void setVAOData();

	/**
	*	@brief Writes the model to a binary file in order to fasten the following executions.
	*	@return Success of writing process.
	*/
	virtual bool writeToBinary(const std::string& filename);

public:
	/**
	*	@brief 
	*/
	PGLLPointCloud(const std::string& filename, const bool useBinary, const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief
	*/
	virtual ~PGLLPointCloud();

	/**
	*	@return
	*/
	AABB getAABB() { return _aabb; }

	/**
	*	@brief 
	*/
	unsigned getNumberOfPoints() { return unsigned(_points.size()); }

	/**
	*	@return
	*/
	std::vector<vec4>* getPoints() { return &_points; }

	/**
	*	@brief Loads the point cloud, either from a binary or a PLY file.
	*	@param modelMatrix Model transformation matrix.
	*	@return True if the point cloud could be properly loaded.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));
};


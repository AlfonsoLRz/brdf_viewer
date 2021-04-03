#pragma once

#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/RenderingShader.h"

/**
*	@file BRDFShader.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 02/04/2021
*/

/**
*	@brief Shader to be compiled from BRDF modules.
*/
class BRDFShader: public RenderingShader
{
protected:
	enum ShaderVariableType { BOOLEAN = 0, INTEGER = 1, FLOAT = 2 };
	
	struct ShaderVariable
	{
		ShaderVariableType	_type;

		union
		{
			bool	_defaultBoolean;
			int		_defaultInteger;
			float	_defaultFloat;
		};

		union
		{
			ivec2	_integerRange;
			vec2	_floatRange;
		};
	};

protected:
	const static std::unordered_map<Model3D::BRDFType, std::string> BRDF_MODULE_PATH;		//!<
	const static std::string INCLUDE_BRDF_HEADER;													//!<
	const static std::string PARAMETERS_BEGIN, PARAMETERS_END;										//!<
	const static std::string SHADER_BEGIN, SHADER_END;												//!<

protected:
	std::vector<ShaderVariable> _shaderVariables;											//!<

protected:
	/**
	*	@brief Tries to compile a shader at a certain path.
	*	@param filename Path of the shader we need to compile.
	*	@param shaderType Type of shader to be compiled: VERTEX, FRAGMENT, GEOMETRY or COMPUTE.
	*	@return Assigned ID for the shader.
	*/
	virtual GLuint compileShader(const char* filename, const GLenum shaderType, Model3D::BRDFType brdfType);

	/**
	*	@brief Seeks parameters which needs to be added to the interface. 
	*/
	void findParameters(const std::string& fileContent, std::vector<ShaderVariable>& variables);

	/**
	*	@brief Seeks parameters which needs to be added to the interface.
	*/
	void findShader(const std::string& fileContent, std::string& shaderContent);

	/**
	*	@return Hash table with path in system for each BRDF module. 
	*/
	static std::unordered_map<Model3D::BRDFType, std::string> getBRDFModulePath();

	/**
	*	@brief Replaces BRDF include line for its BRDF implementation. 
	*/
	bool includeShaderBRDF(std::string& shaderContent, Model3D::BRDFType brdfType);

public:
	/**
	*	@brief Constructor. 
	*/
	BRDFShader();

	/**
	*	@brief Destructor. 
	*/
	virtual ~BRDFShader();
	
	/**
	*	@brief Reads and compiles all the shaders which contain the name specified as an argument.
	*	@filename filename Shader program name. Example: pointCloud => pointCloud-vert.glsl, pointCloud-frag.glsl...
	*/
	virtual GLuint createShaderProgram(const char* filename, Model3D::BRDFType brdf);
};


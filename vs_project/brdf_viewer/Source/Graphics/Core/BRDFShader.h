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
	enum ShaderVariableType { BOOLEAN = 0, INTEGER = 1, FLOAT = 2, NUM_VARIABLE_TYPES };
	inline const static std::string VARIABLE_TYPE_NAME[NUM_VARIABLE_TYPES] = { "bool", "int", "float" };

public:
	struct ShaderVariable
	{
		ShaderVariableType	_type;
		std::string			_name;

		union
		{
			bool	_booleanValue;
			int		_integerValue;
			float	_floatValue;
		};

		union
		{
			ivec2	_integerRange;
			vec2	_floatRange;
		};

		/**
		*	@return Concatenation of uniform variables to include in a shader. 
		*/
		std::string toUniformLine();

		// ----- Getters ------

		/**
		*	@return True if ... 
		*/
		bool isBoolean() { return _type == BOOLEAN; }

		/**
		*	@return True if ...
		*/
		bool isInteger() { return _type == INTEGER; }

		/**
		*	@return True if ...
		*/
		bool isFloat() { return _type == FLOAT; }
	};

protected:
	const static std::unordered_map<Model3D::BRDFType, std::string> BRDF_MODULE_PATH;			//!<
	const static std::string INCLUDE_BRDF_HEADER;												//!<
	const static std::string PARAMETERS_BEGIN, PARAMETERS_END;									//!<
	const static std::string REFLECTANCE_SHADER_BEGIN, REFLECTANCE_SHADER_END;					//!<
	const static std::string RENDERING_SHADER_BEGIN, RENDERING_SHADER_END;						//!<
	const static std::string SUBROUTINE_REFLECTANCE_HEADER;										//!<
	const static std::string SUBROUTINE_RENDERING_HEADER;										//!<

protected:
	static std::unordered_map<Model3D::BRDFType, std::string> _brdfReflectanceContent;			//!<
	static std::unordered_map<Model3D::BRDFType, std::string> _brdfRenderingContent;			//!<
	static std::unordered_map<Model3D::BRDFType, std::vector<ShaderVariable>> _brdfVariables;	//!<

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
	void findShader(const std::string& fileContent, std::string& shaderContent, const std::string& header, const std::string& tail, const std::string& subroutineHeader);

	/**
	*	@return Hash table with path in system for each BRDF module. 
	*/
	static std::unordered_map<Model3D::BRDFType, std::string> getBRDFModulePath();

	/**
	*	@brief Replaces BRDF include line for its BRDF implementation. 
	*/
	bool includeShaderBRDF(std::string& shaderContent, const GLenum shaderType, Model3D::BRDFType brdfType);

	/**
	*	@brief  
	*/
	void joinParameterShader(std::vector<BRDFShader::ShaderVariable>& parameters, const std::string& shaderContent, std::string& result);

	/**
	*	@brief Loads a new BRDF module, both as reflectance and rendering function. 
	*/
	bool loadBRDFModule(Model3D::BRDFType brdfType);

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
	*	@brief  
	*/
	static void clearCache();
	
	/**
	*	@brief Reads and compiles all the shaders which contain the name specified as an argument.
	*	@filename filename Shader program name. Example: pointCloud => pointCloud-vert.glsl, pointCloud-frag.glsl...
	*/
	virtual GLuint createShaderProgram(const char* filename, Model3D::BRDFType brdf);

	/**
	*	@brief  
	*/
	static std::vector<BRDFShader::ShaderVariable>* getParameters(Model3D::BRDFType brdf);
};

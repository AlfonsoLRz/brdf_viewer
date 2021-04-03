#include "stdafx.h"
#include "BRDFShader.h"

#include "Utilities/FileManagement.h"

// [Initialization of static attributes]

const std::unordered_map<Model3D::BRDFType, std::string> BRDFShader::BRDF_MODULE_PATH = BRDFShader::getBRDFModulePath();
const std::string BRDFShader::INCLUDE_BRDF_HEADER = "INCLUDE BRDF";
const std::string BRDFShader::PARAMETERS_BEGIN = "::begin parameters";
const std::string BRDFShader::PARAMETERS_END = "::end parameters";
const std::string BRDFShader::SHADER_BEGIN = "::begin shader";
const std::string BRDFShader::SHADER_END = "::end shader";
 
// [Public methods]

BRDFShader::BRDFShader() : RenderingShader()
{
}

BRDFShader::~BRDFShader()
{
}

GLuint BRDFShader::createShaderProgram(const char* filename, Model3D::BRDFType brdf)
{
	if (_handler <= 0) {										// Shader identifier
		_handler = glCreateProgram();

		if (_handler == 0) {
			fprintf(stderr, "Cannot create shader program: %s!\n", filename);
			return 0;
		}
	}

	// [Vertex shader]
	char fileNameComplete[256];
	strcpy_s(fileNameComplete, filename);
	strcat_s(fileNameComplete, "-vert.glsl");

	const GLuint vertexShaderObject = compileShader(fileNameComplete, GL_VERTEX_SHADER, brdf);
	if (vertexShaderObject == 0) {
		return 0;
	}

	// [Fragment shader]
	strcpy_s(fileNameComplete, filename);
	strcat_s(fileNameComplete, "-frag.glsl");

	const GLuint fragmentShaderObject = compileShader(fileNameComplete, GL_FRAGMENT_SHADER, brdf);
	if (fragmentShaderObject == 0) {
		return 0;
	}

	// [Geometry shader, optional]
	strcpy_s(fileNameComplete, filename);
	strcat_s(fileNameComplete, "-geo.glsl");
	const GLuint geometryShaderObject = compileShader(fileNameComplete, GL_GEOMETRY_SHADER, brdf);

	glAttachShader(_handler, vertexShaderObject);						// Associate shaders with shader program
	glAttachShader(_handler, fragmentShaderObject);
	if (geometryShaderObject != 0) glAttachShader(_handler, geometryShaderObject);

	glLinkProgram(_handler);											// Link shader program to check errors
	GLint linkSuccess = 0;
	glGetProgramiv(_handler, GL_LINK_STATUS, &linkSuccess);

	if (linkSuccess == GL_FALSE)
	{
		GLint logLen = 0;
		glGetProgramiv(_handler, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0) {
			char* cLogString = new char[logLen];
			GLint written = 0;
			glGetProgramInfoLog(_handler, logLen, &written, cLogString);
			_logString.assign(cLogString);
			delete[] cLogString;

			std::cout << "Cannot link shader " << filename << ":" << std::endl << _logString << std::endl;
		}

		return 0;
	}
	else
	{
		_linked = true;
	}

	// Reserve space for subroutines
	GLint numSubroutines;
	glGetProgramStageiv(_handler, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutines);
	_activeSubroutineUniform[VERTEX_SHADER].resize(numSubroutines);
	std::fill(_activeSubroutineUniform[VERTEX_SHADER].begin(), _activeSubroutineUniform[VERTEX_SHADER].end(), -1);				// Non valid ID

	glGetProgramStageiv(_handler, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutines);
	_activeSubroutineUniform[FRAGMENT_SHADER].resize(numSubroutines);
	std::fill(_activeSubroutineUniform[FRAGMENT_SHADER].begin(), _activeSubroutineUniform[FRAGMENT_SHADER].end(), -1);

	glGetProgramStageiv(_handler, GL_GEOMETRY_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORMS, &numSubroutines);
	_activeSubroutineUniform[GEOMETRY_SHADER].resize(numSubroutines);
	std::fill(_activeSubroutineUniform[GEOMETRY_SHADER].begin(), _activeSubroutineUniform[GEOMETRY_SHADER].end(), -1);

	return _handler;
}

// [Protected methods]

GLuint BRDFShader::compileShader(const char* filename, const GLenum shaderType, Model3D::BRDFType brdfType)
{
	if (!fileExists(filename))
	{
		if (shaderType != GL_GEOMETRY_SHADER)
			fprintf(stderr, "Shader source file %s not found.\n", filename);

		return 0;
	}

	std::string shaderSourceString;
	if (!loadFileContent(std::string(filename), shaderSourceString))						// Read shader code
	{
		fprintf(stderr, "Cannot open shader source file.\n");
		return 0;
	}

	if (!includeLibraries(shaderSourceString))												// Libraries code not found 
	{
		fprintf(stderr, "Cannot include the specified modules.\n");
		return 0;
	}

	if (!includeShaderBRDF(shaderSourceString, brdfType))
	{
		fprintf(stderr, "Cannot include the BRDF shader.\n");
		return 0;
	}

	GLuint shaderHandler = glCreateShader(shaderType);
	if (shaderHandler == 0)
	{
		fprintf(stderr, "Cannot create shader object.\n");
		return 0;
	}

	const char* shaderSourceCString = shaderSourceString.c_str();							// Compile shader code
	glShaderSource(shaderHandler, 1, &shaderSourceCString, NULL);
	glCompileShader(shaderHandler);

	GLint compileResult;
	glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &compileResult);					// Result

	if (compileResult == GL_FALSE)
	{
		GLint logLen = 0;
		_logString = "";
		glGetShaderiv(shaderHandler, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char* cLogString = new char[logLen];
			GLint written = 0;

			glGetShaderInfoLog(shaderHandler, logLen, &written, cLogString);
			_logString.assign(cLogString);

			delete[] cLogString;
			std::cout << "Cannot compile shader " << shaderType << std::endl << _logString << std::endl;
		}
	}

	return shaderHandler;
}

void BRDFShader::findParameters(const std::string& fileContent, std::vector<ShaderVariable>& variables)
{
	const std::size_t beginPosition = fileContent.find(PARAMETERS_BEGIN);
	const std::size_t endPosition = fileContent.find(PARAMETERS_END);

	if ((beginPosition == std::string::npos) || (endPosition == std::string::npos))				// Incorrect syntax
	{
		return;
	}

	const std::string parametersString = fileContent.substr(beginPosition + PARAMETERS_BEGIN.size(), endPosition - beginPosition - PARAMETERS_END.size() - 2);
	std::istringstream stringReader (parametersString);
	std::string line;
	std::vector<float> floatTokens;

	std::vector<std::string> stringTokens;
	
	while (std::getline(stringReader, line)) {
		if (!line.empty())
		{
			FileManagement::clearTokens(stringTokens, floatTokens);
			FileManagement::readTokens(line, ' ', stringTokens, floatTokens);

			if (!stringTokens.empty() && !floatTokens.empty())
			{
				if (stringTokens[0] == "float" && floatTokens.size() == 3)
				{
					ShaderVariable variable {};
					variable._type = ShaderVariableType::FLOAT;
					variable._defaultFloat = floatTokens[2];
					variable._floatRange = vec2(floatTokens[0], floatTokens[1]);
					variables.push_back(variable);
				}
				else if (stringTokens[0] == "int" && floatTokens.size() == 3)
				{
					ShaderVariable variable {};
					variable._type = ShaderVariableType::INTEGER;
					variable._defaultInteger = floatTokens[2];
					variable._integerRange = vec2(floatTokens[0], floatTokens[1]);
					variables.push_back(variable);
				}
				else if (stringTokens[0] == "bool" && floatTokens.size() == 1)
				{
					ShaderVariable variable{};
					variable._type = ShaderVariableType::BOOLEAN;
					variable._defaultBoolean = bool(floatTokens[0]);
					variables.push_back(variable);
				}
			}
		}
	}
}

void BRDFShader::findShader(const std::string& fileContent, std::string& shaderContent)
{
	const std::size_t beginPosition = fileContent.find(SHADER_BEGIN);
	const std::size_t endPosition = fileContent.find(SHADER_END);

	if ((beginPosition == std::string::npos) || (endPosition == std::string::npos))				// Incorrect syntax
	{
		return;
	}

	shaderContent = fileContent.substr(beginPosition + SHADER_BEGIN.size(), endPosition - beginPosition - SHADER_END.size() - 2);
}

std::unordered_map<Model3D::BRDFType, std::string> BRDFShader::getBRDFModulePath()
{
	std::unordered_map<Model3D::BRDFType, std::string> brdfModulePath;

	brdfModulePath[Model3D::BRDFType::IDEAL_DIFFUSE] = "Assets/Shaders/BRDFs/IdealDiffuse.brdf";
	brdfModulePath[Model3D::BRDFType::OREN_NAYAR] = "Assets/Shaders/BRDFs/OrenNayar.brdf";
	brdfModulePath[Model3D::BRDFType::PHONG] = "Assets/Shaders/BRDFs/Phong.brdf";

	return brdfModulePath;
}

bool BRDFShader::includeShaderBRDF(std::string& shaderContent, Model3D::BRDFType brdfType)
{
	size_t headerPosition = shaderContent.find(INCLUDE_BRDF_HEADER);

	while (headerPosition != std::string::npos)
	{
		auto it = BRDF_MODULE_PATH.find(brdfType);
		if (it == BRDF_MODULE_PATH.end() || !fileExists(it->second))														// Library refers to a new file, does it exist?
		{
			return false;
		}

		// Read content from BRDF shader
		std::string fileContent, brdfShaderContent;
		std::vector<ShaderVariable> variables;
		this->loadFileContent(it->second, fileContent);

		// Find parameters from loaded string
		BRDFShader::findParameters(fileContent, variables);
		
		// Find shader implementation
		BRDFShader::findShader(fileContent, brdfShaderContent);
		
		shaderContent.erase(shaderContent.begin() + headerPosition, shaderContent.begin() + headerPosition + INCLUDE_BRDF_HEADER.size());			// Replace string in shader code
		shaderContent.insert(headerPosition, brdfShaderContent.c_str());
	}

	return true;
}

#include "stdafx.h"
#include "PGLLPointCloud.h"

#include <algorithm>
#include <filesystem>
#include "Graphics/Application/TextureList.h"
#include "Graphics/Core/ShaderList.h"
#include "Graphics/Core/VAO.h"
#include "Utilities/FileManagement.h"

// Initialization of static attributes
const std::string PGLLPointCloud::BINARY_EXTENSION = ".bin";

/// Public methods

PGLLPointCloud::PGLLPointCloud(const std::string& filename, const bool useBinary, const mat4& modelMatrix) : 
	Model3D(modelMatrix, 1), _filename(filename), _useBinary(useBinary)
{
}

PGLLPointCloud::~PGLLPointCloud()
{
}

bool PGLLPointCloud::load(const mat4& modelMatrix)
{
	if (!_loaded)
	{
		bool success = false, binaryExists = false;

		if (_useBinary && (binaryExists = std::filesystem::exists(_filename + BINARY_EXTENSION)))
		{
			success = this->loadModelFromBinaryFile();
		}

		if (!success)
		{
			success = this->loadModelFromPGLL(modelMatrix);
		}

		if (success)
		{
			this->computeCloudData();
			this->setVAOData();

			if (!binaryExists)
			{
				this->writeToBinary(_filename + BINARY_EXTENSION);
			}
		}

		_loaded = true;
		
		return true;
	}

	return false;
}

/// [Protected methods]

void PGLLPointCloud::computeCloudData()
{
	ModelComponent* modelComp = _modelComp[0];

	// Fill point cloud indices with iota
	modelComp->_pointCloud.resize(_points.size());
	std::iota(modelComp->_pointCloud.begin(), modelComp->_pointCloud.end(), 0);
}

bool PGLLPointCloud::loadModelFromBinaryFile()
{
	return this->readBinary(_filename + BINARY_EXTENSION, _modelComp);
}

bool PGLLPointCloud::loadModelFromPGLL(const mat4& modelMatrix)
{
	float phi, theta, intensity;
	bool foundHeader = false;
	std::string line;
	std::ifstream in;
	std::vector<std::string> stringTokens;
	std::vector<float> floatTokens;

	in.open(_filename);
	if (!in)
	{
		return false;
	}

	while (!foundHeader && !(in >> std::ws).eof())
	{
		std::getline(in, line);

		if (line.find(INTENSITY_HEADER) != std::string::npos) foundHeader = true;
	}

	if (foundHeader)
	{
		std::vector<vec2> minMaxElevation(180);

		for (vec2& minMax : minMaxElevation)
		{
			minMax = vec2(FLT_MAX, FLT_MIN);
		}
		
		while (!(in >> std::ws).eof())
		{
			std::getline(in, line);

			FileManagement::readTokens(line, DELIMITER, stringTokens, floatTokens);

			if (floatTokens.size() == 3)
			{
				phi = glm::radians(floatTokens[1]);
				theta = glm::radians(floatTokens[0]);
				intensity = floatTokens[2];

				vec4 xzDirection = glm::rotate(mat4(1.0f), phi, vec3(.0f, 1.0f, .0f)) * vec4(1.0f, .0f, .0f, .0f);
				vec4 yDirection = glm::rotate(mat4(1.0f), theta, vec3(.0f, 0.0f, 1.0f)) * vec4(1.0f, .0f, .0f, .0f);

				_points.push_back(vec4(glm::normalize(vec3(xzDirection.x, yDirection.y, xzDirection.z)) * floatTokens[2], 1.0f));

				const int azimuth = int(floatTokens[1]);
				minMaxElevation[azimuth].x = glm::min(minMaxElevation[azimuth].x, floatTokens[0]);
				minMaxElevation[azimuth].y = glm::max(minMaxElevation[azimuth].y, floatTokens[0]);

				FileManagement::clearTokens(stringTokens, floatTokens);
			}
		}

		for (int angle = 0; angle < 180; ++angle)
		{
			std::cout << "Angle " << angle << ": " << minMaxElevation[angle].x << ", " << minMaxElevation[angle].y << std::endl;
		}
	}

	in.close();

	return !_points.empty();
}

bool PGLLPointCloud::readBinary(const std::string& filename, const std::vector<Model3D::ModelComponent*>& modelComp)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
	if (!fin.is_open())
	{
		return false;
	}

	size_t numPoints;

	fin.read((char*)&numPoints, sizeof(size_t));
	_points.resize(numPoints);
	fin.read((char*)&_points[0], numPoints * sizeof(vec4));
	fin.read((char*)&_aabb, sizeof(AABB));

	fin.close();

	return true;
}

void PGLLPointCloud::setVAOData()
{
	VAO* vao = new VAO(false);
	ModelComponent* modelComp = _modelComp[0];

	// Refresh point cloud length
	modelComp->_topologyIndicesLength[RendEnum::IBO_POINT_CLOUD] = unsigned(modelComp->_pointCloud.size());

	vao->setVBOData(RendEnum::VBO_POSITION, _points, GL_STATIC_DRAW);
	vao->setIBOData(RendEnum::IBO_POINT_CLOUD, modelComp->_pointCloud);

	modelComp->_vao = vao;
}

bool PGLLPointCloud::writeToBinary(const std::string& filename)
{
	std::ofstream fout(filename, std::ios::out | std::ios::binary);
	if (!fout.is_open())
	{
		return false;
	}

	const size_t numPoints = _points.size();
	fout.write((char*)&numPoints, sizeof(size_t));
	fout.write((char*)&_points[0], numPoints * sizeof(vec4));
	fout.write((char*)&_aabb, sizeof(AABB));

	fout.close();

	return true;
}
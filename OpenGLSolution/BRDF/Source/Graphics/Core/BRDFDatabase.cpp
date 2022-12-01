#include "stdafx.h"
#include "BRDFDatabase.h"

#include "bsdf/powitacq.h"

const std::string BRDFDatabase::BINARY_DATABASE = "database.bin";
const std::string BRDFDatabase::BINARY_MATERIAL_EXTENSION = ".bsdf";
const unsigned BRDFDatabase::THETA_SAMPLES = 90;
const unsigned BRDFDatabase::PHI_SAMPLES = 360;

// [Public methods]

BRDFDatabase::BRDFDatabase(const std::string& folder, bool useBinary)
{
	_materials.reserve(1000);

	for (const auto& file : std::filesystem::directory_iterator(folder))
	{
		std::string fileName = file.path().filename().string();

		if (fileName.find(BINARY_MATERIAL_EXTENSION) != std::string::npos)
		{
			_materials.push_back(const_cast<char*>(fileName.c_str()));
			_materialPath[fileName] = file.path().string();
		}
	}
}

BRDFDatabase::~BRDFDatabase()
{
}

vec2 BRDFDatabase::getWavelengthLimits()
{
	if (!_wavelengths.empty())
	{
		return vec2(_wavelengths[0], _wavelengths[_wavelengths.size() - 1]);
	}

	return vec2(.0f);
}

void BRDFDatabase::loadMaterial(unsigned index, const vec3& L)
{
	if (index < _materials.size())
	{
		powitacq::BRDF brdf(_materialPath[_materials[index]]);
		_wavelengths = std::vector<float>(std::begin(brdf.wavelengths()), std::end(brdf.wavelengths()));
		_reflectance.clear();

		// Sample wi and wo
		#pragma omp parallel for
		for (int theta = 0; theta <= THETA_SAMPLES; ++theta)
		{
			for (int phi = 0; phi <= PHI_SAMPLES; ++phi)
			{
				float f_phi = phi / float(PHI_SAMPLES) * 2.0f * M_PI;
				float f_theta = theta / float(THETA_SAMPLES) * M_PI / 2.0f;
				vec3 fOut = vec3(glm::cos(f_phi), glm::sin(f_theta), -glm::sin(f_phi));
				powitacq::Vector3f wo = powitacq::Vector3f(fOut.x, fOut.z, fOut.y);

				auto spectrum = brdf.eval(powitacq::Vector3f(L.x, L.z, L.y), wo);
				#pragma omp critical
				_reflectance.insert(_reflectance.end(), std::begin(spectrum), std::end(spectrum));
			}
		}
	}
}

void BRDFDatabase::lookUpMaterial(float wl, std::vector<float>& reflectance)
{
	if (!_reflectance.empty())
	{
		unsigned wlIndex = this->findWavelengthIndex(wl), index;
		reflectance.resize((PHI_SAMPLES + 1) * (THETA_SAMPLES + 1));

		#pragma omp parallel for
		for (int theta = 0; theta <= THETA_SAMPLES; ++theta)
		{
			for (int phi = 0; phi <= PHI_SAMPLES; ++phi)
			{
				index = phi * (THETA_SAMPLES + 1) + theta;
				reflectance[index] = _reflectance[index * _wavelengths.size() + wl];
			}
		}
	}
}

// [Protected methods]

unsigned BRDFDatabase::findWavelengthIndex(float wl)
{
	float minDistance = FLT_MAX;

	for (int wlIndex = 0; wlIndex < _wavelengths.size(); ++wlIndex)
	{
		if (glm::distance(_wavelengths[wlIndex], wl) < minDistance)
		{
			minDistance = glm::distance(_wavelengths[wlIndex], wl);
		}
		else
		{
			return wlIndex - 1;
		}
	}

	return _wavelengths.size();
}

bool BRDFDatabase::readBRDF(const std::string& filename)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);
	if (!fin.is_open())
	{
		return false;
	}
}

#pragma once

#define POWITACQ_IMPLEMENTATION

class BRDFDatabase
{
protected:
	const static std::string BINARY_DATABASE;
	const static std::string BINARY_MATERIAL_EXTENSION;
	const static unsigned THETA_SAMPLES, PHI_SAMPLES;

	std::vector<std::string>						_materials;
	std::unordered_map<std::string, std::string>	_materialPath;
	std::vector<float>								_reflectance;
	std::vector<float>								_wavelengths;

protected:
	/**
	*	@return 
	*/
	unsigned findWavelengthIndex(float wl);

	/**
	*	@brief Reads a binary BRDF as stored by external papers.
	*/
	bool readBRDF(const std::string& filename);

public:
	/**
	*	@brief Constructor for a database located in 'filename'.
	*/
	BRDFDatabase(const std::string& folder, bool useBinary = true);

	/**
	*	@brief Destructor.
	*/
	virtual ~BRDFDatabase();

	/**
	*	@return Vector with material names.
	*/
	std::vector<std::string>* getMaterialNames() { return &_materials; }

	/**
	*	@return Limits of loaded wavelengths.
	*/
	vec2 getWavelengthLimits();

	/**
	*	@brief 
	*/
	void loadMaterial(unsigned index, const vec3& L);

	/**
	*	@brief 
	*/
	void lookUpMaterial(float wl, std::vector<float>& reflectance);
};


#pragma once

#include "Graphics/Application/SSAOScene.h"
#include "Graphics/Core/BRDFShader.h"
#include "Graphics/Core/BRDFSphere.h"
#include "Graphics/Core/PlanarSurface.h"

#define NEW_LIGHT "!"

#define CAMERA_POS_HEADER "Position"
#define CAMERA_LOOKAT_HEADER "LookAt"

#define AMBIENT_INTENSITY "AmbientIntensity"
#define DIFFUSE_INTENSITY "DiffuseIntensity"
#define SPECULAR_INTENSITY "SpecularIntensity"
#define LIGHT_TYPE "LightType"
#define ORTHO_SIZE "OrthoBottomLeftCorner"
#define LIGHT_POSITION "LightPosition"
#define LIGHT_DIRECTION "LightDirection"
#define CAST_SHADOWS "CastShadows"
#define SHADOW_INTENSITY "ShadowIntensity"
#define SHADOW_MAP_SIZE "ShadowMapSize"
#define BLUR_SHADOW_SIZE "BlurShadow"
#define SHADOW_CAMERA_ANGLE_X "ShadowCameraAngle_X"
#define SHADOW_CAMERA_ANGLE_Y "ShadowCameraAngle_Y"
#define SHADOW_CAMERA_RASPECT "ShadowCameraRaspect"
#define SHADOW_RADIUS "ShadowRadius"

/**
*	@file CADScene.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 28/11/2020
*/

/**
*	@brief Scene composed of CAD models for the manuscript of this work.
*/
class BRDFScene: public SSAOScene
{
protected:
	const static std::string SCENE_SETTINGS_FOLDER;			//!<	

	// Settings constraints
	const static std::string SCENE_CAMERA_FILE;				//!<
	const static std::string SCENE_LIGHTS_FILE;				//!<

protected:
	// Models
	BRDFSphere*		_brdfSphere;				//!<
	PlanarSurface*	_plane;				//!<

	// Shaders
	BRDFShader* _testShader;	

protected:
	/**
	*	@brief True if the file is a known model file, such as obj.
	*/
	bool isExtensionReadable(const std::string& filename);

	/**
	*	@brief Loads a camera with code-defined values.
	*/
	void loadDefaultCamera(Camera* camera);

	/**
	*	@brief Loads lights with code-defined values.
	*/
	void loadDefaultLights();

	/**
	*	@brief Loads the cameras from where the scene is observed. If no file is accessible, then a default camera is loaded.
	*/
	virtual void loadCameras();

	/**
	*	@brief Loads the lights which affects the scene. If no file is accessible, then default lights are loaded.
	*/
	virtual void loadLights();

	/**
	*	@brief Loads the models which are necessary to render the scene.
	*/
	virtual void loadModels();

	/**
	*	@brief Loads camera values from a file, if possible.
	*/
	bool readCameraFromSettings(Camera* camera);

	/**
	*	@brief Load lights from a file, if possible.
	*/
	bool readLightsFromSettings();

	// ------ Rendering ------

	/**
	*	@brief Renders the scene as a set of triangles.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void drawAsTriangles(Camera* camera, const mat4& mModel, RenderingParameters* rendParams);

public:
	/**
	*	@brief Default constructor.
	*/
	BRDFScene();

	/**
	*	@brief Destructor. Frees memory allocated for 3d models.
	*/
	virtual ~BRDFScene();

	/**
	*	@brief Draws the scene as the rendering parameters specifies.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void render(const mat4& mModel, RenderingParameters* rendParams);

	// ------- Getters --------

	/**
	*	@return
	*/
	Model3D::BRDFType* getSphereBRDF();
};


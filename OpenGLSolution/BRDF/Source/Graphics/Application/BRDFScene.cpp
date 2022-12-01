#include "stdafx.h"
#include "BRDFScene.h"

#include <filesystem>
#include <regex>
#include "Graphics/Application/TextureList.h"
#include "Graphics/Core/CADModel.h"
#include "Graphics/Core/Light.h"
#include "Graphics/Core/OpenGLUtilities.h"

/// Initialization of static attributes
const std::string BRDFScene::SCENE_SETTINGS_FOLDER = "Assets/Scene/Settings/BRDF/";

const std::string BRDFScene::BRDF_MATERIAL_DB_FOLDER = "D:/Github/ScannerSimulator/ScannerSimulator/Assets/BRDF/brdfs_rgl_18/";
const std::string BRDFScene::SCENE_CAMERA_FILE = "Camera.txt";
const std::string BRDFScene::SCENE_LIGHTS_FILE = "Lights.txt";

// [Public methods]

BRDFScene::BRDFScene() :
	_brdfSphere(nullptr), _plane(nullptr), _vectorsVAO(nullptr),
	_brdfPointCloudShader(nullptr), _brdfTriangleMeshShader(nullptr), _brdfTriangleMeshNormalShader(nullptr), _brdfTriangleMeshPositionShader(nullptr),
	_brdfShadowsShader(nullptr), _brdfWireframeShader(nullptr)
{
	_brdfVectorRendering = ShaderList::getInstance()->getRenderingShader(RendEnum::BRDF_VECTOR_SHADER);

	// Define a starting position for view and light vectors
	{
		std::vector<Model3D::VertexGPUData> position{ Model3D::VertexGPUData { vec3(.0f, .0f, .0f)} };
		std::vector<unsigned> indices{ 0 };
		_vectorsVAO = new VAO(true);

		_vectorsVAO->setVBOData(position);
		_vectorsVAO->setIBOData(RendEnum::IBO_POINT_CLOUD, indices);;
	}
}

BRDFScene::~BRDFScene()
{
	delete _vectorsVAO;
	
	delete _brdfPointCloudShader;
	delete _brdfTriangleMeshShader;
	delete _brdfTriangleMeshNormalShader;
	delete _brdfTriangleMeshPositionShader;
	delete _brdfShadowsShader;
	delete _brdfWireframeShader;
}

void BRDFScene::render(const mat4& mModel, RenderingParameters* rendParams)
{
	SSAOScene::render(mModel, rendParams);
}

void BRDFScene::updateBRDF(Model3D::BRDFType newBRDF)
{
	// Change sphere for rendering
	_brdfSphere->getModelCompent(0)->_brdf = newBRDF;
	_cadModel->getModelCompent(0)->_brdf = newBRDF;

	if (newBRDF == Model3D::NONE) return;

	//BRDFShader::clearCache();

	// Delete previous shaders
	delete _brdfPointCloudShader;
	delete _brdfTriangleMeshShader;
	delete _brdfTriangleMeshNormalShader;
	delete _brdfTriangleMeshPositionShader;
	delete _brdfShadowsShader;
	delete _brdfWireframeShader;

	// Compile new shaders
	_brdfPointCloudShader = new BRDFShader();
	_brdfPointCloudShader->createShaderProgram("Assets/Shaders/Points/brdfPointCloud", newBRDF);
	
	_brdfTriangleMeshShader = new BRDFShader();
	_brdfTriangleMeshShader->createShaderProgram("Assets/Shaders/Triangles/brdfTriangleMesh", newBRDF);

	_brdfTriangleMeshNormalShader = new BRDFShader();
	_brdfTriangleMeshNormalShader->createShaderProgram("Assets/Shaders/Triangles/brdfTriangleMeshNormal", newBRDF);

	_brdfTriangleMeshPositionShader = new BRDFShader();
	_brdfTriangleMeshPositionShader->createShaderProgram("Assets/Shaders/Triangles/brdfTriangleMeshPosition", newBRDF);

	_brdfShadowsShader = new BRDFShader();
	_brdfShadowsShader->createShaderProgram("Assets/Shaders/Triangles/brdfShadowsShader", newBRDF);

	_brdfWireframeShader = new BRDFShader();
	_brdfWireframeShader->createShaderProgram("Assets/Shaders/Lines/brdfWireframe", newBRDF);
}

void BRDFScene::updateBSDF(RenderingParameters* rendParams, bool updateMaterial)
{
	std::vector<float> reflectance;

	if (updateMaterial) _brdfDatabase->loadMaterial(rendParams->_bsdfType, rendParams->_L);
	_brdfDatabase->lookUpMaterial(rendParams->_bsdfWavelength, reflectance);
	_brdfSphere->updateReflectance(reflectance);
}

Model3D::BRDFType* BRDFScene::getSphereBRDF()
{
	return &_brdfSphere->getModelCompent(0)->_brdf;
}

// [Protected methods]

bool BRDFScene::isExtensionReadable(const std::string& filename)
{
	return filename.find(CADModel::OBJ_EXTENSION) != std::string::npos;
}

void BRDFScene::loadDefaultCamera(Camera* camera)
{
	camera->setPosition(vec3(3.0f, 1.338f, 2.94f));
	camera->setLookAt(vec3(-5.31f, 1.65f, -2.57f));
}

void BRDFScene::loadDefaultLights()
{
	Light* pointLight_01 = new Light();
	pointLight_01->setLightType(Light::POINT_LIGHT);
	pointLight_01->setPosition(vec3(1.64f, 2.0f, -0.12f));
	pointLight_01->setId(vec3(0.35f));
	pointLight_01->setIs(vec3(0.0f));

	_lights.push_back(std::unique_ptr<Light>(pointLight_01));

	Light* pointLight_02 = new Light();
	pointLight_02->setLightType(Light::POINT_LIGHT);
	pointLight_02->setPosition(vec3(-2.86f, 2.0f, -2.13f));
	pointLight_02->setId(vec3(0.35f));
	pointLight_02->setIs(vec3(0.0f));

	_lights.push_back(std::unique_ptr<Light>(pointLight_02));

	Light* sunLight = new Light();
	Camera* camera = sunLight->getCamera();
	ShadowMap* shadowMap = sunLight->getShadowMap();
	camera->setBottomLeftCorner(vec2(-7.0f, -7.0f));
	shadowMap->modifySize(4096, 4096);
	sunLight->setLightType(Light::DIRECTIONAL_LIGHT);
	sunLight->setPosition(vec3(.0f, 3.0f, -5.0f));
	sunLight->setDirection(vec3(-0.1, -0.8f, 1.0f));
	sunLight->setId(vec3(0.5f));
	sunLight->setIs(vec3(0.0f));
	sunLight->castShadows(true);
	sunLight->setShadowIntensity(0.2f, 1.0f);
	sunLight->setShadowRadius(.001f);
	sunLight->setBlurFilterSize(5);

	_lights.push_back(std::unique_ptr<Light>(sunLight));

	Light* fillLight = new Light();
	fillLight->setLightType(Light::DIRECTIONAL_LIGHT);
	fillLight->setDirection(vec3(-1.0f, 1.0f, 0.0f));
	fillLight->setId(vec3(0.1f));
	fillLight->setIs(vec3(0.0f));

	_lights.push_back(std::unique_ptr<Light>(fillLight));

	Light* rimLight = new Light();
	rimLight->setLightType(Light::RIM_LIGHT);
	rimLight->setIa(vec3(0.015f, 0.015f, 0.05f));

	_lights.push_back(std::unique_ptr<Light>(rimLight));
}

void BRDFScene::loadCameras()
{
	ivec2 canvasSize = _window->getSize();
	Camera* camera = new Camera(canvasSize[0], canvasSize[1]);

	if (!this->readCameraFromSettings(camera))
	{
		this->loadDefaultCamera(camera);
	}

	camera->setPosition(vec3(1.70015f, 1.45277f, 0.683818f));
	camera->setLookAt(vec3(-6.92137f, -2.67601f, -2.16847f));

	//camera->setPosition(vec3(1.72121f, 0.902009f, -0.154246f));
	//camera->setLookAt(vec3(-7.45128f, -2.93413f, 0.659658f));

	_cameraManager->insertCamera(camera);
}

void BRDFScene::loadLights()
{
	if (!this->readLightsFromSettings())
	{
		this->loadDefaultLights();
	}

	Scene::loadLights();
}

void BRDFScene::loadModels()
{
	{
		MaterialList* materialList = MaterialList::getInstance();

		_sceneGroup = new Group3D();

		//_plane = new PlanarSurface(5.0f, 5.0f, 10, 10, 1.0f, 1.0f, glm::rotate(mat4(1.0f), glm::pi<float>() / 2.0f, vec3(.0f, 1.0f, .0f)));
		//_plane->setMaterial(materialList->getMaterial(CGAppEnum::MATERIAL_CHECKER));
		//_plane->setName("Planar Surface", 0);
		//_sceneGroup->addComponent(_plane);

		_brdfSphere = new BRDFSphere();
		_brdfSphere->setMaterial(materialList->getMaterial(CGAppEnum::MATERIAL_HEIGHT));
		_brdfSphere->setName("BRDF Sphere", 0);
		_brdfSphere->setActive(true);
		_sceneGroup->addComponent(_brdfSphere);

		_cadModel = new CADModel("Assets/Models/Research/Dragon", "", true);
		_cadModel->setName("3D Model", 0);
		_cadModel->setActive(false);

		_sceneGroup->addComponent(_cadModel);

		_brdfDatabase = new BRDFDatabase(BRDF_MATERIAL_DB_FOLDER);
	}

	SSAOScene::loadModels();
	this->updateBRDF(Model3D::COOK_TORRANCE);
}

bool BRDFScene::readCameraFromSettings(Camera* camera)
{
	const std::string filename = SCENE_SETTINGS_FOLDER + SCENE_CAMERA_FILE;
	std::string currentLine, lineHeader;	
	std::stringstream line;
	std::ifstream inputStream;	
	vec3 value;

	inputStream.open(filename.c_str());

	if (inputStream.fail()) return false;

	while (!(inputStream >> std::ws).eof())
	{
		std::getline(inputStream, currentLine);

		line.clear();
		line.str(currentLine);
		std::getline(line, lineHeader, ' ');

		if (lineHeader.find(COMMENT_CHAR) == std::string::npos)
		{
			for (int i = 0; i < 3; ++i)
			{
				line >> value[i];
				line.ignore();
			}

			if (lineHeader == CAMERA_POS_HEADER)
			{
				camera->setPosition(value);
			}
			else if (lineHeader == CAMERA_LOOKAT_HEADER)
			{
				camera->setLookAt(value);
			}
		}
	}

	inputStream.close();	

	return true;
}

bool BRDFScene::readLightsFromSettings()
{
	// File management
	const std::string filename = SCENE_SETTINGS_FOLDER + SCENE_LIGHTS_FILE;
	std::string currentLine, lineHeader;
	std::stringstream line;
	std::ifstream inputStream;

	Light* light = nullptr;
	vec3 vec3val;
	vec2 vec2val;
	float floatval;
	std::string stringval;

	inputStream.open(filename.c_str());

	if (inputStream.fail()) return false;

	while (!(inputStream >> std::ws).eof())
	{
		std::getline(inputStream, currentLine);

		line.clear();
		line.str(currentLine);
		std::getline(line, lineHeader, '\t');

		if (lineHeader.empty())
		{
			std::getline(line, lineHeader, ' ');
		}

		if (lineHeader.find(COMMENT_CHAR) == std::string::npos)
		{
			if (lineHeader == NEW_LIGHT)
			{
				if (light) _lights.push_back(std::unique_ptr<Light>(light));

				light = new Light();
			}
			else if (light)
			{
				if (lineHeader.find(LIGHT_POSITION) != std::string::npos)
				{
					for (int i = 0; i < 3; ++i) { line >> vec3val[i]; line.ignore(); }

					light->setPosition(vec3val);
				}
				else if (lineHeader.find(LIGHT_DIRECTION) != std::string::npos)
				{
					for (int i = 0; i < 3; ++i) { line >> vec3val[i]; line.ignore(); }

					light->setDirection(vec3val);
				}
				else if (lineHeader.find(LIGHT_TYPE) != std::string::npos)
				{
					line >> stringval;

					Light::LightModels type = Light::stringToLightModel(stringval);
					light->setLightType(type);
				}
				else if (lineHeader.find(AMBIENT_INTENSITY) != std::string::npos)
				{
					for (int i = 0; i < 3; ++i) { line >> vec3val[i]; line.ignore(); }

					light->setIa(vec3val);
				}
				else if (lineHeader.find(DIFFUSE_INTENSITY) != std::string::npos)
				{
					for (int i = 0; i < 3; ++i) { line >> vec3val[i]; line.ignore(); }

					light->setId(vec3val);
				}
				else if (lineHeader.find(SPECULAR_INTENSITY) != std::string::npos)
				{
					for (int i = 0; i < 3; ++i) { line >> vec3val[i]; line.ignore(); }

					light->setIs(vec3val);
				}
				else if (lineHeader.find(SHADOW_MAP_SIZE) != std::string::npos)
				{
					for (int i = 0; i < 2; ++i) { line >> vec2val[i]; line.ignore(); }

					light->getShadowMap()->modifySize(vec2val.x, vec2val.y);
				}
				else if (lineHeader.find(BLUR_SHADOW_SIZE) != std::string::npos)
				{
					line >> floatval;

					light->setBlurFilterSize(floatval);
				}
				else if (lineHeader.find(ORTHO_SIZE) != std::string::npos)
				{
					for (int i = 0; i < 2; ++i) { line >> vec2val[i]; line.ignore(); }

					light->getCamera()->setBottomLeftCorner(vec2val);
				}
				else if (lineHeader.find(SHADOW_INTENSITY) != std::string::npos)
				{
					for (int i = 0; i < 2; ++i) { line >> vec2val[i]; line.ignore(); }

					light->setShadowIntensity(vec2val.x, vec2val.y);
				}
				else if (lineHeader.find(CAST_SHADOWS) != std::string::npos)
				{
					line >> stringval;

					light->castShadows(stringval == "true" || stringval == "True");
				}
				else if (lineHeader.find(SHADOW_CAMERA_ANGLE_X) != std::string::npos)
				{
					line >> floatval;

					light->getCamera()->setFovX(floatval);
				}
				else if (lineHeader.find(SHADOW_CAMERA_ANGLE_Y) != std::string::npos)
				{
					line >> floatval;

					light->getCamera()->setFovY(floatval);
				}
				else if (lineHeader.find(SHADOW_CAMERA_RASPECT) != std::string::npos)
				{
					for (int i = 0; i < 2; ++i) { line >> vec2val[i]; line.ignore(); }

					light->getCamera()->setRaspect(vec2val.x, vec2val.y);
				}
				else if (lineHeader.find(SHADOW_RADIUS) != std::string::npos)
				{
					line >> floatval;

					light->setShadowRadius(floatval);
				}
			}
		}
	}

	if (light) _lights.push_back(std::unique_ptr<Light>(light));

	inputStream.close();

	return true;
}

void BRDFScene::drawAsPoints(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = mModel;
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	_brdfPointCloudShader->use();
	_brdfPointCloudShader->applyActiveSubroutines();

	this->drawSceneAsPoints(_brdfPointCloudShader, RendEnum::POINT_CLOUD_SHADER, &matrix, rendParams);
}

void BRDFScene::drawAsLines(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = mModel;
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	_brdfWireframeShader->use();
	_brdfWireframeShader->applyActiveSubroutines();

	this->drawSceneAsLines(_brdfWireframeShader, RendEnum::WIREFRAME_SHADER, &matrix, rendParams);
}

void BRDFScene::drawAsTriangles(Camera* camera, const mat4& mModel, RenderingParameters* rendParams)
{
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());
	const mat4 bias = glm::translate(mat4(1.0f), vec3(0.5f)) * glm::scale(mat4(1.0f), vec3(0.5f));						// Proj: [-1, 1] => with bias: [0, 1]

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = camera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = camera->getViewProjMatrix();

		glDepthFunc(GL_LEQUAL);

		_brdfTriangleMeshShader->use();
		_brdfTriangleMeshShader->setUniform("materialScattering", rendParams->_materialScattering);
	}

	{
		for (unsigned int i = 0; i < _lights.size(); ++i)																// Multipass rendering
		{
			if (i == 0)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}

			if (_lights[i]->shouldCastShadows())
			{
				matrix[RendEnum::BIAS_VIEW_PROJ_MATRIX] = bias * _lights[i]->getCamera()->getViewProjMatrix();
			}

			// First shader
			_lights[i]->applyLight(_brdfTriangleMeshShader, matrix[RendEnum::VIEW_MATRIX]);
			_lights[i]->applyShadowMapTexture(_brdfTriangleMeshShader);
			_brdfTriangleMeshShader->applyActiveSubroutines();

			_brdfTriangleMeshShader->setUniform("ignoreTexture", rendParams->_ignoreTexture);
			if (rendParams->_ignoreTexture)
			{
				_brdfTriangleMeshShader->setUniform("altColor", rendParams->_textureReplacementColor);
			}

			_brdfTriangleMeshShader->setSubroutineUniform(GL_VERTEX_SHADER, "vertexModifierUniform", "bsdfModifier");
			_sceneGroup->drawAsTriangles(_brdfTriangleMeshShader, RendEnum::TRIANGLE_MESH_SHADER, matrix);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);					// Back to initial state
	glDepthFunc(GL_LESS);
}

void BRDFScene::drawAsTriangles4Position(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = activeCamera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

		_brdfTriangleMeshPositionShader->use();

		this->drawSceneAsTriangles4Position(_brdfTriangleMeshPositionShader, RendEnum::TRIANGLE_MESH_POSITION_SHADER, &matrix, rendParams);
	}
}

void BRDFScene::drawAsTriangles4Normal(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = activeCamera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

		_brdfTriangleMeshNormalShader->use();

		this->drawSceneAsTriangles4Normal(_brdfTriangleMeshNormalShader, RendEnum::TRIANGLE_MESH_POSITION_SHADER, &matrix, rendParams);
	}
}

void BRDFScene::drawAsTriangles4Shadows(const mat4& mModel, RenderingParameters* rendParams)
{
	const ivec2 canvasSize = rendParams->_viewportSize;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		
		glEnable(GL_CULL_FACE);

		_brdfShadowsShader->use();
	}

	for (unsigned int i = 0; i < _lights.size(); ++i)
	{
		Light* light = _lights[i].get();

		if (light->shouldCastShadows() && _computeShadowMap[i])
		{
			ShadowMap* shadowMap = light->getShadowMap();
			const ivec2 size = shadowMap->getSize();

			{
				shadowMap->bindFBO();
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, size.x, size.y);
				_brdfShadowsShader->applyActiveSubroutines();
			}

			matrix[RendEnum::VIEW_PROJ_MATRIX] = light->getCamera()->getViewProjMatrix();

			_sceneGroup->drawAsTriangles4Shadows(_brdfShadowsShader, RendEnum::SHADOWS_SHADER, matrix);

			//_computeShadowMap[i] = false;
		}
	}

	glViewport(0, 0, canvasSize.x, canvasSize.y);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BRDFScene::renderOtherStructures(const mat4& mModel, RenderingParameters* rendParams)
{
	SSAOScene::renderOtherStructures(mModel, rendParams);

	if (rendParams->_renderBRDFVectors)
	{
		Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
		std::vector<mat4> matrix(RendEnum::numMatricesTypes());

		{
			matrix[RendEnum::MODEL_MATRIX] = mModel;
			matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

			_brdfVectorRendering->use();
			_brdfVectorRendering->applyActiveSubroutines();
			_brdfVectorRendering->setUniform("mModelViewProj", matrix[RendEnum::VIEW_PROJ_MATRIX] * matrix[RendEnum::MODEL_MATRIX]);
			_brdfVectorRendering->setUniform("lightDirection", glm::normalize(rendParams->_L) * rendParams->_vectorScale);
			_brdfVectorRendering->setUniform("lightVectorColor", rendParams->_lightColor);
			_brdfVectorRendering->setUniform("viewDirection", glm::normalize(rendParams->_V) * rendParams->_vectorScale);
			_brdfVectorRendering->setUniform("viewVectorColor", rendParams->_viewColor);

			_vectorsVAO->drawObject(RendEnum::IBO_POINT_CLOUD, GL_POINTS, 1);
		}
	}
}

void BRDFScene::renderScene(const mat4& mModel, RenderingParameters* rendParams)
{
	int visualizationMode = rendParams->_visualizationMode;

	switch (visualizationMode)
	{
	case CGAppEnum::VIS_POINTS:
		this->renderPointCloud(mModel, rendParams);
		break;
	case CGAppEnum::VIS_LINES:
		this->renderWireframe(mModel, rendParams);
		break;
	case CGAppEnum::VIS_TRIANGLES:
		this->renderTriangleMesh(mModel, rendParams);
		break;
	case CGAppEnum::VIS_ALL_TOGETHER:
		this->renderTriangleMesh(mModel, rendParams);
		this->renderWireframe(mModel, rendParams);
		this->renderPointCloud(mModel, rendParams);
		break;
	}

	this->renderVectors(mModel, rendParams);
	this->renderOtherStructures(mModel, rendParams);
}

void BRDFScene::renderVectors(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = mat4(1.0f);
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	// BVH rendering
	{
		//RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::VECTOR_INSTANCE_SHADER);
		//shader->use();
		//shader->applyActiveSubroutines();

		//_sceneGroup->drawBVH(shader, RendEnum::WIREFRAME_SHADER, matrix);
	}
}

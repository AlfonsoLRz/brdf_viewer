#include "stdafx.h"
#include "BRDFSphere.h"

#include "Graphics/Application/Renderer.h"
#include "OpenGLUtilities.h"

// [Initialization of static attributes

const unsigned BRDFSphere::SECTORS = 360 * 1;
const unsigned BRDFSphere::STACKS = 90 * 1;

// [Public methods]

BRDFSphere::BRDFSphere(const mat4& modelMatrix) : Model3D(modelMatrix)
{
}

BRDFSphere::~BRDFSphere()
{
}

bool BRDFSphere::load(const mat4& modelMatrix)
{
	if (!_loaded)
	{
		ModelComponent* modelComponent = Primitives::getSemisphereModelComp(SECTORS, STACKS, 1.0f);
		_modelComp[0]->_geometry = std::move(modelComponent->_geometry);
		_modelComp[0]->_topology = std::move(modelComponent->_topology);
		_modelComp[0]->_triangleMesh = std::move(modelComponent->_triangleMesh);
		_modelComp[0]->buildPointCloudTopology();
		_modelComp[0]->buildWireframeTopology();

		this->setVAOData();
		
		_loaded = true;

		return true;
	}

	return false;
}

void BRDFSphere::updateReflectance(std::vector<float>& reflectance)
{
	for (int vertexId = 0; vertexId < reflectance.size(); ++vertexId)
	{
		_modelComp[0]->_geometry[vertexId]._bsdf = reflectance[vertexId];
	}

	_modelComp[0]->_vao->setVBOData(_modelComp[0]->_geometry, GL_DYNAMIC_DRAW);
}

// [Protected methods]

void BRDFSphere::renderTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix, ModelComponent* modelComp, const GLuint primitive)
{
	VAO* vao = modelComp->_vao;
	Material* material = modelComp->_material;

	if (vao)
	{
		shader->setUniform("heightBoundaries", vec2(.0f, 1.0f));
		shader->setUniform("heightScale", Renderer::getInstance()->getRenderingParameters()->_heightTextureScale);
		this->setShaderUniforms(shader, shaderType, matrix);

		if (material)
		{
			material->applyMaterial(shader);
			shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "kadUniform", "getHeightKad");
			shader->applyActiveSubroutines();
		}

		vao->drawObject(RendEnum::IBO_TRIANGLE_MESH, primitive, GLuint(modelComp->_topologyIndicesLength[RendEnum::IBO_TRIANGLE_MESH]));
	}
}
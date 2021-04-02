#include "stdafx.h"
#include "BRDFSphere.h"

#include "OpenGLUtilities.h"

// [Initialization of static attributes

const unsigned BRDFSphere::SECTORS = 360 * 3;
const unsigned BRDFSphere::STACKS = 90 * 3;

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

// [Protected methods]

void BRDFSphere::renderTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix, ModelComponent* modelComp, const GLuint primitive)
{
	VAO* vao = modelComp->_vao;
	Material* material = modelComp->_material;

	if (vao)
	{
		shader->setUniform("heightBoundaries", vec2(.0f, 1.0f));
		
		this->setShaderUniforms(shader, shaderType, matrix);

		if (material)
		{
			material->applyMaterial(shader);
		}

		vao->drawObject(RendEnum::IBO_TRIANGLE_MESH, primitive, GLuint(modelComp->_topologyIndicesLength[RendEnum::IBO_TRIANGLE_MESH]));
	}
}
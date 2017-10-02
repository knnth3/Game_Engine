#include <glm\gtx\transform.hpp>
#include "Model.h"

using namespace Lime::Model;
using namespace std;

c_uint Lime::Model::Polygon::GetVertexCount()
{
	return m_vertexCount;
}

Lime::Model::Model3D::Model3D()
{
	m_outRotation = glm::mat4();
	m_inRotation = glm::mat4();
	m_scaleMatrix = glm::mat4();
	m_translation = glm::mat4();
	m_localToWorld = glm::mat4();
	m_position = glm::vec3();
	m_offset = glm::vec3();
	m_color = glm::vec4();
	m_texture = 0;
	m_mesh = nullptr;
	m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Lime::Model::Model3D::Model3D(const MeshID id) :
	Model3D()
{
	AddMesh(id);
}

void Lime::Model::Model3D::Scale(const float x, const float y, const float z)
{
	m_scale = glm::vec3(x, y, z);
	m_offset.x *= m_scale.x;
	m_offset.y *= m_scale.y;
	m_offset.z *= m_scale.z;
	m_scaleMatrix = glm::scale(m_scale);
}

void Lime::Model::Model3D::SetPosition(const float x, const float y, const float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Lime::Model::Model3D::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

void Lime::Model::Model3D::Rotate(float x, float y, float z)
{
	RotateMatrix(m_outRotation, glm::vec3(x,y,z));
}

void Lime::Model::Model3D::RotateAtOrigin(float x, float y, float z)
{
	RotateMatrix(m_inRotation, glm::vec3(x,y,z));
}

void Lime::Model::Model3D::SetColor(float r, float g, float b)
{
	m_color = glm::vec4(r, g, b, 1.0f);
}

void Lime::Model::Model3D::SetColor(float r, float g, float b, float a)
{
	m_color = glm::vec4(r, g, b, a);
}

void Lime::Model::Model3D::SetColor(glm::vec4 color)
{
	m_color = color;
}

void Lime::Model::Model3D::SetOpacity(float alpha)
{
	m_color.a = alpha;
}

void Lime::Model::Model3D::SetOffset(float offset)
{
	m_offset = glm::vec3(offset * m_scale.x, 0.0f, 0.0f);
}

void Lime::Model::Model3D::SetTexture(Texture tex)
{
	m_texture = tex;
}

void Lime::Model::Model3D::AddMesh(MeshID id)
{
	MeshLoader::GrabMeshData(id, m_mesh);
}

glm::vec3 Lime::Model::Model3D::GetPosition() const
{
	return m_position - m_offset;
}

glm::mat4 Lime::Model::Model3D::GetModelMatrix()
{
	CreateLocalToWorld();
	return m_localToWorld;
}

glm::vec4 Lime::Model::Model3D::GetColor()
{
	return m_color;
}

Texture Lime::Model::Model3D::GetTexture()
{
	return m_texture;
}

void Lime::Model::Model3D::operator=(const MeshID id)
{
	AddMesh(id);
}

void Lime::Model::Model3D::CreateLocalToWorld()
{
	glm::vec3 pos = m_position - m_offset;
	m_translation = glm::translate(glm::mat4(), glm::vec3(pos.x, pos.y, pos.z));
	m_localToWorld = m_inRotation * m_translation * m_outRotation * m_scaleMatrix;
}

void Lime::Model::Model3D::RotateMatrix(glm::mat4 & matrix, glm::vec3 rot)
{
	matrix = glm::mat4();
	if (rot.x != 0.0f)
	{
		glm::vec3 rotaxis(1.0f, 0.0f, 0.0f);
		matrix = glm::rotate(matrix, rot.x, rotaxis);
	}
	if (rot.y != 0.0f)
	{
		glm::vec3 rotaxis(0.0f, 1.0f, 0.0f);
		matrix = glm::rotate(matrix, rot.y, rotaxis);
	}
	if (rot.z != 0.0f)
	{
		glm::vec3 rotaxis(0.0f, 0.0f, 1.0f);
		matrix = glm::rotate(matrix, rot.z, rotaxis);
	}
}

MeshID Lime::Model::MeshLoader::LoadModel(std::string filename)
{
	return MeshID();
}

void Lime::Model::MeshLoader::GrabMeshData(MeshID id, MeshData_ptr & ptr)
{
	try
	{
		ptr = m_modelLibrary.at(id);
	}
	catch (exception e)
	{
		ptr = nullptr;
	}
}

c_uint Lime::Model::MeshLoader::AssignID()
{
	static uint32_t ID = 0;
	return ID++;
}

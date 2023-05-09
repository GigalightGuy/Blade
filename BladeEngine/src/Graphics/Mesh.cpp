#include "Mesh.hpp"

#include "Vertex.hpp"
#include "GraphicsManager.hpp"

namespace BladeEngine::Graphics {

	Mesh* Mesh::s_Quad = nullptr;

	Mesh::Mesh() { }

	Mesh::~Mesh()
	{
		m_Indices.Release();
		m_Vertices.Release();
		m_UVs.Release();
	}

	void Mesh::SetIndices(uint16_t* indices, size_t count)
	{
		m_Indices = Buffer::Copy((uint8_t*)indices, count * sizeof(uint16_t));
	}

	void Mesh::SetVertices(glm::vec3* vertices, size_t count)
	{
		m_Vertices = Buffer::Copy((uint8_t*)vertices, count * sizeof(glm::vec3));
	}

	void Mesh::SetUVs(glm::vec2* uvs, size_t count)
	{
		m_UVs = Buffer::Copy((uint8_t*)uvs, count * sizeof(glm::vec2));
	}

	void Mesh::Create()
	{
		Buffer vertexBuffer;

		glm::vec3* vertices = m_Vertices.As<glm::vec3>();
		glm::vec2* uvs = m_UVs.As<glm::vec2>();

		size_t vertexCount = m_Vertices.Size / sizeof(glm::vec3);

		vertexBuffer.Allocate(vertexCount * sizeof(VertexColorTexture));

		VertexColorTexture* vertexBufferData = vertexBuffer.As<VertexColorTexture>();

		for (int i = 0; i < vertexCount; i++)
		{
			vertexBufferData[i].color = glm::vec4(1);
			vertexBufferData[i].position = vertices[i];
			vertexBufferData[i].textureCoordinate = uvs[i];
		}


		Buffer indexBuffer;

		uint16_t* indices = m_Indices.As<uint16_t>();

		size_t indexCount = m_Indices.Size / sizeof(uint16_t);

		indexBuffer.Allocate(indexCount * sizeof(uint16_t));

		uint16_t* indexBufferData = indexBuffer.As<uint16_t>();

		for (int i = 0; i < indexCount; i++)
		{
			indexBufferData[i] = indices[i];
		}

		m_GPUMesh = GraphicsManager::Instance()->UploadMeshToGPU(vertexBuffer, indexBuffer);
	}

	void Mesh::Release()
	{
		GraphicsManager::Instance()->ReleaseGPUMesh(m_GPUMesh);
		m_GPUMesh = nullptr;
	}

	Mesh* Mesh::CreateQuad(float width, float height)
	{
		Mesh* mesh = new Mesh();

		glm::vec3* vertices = new glm::vec3[4] {
			{-width / 2.0f,   height / 2.0f, 0.0f},
			{-width / 2.0f,  -height / 2.0f, 0.0f},
			{ width / 2.0f,  -height / 2.0f, 0.0f},
			{ width / 2.0f,   height / 2.0f, 0.0f},
		};

		uint16_t* indices = new uint16_t[6] {
			0, 1, 2, 
			2, 3, 0
		};

		glm::vec2* uvs = new glm::vec2[4]{
			{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}
		};

		mesh->SetVertices(vertices, 4);
		mesh->SetIndices(indices, 6);
		mesh->SetUVs(uvs, 4);

		return mesh;
	}

	void Mesh::LoadDefaultMeshes()
	{
		s_Quad = CreateQuad(1.0f, 1.0f);

		s_Quad->Create();
	}

	void Mesh::UnloadDefaultMeshes()
	{
		s_Quad->Release();

		delete s_Quad;
		s_Quad = nullptr;
	}

}



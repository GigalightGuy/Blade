#pragma once

#include "../Core/Buffer.hpp"

#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <vector>

namespace BladeEngine::Graphics {
	
	class Mesh 
	{
	public:
		Mesh();
		~Mesh();

		void SetIndices(uint16_t* indices, size_t count);
		void SetVertices(glm::vec3* vertices, size_t count);
		void SetUVs(glm::vec2* uvs, size_t count);

		void Create();
		void Release();

		void* GetGPUMesh() const { return m_GPUMesh; }

	public:
		static const Mesh* Quad() { return s_Quad; }

		static Mesh* CreateQuad(float width, float height);

		static void LoadDefaultMeshes();
		static void UnloadDefaultMeshes();

	private:
		Buffer m_Indices;
		Buffer m_Vertices;
		Buffer m_UVs;

		void* m_GPUMesh = nullptr;

	private:
		static Mesh* s_Quad;
	};

} // namespace BladeEngine::Graphics

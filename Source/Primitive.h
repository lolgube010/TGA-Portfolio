#pragma once
#include "Mesh.h"
#include "uppgift05_helper.h"
//#include <type_traits>

// TODO- REMOVE TEMPLATE! MAYBE.
template <int VertexCount, int IndexCount>
struct PrimitiveData
{
	static constexpr int VERTEX_COUNT = VertexCount;
	static constexpr int INDEX_COUNT = IndexCount;
	std::array<Mesh::Vertex, VERTEX_COUNT> myVertices{};
	std::array<unsigned int, INDEX_COUNT> myIndices{};
};

// Define a type trait to check if a type has certain members
template<typename T>
struct has_required_members
{
private:
	typedef char yes[1];
	typedef char no[2];

	template<typename C>
	static yes& test(decltype(&C::myVertices), decltype(&C::myIndices));

	template<typename C>
	static no& test(...);

public:
	static constexpr bool value = sizeof(test<T>(nullptr, nullptr)) == sizeof(yes);
};


template <typename PrimitiveType>
class Primitive : public Mesh
{
	static_assert(has_required_members<PrimitiveType>::value, "PrimitiveType");
public:
	Primitive() : myPrimitive() {}

	bool Init(ID3D11Device* aDevice, const ComPtr<ID3D11PixelShader>& aPixelShader, const ComPtr<ID3D11VertexShader>& aVertexShader, const ComPtr<ID3D11InputLayout>& aInputLayout, bool aSaveTriangles = false);
	void Render(ID3D11DeviceContext* aDeviceContext) override;
	void RenderInstanced(ID3D11DeviceContext* aDeviceContext, UINT aInstanceCount) override;
	PrimitiveType GetPrimitive();
private:
	PrimitiveType myPrimitive;
};

template <typename PrimitiveType>
bool Primitive<PrimitiveType>::Init(ID3D11Device* aDevice, const ComPtr<ID3D11PixelShader>& aPixelShader, const ComPtr<ID3D11VertexShader>& aVertexShader, const ComPtr<ID3D11InputLayout>& aInputLayout, const bool aSaveTriangles)
{
	return Mesh::Init(
		aDevice,
		myPrimitive.myVertices.data(),
		PrimitiveType::VERTEX_COUNT,
		myPrimitive.myIndices.data(),
		PrimitiveType::INDEX_COUNT,
		aPixelShader,
		aVertexShader, 
		aInputLayout, 
		aSaveTriangles
	);
}

template <typename PrimitiveType>
void Primitive<PrimitiveType>::Render(ID3D11DeviceContext* aDeviceContext)
{
	Mesh::Render(aDeviceContext);
}

template <typename PrimitiveType>
void Primitive<PrimitiveType>::RenderInstanced(ID3D11DeviceContext* aDeviceContext, const UINT aInstanceCount)
{
	Mesh::RenderInstanced(aDeviceContext, aInstanceCount);
}

template <typename PrimitiveType>
PrimitiveType Primitive<PrimitiveType>::GetPrimitive()
{
	return myPrimitive;
}

struct Tetrahedron : PrimitiveData<4, 12> {
	Tetrahedron() {
		myVertices = { {
				// idk how i came to this but the 2nd one is easier
					/*{1.f, 0.f, -sqrtf(1.f / 3.f), 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f},
					{-1.f, 0.f, -sqrtf(1.f / 3.f), 1.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f},
					{0.f, 0.f, 2.f * sqrtf(1.f / 3.f), 1.f, 0.f, 0.f, 1.f, 1.f, 0.5f, 1.f},
					{0.f, sqrtf(2.f), 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.5f, .5f}*/

					{ .5f, -.5f, -.5f, 1.f, 1.f, 0.f, 0.f, 1.f, {1.f, 0.f} },
					{-.5f, -.5f, -.5f, 1.f, 0.f, 1.f, 0.f, 1.f, {0.f, 0.f}},
					{0.f, -.5f, .5f, 1.f, 0.f, 0.f, 1.f, 1.f, {0.5f, 1.f}},
					{0.f, .5f, -0.1667f, 1.f, 1.f, 1.f, 0.f, 1.f, {0.5f, .5f}}
				} };
		myIndices = { 1, 0, 2, 2, 0, 3, 1, 3, 0, 1, 2, 3 };
	}
};

// todo, seperate to simple and fancy ver? since this isn't optimized lol
struct Cube : PrimitiveData<24, 36>
{
	Cube() {
		myVertices = {
		{
			{0.5f, -0.5f, 0.5f, 1, 1, 1, 1, 1, {0, 1}},
			{0.5f,  0.5f, 0.5f, 1, 1, 1, 1, 1, {0, 0}},
			{-0.5f, 0.5f, 0.5f, 1, 1, 1, 1, 1, {1, 0}},
			{-0.5f,-0.5f, 0.5f, 1, 1, 1, 1, 1, {1, 1}},
			{-0.5f,-0.5f, 0.5f, 1, 1, 0, 0, 1, {0, 1}},
			{-0.5f, 0.5f, 0.5f, 1, 1, 0, 0, 1, {0, 0}},
			{-0.5f, 0.5f,-0.5f, 1, 1, 0, 0, 1, {1, 0}},
			{-0.5f,-0.5f,-0.5f, 1, 1, 0, 0, 1, {1, 1}},
			{-0.5f,-0.5f,-0.5f, 1, 0, 1, 0, 1, {0, 1}},
			{-0.5f, 0.5f,-0.5f, 1, 0, 1, 0, 1, {0, 0}},
			{0.5f,  0.5f,-0.5f, 1, 0, 1, 0, 1, {1, 0}},
			{0.5f, -0.5f,-0.5f, 1, 0, 1, 0, 1, {1, 1}},
			{0.5f, -0.5f,-0.5f, 1, 0, 0, 1, 1, {0, 1}},
			{0.5f,  0.5f,-0.5f, 1, 0, 0, 1, 1, {0, 0}},
			{0.5f,  0.5f, 0.5f, 1, 0, 0, 1, 1, {1, 0}},
			{0.5f, -0.5f, 0.5f, 1, 0, 0, 1, 1, {1, 1}},
			{0.5f,  0.5f, 0.5f, 1, 1, 1, 0, 1, {0, 1}},
			{0.5f,  0.5f,-0.5f, 1, 1, 1, 0, 1, {0, 0}},
			{-0.5f, 0.5f,-0.5f, 1, 1, 1, 0, 1, {1, 0}},
			{-0.5f, 0.5f, 0.5f, 1, 1, 1, 0, 1, {1, 1}},
			{-0.5f,-0.5f, 0.5f, 1, 1, 0, 1, 1, {0, 1}},
			{-0.5f,-0.5f,-0.5f, 1, 1, 0, 1, 1, {0, 0}},
			{0.5f, -0.5f,-0.5f, 1, 1, 0, 1, 1, {1, 0}},
			{0.5f, -0.5f, 0.5f, 1, 1, 0, 1, 1, {1, 1}},
		} };

		myIndices =
		{
			0, 1, 2,
			0, 2, 3,
			4, 5, 6,
			4, 6, 7,
			8, 9, 10,
			8, 10, 11,
			12, 13, 14,
			12, 14, 15,
			16, 17, 18,
			16, 18, 19,
			20, 21, 22,
			20, 22, 23
		};
	}
};

struct Cone : PrimitiveData<1, 1>
{
	Cone() {
		myVertices = {
		{
				//{1.f, 0.f, -sqrtf(1.f / 3.f), 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f}, // Vertex 0
			} };

		myIndices =
		{
			//1, 0, 2,
		};
	}
};

struct Torus : PrimitiveData<1, 1>
{
	Torus() {
		myVertices = {
		{
				//{1.f, 0.f, -sqrtf(1.f / 3.f), 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f}, // Vertex 0
			} };

		myIndices =
		{
			//1, 0, 2,
		};
	}
};

template<float sizeX = 1.f, float sizeZ = 1.f>
struct SimplePlane : PrimitiveData<4, 6>
{
	SimplePlane()
	{
		float halfSizeX = sizeX * .5f;
		float halfSizeZ = sizeZ * .5f;
		myVertices = {
			{
				{ halfSizeX, 0.0f,  halfSizeZ, 1, 1, 1, 1, 1, {1, 0} }, // Top right
				{ halfSizeX, 0.0f, -halfSizeZ, 1, 1, 1, 1, 1, {1, 1} }, // Bottom right
				{-halfSizeX, 0.0f, -halfSizeZ, 1, 1, 1, 1, 1, {0, 1} }, // Bottom left
				{-halfSizeX, 0.0f,  halfSizeZ, 1, 1, 1, 1, 1, {0, 0} }, // Top left
			}
		};

		myIndices = {
			0, 1, 2, // First triangle
			0, 2, 3  // Second triangle
		};
	}
};


struct Cylinder : PrimitiveData<1, 1>
{
	Cylinder() {
		myVertices = {
		{
				//{1.f, 0.f, -sqrtf(1.f / 3.f), 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f}, // Vertex 0
		} };

		myIndices =
		{
			//1, 0, 2,
		};
	}
};

struct Icosahedron : PrimitiveData<12, 60>
{
	Icosahedron()
	{
	}
};

struct Sphere : PrimitiveData<8, 48>
{
	Sphere()
	{
	}
};

struct Octahedron : PrimitiveData<6, 24>
{
	Octahedron() {
		myVertices = {
			{
				{0.f, 1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, {0.f, 0.f}}, // Top vertex
				{1.f, 0.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, {1.f, 0.f}}, // Right vertex
				{-1.f, 0.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, {0.f, 0.f}}, // Left vertex
				{0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, {0.5f, 0.f}}, // Front vertex
				{0.f, 0.f, -1.f, 1.f, 1.f, 1.f, 0.f, 1.f,{ 0.f, 0.f}}, // Back vertex
				{0.f, -1.f, 0.f, 1.f, 1.f, 1.f, 0.f, 1.f, {0.5f, 1.f}} // Bottom vertex
			}
		};

		myIndices =
		{
			0, 3, 1, // Top triangle
			0, 1, 4, // Right triangle
			0, 4, 2, // Left triangle
			0, 2, 3, // Front triangle
			3, 5, 1, // Back-right triangle
			1, 5, 4, // Back-left triangle
			4, 5, 2, // Bottom-right triangle
			2, 5, 3 // Bottom-left triangle
		};
	}
};

struct Pyramid : PrimitiveData<6, 18>
{
	Pyramid()
	{
		myVertices =
		{
		{
		{0.f, .5f, 0.f, 1, 1, 0, 0, 1, {.5f, 0.f}}, // top - 0
		{-.5f, -.5f, .5f, 1, 0, 1, 0, 1, {1, 1.f}}, // forward left - 1
		{.5f, -.5f, .5f, 1, 0, 1, 0, 1, {0, 1.f}}, // forward right - 2
		{-.5f, -.5f, -.5f, 1, 0, 0, 1, 1, {0, 1.f}}, // back left - 3
		{.5f, -.5f, -.5f, 1, 1, 1, 1, 1, {1, 1.f}}, // back right - 4
		} };

		myIndices =
		{
			0, 1, 2,
			1, 0, 3,
			3, 0, 4,
			4, 0, 2,
			1, 3, 4,
			2, 1, 4
		};
	}
};

//template<int factor> // legacy code kept if i ever need to make the vertex/indexCount calculation longer / less straightforward. see below code.
//struct ___TessellatedPlaneVertexAndIndexGeneratorDoNotUseThis
//{
//    static constexpr int VertexCount = (factor + 1) * (factor + 1);
//    static constexpr int IndexCount = factor * factor * 6;
//};

template<int factor>				//  vertexcount                 // indexCount
struct TessellatedPlane : PrimitiveData<(factor + 1)* (factor + 1), factor* factor * 6>
{
	TessellatedPlane()
	{
		int vertexIndex = 0;
		int indexIndex = 0;
		constexpr int numVerticesPerRow = factor + 1;

		constexpr float step = 1.0f / factor;

		// Generate vertices
		for (int y = 0; y <= factor; ++y) {
			for (int x = 0; x <= factor; ++x) {
				const float fx = x * step;
				const float fy = y * step;
				this->myVertices[vertexIndex++] = {
					fx, 0.0f, fy, 1.0f, // Position
					1.0f, 1.0f, 1.0f, 1.0f, // Color
					{fx, fy}, // Texture coordinates
					//0.0f, 1.0f, 0.0f // Normal
				};
			}
		}

		// Generate indices
		for (int y = 0; y < factor; ++y) {
			for (int x = 0; x < factor; ++x) {
				const int start = y * numVerticesPerRow + x;

				this->myIndices[indexIndex++] = start;
				this->myIndices[indexIndex++] = start + numVerticesPerRow;
				this->myIndices[indexIndex++] = start + 1;

				this->myIndices[indexIndex++] = start + 1;
				this->myIndices[indexIndex++] = start + numVerticesPerRow;
				this->myIndices[indexIndex++] = start + numVerticesPerRow + 1;
			}
		}
	}
};

namespace TerrainParams // todo- somehow figure out a way to do this in a cleaner way. this is hardcoded for now.
{
	constexpr int initialNoiseResolution = 8;
	constexpr unsigned int resolution = 512;

	constexpr float amplitude = 3.f;

	constexpr float sizeX = 25.f;
	constexpr float sizeZ = 25.f;

	// can't be members cause of templates.
	constexpr int vertexCount = resolution * resolution;
	constexpr int indexCount = (resolution - 1) * (resolution - 1) * 6;
}

struct Terrain : PrimitiveData<TerrainParams::vertexCount, TerrainParams::indexCount>
{
	Terrain()
	{
		int currentNoiseResolution = TerrainParams::initialNoiseResolution;

		float amplitude = TerrainParams::amplitude;

		// sets our vector size
		myHeightMap.resize(static_cast<std::vector<float>::size_type>(TerrainParams::initialNoiseResolution) * TerrainParams::initialNoiseResolution, .5f);

		// noise and upsample loop
		while (currentNoiseResolution < TerrainParams::resolution)
		{
			TerrainManip::AddNoise(myHeightMap, amplitude); // adds noise to this entier row of vectors
			myHeightMap = TerrainManip::Upsample2X(myHeightMap, currentNoiseResolution);

			currentNoiseResolution *= 2;
			amplitude *= .25f;
		}

		// loop to set all the vertice pos (xyz) and uv.
		for (size_t i = 0; i < TerrainParams::resolution; ++i)
		{
			for (size_t j = 0; j < TerrainParams::resolution; ++j)
			{
				const float x = (i / static_cast<float>(TerrainParams::resolution - 1) - .5f) * TerrainParams::sizeX;
				const float z = j / static_cast<float>(TerrainParams::resolution - 1) * TerrainParams::sizeZ;
				const float height = myHeightMap[i * currentNoiseResolution + j];

				myVertices[i * TerrainParams::resolution + j] =
				{
					x,
					height,
					z,
					1.f,
					1.f, 1.f, 1.f, 1.f,
					{x, z},
				};  // NOLINT(clang-diagnostic-missing-field-initializers)
			}
		}

		// compute vertice normals
		for (size_t i = 0; i < TerrainParams::resolution - 1; ++i)
		{
			for (size_t j = 0; j < TerrainParams::resolution - 1; ++j)
			{
				Mesh::Vertex& vertex00 = myVertices[(i + 0) * TerrainParams::resolution + j + 0];
				Mesh::Vertex& vertex01 = myVertices[(i + 1) * TerrainParams::resolution + j + 0];
				Mesh::Vertex& vertex10 = myVertices[(i + 0) * TerrainParams::resolution + j + 1];
				Mesh::Vertex& vertex11 = myVertices[(i + 1) * TerrainParams::resolution + j + 1];

				CU::Vector3f p01(vertex01.x, vertex01.y, vertex01.z);
				CU::Vector3f p10(vertex10.x, vertex10.y, vertex10.z);
				CU::Vector3f p11(vertex11.x, vertex11.y, vertex11.z);

				CU::Vector3f d0(p01 - p10);
				CU::Vector3f d1(p11 - p01);

				const CU::Vector3f n = d1.Cross(d0).GetNormalized();

				// Create an array of pointers to the vertices and Iterate over the vertices and update their normals
				for (Mesh::Vertex* vertices[] = { &vertex00, &vertex01, &vertex10, &vertex11 }; Mesh::Vertex * vertex : vertices)
				{
					vertex->normal += n;
				}
			}
		}
		// normalizes the accumulated normals. It is important to separate these steps because each vertex normal must be averaged over all contributing faces before normalization. 
		for (int i = 0; i < TerrainParams::resolution; i++)
		{
			for (int j = 0; j < TerrainParams::resolution; j++)
			{
				Mesh::Vertex& vertex = myVertices[i * TerrainParams::resolution + j]; // same as structued binding below. just do vertex.stuff to revert
				vertex.normal.Normalize();

				//vertex.tangent = vertex.normal.Cross(CU::Vector3f(0.f, 0.f, 1.f)).GetNormalized();
				//vertex.binormal = vertex.normal.Cross(CU::Vector3f(1.f, 0.f, 0.f)).GetNormalized(); // ???

				//vertex.binormal = vertex.normal.Cross(vertex.tangent).GetNormalized(); // alt
			}
		}

		//// Compute tangents and bitangents
		for (size_t i = 0; i < TerrainParams::resolution - 1; ++i)
		{
			for (size_t j = 0; j < TerrainParams::resolution - 1; ++j)
			{
				// Access the three vertices of the current triangle
				Mesh::Vertex& vertex00 = myVertices[i * TerrainParams::resolution + j];
				Mesh::Vertex& vertex01 = myVertices[(i + 1) * TerrainParams::resolution + j];
				Mesh::Vertex& vertex10 = myVertices[i * TerrainParams::resolution + j + 1];

				// Positions of the vertices
				CU::Vector3f pos1(vertex00.x, vertex00.y, vertex00.z);
				CU::Vector3f pos2(vertex01.x, vertex01.y, vertex01.z);
				CU::Vector3f pos3(vertex10.x, vertex10.y, vertex10.z);

				// Edges of the triangle
				CU::Vector3f edge1 = pos2 - pos1;
				CU::Vector3f edge2 = pos3 - pos1;

				// UV deltas
				CU::Vector2f deltaUV1 = vertex01.uv - vertex00.uv;
				CU::Vector2f deltaUV2 = vertex10.uv - vertex00.uv;

				// Calculate the reciprocal of the determinant
				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

				// Calculate tangent using the cross product
				CU::Vector3f tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * f;
				tangent.Normalize();

				// Calculate bitangent using the cross product
				CU::Vector3f biTangent = (edge2 * deltaUV1.x - edge1 * deltaUV2.x) * f;
				biTangent.Normalize();

				// Assign tangent and bitangent to vertices
				for (Mesh::Vertex* vertex : { &vertex00, &vertex01, &vertex10 })
				{
					vertex->tangent = tangent;
					vertex->binormal = biTangent;
				}
			}
		}

		// set indices
		for (unsigned int i = 0; i < TerrainParams::resolution - 1; i++)
		{
			for (unsigned int j = 0; j < TerrainParams::resolution - 1; j++)
			{
				const int startHalf1 = (i + 0) * TerrainParams::resolution + j;
				const int index00 = startHalf1 + 0;
				const int index01 = startHalf1 + 1;

				const int startHalf2 = (i + 1) * TerrainParams::resolution + j;
				const int index10 = startHalf2 + 0;
				const int index11 = startHalf2 + 1;

				const size_t indexStart = 6 * (i * (TerrainParams::resolution - 1) + j);
				myIndices[indexStart + 0] = index00;
				myIndices[indexStart + 1] = index01;
				myIndices[indexStart + 2] = index10;

				myIndices[indexStart + 3] = index01;
				myIndices[indexStart + 4] = index11;
				myIndices[indexStart + 5] = index10;
			}
		}
	}
private:
	std::vector<float> myHeightMap;
};
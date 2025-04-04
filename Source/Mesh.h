#pragma once
#include <d3d11.h>
#include <DirectXCollision.h>
#include <vector>
#include <CommonUtilities/Matrix4x4.h>
#include "wrl/client.h"
#include "CommonUtilities/Vector3.h"
#include "CommonUtilities/Vector2.h"
#include "string"
#include "CommonUtilities/Matrix3x3.h"

// TODO
// re-write this mesh class to be "meshInstance" and not the data holder
// make another class that holds each specific mesh data we load in, and then have this class manage each instance somehow.
// so the meshManager or something deals with storing the non-instance specific data
// and all this class does is just take in the instance data.
// each mesh holder also manages the vertex buffer for that model (each instance shares buffer!!)

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	struct Vertex
	{
		float x = 0, y = 0, z = 0, w = 0; // W is for scaling! kinda ish. lol see : https://stackoverflow.com/questions/2422750/in-opengl-vertex-shaders-what-is-w-and-why-do-i-divide-by-it
		float r = 0, g = 0, b = 0, a = 0;
		CU::Vector2f uv{};
		CU::Vector3f normal{};
		CU::Vector3f tangent{};
		CU::Vector3f binormal{}; // or biTangent
	};
	bool Init(ID3D11Device* aDevice, const Vertex* aVertices, int aVertexCount, const unsigned int* aIndices, int aIndexCount, const ComPtr<
	          ID3D11PixelShader>& aPixelShader, const ComPtr<ID3D11VertexShader>& aVertexShader, const ComPtr<ID3D11InputLayout>& aInputLayout, bool aSaveTriangles);
	virtual void Render(ID3D11DeviceContext* aDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext* aDeviceContext, UINT aInstanceCount);

	[[nodiscard]] virtual CU::Matrix4x4f GetTransform() const;
	[[nodiscard]] virtual const CU::Matrix4x4f* GetTransformPtr() const;

	virtual void SetTransform(const CU::Matrix4x4f& aTransform);
	virtual void SetTransform3X3(const CU::Matrix4x4f& aTransform);
	virtual void ApplyRotation(const CU::Matrix4x4f& aTransform);
	virtual void SetTransform3X3(const CU::Matrix3x3f& aTransform);

	virtual CU::Vector3f GetPosition();
	virtual DirectX::XMFLOAT3 GetPositionXMFLOAT3();
	virtual void SetPosition(CU::Vector3f aPos);
	virtual void SetPositionXMFLOAT3(const DirectX::XMFLOAT3& aPos);
	virtual void SetPosition(float aX, float aY, float aZ);

	virtual void SetRotationX(CU::Vector3f aRotX);
	virtual void SetRotationY(CU::Vector3f aRotY);
	virtual void SetRotationZ(CU::Vector3f aRotZ);

	virtual void SetScale(CU::Vector3f aScale);
private:
	void InitBoundingBox(const Vertex* aVertices);
	void InitTriangles(const Vertex* aVertices, const unsigned int* aIndices);
protected:
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	CU::Matrix4x4f myTransform;

	ComPtr<ID3D11InputLayout> myInputLayout; // TODO- this is kind of related to both shaders and our vertex loader. hm.
	ComPtr<ID3D11VertexShader> myVertexShader; // TODO- MOVE TO SHADER.H
	ComPtr<ID3D11PixelShader> myPixelShader; // TODO- MOVE TO SHADER.H
public:
	struct MeshTri // pack of three vertices used for mesh voxelization
	{
		std::array<Vertex, 3> vertices;
	};
	DirectX::BoundingBox myBoundingBox;

	std::vector<MeshTri> GetMeshTriangles(bool aTransformed);
private:
	// also, each element = one unique triangle, since they're based off of the indices. we might get duplicate tris that way but honestly for this idc rn. can be made more effective l8r.
	std::vector<MeshTri> myMeshTriangles; // private since we want to transform the vertices when getting them.
	int myIndexCount = 0;
	int myVertexCount = 0;
};
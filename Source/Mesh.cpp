#include "Mesh.h"
#include <d3d11.h>
#include <fstream>
#include <vector>

#include "MomoMath.h"
#include "CommonUtilities/StringManip.h"

Mesh::Mesh() = default;
Mesh::~Mesh() = default;

bool Mesh::Init(ID3D11Device* aDevice, const Vertex* aVertices, const int aVertexCount, const unsigned int* aIndices, const int aIndexCount, const ComPtr<ID3D11PixelShader>& aPixelShader, const ComPtr<ID3D11VertexShader>& aVertexShader, const ComPtr<ID3D11InputLayout>& aInputLayout, const bool aSaveTriangles)
{
	myVertexCount = aVertexCount;
	myIndexCount = aIndexCount;

	InitBoundingBox(aVertices);

	if (aSaveTriangles)
	{
		InitTriangles(aVertices, aIndices);
	}

	HRESULT result;
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * myVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData = { nullptr };
		vertexData.pSysMem = aVertices;

		result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			MessageBox(nullptr, L"shit's fucked", L"vertex buffer failed to init!", MB_ICONERROR | MB_HELP);
			return false;
		}
	}
	{
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * myIndexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData = { nullptr };
		indexData.pSysMem = aIndices;

		result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
		{
			MessageBox(nullptr, L"shit's fucked", L"index buffer failed to init!", MB_ICONERROR | MB_HELP);
			return false;
		}
	}

	myVertexShader = aVertexShader;
	myPixelShader = aPixelShader;
	myInputLayout = aInputLayout;

	return true;
}

void Mesh::Render(ID3D11DeviceContext* aDeviceContext)
{
	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	aDeviceContext->IASetInputLayout(myInputLayout.Get());

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	aDeviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);

	// index/indices
	aDeviceContext->DrawIndexed(myIndexCount, 0, 0);
}

void Mesh::RenderInstanced(ID3D11DeviceContext* aDeviceContext, const UINT aInstanceCount)
{
	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	aDeviceContext->IASetInputLayout(myInputLayout.Get());

	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;
	aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	aDeviceContext->VSSetShader(myVertexShader.Get(), nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader.Get(), nullptr, 0);

	// index/indices
	//aDeviceContext->DrawIndexed(myIndexCount, 0, 0);
	aDeviceContext->DrawIndexedInstanced(myIndexCount, aInstanceCount, 0, 0, 0);
}

CU::Matrix4x4f Mesh::GetTransform() const
{
	return myTransform;
}

const CU::Matrix4x4f* Mesh::GetTransformPtr() const
{
	return &myTransform;
}

void Mesh::SetTransform(const CU::Matrix4x4f& aTransform)
{
	myTransform = aTransform;
}

void Mesh::SetTransform3X3(const CU::Matrix4x4f& aTransform)
{
	myTransform.Set3X3(aTransform);
}

void Mesh::ApplyRotation(const CU::Matrix4x4f& aTransform)
{
	SetTransform3X3(aTransform);
}

void Mesh::SetTransform3X3(const CU::Matrix3x3f& aTransform)
{
	myTransform.Set3X3(aTransform);
}

CU::Vector3f Mesh::GetPosition()
{
	return myTransform.GetPosition();
}

DirectX::XMFLOAT3 Mesh::GetPositionXMFLOAT3()
{
	auto old = myTransform.GetPosition();
	return { old.x, old.y, old.z };
}

void Mesh::SetPosition(const CU::Vector3f aPos)
{
	myTransform.SetPosition(aPos);
	myBoundingBox.Center = { aPos.x, aPos.y, aPos.z };
}

void Mesh::SetPositionXMFLOAT3(const DirectX::XMFLOAT3& aPos)
{
	myTransform.SetPosition({ aPos.x, aPos.y, aPos.z });
	myBoundingBox.Center = aPos;
}

void Mesh::SetPosition(float aX, float aY, float aZ)
{
	myTransform.SetPosition(CU::Vector3f{ aX, aY, aZ });
	myBoundingBox.Center = { aX, aY, aZ };
}

void Mesh::SetRotationX(const CU::Vector3f aRotX)
{
	myTransform.SetRotationX(aRotX);
}

void Mesh::SetRotationY(const CU::Vector3f aRotY)
{
	myTransform.SetRotationY(aRotY);
}

void Mesh::SetRotationZ(const CU::Vector3f aRotZ)
{
	myTransform.SetRotationZ(aRotZ);
}

void Mesh::SetScale(const CU::Vector3f aScale) // TODO- MAKE THIS APPLY TO UV'S AS WELL.
{
	myTransform = CommonUtilities::Matrix4x4f::CreateScaleMatrix(aScale) * myTransform;
	auto pos = myTransform.GetPosition();
	myBoundingBox.Center = { pos.x, pos.y, pos.z };
	myBoundingBox.Extents = MomoMath::MultiplyXMFLOAT3(myBoundingBox.Extents, aScale);
}

void Mesh::InitBoundingBox(const Vertex* aVertices)
{
	if (!aVertices || myVertexCount == 0)
		return;

	// Prepare an array of XMFLOAT3 points
	std::vector<DirectX::XMFLOAT3> points(myVertexCount);

	for (size_t i = 0; i < myVertexCount; ++i)
	{
		points[i] = DirectX::XMFLOAT3(aVertices[i].x, aVertices[i].y, aVertices[i].z);
	}
	DirectX::BoundingBox::CreateFromPoints(myBoundingBox, myVertexCount, points.data(), sizeof(DirectX::XMFLOAT3));

	//auto newBBPosition = myTransform.GetPosition();
	//myBoundingBox.Center = { newBBPosition.x, newBBPosition.y, newBBPosition.z };
}

void Mesh::InitTriangles(const Vertex* aVertices, const unsigned int* aIndices)
{
	//std::vector<unsigned int> indiceDebugVec;
	// TODO- SLIM DOWN VERTEX STRUCT SO THAT WE DON'T SEND AS MUCH DATA!
	for (int i = 0; i < myIndexCount; i += 3)
	{
		MeshTri triangle;
		triangle.vertices[0] = aVertices[aIndices[i]];     // First vertex of the triangle
		triangle.vertices[1] = aVertices[aIndices[i + 1]]; // Second vertex of the triangle
		triangle.vertices[2] = aVertices[aIndices[i + 2]]; // Third vertex of the triangle
		//indiceDebugVec.push_back(aIndices[i]);
		//indiceDebugVec.push_back(aIndices[i+ 1]);
		//indiceDebugVec.push_back(aIndices[i + 2]);
		myMeshTriangles.push_back(triangle);
	}
}

std::vector<Mesh::MeshTri> Mesh::GetMeshTriangles(const bool aTransformed)
{
	if (!aTransformed)
	{
		return myMeshTriangles;
	}

	std::vector<MeshTri> returnVec;
	for (const auto& tri : myMeshTriangles)
	{
		MeshTri meshTri;
		for (size_t i = 0; i < tri.vertices.size(); ++i)
		{
			const auto& vertex = tri.vertices.at(i);
			CU::Vector4f vertexObjectPos = { vertex.x, vertex.y, vertex.z, vertex.w };
			//CU::Vector4f vertexObjectNormal = { vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.f };

			float meshIntersectionBias = .99f;
			vertexObjectPos.x *= meshIntersectionBias;
			vertexObjectPos.y *= meshIntersectionBias;
			vertexObjectPos.z *= meshIntersectionBias;

			CU::Vector4f vertexWorldPos = vertexObjectPos * myTransform;
			//CU::Vector4f vertexWorldPos = myTransform * vertexObjectPos;
			//CU::Vector4f worldNormal = vertexObjectNormal * myTransform;

			Vertex vert;
			vert.x = vertexWorldPos.x;
			vert.y = vertexWorldPos.y;
			vert.z = vertexWorldPos.z;
			vert.w = vertexWorldPos.w;

			//vert.normal = { worldNormal.x, worldNormal.y, worldNormal.z }; // TODO- should be able to skip this with good results.

			meshTri.vertices.at(i) = vert;
		}
		returnVec.push_back(meshTri);
	}
	return returnVec;
}

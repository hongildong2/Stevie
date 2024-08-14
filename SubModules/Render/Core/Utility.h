#pragma once
#include "pch.h"
#include "DeviceResources.h"

#include "SubModules\Render\Mesh\MeshCommon.h"

namespace Utility
{


	namespace DXResource
	{
		void CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut);
		void CreateStructuredBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, UINT subResourceIndex, ID3D11ShaderResourceView** ppSRVOut);
		void UpdateBuffer(ID3D11DeviceContext1* pContext, ID3D11Buffer* pBuffer, UINT uElementSize, UINT uCount, void* pData);

		template<typename T>
		void CreateConstantBuffer(T& constant, Microsoft::WRL::ComPtr<ID3D11Device1> device, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.ByteWidth = sizeof(T);
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = &constant;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			DX::ThrowIfFailed(device->CreateBuffer(&desc, &InitData, buffer.GetAddressOf()));
		}

		template<typename T>
		void UpdateConstantBuffer(T& constant, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

			DX::ThrowIfFailed(context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

			memcpy(mappedResource.pData, &constant, sizeof(T));

			context->Unmap(buffer.Get(), 0);
		}
	}

	void ComputeShaderBarrier(ID3D11DeviceContext1* context);

}


namespace GeometryGenerator
{
	/*
	* reference : honglab introduction to graphics course
	*/
	MeshData MakeSphere(const float radius, const unsigned int numSlices, const unsigned int numStacks);
	MeshData MakeBox(const float scale);
	MeshData MakeSquare(const float scale);
	void MakeCWQuadPatches(unsigned int splitLevel, MeshData* const outMesh);
}

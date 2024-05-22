#pragma once
#include "pch.h"
#include "Model.h"

// Β«Ελ
namespace Utility
{
	namespace DXResource
	{
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

			HRESULT hr = device->CreateBuffer(&desc, &InitData, buffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template<typename T>
		void UpdateConstantBuffer(T& constant, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

			context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			memcpy(mappedResource.pData, &constant, sizeof(T));

			context->Unmap(buffer.Get(), 0);
		}



	}
}
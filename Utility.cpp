#include "pch.h"
#include "Utility.h"

namespace Utility
{
	namespace DXResource
	{
		void CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut)
		{
			*ppBufOut = nullptr; // 이건 왜하는거지?? 메모리 안정성을 위해서?

			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			desc.ByteWidth = uElementSize * uCount;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.StructureByteStride = uElementSize;

			if (pInitData)
			{
				D3D11_SUBRESOURCE_DATA InitData;
				InitData.pSysMem = pInitData;
				DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, &InitData, ppBufOut));
			}
			else
			{
				DX::ThrowIfFailed(pDevice->CreateBuffer(&desc, nullptr, ppBufOut));
			}
		}

		void CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
		{
			D3D11_BUFFER_DESC descBuf = {};
			pBuffer->GetDesc(&descBuf);

			D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			desc.BufferEx.FirstElement = 0;

			if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
			{
				// This is a Raw Buffer

				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
				desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
			}
			else
				if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
				{
					// This is a Structured Buffer
					desc.Format = DXGI_FORMAT_UNKNOWN;
					desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
				}
				else
				{
					DX::ThrowIfFailed(E_INVALIDARG);
				}

			DX::ThrowIfFailed(pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut));
		}

		void UpdateBuffer(ID3D11DeviceContext1* pContext, ID3D11Buffer* pBuffer, UINT uElementSize, UINT uCount, const void* pData)
		{
			assert(pContext != nullptr && pBuffer != nullptr);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

			DX::ThrowIfFailed(pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

			assert(uElementSize == mappedResource.RowPitch);

			for (UINT i = 0; i < uCount; ++i)
			{
				const void* pDataToWrite = static_cast<const char*>(pData) + (i * uElementSize);
				void* pMapped = static_cast<char*>(mappedResource.pData) + (i * mappedResource.RowPitch);

				std::memcpy(pMapped, pDataToWrite, uElementSize);
			}

			pContext->Unmap(pBuffer, 0);
		}
	}

	void ComputeShaderBarrier(ID3D11DeviceContext1* context)
	{
		ID3D11ShaderResourceView* nullSRV[6] = { 0, };
		context->CSSetShaderResources(0, 6, nullSRV);
		ID3D11UnorderedAccessView* nullUAV[6] = { 0, };
		context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
	}


}
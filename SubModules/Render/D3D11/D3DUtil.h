#pragma once
#include "pch.h"
#include "../RenderDefs.h"

interface IDeviceNotify
{
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceRestored() = 0;

protected:
	~IDeviceNotify() = default;
};


// Helper utility converts D3D API failures into exceptions.
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		MY_ASSERT(FALSE);
	}
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, const D3D_SHADER_MACRO* defines, _Outptr_ ID3DBlob** blob);

inline D3D11_PRIMITIVE_TOPOLOGY GetD3D11TopologyType(EPrimitiveTopologyType type)
{

	switch (type)
	{
	case EPrimitiveTopologyType::TRIANGLE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case EPrimitiveTopologyType::QUAD_PATCH:
		return D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	case EPrimitiveTopologyType::POINT:
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	case EPrimitiveTopologyType::LINE:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	default:
		return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

#pragma once
#include "pch.h"
#include "../RenderDefs.h"

namespace DX
{

	interface IDeviceNotify
	{
		virtual void OnDeviceLost() = 0;
		virtual void OnDeviceRestored() = 0;

	protected:
		~IDeviceNotify() = default;
	};

	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) noexcept : result(hr) {}

		const char* what() const noexcept override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}

	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, const D3D_SHADER_MACRO* defines, _Outptr_ ID3DBlob** blob);

	namespace D3D11
	{
		inline D3D11_PRIMITIVE_TOPOLOGY GetD3D11TopologyType(EPrimitiveTopologyType type)
		{

			switch (type)
			{
			case EPrimitiveTopologyType::TRIANGLE:
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


	}

}
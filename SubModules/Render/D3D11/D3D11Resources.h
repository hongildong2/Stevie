#pragma once
#include "../RShader.h"
#include "../RSamplerState.h"
#include "../RDepthStencilState.h"
#include "../RRasterizerState.h"

template<typename T>
class D3D11Resource
{
public:
	inline T* Get() const
	{
		return m_resource.Get();
	}

	inline T* const* GetAddressOf() const
	{
		return m_resource.GetAddressOf();
	}

	inline T** ReleaseAndGetAddressOf()
	{
		return m_resource.ReleaseAndGetAddressOf();
	}

protected:
	Microsoft::WRL::ComPtr<T> m_resource;
};


class D3D11VertexShader final : public RVertexShader, public D3D11Resource<ID3D11VertexShader>
{
public:
	D3D11VertexShader(const WCHAR* name) : RVertexShader(name) {};
	~D3D11VertexShader() = default;
};


class D3D11PixelShader final : public RPixelShader, public D3D11Resource<ID3D11PixelShader>
{
public:
	D3D11PixelShader(const WCHAR* name) : RPixelShader(name) {};
	~D3D11PixelShader() = default;
};


class D3D11ComputeShader final : public RComputeShader, public D3D11Resource<ID3D11ComputeShader>
{
public:
	D3D11ComputeShader(const WCHAR* name) : RComputeShader(name) {};
	~D3D11ComputeShader() = default;
};

class D3D11DomainShader final : public RDomainShader, public D3D11Resource<ID3D11DomainShader>
{
public:
	D3D11DomainShader(const WCHAR* name) : RDomainShader(name) {};
	~D3D11DomainShader() = default;
};

class D3D11HullShader final : public RHullShader, public D3D11Resource<ID3D11HullShader>
{
public:
	D3D11HullShader(const WCHAR* name) : RHullShader(name) {};
	~D3D11HullShader() = default;
};

class D3D11GeometryShader final : public RGeometryShader, public D3D11Resource<ID3D11GeometryShader>
{
public:
	D3D11GeometryShader(const WCHAR* name) : RGeometryShader(name) {};
	~D3D11GeometryShader() = default;
};


class D3D11SamplerState final : public RSamplerState, public D3D11Resource<ID3D11SamplerState>
{
public:
	D3D11SamplerState() = default;
	~D3D11SamplerState() = default;
};

class D3D11BlendState : public D3D11Resource<ID3D11BlendState>
{
public:
	D3D11BlendState() = default;
	~D3D11BlendState() = default;
};

class D3D11RasterizerState final : public RRasterizerState, public D3D11Resource<ID3D11RasterizerState>
{
public:
	D3D11RasterizerState() = default;
	~D3D11RasterizerState() = default;
};

class D3D11InputLayout : public D3D11Resource<ID3D11InputLayout>
{
public:
	D3D11InputLayout() = default;
	~D3D11InputLayout() = default;
};
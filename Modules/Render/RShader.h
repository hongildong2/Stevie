#pragma once
#include "pch.h"

#ifdef API_D3D11
#include "Modules/Render/D3D11/D3D11Resources.h"
#define RHIVertexShader D3D11VertexShader
#define RHIPixelShader D3D11PixelShader
#define RHIDomainShader D3D11DomainShader
#define RHIHullShader D3D11HullShader
#define RHIComputeShader D3D11ComputeShader
#define RHIGeometryShader D3D11GeometryShader
#endif



/*
Renderer finds "{name}_{type}.hlsl" in shader base path, then shader is managed in shader manager
All in upper case
*/

class RShader
{
public:
	RShader(const EShaderType type, const WCHAR* name);
	~RShader() = default;

	inline const WCHAR* GetName() const
	{
		return m_name.c_str();
	}

protected:
	const EShaderType m_type;
	const std::wstring m_name;
};

class RVertexShader : public RShader, public RHIVertexShader
{
public:
	RVertexShader(const WCHAR* name);
	~RVertexShader() = default;
};

class RPixelShader : public RShader, public RHIPixelShader
{
public:
	RPixelShader(const WCHAR* name);
	~RPixelShader() = default;
};

class RComputeShader : public RShader, public RHIComputeShader
{
public:
	RComputeShader(const WCHAR* name);
	~RComputeShader() = default;
};

class RDomainShader : public RShader, public RHIDomainShader
{
public:
	RDomainShader(const WCHAR* name);
	~RDomainShader() = default;
};

class RHullShader : public RShader, public RHIHullShader
{
public:
	RHullShader(const WCHAR* name);
	~RHullShader() = default;
};

class RGeometryShader : public RShader, public RHIGeometryShader
{
public:
	RGeometryShader(const WCHAR* name);
	~RGeometryShader() = default;
};


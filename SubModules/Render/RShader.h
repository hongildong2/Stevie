#pragma once
#include "RenderDefs.h"


/*
Renderer finds "{name}_{type}.hlsl" in shader base path, then shader is managed in shader manager
All in upper case
*/

class RShader : public IRenderResource
{
public:
	RShader(const EShaderType type, const WCHAR* name);
	~RShader() = default;

	virtual void* GetShaderPtr() const = 0;

protected:
	const EShaderType m_type;
	const std::wstring m_name;
};

class RVertexShader : public RShader
{
public:
	RVertexShader(const WCHAR* name);
	~RVertexShader() = default
};

class RPixelShader : public RShader
{
public:
	RPixelShader(const WCHAR* name);
	~RPixelShader() = default
};

class RComputeShader : public RShader
{
public:
	RComputeShader(const WCHAR* name);
	~RComputeShader() = default
};

class RDomainShader : public RShader
{
public:
	RDomainShader(const WCHAR* name);
	~RDomainShader() = default
};

class RHullShader : public RShader
{
public:
	RHullShader(const WCHAR* name);
	~RHullShader() = default
};

class RGeometryShader : public RShader
{
public:
	RGeometryShader(const WCHAR* name);
	~RGeometryShader() = default
};


#pragma once
#include "IRenderer.h"

enum class EShaderType
{
	VERTEX_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
	DOMAIN_SHADER,
	HULL_SHADER,
	GEOMETRY_SHADER
};


// Renderer finds "{name}_{type}.hlsl" in shader base path, then shader is managed in shader manager
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


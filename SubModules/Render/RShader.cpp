#include "pch.h"
#include "RShader.h"

RShader::RShader(const EShaderType type, const WCHAR* name)
	: IRenderResource()
	, m_type(type)
	, m_name(name)
{
}

RVertexShader::RVertexShader(const WCHAR* name)
	: RShader(EShaderType::VERTEX_SHADER, name)
{
}

RPixelShader::RPixelShader(const WCHAR* name)
	: RShader(EShaderType::PIXEL_SHADER, name)
{
}

RComputeShader::RComputeShader(const WCHAR* name)
	: RShader(EShaderType::COMPUTE_SHADER, name)
{
}

RDomainShader::RDomainShader(const WCHAR* name)
	: RShader(EShaderType::DOMAIN_SHADER, name)
{
}

RHullShader::RHullShader(const WCHAR* name)
	: RShader(EShaderType::HULL_SHADER, name)
{

}

RGeometryShader::RGeometryShader(const WCHAR* name)
	: RShader(EShaderType::GEOMETRY_SHADER, name)
{
}




#include "pch.h"
#include "D3D11Shader.h"
#include "D3D11Renderer.h"

D3D11Shader::D3D11Shader(const EShaderType type, const WCHAR* name)
	: RShader(type, name)
{
}

void D3D11Shader::Initialize(const D3D11Renderer* pRenderer)
{
	m_pRenderer = pRenderer;
	auto* pDevice = m_pRenderer->GetDeviceResources()->GetD3DDevice();
	const WCHAR* BASE_PATH = m_pRenderer->GetShaderPath();

	constexpr size_t BUFFER_COUNT = 500;
	WCHAR shaderFileNameBuffer[BUFFER_COUNT] = { NULL, };
	const WCHAR* SHADER_SUFFIX = ToString(m_type);

	swprintf(shaderFileNameBuffer, BUFFER_COUNT, L"%s%s_%s.hlsl", BASE_PATH, m_name, SHADER_SUFFIX);


	static const D3D11_INPUT_ELEMENT_DESC BASIC_INPUT_LAYOUT[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static const D3D11_INPUT_ELEMENT_DESC SCREEN_QUAD_INPUT_LAYOUT[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	InitShader(shaderFileNameBuffer);
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	DX::ThrowIfFailed(DX::CompileShader(shaderFileNameBuffer, "main", "vs_5_0", NULL, shaderBlob.GetAddressOf()));
	DX::ThrowIfFailed(pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, screenQuadVS.GetAddressOf()));



}

D3D11VertexShader::D3D11VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader)
{
	m_vertexShader.As(&vertexShader)
}

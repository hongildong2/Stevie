#pragma once
#include "../RShader.h"


class D3D11VertexShader final : public RVertexShader
{
public:
	D3D11VertexShader(Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader);
	~D3D11VertexShader() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
};

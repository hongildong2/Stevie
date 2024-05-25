#pragma once
#include <wrl/client.h>
#include <d3d11.h>


class ComputePSO
{
public:
	void operator=(const ComputePSO& pso)
	{
		m_computeShader = pso.m_computeShader;
	};

public:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;
};

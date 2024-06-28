#include "pch.h"
#include "Ocean.h"

Ocean::Ocean(ID3D11Device1* device)
{
	// create d3d resources
	D3D11_TEXTURE3D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// load gaussian texture
	// create Sqaure that contains total N^2 grids
}

void Ocean::InitData(ID3D11DeviceContext1* context)
{
	// create buffers, textures

	// run initspectrum CS, get initial spectrum map
	// get initial wave vectors

	// conjugate necessary?

	// TODO : foam simulations

}

void Ocean::Update(ID3D11DeviceContext1* context)
{
	// If wave constant changed, re-run InitData routine
	if (false)
	{
		InitData(context);
	}

	// apply delta time, update spectrum map using tilde h0
	// RUN Time dependent sepctrum CS using textures tilde h0(k,t), and waveData that contains wave vector k
	// save result into FFT texture for IFFT CS to use it 

	// run IFFT CS, renew height map from spectrum map, get normal map from height map
	// use FFT texture updated prev time dependent spectrum CS
	// Run FFT CS twice on FFT texture as it is 2D FFT, one with horizontally and the other one with vertically(order can be changed)

	// TODO : run Foam Simulation on height map(Result IFFT Texture), get Turbulence Map using Jacobian and displacement



}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}

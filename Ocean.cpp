#include "pch.h"
#include "Ocean.h"

Ocean::Ocean(ID3D11Device1* device)
{
	// create d3d resources
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
	// RUN Time dependent sepctrum CS

	// run IFFT CS, renew height map from spectrum map, get normal map from height map

	// TODO : run FFT on height map, get displacement map with Jacobian map from displacement map also for foam simulaation



}

void Ocean::Draw(ID3D11DeviceContext1* context)
{
	// Set vertex shader <- resources : Height map, displacement map
	// Set Pixel Shader <- resources : Normal map, Jacobian Map, PBR constants
}

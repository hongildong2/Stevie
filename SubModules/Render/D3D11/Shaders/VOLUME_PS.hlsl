#include "INCL_ShaderTypes.hlsli"
#include "INCL_GPURandom.hlsli"

cbuffer GlobalConstants : register(b0)
{
	GlobalConstant globalConstants;
};

cbuffer MeshConstants : register(b1)
{
	MeshConstant meshConstants;
}
SamplerState linearClamp : register(s0);

Texture3D<float> densityTex : register(t3);
Texture3D<float> lightingTex : register(t4);

float QueryVolumetricSDF(float3 posModel, float3 uvw)
{
	float fbm = worleyFbm(uvw, 4.f);

		// sdf, shaping volume
	float sdf1 = length(posModel - -0.1) - 0.5;
	float sdf2 = length(posModel - 0.2) - 0.5;
        
	float sdf = min(sdf1, sdf2) + (fbm * 2.f - 1.f) * 0.3f;
	
	return sdf;
}

float3 ModelToUVW(float3 posModel)
{
	return (posModel.xyz + 1.0) * 0.5;
}

// https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-2.html
float BeerLambert(float absorptionCoefficient, float distanceTraveled)
{
	return exp(-absorptionCoefficient * distanceTraveled);
}

// Henyey-Greenstein phase function
// Graph: https://www.researchgate.net/figure/Henyey-Greenstein-phase-function-as-a-function-of-O-O-for-isotropic-scattering-g_fig1_338086693
float HenyeyGreensteinPhase(in float3 L, in float3 V, in float aniso)
{
    // V: eye - pos 
    // L: 조명을 향하는 방향
    // https://www.shadertoy.com/view/7s3SRH
    
	float cosT = dot(L, -V);
	float g = aniso;
	return (1.0 - g * g) / (4.0 * PI * pow(abs(1.0 + g * g - 2.0 * g * cosT), 3.0 / 2.0));
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float3 eyeModel = mul(float4(globalConstants.eyeWorld, 1), meshConstants.worldInv).xyz; // world eye to model coordinate system
	float3 VModel = normalize(input.positionModel - eyeModel); // view direction, omega in volumetric rendering
	
	int numSteps = 128;
	float volumeSize = 2.0;
	float stepSize = volumeSize / float(numSteps);
	
	float3 volumeAlbedo = float3(1, 1, 1); // cloud
	float3 L = float3(1, 1, 1) * 40.0;
	
	// temp
	float3 lightDir = float3(0, 1, 0); 
	float anisoParam = 0.2;
	float absorptionCoeff = 100.0; // sig_a

	
	// init
	float3 color = 0.7;
	float transmission = 1.0;
	
	float3 marchPointModel = input.positionModel + VModel * 1e-6;
	
	
	// forward march
	[loop]
	for (int i = 0; i < numSteps; ++i)
	{
		float3 uvw = ModelToUVW(marchPointModel);
		float density = densityTex.SampleLevel(linearClamp, uvw, 0).r;
		float lighting = lightingTex.SampleLevel(linearClamp, uvw, 0).r;
		
		
		float sdf = QueryVolumetricSDF(marchPointModel, uvw);
        
		if (sdf <= 0.0)
		{
            // inside object
		}
		else
		{
			density *= saturate(1.0 - sdf * 10.0); // outside of volume, smoothing
		}
		
		if (density > 1e-3)
		{
			float deltaTransmission = transmission;
			transmission *= BeerLambert(absorptionCoeff * density, stepSize); // absortion
			deltaTransmission -= transmission;
			
			// In-Scattering
			color += deltaTransmission * volumeAlbedo * L * density * lighting * HenyeyGreensteinPhase(lightDir, VModel, anisoParam);
		}
		
		marchPointModel += VModel * stepSize; // march along V
		
		if (abs(marchPointModel.x) > 1 || abs(marchPointModel.y) > 1 || abs(marchPointModel.z) > 1)
		{
			break;
		}
		
		if (transmission < 1e-3) // saturated
		{
			break;
		}
	}
	
	color = saturate(color);
	// transmission = saturate(transmission * 10.0 / 9.0);

	return float4(color, 1.0 - transmission);
}
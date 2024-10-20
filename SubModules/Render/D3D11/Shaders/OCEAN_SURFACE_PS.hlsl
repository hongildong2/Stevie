#include "RenderingCommons.hlsli"
#include "PBRCommons.hlsli"
#include "BrunetonLighting.hlsli"
#include "OceanGlobal.hlsli"
#include "ScreenSpace.hlsli"


struct BrunetonInput
{
	float3 lightDirWorld;
	float3 viewDirWorld;
	float3 normalWorld;
	float3 tangentXWorld;
	float3 tangentYWorld;
	float2 slopeVarianceSquared;
};

struct RenderingParameter
{
	float horizonFogParameter; // currently 1.8
	float sssNormalStrength; // 0.2, SubsurfacScattering
	float sssOceanWaveReferenceHeight; // 5;
	float sssWaveHeightBias; // 1.3

	float sssSunStrength; // 3
	float sssEnvironmentStrength; // 3
	float directLightScaler; // 15
	float roughnessMultiplier; // 10

	float3 depthScatterColor; // oceanColor * 0.7;
	float sssSpread; // 0.014

	float3 sssColor; // oceanColor * 0.9
	float sssFadeDistance; // 2

		// Wave Variance, affects fresnel and roughness
	float windSpeed; // 5.0
	float waveAlignment; // 1
	float scale; // 2048
	float meanFresnelWeight; // 0.02

	float specularStrength; // 2.5
	float shadowMultiplier; // 1
	float foamWaveSharpness; // 0.9 :: Foam Parameters
	float foamPersistency; // 0.5

	float foamDensity; //0.11
	float foamCoverage; // 0.65
	float foamTrailness; // 0.5
	float foamValueBias; // 0.03 0~1
};

cbuffer RenderParamBuffer : register(b5)
{
	RenderingParameter renderParam;
}

Texture2DArray<float4> OceanTurbulenceMap : register(t100);
StructuredBuffer<CombineParameter> OceanCascadeParameters : register(t101);
Texture2D<float3> SkyTexture : register(t102);
Texture2D<float> foamTexture : register(t103);

float2 SlopeVarianceSquared(float windSpeed, float viewDist, float alignement, float scale)
{
	float upwind = 0.01 * sqrt(windSpeed) * viewDist / (viewDist + scale);
	return float2(upwind, upwind * (1 - 0.3 * alignement));
}

float4 HorizonBlend(float3 viewDir, float3 viewDist, float3 eyeWorld)
{
	float3 dir = -float3(viewDir.x, 0, viewDir.z);
	
	float3 horizonColor = cubeMap.Sample(linearWrap, dir);
	
	float distanceScale = 20 + 7 * abs(eyeWorld.y);

	
	float t = exp(-5 / max(renderParam.horizonFogParameter, 0.01) * (abs(viewDir.y) + distanceScale / (viewDist + distanceScale)));
	
	return float4(horizonColor, t);
}

float2 SubsurfaceScatteringFactor(float3 viewDir, float3 viewDist, float3 lightDir, float3 posWorld, float3 normalWorld)
{
	float normalFactor = saturate(dot(normalize(lerp(viewDir, normalWorld, renderParam.sssNormalStrength)), viewDir));
	float heightFactor = saturate((posWorld.y + renderParam.sssOceanWaveReferenceHeight * renderParam.sssWaveHeightBias * 0.5 / max(0.5, renderParam.sssOceanWaveReferenceHeight)));
	
	heightFactor = pow(abs(heightFactor), max(1, renderParam.sssOceanWaveReferenceHeight * 0.4));
	
	float sun = renderParam.sssSunStrength * normalFactor * heightFactor * pow(saturate(dot(lightDir, -viewDir)), min(50, 1.0 / renderParam.sssSpread));
	float environment = renderParam.sssEnvironmentStrength * normalFactor * heightFactor * saturate(1 - viewDir.y);

	float2 sssFactor = float2(sun, environment);

	sssFactor *= renderParam.sssFadeDistance / (renderParam.sssFadeDistance + viewDist);

	return sssFactor;
}

float3 Refraction(float3 oceanColor, float3 lightColor, float NdotL, float2 sss)
{
	float3 color = renderParam.depthScatterColor;
	float3 sssColor = renderParam.sssColor;
	
	color += sssColor * saturate(sss.x + sss.y);

	color += (NdotL * 0.8 + 0.2) * lightColor * oceanColor;

	return color;
}


float4 main(PixelShaderInput input) : SV_TARGET
{
	const float WATER_F0 = 0.02;
	const float3 UP_VEC = float3(0, 1, 0);
	
	// 라이트 추가하면 수정바람..
	Light globalSunLight = globalLights[SUN_LIGHT_INDEX];
	Texture2D sunShadowMap = shadowMaps[SUN_LIGHT_INDEX];

	
	float3 tangentY = float3(0, input.normalWorld.z, -input.normalWorld.y);
	tangentY /= max(0.001, length(tangentY));
	float3 tangentX = normalize(cross(tangentY, input.normalWorld));

	
	float3 V = normalize(globalConstants.eyeWorld - input.positionWorld);
	// float3 V = normalize(-eyeDir);
	float3 L = normalize(globalSunLight.positionWorld - input.positionWorld);
	// float3 L = normalize(-globalSunLight.direction);
	float NdotL = saturate(dot(input.normalWorld, L));
	
	// Brunton Ocean PBR
	BrunetonInput BrInput;
	BrInput.lightDirWorld = globalSunLight.direction;
	BrInput.viewDirWorld = V;
	BrInput.normalWorld = input.normalWorld;
	BrInput.tangentXWorld = tangentX;
	BrInput.tangentYWorld = tangentY;
	
	float viewDist = distance(input.positionWorld, globalConstants.eyeWorld);
	float windSpeed = renderParam.windSpeed;
	float waveAlignment = renderParam.waveAlignment;
	float scale = renderParam.scale;
	float roughFac = renderParam.roughnessMultiplier;
	float roughness = materialConstant.roughness;
	
	BrInput.slopeVarianceSquared = roughFac * (1 + roughness * 0.3)
								* SlopeVarianceSquared(windSpeed, viewDist, waveAlignment, scale);
								
	float meanFresnel = renderParam.meanFresnelWeight * MeanFresnel(BrInput.viewDirWorld, BrInput.normalWorld, BrInput.slopeVarianceSquared);
	float effectiveFresnel = saturate(WATER_F0 + (1 - WATER_F0) * meanFresnel); // 0.033 too much effective fresnel, skymap mapping is weird
	
	const float MIN_ROUGHNESS_BIAS = 0.02;
	
	// Calc BRDF
	
	ShadowInput shadowIn =
	{
		sunShadowMap, globalSunLight, input.positionWorld, shadowPointSampler, shadowCompareSampler, globalConstants.nearZ

	};
	float shadowFactorBySunlight = renderParam.shadowMultiplier * GetShadowFactor(shadowIn);

	float specular = ReflectedSunRadiance(BrInput.lightDirWorld, BrInput.viewDirWorld, BrInput.normalWorld, BrInput.tangentXWorld, BrInput.tangentYWorld, max(1e-4, BrInput.slopeVarianceSquared + MIN_ROUGHNESS_BIAS))
						* renderParam.specularStrength * globalSunLight.color;
						
	float2 sssF = SubsurfaceScatteringFactor(V, viewDist, globalSunLight.direction, input.positionWorld, input.normalWorld);
	
	float3 reflected = MeanSkyRadiance(SkyTexture, linearWrap, BrInput.viewDirWorld, BrInput.normalWorld, BrInput.tangentXWorld, BrInput.tangentYWorld, BrInput.slopeVarianceSquared) * materialConstant.IBLStrength;
	float3 refracted = Refraction(materialConstant.albedo, globalSunLight.color, NdotL, sssF);
	float4 horizon = HorizonBlend(V, viewDist, globalConstants.eyeWorld);
	
	float3 color = specular + lerp(refracted, reflected, effectiveFresnel);
	color *= shadowFactorBySunlight;
	
	
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	for (uint lightIndex = 0; lightIndex < globalConstants.globalLightsCount - 1; ++lightIndex)
	{
		ShadowInput directLightShadowInput =
		{
			shadowMaps[lightIndex], globalLights[lightIndex], input.positionWorld, shadowPointSampler, shadowCompareSampler, globalConstants.nearZ
		};
		Lo += RadianceLByDirectLight(directLightShadowInput, globalLights[lightIndex], WATER_F0, input.normalWorld, V, input.positionWorld, materialConstant.albedo, roughness, materialConstant.metallic);
	}
	
	color += Lo * renderParam.directLightScaler;
	color = lerp(color, horizon.rgb, horizon.a);
	
	// Foam
	OceanSamplingInput oceanIn;
	oceanIn.parameters = OceanCascadeParameters;
	oceanIn.tex = OceanTurbulenceMap;
	oceanIn.uv = input.texcoordinate;
	oceanIn.cascadesCount = CASCADE_COUNT;
	oceanIn.ss = linearWrap;
	oceanIn.simulationScaleInMeter = SIMULATION_SIZE_IN_METER;
		
	FoamParameter foamParamettter;
	foamParamettter.waveSharpness = renderParam.foamWaveSharpness;
	foamParamettter.foamPersistency = renderParam.foamPersistency;
	foamParamettter.foamDensity = renderParam.foamDensity;
	foamParamettter.foamCoverage = renderParam.foamCoverage;
	foamParamettter.foamTrailness = renderParam.foamTrailness;
	foamParamettter.foamValueBias = renderParam.foamValueBias; // 0 ~ 1
	
	FoamInput foamIn;
	foamIn.worldUV = input.texcoordinate;
	foamIn.viewDist = viewDist;
	foamIn.oceanSampling = oceanIn;
	foamIn.foamParam = foamParamettter;
	
	FoamOutput foamOut = GetFoamOutput(foamTexture, linearWrap, foamIn);
	
	ScreenSpaceContactFoam(foamOut, cameraDepthMap, shadowPointSampler, input.positionProjection, globalConstants.invProj);
	
	const float SUN_SHADOW_ATTENUATION = 0.8;
	float3 foamColor = LitFoamColor(foamOut, irradianceMap, linearWrap, input.normalWorld, NdotL, globalSunLight.color, SUN_SHADOW_ATTENUATION);
	color = lerp(color, foamOut.albedo, foamOut.coverage);
	
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1);
}
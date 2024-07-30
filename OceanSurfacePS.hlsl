#include "RenderingCommons.hlsli"
#include "PBRCommons.hlsli"
#include "BrunetonLighting.hlsli"
#include "OceanGlobal.hlsli"


struct BrunetonInput
{
	float3 lightDirWorld;
	float3 viewDirWorld;
	float3 normalWorld;
	float3 tangentXWorld;
	float3 tangentYWorld;
	float2 slopeVarianceSquared;
};

Texture2DArray<float4> OceanTurbulenceMap : register(t100);
StructuredBuffer<CombineParameter> OceanCascadeParameters : register(t101);

float2 SlopeVarianceSquared(float windSpeed, float viewDist, float alignement, float scale)
{
	float upwind = 0.01 * sqrt(windSpeed) * viewDist / (viewDist + scale);
	return float2(upwind, upwind * (1 - 0.3 * alignement));
}

float4 HorizonBlend(float3 viewDir, float3 viewDist, float3 eyeWorld)
{
	float3 dir = -float3(viewDir.x, 0, viewDir.z);
	float3 horizonColor = cubeMap.Sample(linearWrap, dir);
	
	float distanceScale = 100 + 7 * abs(eyeWorld.y);
	
	float HORIZON_FOG_PARAM = 0.01;
	
	float t = exp(-5 / max(HORIZON_FOG_PARAM, 0.01) * (abs(viewDir.y) + distanceScale / (viewDist + distanceScale)));
	
	return float4(horizonColor, t);
}

float2 SubsurfaceScatteringFactor(float3 viewDir, float3 viewDist, float3 lightDir, float3 posWorld, float3 normalWorld)
{
	const float SSS_NORMAL_STRENGTH = 0.5;
	const float OCEAN_REFERENCE_WAVE_HEIGHT = 0.7;
	const float WAVE_HEIGHT_BIAS = 1.3;
	float normalFactor = saturate(dot(normalize(lerp(viewDir, normalWorld, SSS_NORMAL_STRENGTH)), viewDir));
	float heightFactor = saturate((posWorld.y + OCEAN_REFERENCE_WAVE_HEIGHT * WAVE_HEIGHT_BIAS * 0.5 / max(0.5, OCEAN_REFERENCE_WAVE_HEIGHT)));
	
	heightFactor = pow(abs(heightFactor), max(1, OCEAN_REFERENCE_WAVE_HEIGHT * 0.4));
	
	const float SSS_SUN_STRENGTH = 1.3;
	const float SSS_ENVIRONMEN_STRENGTH = 1.0;
	const float SSS_SPREAD = 0.014;
	
	float sun = SSS_SUN_STRENGTH * normalFactor * heightFactor * pow(saturate(dot(lightDir, -viewDir)), min(50, 1.0 / SSS_SPREAD));
	float environment = SSS_ENVIRONMEN_STRENGTH * normalFactor * heightFactor * saturate(1 - viewDir.y);

	const float SSS_FADE_DISTANCE = 20.0;
	float2 sssFactor = float2(sun, environment);

	sssFactor *= SSS_FADE_DISTANCE / (SSS_FADE_DISTANCE + viewDist);

	return sssFactor;
}

float3 Refraction(float3 oceanColor, float3 lightColor, float NdotL, float2 sss)
{
	const float3 DEPTH_SCATTER_COLOR = oceanColor * 0.7;
	const float3 SSS_COLOR = oceanColor * 0.9;
	
	float3 color = DEPTH_SCATTER_COLOR;
	float3 sssColor = SSS_COLOR;
	
	color += sssColor * saturate(sss.x + sss.y);

	color += (NdotL * 0.8 + 0.2) * lightColor * oceanColor;

	return color;
}

float3 LitFoamColor(FoamOutput foamData, float3 N, float NdotL, float3 lightColor, float lightAttenuation)
{
	const float3 FOAM_TINT_RGB = 0;
	float ndotl = (0.2 + 0.8 * NdotL) * lightAttenuation;
	float3 envDiffuse = irradianceMap.Sample(linearWrap, N);
	return foamData.albedo * (ndotl * lightColor + envDiffuse);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	const float WATER_F0 = 0.02;
	const float3 UP_VEC = float3(0, 1, 0);
	Light globalSunLight = globalLights[getSunlightIndex()];
	
	float3 tangentY = float3(0, input.normalWorld.z, -input.normalWorld.y);
	tangentY /= max(0.001, length(tangentY));
	float3 tangentX = normalize(cross(tangentY, input.normalWorld));

	float3 V = normalize(input.positionWorld - eyeWorld);

	float3 L = normalize(globalSunLight.positionWorld - input.positionWorld);
	float NdotL = saturate(dot(input.normalWorld, L));
	
	// Brunton Ocean PBR
	BrunetonInput BrInput;
	BrInput.lightDirWorld = globalSunLight.direction;
	BrInput.viewDirWorld = V;
	BrInput.normalWorld = input.normalWorld;
	BrInput.tangentXWorld = tangentX;
	BrInput.tangentYWorld = tangentY;
	
	float viewDist = distance(input.positionWorld, eyeWorld);
	float windSpeed = 10.0;
	float waveAlignment = 1.0;
	float scale = 1;
	BrInput.slopeVarianceSquared = materialConstant.roughnessFactor * (1 + materialConstant.roughness * 0.3)
								* SlopeVarianceSquared(windSpeed, viewDist, waveAlignment, scale);
								
	float meanFresnel = MeanFresnel(BrInput.viewDirWorld, BrInput.normalWorld, BrInput.slopeVarianceSquared);
	float effectiveFresnel = saturate(WATER_F0 + (1 - WATER_F0) * meanFresnel);
	
	const float MIN_ROUGHNESS_BIAS = 0.02;
	const float SPECULAR_STRENGTH = materialConstant.t1;
	
	// Calc BRDF
	float specular = ReflectedSunRadiance(BrInput.lightDirWorld, BrInput.viewDirWorld, BrInput.normalWorld, BrInput.tangentXWorld, BrInput.tangentYWorld, max(1e-4, BrInput.slopeVarianceSquared + MIN_ROUGHNESS_BIAS))
						* SPECULAR_STRENGTH * globalSunLight.color;
						
	float2 sssF = SubsurfaceScatteringFactor(V, viewDist, globalSunLight.direction, input.positionWorld, input.normalWorld);
	
	// 그냥 skymap 텍스쳐 추가
	float3 reflected = MeanSkyRadiance(cubeMap, linearWrap, BrInput.viewDirWorld, BrInput.normalWorld, BrInput.tangentXWorld, BrInput.tangentYWorld, BrInput.slopeVarianceSquared);
	// float3 reflected = MeanSkyRadianceUVWorld(cubeMap, linearWrap, input.texcoordinate, BrInput.slopeVarianceSquared);
	float3 refracted = Refraction(materialConstant.albedo, globalSunLight.color, NdotL, sssF);
	float4 horizon = HorizonBlend(V, viewDist, eyeWorld);
	
	float3 color = specular + lerp(refracted, reflected, effectiveFresnel);
	color = lerp(color, horizon.rgb, horizon.a);
	
	// Foam
	OceanSamplingInput oceanIn;
	oceanIn.parameters = OceanCascadeParameters;
	oceanIn.tex = OceanTurbulenceMap;
	oceanIn.uv = input.texcoordinate;
	oceanIn.cascadesCount = CASCADE_COUNT;
	oceanIn.ss = linearWrap;
	oceanIn.simulationScaleInMeter = SIMULATION_SIZE_IN_METER;
		
	FoamParameter tempP;
	tempP.waveSharpness = 0.8;
	tempP.foamPersistency = 0.01;
	tempP.foamDensity = 0.08;
	tempP.foamCoverage = 0.65;
	tempP.foamTrailness = 0;
	tempP.foamValueBias = 0.03; // 0 ~ 1
	
	FoamInput foamIn;
	foamIn.worldUV = input.texcoordinate;
	foamIn.viewDist = viewDist;
	foamIn.oceanSampling = oceanIn;
	foamIn.foamParam = tempP;
	
	FoamOutput foamOut = GetFoamOutput(foamIn);
	
	const float SUN_SHADOW_ATTENUATION = 0.8;
	float3 foamColor = LitFoamColor(foamOut, input.normalWorld, NdotL, globalSunLight.color, SUN_SHADOW_ATTENUATION);
	color = lerp(color, foamOut.albedo, foamOut.coverage);
	
	return float4(color, 1);
}
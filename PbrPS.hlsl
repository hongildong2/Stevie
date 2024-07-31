#include "RenderingCommons.hlsli"
#include "PBRCommons.hlsli"

#ifdef OCEAN_SHADER
#include "OceanGlobal.hlsli"
Texture2DArray<float4> OceanTurbulenceMap : register(t100);
StructuredBuffer<CombineParameter> OceanCascadeParameters : register(t101);
#endif


float3 GetNormal(PixelShaderInput input)
{
	float3 normalWorld = normalize(input.normalWorld);

	float3 normal = normalTex.Sample(linearWrap, input.texcoordinate).rgb;
	normal = 2.0 * normal - 1.0; // 범위 조절 [-1.0, 1.0]

    // OpenGL 용 노멀맵일 경우에는 y 방향을 뒤집어줍니다.
	// normal.y = invertNormalMapY ? -normal.y : normal.y;
        
	float3 N = normalWorld;
	float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
	float3 B = cross(N, T);
        
    // matrix는 float4x4, 여기서는 벡터 변환용이라서 3x3 사용
	float3x3 TBN = float3x3(T, B, N);
	normalWorld = normalize(mul(normal, TBN));
    
	return normalWorld;
}


float4 main(PixelShaderInput input) : SV_TARGET
{

	float3 albedo;
	float ao;
	float metallic;
	float roughness;
	
	float viewDist = distance(eyeWorld, input.positionWorld);
	
	if (materialConstant.bUseTexture)
	{
		albedo = albedoTex.Sample(linearWrap, input.texcoordinate).rgb;
		ao = aoTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.aoFactor;
		metallic = metallicTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.metallicFactor;
		roughness = roughnessTex.Sample(linearWrap, input.texcoordinate).r * materialConstant.roughnessFactor;
	}
	else
	{
		albedo = materialConstant.albedo;
		ao = 1.f;
		metallic = materialConstant.metallic;
		roughness = materialConstant.roughness;
	}
	
	
	Light sunLight = globalLights[globalLightsCount];
	
	float3 N = materialConstant.bUseTexture ? GetNormal(input) : input.normalWorld;
	float3 V = normalize(eyeWorld - input.positionWorld);
	
	
	
	#ifdef OCEAN_SHADER
	float3 L = normalize(-sunLight.direction);
	V = normalize(-eyeDir);
	float2 xzVec = normalize((input.texcoordinate * 2.0) - 1.0);
	float3 xzVecWorld = float3(xzVec.x, 0, xzVec.y);
	float3 oceanEnvVec = normalize(xzVecWorld + N);
	float oceanAlignment = 0.14;
	
	float NdotL = saturate(dot(N, L));
	#endif
	
	
	float3 F0 = float3(0.08, 0.08, 0.08) * materialConstant.specular; // specular 1 == 8%
	F0 = lerp(F0, albedo, metallic);
	float3 Lo = float3(0.0, 0.0, 0.0);
	#ifdef OCEAN_SHADER
	// must add sunlight!
	for (uint lightIndex = 0; lightIndex < globalLightsCount; ++lightIndex)
	{
		Lo += RadianceLByDirectLight(globalLights[lightIndex], F0, N, V, input.positionWorld, albedo, roughness, metallic); // TODO : light type differentiation
	}
	#else
	for (uint lightIndex = 0; lightIndex < globalLightsCount; ++lightIndex)
	{
		Lo += RadianceLByDirectLight(globalLights[lightIndex], F0, N, V, input.positionWorld, albedo, roughness, metallic); // TODO : light type differentiation
	}
	#endif
	

	// IBL	

	#ifdef OCEAN_SHADER
	// 바다는 크니까, L과 V를 대국적으로 생각해야한다.
	float3 F = fresnelSchlickRoughness(oceanAlignment, F0, roughness);
	float3 irradiance = irradianceMap.Sample(linearWrap, oceanEnvVec).rgb;
	float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, oceanEnvVec, materialConstant.t1).rgb;
	float2 envBRDF = BRDFMap.Sample(linearClamp, float2(oceanAlignment, roughness)).xy; // 바다는 크니깐..
	
	#else
	
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	float3 irradiance = irradianceMap.Sample(linearWrap, N).rgb;
	float3 prefilteredColor = SpecularMap.SampleLevel(linearWrap, reflect(-V, N), materialConstant.t1).rgb;
	float2 envBRDF = BRDFMap.Sample(linearClamp, float2(max(dot(N, V), 0.0), roughness)).xy;
	
	#endif
	
	float3 kS = F;
	float3 kD = lerp(1.0 - kS, 0.0, metallic);
	
	float3 diffuse = irradiance * albedo;
	float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
	
	float3 ambient = (kD * diffuse + specular) * ao;	
	
	float3 color = ambient + Lo; // IBL + Lights
	
	#ifdef OCEAN_SHADER 		// Foam
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
	ScreenSpaceContactFoam(foamOut, cameraDepthMap, linearClamp, input.positionProjection, viewDist);
	
	
	const float SUN_SHADOW_ATTENUATION = 0.8;
	float3 foamColor = LitFoamColor(foamOut, irradianceMap, linearWrap, N, NdotL, sunLight.color, SUN_SHADOW_ATTENUATION);
	color = lerp(color, foamOut.albedo, foamOut.coverage);
	#endif

	
	
	color = clamp(color, 0.0, 1000.0);
	return float4(color, 1.0f);
}
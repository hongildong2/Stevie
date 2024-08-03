#ifndef __COORD_TRANSFORM__
#define __COORD_TRANSFORM__
#include "RenderingCommons.hlsli"

#define LIGHT_FRUSTRUM_WIDTH (0.34641)

static const float2 diskSamples64[64] =
{
	float2(0.0, 0.0),
            float2(-0.12499844227275288, 0.000624042775189866), float2(0.1297518688031755, -0.12006020382326336),
            float2(-0.017851253586055427, 0.21576916541852392), float2(-0.1530983013115895, -0.19763833164521946),
            float2(0.27547541035593626, 0.0473106572479027), float2(-0.257522587854559, 0.16562643733622642),
            float2(0.0842605283808073, -0.3198048832600703), float2(0.1645196099088727, 0.3129429627830483),
            float2(-0.3528833088400373, -0.12687935349026194), float2(0.36462214742013344, -0.1526456341030772),
            float2(-0.17384046457324884, 0.37637015407303087), float2(-0.1316547617859344, -0.4125130588224921),
            float2(0.3910687393754993, 0.2240317858770442), float2(-0.45629121277761536, 0.10270505898899496),
            float2(0.27645268679640483, -0.3974278701387824), float2(0.06673001731984558, 0.49552709793561556),
            float2(-0.39574431915605623, -0.33016879600548193), float2(0.5297612167716342, -0.024557141621887494),
            float2(-0.3842909284448636, 0.3862583103507092), float2(0.0230336562454131, -0.5585422550532486),
            float2(0.36920334463249477, 0.43796562686149154), float2(-0.5814490172413539, -0.07527974727019048),
            float2(0.4903718680780365, -0.3448339179919178), float2(-0.13142003698572613, 0.5981043168868373),
            float2(-0.31344141845114937, -0.540721256470773), float2(0.608184438565748, 0.19068741092811003),
            float2(-0.5882602609696388, 0.27536315179038107), float2(0.25230610046544444, -0.6114259003901626),
            float2(0.23098706800827415, 0.6322736546883326), float2(-0.6076303951666067, -0.31549215975943595),
            float2(0.6720886334230931, -0.1807536135834609), float2(-0.37945598830371974, 0.5966683776943834),
            float2(-0.1251555455510758, -0.7070792667147104), float2(0.5784815570900413, 0.44340623372555477),
            float2(-0.7366710399837763, 0.0647362251696953), float2(0.50655463562529, -0.553084443034271),
            float2(8.672987356252326e-05, 0.760345311340794), float2(-0.5205650355786364, -0.5681215043747359),
            float2(0.7776435491294021, 0.06815798190547596), float2(-0.6273416101921778, 0.48108471615868836),
            float2(0.1393236805531513, -0.7881712453757264), float2(0.4348773806743975, 0.6834703093608201),
            float2(-0.7916014213464706, -0.21270211499241704), float2(0.7357897682897174, -0.38224784745000717),
            float2(-0.2875567908732709, 0.7876776574352392), float2(-0.3235695699691864, -0.7836151691933712),
            float2(0.7762165924462436, 0.3631291803355136), float2(-0.8263007976064866, 0.2592816844184794),
            float2(0.4386452756167397, -0.7571098481588484), float2(0.18988542402304126, 0.8632459242554175),
            float2(-0.7303253445407815, -0.5133224046555819), float2(0.8939004035324556, -0.11593993515830946),
            float2(-0.5863762307291154, 0.6959079795748251), float2(-0.03805753378232556, -0.9177699189461416),
            float2(0.653979655650389, 0.657027860897389), float2(-0.9344208130797295, -0.04310155546401203),
            float2(0.7245109901504777, -0.6047386420191574), float2(-0.12683493131695708, 0.9434844461875473),
            float2(-0.5484582700240663, -0.7880790100251422), float2(0.9446610338564589, 0.2124041692463835),
            float2(-0.8470120123194587, 0.48548496473788055), float2(0.29904134279525085, -0.9377229203230629),
            float2(0.41623562331748715, 0.9006236205438447),
};

struct SamplingVertexShaderInput
{
	float3 positionModel : POSITION;
	float2 texcoord : TEXCOORD;
};

struct SamplingPixelShaderInput
{
	float3 positionModel : POSITION;
	float4 positionProj : SV_POSITION; // 여기에 뭔 값이라도 넣어줘야 파이프라인에서 래스터를 하지!! screen quad는 그냥 우연으로 됐던것 뿐
	float2 texcoord : TEXCOORD;
};

float4 TexcoordToView(float2 texcoord, Texture2D<float> depthOnlyTex, SamplerState linearClamp, matrix invProj)
{
	float4 posProj;

    // [0, 1]x[0, 1] -> [-1, 1]x[-1, 1] from texcoord to NDC
	posProj.xy = texcoord * 2.0 - 1.0;
	posProj.y *= -1; // y 좌표 뒤집기
	posProj.z = depthOnlyTex.Sample(linearClamp, texcoord).r;
	posProj.w = 1.0;
	// NDC now recovered
    
    // ProjectSpace -> ViewSpace
    //float4 posView = mul(posProj, lights[0].invProj);
	float4 posView = mul(posProj, invProj);
	posView.xyz /= posView.w;
    
	// view Pos now recovered
	return posView;
}

float2 ScreenToTexcoord(float2 positionScreen)
{
	float2 texcoord = float2(positionScreen.x, -positionScreen.y);
	texcoord += 1.0;
	texcoord *= 0.5;
	
	return texcoord;
}

float NDCToViewDepth(float NDCDepth, matrix invProj)
{
	float4 pointView = mul(float4(0, 0, NDCDepth, 1), invProj); // into view space
	return pointView.z / pointView.w; // perspective divide

}
// to sample skymap from texture cube
float3 GetSkyCubeNormal(float2 uv)
{
	// normal to sky, y is always 1 xz -11 -> 00, xz 1-1 -> 11
	const float3 UNIT_Y = float3(0, 1, 0);
	float2 xzVector = (uv * 2.0) - float2(1, 1);
	xzVector.y = -xzVector.y;	
	float3 xzUnitVec = float3(xzVector.x, 0, xzVector.y);
	
	float3 uvToNormal = normalize(xzUnitVec + UNIT_Y);
	
	return uvToNormal;
}

float PCF_Filter(float2 uv, float zReceiverNDC, float filterRadiusUV, Texture2D shadowMap)
{
	float sum = 0.0f;
	for (int i = 0; i < 64; ++i)
	{
		float2 offset = diskSamples64[i] * filterRadiusUV;
		sum += shadowMap.SampleCmpLevelZero(
            shadowCompareSampler, uv + offset, zReceiverNDC);
	}
	return sum / 64.0;
}
void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv, float zReceiverView, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
	float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTRUM_WIDTH;
	
	float searchRadius = lightRadiusUV * (zReceiverView - nearZ) / zReceiverView;
	
	float blockerSum = 0;
	numBlockers = 0;
	
	for (uint i = 0; i < 64; ++i)
	{
		float shadowMapDepth = shadowMap.SampleLevel(shadowPointSampler, float2(uv + diskSamples64[i] * searchRadius), 0).r;
		
		shadowMapDepth = NDCToViewDepth(shadowMapDepth, invProj);
		
		if (shadowMapDepth < zReceiverView)
		{
			blockerSum += shadowMapDepth;
			numBlockers++;
		}
	}
	
	avgBlockerDepthView = blockerSum / numBlockers;
}

// honglab course
float PCSS(float2 uv, float zReceiverNDC, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
	float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTRUM_WIDTH;
	float zReceiverView = NDCToViewDepth(zReceiverNDC, invProj);
	
	float avgBlockerDepthView = 0;
	float numBlockers = 0;
	
	
	FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap, invProj, lightRadiusWorld);

	if (numBlockers < 1)
	{
		return 1.f;
	}
	else
	{
	        // STEP 2: penumbra size
		float penumbraRatio = (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView;
		float filterRadiusUV = penumbraRatio * lightRadiusUV * nearZ / zReceiverView;

        // STEP 3: filtering
		return PCF_Filter(uv, zReceiverNDC, filterRadiusUV, shadowMap);
	}	
}

float GetShadowFactor(Texture2D shadowMap, Light light, float3 posWorld)
{
	// current pixel's pos into light's space
	float4 positionLightScreen = mul(float4(posWorld, 1), light.view);
	positionLightScreen = mul(positionLightScreen, light.proj);
	positionLightScreen.xyz /= positionLightScreen.w; // manual perspective devide, clip space -> NDC
	
	float2 texcoordLightSpace = ScreenToTexcoord(positionLightScreen.xy);
	
	const float BIAS = 0.001;
	return PCSS(texcoordLightSpace, positionLightScreen.z - BIAS, shadowMap, light.invProj, light.radius);
}
#endif /* __COORD_TRANSFORM__ */
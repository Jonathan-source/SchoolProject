#include "HelperFunctions.hlsli"

Texture2D GPositionTexture          : register(t0);
Texture2D GNormalTexture            : register(t1);
Texture2D GDiffuseTexture           : register(t2);
StructuredBuffer<Light> SceneLights : register(t3);
Texture2D GDepthTexture              : register(t4);

SamplerState PointSampler           : register(s0);

// GLOBAL DEFINES.
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer DepthMatrixBuffer : register(b0)
{
    row_major matrix LightProjectionMatrix;
};


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer PerFrame : register(b1)
{
    row_major matrix ProjectionMatrix;
    row_major matrix ViewMatrix;
    float4 CameraPosition;
    float4 MousePosition;
    float4 GlobalAmbient;   // w = strenght.
    uint NumLights;
};


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer ImGUI : register(b2)
{
    int bPrintGPositionTexture;
    int bPrintGDiffuseTexture;
    int bPrintGNormalTexture;
    int bPrintGDepthTexture;
};


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
struct PixelInputType
{
    float4 position : SV_POSITION; 
    float4 normal   : NORMAL;      
    float2 texCoord : TEXCOORD0;    
};





//--------------------------------------------------------------------------------------
// MAIN
//--------------------------------------------------------------------------------------
float4 main(PixelInputType input) : SV_TARGET
{
    // Initialize output.
    float4 outputColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Sample the color / normal / position (in world space) from respective render texture.
    const float3 surfacePosition    = GPositionTexture.Sample(PointSampler, input.texCoord.xy).xyz; 
	const float4 surfaceColor       = GDiffuseTexture.Sample(PointSampler, input.texCoord.xy);
    float3 surfaceNormal            = GNormalTexture.Sample(PointSampler, input.texCoord.xy).xyz;
    const float surfaceDepth        = GDepthTexture.Sample(PointSampler, input.texCoord.xy).r;

    // ImGuiDebug
    if (bPrintGPositionTexture)
        return float4(surfacePosition, 1.0f);
    if (bPrintGDiffuseTexture)
        return surfaceColor;
    if (bPrintGNormalTexture)
        return float4(surfaceNormal, 1.0f);
    if(bPrintGDepthTexture)
        return float4(surfaceDepth, surfaceDepth, surfaceDepth, 1.0f);


    // The vertex's normal vector is being interpolated across the primitive
    // which can make it un-normalized. So normalize the vertex's normal vector.
    surfaceNormal = normalize(surfaceNormal);

    // Calculate Ambient Term:  
    const float4 ambient = float4(GlobalAmbient.xyz * GlobalAmbient.w, 1.0f);
    outputColor = ambient * surfaceColor;
	
    // FOR EACH LIGHT:
    [unroll(10)]
    for (int i = 0; i < NumLights; i++)
    {
        // Get current light.
        const Light currentLight = SceneLights[i];
        
        // Ignore non-enabled lights.
    if (currentLight.enabled == 0) 
        continue;

        // Calculate point to light vector. 
        const float3 pointToLight = (currentLight.type == DIRECTIONAL_LIGHT) ? -normalize(currentLight.direction.xyz) : (currentLight.position.xyz - surfacePosition);
 	
        // Skip point and spot lights that are out of range of the point being shaded.
        const float dist = length(pointToLight);
    if (currentLight.type != DIRECTIONAL_LIGHT &&
            dist > currentLight.range)
        continue;

        // Calculate point to camera vector.
        const float3 pointToCamera = normalize(CameraPosition.xyz - surfacePosition.xyz);
    	    	
        switch (currentLight.type)
        {
		case POINT_LIGHT:
            outputColor += PointLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor);
		break;
		case DIRECTIONAL_LIGHT:
                if (i == 0)
                {
                    // Shadow calculations.
                    float4 positionL = mul(float4(surfacePosition, 1.0f), LightProjectionMatrix);
                    positionL.xy /= positionL.w;
                    float2 smTex = float2(0.5f * positionL.x + 0.5f, -0.5f * positionL.y + 0.5f);
                    float depth = positionL.z / positionL.w;
                    float bias = 0.01f;
                    float dx = 1.0f / 1080;
                    float s0 = (GDepthTexture.Sample(PointSampler, smTex).r + bias < depth) ? 0.0f : 1.0f;
                    float s1 = (GDepthTexture.Sample(PointSampler, smTex + float2(dx, 0.0f)).r + bias < depth) ? 0.0f : 1.0f;
                    float s2 = (GDepthTexture.Sample(PointSampler, smTex + float2(0.0f, dx)).r + bias < depth) ? 0.0f : 1.0f;
                    float s3 = (GDepthTexture.Sample(PointSampler, smTex + float2(dx, dx)).r + bias < depth) ? 0.0f : 1.0f;

                    float2 texelPos = smTex * 1024;
                    float2 lerps = frac(texelPos);

                    float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);
            
                    outputColor += DirectionalLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor) * shadowCoeff;
                }
                else
                {
                  outputColor += DirectionalLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor);
                }
       	break;
		case SPOT_LIGHT:
			// TODO?
		break;
		default:
			break;
        }
    }
		
	return outputColor;
}

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
            dist > currentLight.range) continue;

        // Calculate point to camera vector.
        const float3 pointToCamera = normalize(CameraPosition.xyz - surfacePosition.xyz);
    	    	
        switch (currentLight.type)
        {
		case POINT_LIGHT:
            outputColor += PointLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor);
		break;
		case DIRECTIONAL_LIGHT:
            float4 positionL = mul(surfacePosition, LightProjectionMatrix);
            if(length(positionL) > surfaceDepth)
                outputColor += DirectionalLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor);
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

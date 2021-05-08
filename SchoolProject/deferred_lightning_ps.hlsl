Texture2D GPositionTexture  : register(t0);
Texture2D GNormalTexture    : register(t1);
Texture2D GDiffuseTexture   : register(t2);

Texture2D EmissiveTexture   : register(t4);

SamplerState PointSampler   : register(s0);


// GLOBALS
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2
#define NUMBER_OF_LIGHTS 1

uniform bool bToonShading = false;
uniform float numToonLevels = 3.0;

//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer PerFrame : register(b1)
{
    row_major matrix ProjectionMatrix;
    row_major matrix ViewMatrix;
    float4 CameraPosition;
    float4 MousePosition;
};





//--------------------------------------------------------------------------------------
// Lights.
//--------------------------------------------------------------------------------------
struct Light
{  
    float4 position;        // 16 bytes
    float4 direction;       // 16 bytes
    float4 color;           // 16 bytes
    float specularPower;    // 4 bytes
    float shininess;        // 4 bytes
    float intensity;        // 4 bytes
    float range;            // 4 bytes
    bool enabled;           // 4 bytes
    uint type;              // 4 bytes
};                          // Total: 72 bytes.

StructuredBuffer<Light> SceneLights : register(t3);




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
// 
//--------------------------------------------------------------------------------------
struct PixelOutputType
{
    float4 color            : SV_TARGET;
    float4 brightnessColor  : SV_Target1;
};




//--------------------------------------------------------------------------------------
// Helper functions for light calculations.
//--------------------------------------------------------------------------------------
float ComputeAttenuation(float dist, float maxRange)
{
    return 1.0f - smoothstep(maxRange * 0.75f, maxRange, dist); // Smoothstep() returns 0 when the distance to the light is less than 3/4.
}

float4 ComputeDiffuse(Light light, float3 toLight, float3 N)
{
    const float diffuseFactor = saturate(dot(N, toLight));
    return light.color * diffuseFactor; 
}

float4 ComputeSpecular(Light light, float3 toEye, float3 toLight, float3 N)
{
    const float3 reflection = reflect(-toLight, N);
    float specularFactor = saturate(dot(toEye, reflection));
    if (light.shininess > 1.0f)
        specularFactor = pow(specularFactor, light.shininess);
    return light.color * specularFactor * light.specularPower;
}

float4 ComputeToonDiffuse(Light light, float3 toLight, float3 N)
{
    float diffuseFactor = saturate(dot(N, toLight));
    const float atLevel = floor(diffuseFactor * numToonLevels);
    diffuseFactor = atLevel / numToonLevels;
    return light.color * diffuseFactor;
}

float4 ComputeToonSpecular(Light light, float3 toEye, float3 toLight, float3 N)
{
    const float3 reflection = reflect(-toLight, N);
    float specularFactor = saturate(dot(toEye, reflection));
    const float atLevel = floor(specularFactor * numToonLevels);
    specularFactor = atLevel / numToonLevels;	
    if (light.shininess > 1.0f)
        specularFactor = pow(specularFactor, light.shininess);
    return light.color * specularFactor * light.specularPower;
}

float4 ComputeBrightnessColor(float4 fragmentColor)
{
    float4 brightnessColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    const float brightness = dot(fragmentColor.rgb, float3(0.2126, 0.7152, 0.0722));
    [flatten]
    if (brightness > 0.5)
        brightnessColor = float4(fragmentColor.rgb, 1.0);
    return brightnessColor;
}




//--------------------------------------------------------------------------------------
// Point Light Calculation.
//--------------------------------------------------------------------------------------
float4 PointLight(Light currentLight, float3 pointToLight, float3 pointToCamera, float3 surfaceNormal, float4 surfaceColor, float4 emissiveValue)
{
	// Initialize outputs.
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Calculate distance to the light source.
    const float dist = length(pointToLight);
    pointToLight /= dist; // Normalize.

	// Get attenuation.
    const float attenuation = ComputeAttenuation(dist, currentLight.range);

	
    // Compute Diffuse Term:
    if (bToonShading)
        diffuse = ComputeToonDiffuse(currentLight, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;
    else
        diffuse = ComputeDiffuse(currentLight, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;

	
    // Compute Specular Term:
    [flatten]
    if (length(diffuse) > 0.0f) 
    {
        if (bToonShading)
            specular = ComputeToonSpecular(currentLight, pointToCamera, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;
		else
			specular = ComputeSpecular(currentLight, pointToCamera, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;
    }


    if (emissiveValue.r > 0.1)
        diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
    return (diffuse + specular) * surfaceColor;
}

//--------------------------------------------------------------------------------------
// Spot Light Calculation.
//--------------------------------------------------------------------------------------
void SpotLight(Light light, float3 toEye, float3 toLight, float3 N)
{
	// TODO:
}

//--------------------------------------------------------------------------------------
// Directional Light Calculation.
//--------------------------------------------------------------------------------------
float4 DirectionalLight(Light currentLight, float3 pointToLight, float3 pointToCamera, float3 surfaceNormal, float4 surfaceColor, float4 emissiveValue)
{
    // Initialize outputs.
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
    if (bToonShading)
    {
        diffuse = ComputeToonDiffuse(currentLight, pointToLight, surfaceNormal) * currentLight.intensity;
        specular = ComputeToonSpecular(currentLight, pointToCamera, pointToLight, surfaceNormal) * currentLight.intensity;
    }
	else
    {
        diffuse = ComputeDiffuse(currentLight, pointToLight, surfaceNormal) * currentLight.intensity;
        specular = ComputeSpecular(currentLight, pointToCamera, pointToLight, surfaceNormal) * currentLight.intensity;	
    }
	
    return (diffuse + specular) * surfaceColor;
}









//--------------------------------------------------------------------------------------
// MAIN
//--------------------------------------------------------------------------------------
[earlydepthstencil]
PixelOutputType main(PixelInputType input)
{
    // Initialize output.
    PixelOutputType output;
	output.brightnessColor  = float4(0.0f, 0.0f, 0.0f, 1.0f);


	// Sample the color / normal / position (in world space) from respective render texture.
	const float4 surfaceColor       = GDiffuseTexture.Sample(PointSampler, input.texCoord.xy);
    float3 surfaceNormal            = GNormalTexture.Sample(PointSampler, input.texCoord.xy).xyz;
    const float3 surfacePosition    = GPositionTexture.Sample(PointSampler, input.texCoord.xy).xyz;
    const float4 emissiveValue      = EmissiveTexture.Sample(PointSampler, input.texCoord.xy);             
	
    // If surfaceNormal is close to (0, 0, 0), render without lighting
	// Mainly used for drawing BoxColliders.
    if (length(surfaceNormal) < 0.01f)
    {
        output.color = surfaceColor;
        return output;
    }
	
    // The vertex's normal vector is being interpolated across the primitive
    // which can make it un-normalized. So normalize the vertex's normal vector.
    surfaceNormal = normalize(surfaceNormal);


    // Calculate Ambient Term:  
    const float4 ambient = float4(0.2f, 0.2f, 0.2f, 1.0f);
    output.color = ambient * surfaceColor;
	
    //
    // FOR EACH LIGHT:
    //
    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {

        // Get current light.
        const Light currentLight = SceneLights[i];
        
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
            output.color += PointLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor, emissiveValue);
		break;
		case DIRECTIONAL_LIGHT:
            output.color += DirectionalLight(currentLight, pointToLight, pointToCamera, surfaceNormal, surfaceColor, emissiveValue);
		break;
		case SPOT_LIGHT:
			// TODO?
		break;
		default:
			break;
        }
    }
	
	// Calculate and store brightness color.
    output.brightnessColor = ComputeBrightnessColor(output.color);
	
	return output;
}

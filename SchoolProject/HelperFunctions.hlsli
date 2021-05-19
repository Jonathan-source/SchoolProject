
//--------------------------------------------------------------------------------------
// Light Data Structure.
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
    uint enabled;           // 4 bytes
    uint type;              // 4 bytes
};                          // Total: 72 bytes.






//--------------------------------------------------------------------------------------
// Helper functions for light calculations.
//--------------------------------------------------------------------------------------
float ComputeAttenuation(float dist, float maxRange)
{
    return 1.0f - smoothstep(maxRange * 0.75f, maxRange, dist); // Smoothstep() returns 0 when the distance to the light is less than 3/4.
}

float4 ComputeDiffuse(float4 fragmentColor, float3 toLight, float3 N)
{
    const float diffuseFactor = saturate(dot(N, toLight));
    return fragmentColor * diffuseFactor;
}

float4 ComputeSpecular(Light light, float3 toEye, float3 toLight, float3 N)
{
    const float3 reflection = reflect(-toLight, N);
    float specularFactor = saturate(dot(toEye, reflection));
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
float4 PointLight(Light currentLight, float3 pointToLight, float3 pointToCamera, float3 surfaceNormal, float4 surfaceColor)
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
	diffuse = ComputeDiffuse(currentLight.color, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;

    // Compute Specular Term:
    [flatten]
    if (length(diffuse) > 0.0f)
		specular = ComputeSpecular(currentLight,pointToCamera, pointToLight, surfaceNormal) * attenuation * currentLight.intensity;
	
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
float4 DirectionalLight(Light currentLight, float3 pointToLight, float3 pointToCamera, float3 surfaceNormal, float4 surfaceColor)
{
    // Initialize outputs.
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 1.0f);

    diffuse = ComputeDiffuse(currentLight.color, pointToLight, surfaceNormal) * currentLight.intensity;
    specular = ComputeSpecular(currentLight, pointToCamera, pointToLight, surfaceNormal) * currentLight.intensity;
    
    return (diffuse + specular) * surfaceColor;
}
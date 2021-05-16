Texture2D DiffuseTexture        : TEXTURE : register(t0);
Texture2D NormalTexture         : TEXTURE : register(t1);
Texture2D EmissiveTexture       : TEXTURE : register(t2);

SamplerState LinearSampler      : SAMPLER : register(s0);


//Texture2D SpecularTexture       : register(t3);


//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct PixelShaderInput
{   
    float4 positionCS   : SV_POSITION;      // 16 bytes
    float2 texCoord     : TEXCOORD;         // 8 bytes
    float4 normalWS     : NORMALWS;         // 16 bytes
    float4 positionWS   : POSITIONWS;       // 16 bytes
    float4 tangentWS    : TANGENTWS;        // 16 bytes
    float4 bitangentWS  : BITANGENTWS;      // 16 bytes
};                                          // Total: 88 bytes.


//--------------------------------------------------------------------------------------
// Used to store the final result of the lighting pass.
//--------------------------------------------------------------------------------------
struct PixelShaderOutput
{
    float4 position     : SV_Target0;   
    float4 normal       : SV_Target1;
    float4 diffuse      : SV_Target2;
};


//--------------------------------------------------------------------------------------
// Normal mapping.
//--------------------------------------------------------------------------------------
float4 normalMapping(PixelShaderInput input)
{   
    // Normalize the TBN after interpolation.
    const float3x3 TBN = float3x3(  normalize(input.tangentWS.xyz),
                                    normalize(input.bitangentWS.xyz),
                                    normalize(input.normalWS.xyz));
    
    // Sample the tangent-space normal map in range [0,1] and decompress. 
    // Tangent and binormal (UV) are used as direction vectors.
    float3 normalTS = NormalTexture.Sample(LinearSampler, input.texCoord.xy).rgb;
    //normalTS.xyz = normalTS.xzy;
    //normalTS.y = -normalTS.y;
    // Change normal map range from [0, 1] to [-1, 1].
    normalTS = normalize(normalTS * 2.0f - 1.0f);
    
    // Transform normal from local tangent space to world space.
    float3 normalWS = mul(normalTS, TBN);

    return normalize(float4(normalWS, 0.0f));
}




//--------------------------------------------------------------------------------------
// G-Buffer pixel shader.
//--------------------------------------------------------------------------------------
[earlydepthstencil]
PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    // Interpolating normal may cause unnormalize.
    input.normalWS = normalize(input.normalWS);
    
    // Sample from DiffuseTexture map.
    output.diffuse = float4(1.0f, 1.0f, 0.0f, 1.0f); //DiffuseTexture.Sample(LinearSampler, input.texCoord);

    // Sample from EmissiveTexture map.
    // output.emissive = EmissiveTexture.Sample(LinearSampler, input.texCoord);
	
    // Sample from NormalTexture map.	
    output.normal = input.normalWS; 
    output.normal.a = output.diffuse.a; // Because the normal-"Color" needs to blend just like diffuse

    output.position = input.positionWS;
    output.position.a = output.diffuse.a; // Because the position-"Color" needs to blend just like diffuse

    return output;
}

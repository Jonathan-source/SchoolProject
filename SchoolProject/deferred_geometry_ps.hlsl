Texture2D DiffuseTexture        : TEXTURE : register(t0);
Texture2D NormalTexture         : TEXTURE : register(t1);

SamplerState LinearSampler      : SAMPLER : register(s0);


//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
cbuffer Material : register(b0)
{
    float4 Ka;                  // 16 bytes
    float4 Kd;                  // 16 bytes
    float4 Ke;                  // 16 bytes
    float4 Ks;                  // 16 bytes
    int HasAmbientMap;          // 4 bytes
    int HasDiffuseMap;          // 4 bytes
    int HasEmissiveMap;         // 4 bytes
    int HasSpecularMap;         // 4 bytes
    int HasNormalMap;           // 4 bytes
};                              // Total: 84 bytes.


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
// 
//--------------------------------------------------------------------------------------
struct PixelShaderOutput
{
    float4 position     : SV_Target0;   
    float4 normal       : SV_Target1;
    float4 diffuse      : SV_Target2;
};


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
float4 DoNormalMapping(PixelShaderInput input)
{   
    // Normalize the TBN after interpolation.
    const float3x3 TBN = float3x3(  normalize(input.tangentWS.xyz),
                                    normalize(input.bitangentWS.xyz),
                                    normalize(input.normalWS.xyz));
    
    // Sample the tangent-space normal map in range [0,1] and decompress. 
    // Tangent and binormal (UV) are used as direction vectors.
    float3 normalTS = NormalTexture.Sample(LinearSampler, input.texCoord.xy).rgb;
    
    // Change normal map range from [0, 1] to [-1, 1].
    normalTS = normalize(normalTS * 2.0f - 1.0f);
    
    // Transform normal from local tangent space to world space.
    float3 normalWS = mul(normalTS, TBN);

    return normalize(float4(normalWS, 0.0f));
}




//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
[earlydepthstencil]
PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    // Interpolating normal may cause unnormalize.
    input.normalWS = normalize(input.normalWS);
    
    output.diffuse = Kd;
    if(HasDiffuseMap)   // Sample from DiffuseTexture map.
        output.diffuse = DiffuseTexture.Sample(LinearSampler, input.texCoord);
	
    output.normal = input.normalWS;
    if(HasNormalMap)    // Sample from NormalTexture map.	
        output.normal = DoNormalMapping(input);
        
    output.position = input.positionWS;

    return output;
}

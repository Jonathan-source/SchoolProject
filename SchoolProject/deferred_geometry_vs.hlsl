
//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer PerObject : register(b0)
{
    row_major matrix WorldInvTransposeMatrix;   // 64 bytes
    row_major matrix WorldMatrix;               // 64 bytes
};                                              // Total: 128 bytes.


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer PerFrame : register(b1)
{
    row_major matrix ProjectionMatrix;      // 64 bytes
    row_major matrix ViewMatrix;            // 64 bytes
    float4 CameraPosition;                  // 16 bytes
    float4 MousePosition;                   // 16 bytes
};                                          // Total: 160 bytes.




//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct VertexShaderInput
{
    float4 position : POSITION;     // 12 bytes
    float4 normal   : NORMAL;       // 12 bytes
    float2 texCoord : TEXCOORD0;    // 8 bytes
    float4 tangent  : TANGENT;      // 12 bytes
};                                  // Total: 44 bytes.




//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct VertexShaderOutput
{
    float4 positionCS   : SV_POSITION;      // 16 bytes
    float2 texCoord     : TEXCOORD;         // 8 bytes
    float4 normalWS     : NORMALWS;         // 12 bytes
    float4 positionWS   : POSITIONWS;       // 12 bytes
    float4 tangentWS    : TANGENTWS;        // 12 bytes
    float4 bitangentWS  : BITANGENTWS;      // 12 bytes
};                                          // Total: 72 bytes.




//--------------------------------------------------------------------------------------
// G-Buffer vertex shader.
//--------------------------------------------------------------------------------------
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // Convert position and normal to world space.
    output.positionWS   = mul(input.position, WorldMatrix);
    float4 normalWS     = mul(input.normal, WorldMatrix);
    
    // Invert the effects of translated normals.
    normalWS            = mul(input.normal, WorldInvTransposeMatrix);
    output.normalWS     = normalize(normalWS);
    
    // Reconstruct the rest of the tangent frame.
    float4 tangentWS    = normalize(mul(input.tangent, WorldMatrix));
    
    // Re-orthogonalize tangent with respect to normal.
    tangentWS           = normalize(tangentWS - dot(tangentWS, normalWS) * normalWS);
    float4 bitangentWS  = float4(normalize(cross(normalWS.xyz, tangentWS.xyz)), 0.0f);
    
    output.tangentWS    = tangentWS;
    output.bitangentWS  = bitangentWS;
    
    // Calculate the view-space and clip-space position.
    float4 positionVS   = mul(output.positionWS, ViewMatrix);
    output.positionCS   = mul(positionVS, ProjectionMatrix);
    
    output.texCoord     = input.texCoord;
    
    return output;
}

//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct VertexInputType
{
    float3 position : POSITION;     // 12 bytes
    float3 normal   : NORMAL;       // 12 bytes
    float2 texCoord : TEXCOORD0;    // 8 bytes
};                                  // Total: 32 bytes.




//--------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------
struct PixelInputType
{
    float4 position : SV_POSITION;  // 16 bytes
    float4 normal   : NORMAL;       // 16 bytes
    float2 texCoord : TEXCOORD0;    // 8 bytes
};                                  // Total 40 bytes.




//--------------------------------------------------------------------------------------
// Light vertex shader.
//--------------------------------------------------------------------------------------
PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    output.position = float4(input.position, 1.0f);  
    output.normal   = float4(input.normal, 0.0f);
    output.texCoord = input.texCoord;
   
    return output;
}
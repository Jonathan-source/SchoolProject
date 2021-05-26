//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer DepthMatrixBuffer : register(b0)
{
    row_major matrix LightViewProjectionMatrix;
};                                            


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
cbuffer PerObject : register(b1)
{
    row_major matrix WorldInvTransposeMatrix;   
    row_major matrix WorldMatrix;               
};


//--------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------
float4 main(float4 position : POSITION) : SV_POSITION
{
    position.w = 1.0f;
    // Convert position and normal to world space.
    position = mul(position, WorldMatrix);
    return mul(position, LightViewProjectionMatrix);
}
struct GSOutput
{
    float4 positionCS : SV_POSITION; // 16 bytes
    float2 texCoord : TEXCOORD; // 8 bytes
    float3 normalWS : NORMALWS; // 12 bytes
    float3 positionWS : POSITIONWS; // 12 bytes
    float3 tangentWS : TANGENTWS; // 12 bytes
    float3 bitangentWS : BITANGENTWS;
};

//--------------------------------------------------------------------------------------
// Per Frame.
//--------------------------------------------------------------------------------------
cbuffer MouseParams : register(b0)
{
    float4 mousePosition;
}

[maxvertexcount(3)]
void main(
	triangle float4 positionCS[3] : SV_POSITION,
    triangle float2 texCoord[3] : TEXCOORD,
    triangle float3 normalWS[3] : NORMALWS,
    triangle float3 positionWS[3] : POSITIONWS,
    triangle float3 tangentWS[3] : TANGENTWS,
    triangle float3 bitangentWS[3] : BITANGENTWS,
	inout TriangleStream<GSOutput> output
)
{
    /*
        with the vertices in clipspace coords, form vertices v = (vx, vy, vw) in clip
        space and compute the determinant d = |v0, v1, v2| [1317]. If d ? 0, the triangle can
        be culled.
    */
    
    // Matrix made from triangle vertices in Clip space,
    float3x3 vertices =
    {
        float3(positionCS[0].xyz),
        float3(positionCS[1].xyz),
        float3(positionCS[2].xyz)
    };
    
    
    float d = determinant(vertices);

    //If d ? 0, the triangle can be culled
    if (d <= 0.001f)
    {
        for (uint i = 0; i < 3; i++)
        {
            GSOutput element;
            element.positionCS = positionCS[i];
            element.normalWS = normalWS[i];
            element.texCoord = texCoord[i];
            element.positionWS = positionWS[i];
            element.tangentWS = tangentWS[i];
            element.bitangentWS = bitangentWS[i];
            output.Append(element);
        }
    }

    

}
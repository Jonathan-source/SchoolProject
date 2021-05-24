// The output UAV used by the CS, in this case the backbuffer. 
// Float4 is specified in swap chain description.
RWTexture2D<unorm float4> backBuffer : register(u0); 

// Group size may not exceed 1024.
#define SIZE_X 8
#define SIZE_Y 8
#define KERNAL_SIZE 7


//-----------------------------------------------------------
// KERNEL_RANGE is this: center + half width of the kernel
//
// consider a blur kernel 7x7 - '*' indicates the center of the kernel
//
// x   x   x   x   x    x   x    
// x   x   x   x   x    x   x 
// x   x   x   x   x    x   x     
// x   x   x   x*  x    x   x
// x   x   x   x   x    x   x
// x   x   x   x   x    x   x
// x   x   x   x   x    x   x
//
// separate into 1D kernels
//
// x   x   x   x*  x    x   x
//              ^-----------^
//              KERNEL_RANGE




//--------------------------------------------------------------------------------------
// The weights are calculated by numerical integration of the continuous 
// gaussian distribution over each discrete kernel tap, and will be used directly
// in a single pass blur algorithm: n^2 samples per pixel.
// Sigma: 2.4   Kernal Size: 7x7
// Source: http://dev.theomader.com/gaussian-kernel-calculator/
//--------------------------------------------------------------------------------------
static const float kernal_coefficients[KERNAL_SIZE][KERNAL_SIZE] =
{
    0.007982, 0.012243, 0.015826, 0.017239, 0.015826, 0.012243, 0.007982,
    0.012243, 0.01878, 0.024275, 0.026443, 0.024275, 0.01878, 0.012243,
    0.015826, 0.024275, 0.031379, 0.034181, 0.031379, 0.024275, 0.015826,
    0.017239, 0.026443, 0.034181, 0.037234, 0.034181, 0.026443, 0.017239,
    0.015826, 0.024275, 0.031379, 0.034181, 0.031379, 0.024275, 0.015826,
    0.012243, 0.01878, 0.024275, 0.026443, 0.024275, 0.01878, 0.012243,
    0.007982, 0.012243, 0.015826, 0.017239, 0.015826, 0.012243, 0.007982
};





//--------------------------------------------------------------------------------------
// The brute force approach to implementing the bilateral filter.
//--------------------------------------------------------------------------------------
[numthreads(SIZE_X, SIZE_Y, 1)]
void main(int3 GroupID             : SV_GroupID,               // To which group it belongs in 3D.
          int3 DispatchThreadID    : SV_DispatchThreadID,      // What is the thread id in the whole dispatch.
          int3 GroupThreadID       : SV_GroupThreadID,         // What is the thread id inside the group.
          int GroupIndex           : SV_GroupIndex)            // "Flattened" 1D version of thread id inside the group.
{
    // Offset the texture location to the first sample location
    int3 textureCoords = DispatchThreadID - int3(3, 3, 0);

    // Each thread will load its own depth/occlusion values
    float4 centerColor = backBuffer.Load(DispatchThreadID);

    const float rsigma = 0.051f;

    // Range sigma value const float rsigma = 0.051f;
    float4 outputColor = 0.0f;
    float4 weight = 0.0f;


    for (int x = 0; x < KERNAL_SIZE; x++)
    {
        for (int y = 0; y < KERNAL_SIZE; y++)
        {
            // Get the current sample 
            float4 sampleColor = backBuffer.Load(textureCoords + int3(x, y, 0));
            float4 delta = centerColor - sampleColor;
            float4 range = exp((-1.0f * delta * delta) / (2.0f * rsigma * rsigma));

            // Sum both the color result and the total weighting used 
            outputColor += sampleColor * range * kernal_coefficients[x][y];
            weight += range * kernal_coefficients[x][y];
        }
    }

    // Output to chosen UAV.
    backBuffer[DispatchThreadID.xy] = outputColor / weight;
}
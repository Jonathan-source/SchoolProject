struct PixelShaderInput
{
	float4 position : SV_POSITION;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
	return(0.0f, 0.0f, 0.0f, 0.0f);
	// if discard is called, depth buffer won't have any data.
}
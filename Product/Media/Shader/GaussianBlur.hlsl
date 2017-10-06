/* Gaussian bloom, requires offsets and weights to be provided externally
*/
float4 bloom(
		float2 uv : TEXCOORD0,
		uniform float2 sampleOffsets[15],
		uniform float4 sampleWeights[15],	
		uniform sampler2D inRTT : register(s0)
		) : COLOR
{
    float4 accum = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float2 sampleUV;
    
    for( int i = 0; i < 15; i++ )
    {
        // Sample from adjacent points, 7 each side and central
        sampleUV = uv + sampleOffsets[i];
        accum += sampleWeights[i] * tex2D(inRTT, sampleUV);
    }
    
    return accum;
	
}
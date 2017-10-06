float4x4 	worldViewproj;
float4 		colour;
float   	lineWidth;

struct VS_INPUT
{
    vector position    : POSITION;
    vector normal      : NORMAL0;
};

struct VS_OUTPUT
{
    vector position : POSITION;
    vector diffuse  : COLOR;
};


VS_OUTPUT Main(VS_INPUT input)
{
    VS_OUTPUT output 	= 	(VS_OUTPUT)0;
	input.normal.w   	= 	0.0f;
    input.position 		+= 	(lineWidth * input.normal);

	output.position 	= 	mul(worldViewproj, input.position);
    output.diffuse 		= 	colour;
    
    return output;
}



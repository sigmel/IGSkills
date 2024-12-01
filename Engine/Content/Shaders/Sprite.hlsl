struct VertexShaderInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertexShaderOutput
{
    float4 color : COLOR;
    float4 position : SV_Position;
};

struct PixelShaderInput
{
    float4 color : COLOR;
};

VertexShaderOutput VSMain(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;
    
    output.position = float4(input.position, 1);
    output.color = input.color;
    
    return output;
}

float4 PSMain(PixelShaderInput input) : SV_Target
{
    return input.color;
}
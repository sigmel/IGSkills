struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : UV;
};

struct VertexShaderOutput
{
    float2 uv : TEXCOORD0;
    float4 position : SV_Position;
};

struct PixelShaderInput
{
    float2 uv : TEXCOORD0;
};

Texture2D texture0 : register(t0);
SamplerState samplerState : register(s0);

VertexShaderOutput VSMain(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput)0;
    
    output.position = float4(input.position, 1);
    output.uv = input.uv;
    
    return output;
}

float4 PSMain(PixelShaderInput input) : SV_Target
{
    return texture0.Sample(samplerState, input.uv);
}
cbuffer PerObject : register(b0)
{
    float4x4 WorldViewProj;
}

struct VSInput {
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 posH : SV_Position;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(VSInput IN)
{
    VSOutput O;
    
    O.posH = mul(float4(IN.pos, 1), WorldViewProj);
    O.uv = IN.uv;
    
    return O;
}

Texture2D tex0 : register(t0);
SamplerState samp0 : register(s0);

float4 PSMain(VSOutput IN) : SV_Target
{
    return float4(1,1, 1, 1);
}

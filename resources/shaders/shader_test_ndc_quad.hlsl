cbuffer MatrixView : register(b0)
{
    float4x4 WorldViewProj;
    float3 PosView;
    float paddingPos;
    float3 FrontView;
    float paddingFront;
    float3 UpView;
    float paddingView;
    float3 RightView;
    float paddingRight;

}

cbuffer Utils : register(b1)
{
    float time;
    uint quads_row;
    uint count_lights;
    float padding;
}

struct VSOutput
{
    float4 pos    : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};


VSOutput VSMain(float3 pos : POSITION, float2 texcoord: TEXCOORD)
{
    VSOutput O;
    
    O.texCoord = texcoord;
    O.pos = mul(WorldViewProj, float4(pos, 1.f));
    
    return O;
}

float4 PSMain(VSOutput IN) : SV_Target
{
    return float4(float3(IN.texCoord,0.f), 1.f);

}




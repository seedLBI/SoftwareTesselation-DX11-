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

struct LightObject
{
    float4 pos;
    float4 color;
};

cbuffer Light : register(b2)
{
    LightObject lights[16];
}

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 size : TEXCOORD0;
    float3 color : TEXCOORD1;
};


VSOutput VSMain(uint i_id : SV_InstanceID)
{
    
    VSOutput O;
    O.pos = float4(lights[i_id].pos.xyz,1);
    O.size = float2((length(lights[i_id].color.xyz) / 100.f).xx);
    O.color = lights[i_id].color.xyz;

    
    
    return O;
}


struct PSInput
{
    float4 pos   : SV_POSITION;
    float2 uv    : TEXCOORD0;
    float3 color : TEXCOORD1;
};

[maxvertexcount(6)]
void GSMain(point VSOutput input[1], inout TriangleStream<PSInput> triStream)
{
    float halfW = input[0].size.x * 0.5f;
    float halfH = input[0].size.y * 0.5f;

    float3 R = normalize(RightView) * halfW;
    float3 U = normalize(UpView) * halfH;

    float3 center = input[0].pos;

    float3 pTL = center + (-R + U); // top-left
    float3 pBL = center + (-R - U); // bottom-left
    float3 pTR = center + (R + U);  // top-right
    float3 pBR = center + (R - U);  // bottom-right

    
    PSInput v;

    v.color = input[0].color;
    
    // 1
    v.pos = mul(float4(pTL, 1.0f), WorldViewProj);
    v.uv = float2(0.0f, 0.0f);
    triStream.Append(v);
    v.pos = mul(float4(pBL, 1.0f), WorldViewProj);
    v.uv = float2(0.0f, 1.0f);
    triStream.Append(v);
    v.pos = mul(float4(pTR, 1.0f), WorldViewProj);
    v.uv = float2(1.0f, 0.0f);
    triStream.Append(v);

    // 2
    v.pos = mul(float4(pTR, 1.0f), WorldViewProj);
    v.uv = float2(1.0f, 0.0f);
    triStream.Append(v);
    v.pos = mul(float4(pBL, 1.0f), WorldViewProj);
    v.uv = float2(0.0f, 1.0f);
    triStream.Append(v);
    v.pos = mul(float4(pBR, 1.0f), WorldViewProj);
    v.uv = float2(1.0f, 1.0f);
    triStream.Append(v);

    
    triStream.RestartStrip();
}


float simpleDither(float2 pos)
{
    return frac(52.9819189 * frac(dot(pos, float2(0.06711056, 0.00583715)))) - 0.5;
}

float4 PSMain(PSInput IN) : SV_Target
{
    float2 center = float2(0.5, 0.5);
    const float maxDistToCorner = 0.5;

    float d = length(IN.uv - center) / maxDistToCorner;
    d = saturate(d);

    float alpha = 1.0 - d;
    
    
    float3 color = IN.color;
    
    

    
    
    // HDR tonemapping
    //color = color / (color + float3(1.0f.xxx));
    
    

    
    
    
    
    // gamma correct
    //float3 gamma = float3((1.0f / 2.2f).xxx);
    //color = pow(color, gamma);
    
    
    float dither = simpleDither(IN.pos.xy) * 0.1;
    //color = saturate(color + dither);
    alpha = saturate(alpha + dither * 0.5);
    
    
   
    return float4(color, alpha*alpha);

}




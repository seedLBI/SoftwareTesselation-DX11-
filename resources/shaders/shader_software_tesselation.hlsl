cbuffer MatrixView : register(b0)
{
    float4x4 WorldViewProj;
}

cbuffer Indexes : register(b1)
{
    uint   PointsPerRow;
    float  Padding0;
    float2 Padding1;
}

cbuffer Time : register(b2)
{
    float time;
}


struct VSOutput
{
    float4 posH : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};


float hash(float n)
{
    return frac(sin(n) * 43758.5453123);
}

float4 randomColor(uint id)
{
    return float4(hash(float(id + 0)), hash(float(id + 1)), hash(float(id + 2)), 1.f);
}


struct PointNormal
{
    float3 pos : POSITION0;
    float3 normal : NORMAL0;
};


PointNormal uv_to_rectangle(float u, float v)
{
    PointNormal pn;
    pn.pos = float3(-(v * 2.f - 1.f), 0.f, -(u * 2.f - 1.f));
    pn.normal = float3(0.f, 1.f, 0.f);
    return pn;
}
PointNormal uv_to_circle(float u, float v)
{
    float r = 1.f;
    float PI = 3.14159265359;
    
    float x = v * cos(2.f * PI * u);
    float y = 0.f;
    float z = v * sin(2.f * PI * u);
    
    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = float3(0.f, 1.f, 0.f);
    
    return pn;
}
PointNormal uv_to_cylinder(float u, float v)
{
    float r = 1.f;
    float PI = 3.14159265359;
    
    float x = r * cos(2.f * PI * u);
    float y = v * 2.f - 1.f;
    float z = r * sin(2.f * PI * u);
    
    
    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = float3(cos(2.f * PI * u), 0.f, sin(2.f * PI * u));
    
    return pn;
}
PointNormal uv_to_sphere(float u, float v)
{
    float r = 1.f;
    float PI = 3.14159265359;
    float PI_half = PI * 0.5f;
    
    float v_minus_one_to_one = v * 2.f - 1.f;
    
    
    float ur = 2.f * PI * u;
    float vr = v_minus_one_to_one * PI_half;

    float x = r * cos(ur) * cos(vr);
    float y = r * sin(vr);
    float z = r * sin(ur) * cos(vr);

    
    
    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = float3(x,y,z)/r;
    
    return pn;
}
PointNormal uv_to_torus(float u, float v)
{
    float r = 0.5f;   
    float R = 1.f;
    
    float PI = 3.14159265359;
    float PI_half = PI * 0.5f;
    
    float v_minus_one_to_one = v * 2.f - 1.f;
    
    
    float ur = 2.f * PI * u;
    float vr = 2.f * PI * v;

    float x = cos(ur) * (R + r * sin(vr));
    float y =                r * cos(vr);
    float z = sin(ur) * (R + r * sin(vr));

    
    
    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = float3(cos(ur) * sin(vr), cos(vr), sin(ur) * sin(vr));
    
    return pn;
}
PointNormal uv_to_torus_knot(float u, float v)
{
    float r = 0.25f;
    float R = 1.f;
    
    float p = 3.f;
    float q = 2.f;
    
    float PI = 3.14159265359;
    float PI_half = PI * 0.5f;
    
    float v_minus_one_to_one = v * 2.f - 1.f;
    
    
    float ur = 2.f * PI * u;
    float vr = 2.f * PI * v;

    float x = (R + r * cos(vr)) * cos(ur);
    float y = r * sin(vr);
    float z = (R + r * cos(vr)) * sin(ur);

    
    x += (2.f + cos(q * ur)) * cos(p * ur);
    z += (2.f + cos(q * ur)) * sin(p * ur);
    y += 2.f * (-sin(q * ur));
    
    
    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = float3(x, y, z);
    
    return pn;
}



PointNormal lerp(PointNormal a, PointNormal b, float s)
{
    PointNormal pn;
    pn.pos = lerp(a.pos, b.pos, s);
    pn.normal = lerp(a.normal, b.normal, s);
    return pn;
}

PointNormal pickPN(float u, float v, int i)
{
    if (i == 0)
        return uv_to_circle(u,v);
    if (i == 1)
        return uv_to_cylinder(u,v);
    if (i == 2)
        return uv_to_sphere(u,v);
    
    return uv_to_torus(u,v);
}

VSOutput VSMain(uint vid : SV_VertexID)
{
    VSOutput O;
    
    uint N = max(2, PointsPerRow);
    
    uint x = vid % N;
    uint y = vid / N;
    
    float u = x / float(N - 1);
    float v = y / float(N - 1); 
    
    float timee = time * 0.25;
    float phase = frac(timee);

    const int NN = 4;
    float t_total = phase * float(NN);
    int idx = int(floor(t_total));
    float local = frac(t_total);

    float s = smoothstep(0.0, 1.0, local);

    PointNormal a = pickPN(u,v,idx);
    PointNormal b = pickPN(u,v,(idx + 1) % NN);

    PointNormal outt = lerp(a, b, s);
    
    
    O.posH = mul(float4(outt.pos, 1), WorldViewProj);
    O.uv = float2(u,v);
    O.normal = outt.normal;

    return O;
}

float4 PSMain(VSOutput IN, uint pid : SV_PrimitiveId) : SV_Target
{
    
    uint2 coords = (uint2) (IN.uv * 256.0);
    uint val1 = coords.x ^ coords.y;
    
    float xor_pattern = float(val1 % 256) / 255.f;

    float4 xor_color = float4(1.f - xor_pattern, xor_pattern, float(val1 % 128) / 127.f, 1.f);
    

    return lerp(xor_color, float4(IN.normal, 1.f), 0.5f);
}
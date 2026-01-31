cbuffer MatrixView : register(b0)
{
    float4x4 WorldViewProj;
    float3 PosView;
    float3 FrontView;
    float3 UpView;
    float3 RightView;
}

cbuffer Utils : register(b1)
{
    float time;
    uint  quads_row;
    uint  count_lights;
    
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


cbuffer ObjectInfo : register(b3)
{
    float3 obj_pos;
    float obj_padding1;
    float3 obj_albedo;
    float  obj_ao;
    float  obj_metallic;
    float  obj_roughness;
    
    float3 obj_padding;
}


Texture2D tex_albedo       : register(t0);
Texture2D tex_ao           : register(t1);
Texture2D tex_displacement : register(t2);
Texture2D tex_metallic     : register(t3);
Texture2D tex_opacity      : register(t4);
Texture2D tex_roughness    : register(t5);
Texture2D tex_normal       : register(t6);

SamplerState Samp : register(s0);


static const float PI = 3.14159265359;

struct PointNormal
{
    float3 pos       : POSITION0;
    float3 normal    : NORMAL0;
    float3 tangent   : NORMAL1;
    float3 bitangent : NORMAL2;
};
PointNormal uv_to_torus(float2 uv)
{
    float r = 0.5f;
    float R = 1.f;

    float ur = 2.f * PI * uv.x;
    float vr = 2.f * PI * uv.y;

    float x = cos(ur) * (R + r * sin(vr));
    float y = r * cos(vr);
    float z = sin(ur) * (R + r * sin(vr));

    PointNormal pn;
    pn.pos = float3(x, y, z);
    pn.normal = normalize(float3(cos(ur) * sin(vr), cos(vr), sin(ur) * sin(vr)));
    float3 dpdu = 2.f * PI * float3(
        -sin(ur) * (R + r * sin(vr)), 
        0.f, 
        cos(ur) * (R + r * sin(vr))
    );
    float3 dpdv = 2.f * PI * float3(
        cos(ur) * r * cos(vr), 
        -r * sin(vr), 
        sin(ur) * r * cos(vr)
    );

    float3 t = normalize(dpdu);
    float3 b = dpdv - dot(dpdv, t) * t;
    b = normalize(b);

    float3 c = cross(t, b);
    if (dot(c, pn.normal) < 0.f)
    {
        b = -b;
    }

    pn.tangent = t;
    pn.bitangent = b;
    
    return pn;
}
PointNormal uv_to_sphere(float2 uv)
{
    float r = 1.f;
    
    PointNormal pn;
    pn.normal    = float3(
        cos(2.f * PI * uv.x) * sin(PI * uv.y),
        -cos(PI * uv.y),
        sin(2.f * PI * uv.x) * sin(PI * uv.y)
    );
    
    pn.tangent   = float3(
        -sin(2.f * PI * uv.x),
        0, 
        cos(2.f * PI * uv.x)
    );
    
    pn.bitangent = float3(
        cos(2 * PI * uv.x) * cos(PI * uv.y),
        sin(PI * uv.y),
        sin(2 * PI * uv.x) * cos(PI * uv.y)
    );
    
    pn.pos = r * pn.normal;

    return pn;
}


float2 getUV_fromGrid(uint quads_per_row, uint v_id, uint i_id)
{
    uint N = max(1, quads_per_row);
    
    uint2 quad_index = uint2(
        i_id % N, // col
        i_id / N  // row
    );
    
    float size_quad = 1.f / float(N);
    
    float2 map_vertex[6] =
    {
        float2(0, 0),
        float2(0, 1),
        float2(1, 1),

        float2(0, 0),
        float2(1, 1),
        float2(1, 0)
    };
    
    float2 uv = quad_index / float(N) + map_vertex[v_id] * size_quad;
    return uv;
}

struct VSOutput
{
    float2 uv        : TEXCOORD0;
    float4 posVertex : SV_Position;
    float3 posWorld  : POSITION0;
    float3 normal    : NORMAL0;
    float3 tangent   : NORMAL1;
    float3 bitangent : NORMAL2;
};

VSOutput VSMain(uint v_id : SV_VertexID, uint i_id : SV_InstanceID)
{
    float2 uv = getUV_fromGrid(quads_row, v_id, i_id);
    
    PointNormal geom = uv_to_sphere(uv);

    geom.pos += obj_pos;
    
    
    //float displacement = tex_displacement.SampleLevel(Samp, uv * 1.f,0).x;
    //displacement = displacement * 2.0 - 1.0;
    //geom.pos += geom.normal * displacement * 0.05f;
        
    
    VSOutput O;
    O.uv        = uv * 1.f;
    O.posVertex = mul(float4(geom.pos, 1), WorldViewProj);
    O.posWorld  = geom.pos;
    O.normal    = geom.normal;
    O.tangent   = geom.tangent;
    O.bitangent = geom.bitangent;

    return O;
}


float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    float t = pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0 - F0) * t;
}

float3 NormalMapping(float3 input_tex_normal, float3 tangent, float3 bitangent, float3 normal)
{
    float3x3 TBN = (float3x3(tangent, bitangent, normal));
    input_tex_normal = input_tex_normal * 2.0 - 1.0;
    input_tex_normal = normalize(mul(input_tex_normal, TBN));
    
    return input_tex_normal;
}


float3 PBR(float3 posWorld, float3 normal, float3 albedo, float ao, float metallic, float roughness)
{
    float3 N = normalize(normal);
    float3 V = normalize(PosView - posWorld);
    
    float3 F0 = float3(0.04f.xxx);
    F0 = lerp(F0, albedo, metallic);
    
    float3 Lo = float3(0.0f.xxx);
    
    for (uint i = 0; i < count_lights; ++i)
    {
        float3 lpwp = lights[i].pos.xyz - posWorld;
        
        float3 L = normalize(lpwp);
        float3 H = normalize(V + L);
        float distance = length(lpwp);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = lights[i].color.rgb * attenuation;
        
        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;
        
        float3 kS = F;
        float3 kD = float3(1.0f.xxx) - kS;
        kD *= 1.0 - metallic;
        
        float NdotL = max(dot(N, L), 0.0);
        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    
    float3 ambient = float3(0.00f.xxx) * albedo * ao;
    float3 color = ambient + Lo;
    
    // HDR tonemapping
    color = color / (color + float3(1.0f.xxx));
    
    // gamma correct
    float3 gammaKoef = float3((1.0f / 2.2f).xxx);
    color = pow(color, gammaKoef);
    
    return color;
}



//
// PIXEL PIXEL PIXEL PIXEL      PIXEL PIXEL PIXEL PIXEL
// PIXEL PIXEL PIXEL       PIXEL      PIXEL PIXEL PIXEL
// PIXEL       PIXEL PIXEL      PIXEL PIXEL PIXEL PIXEL PIXEL
//
float4 PSMain(VSOutput IN) : SV_Target
{
    /*
    float3 albedo       = tex_albedo.Sample(Samp, IN.uv).xyz;
    float3 normal       = tex_normal.Sample(Samp, IN.uv).xyz;
    float  ao           = tex_ao.Sample(Samp, IN.uv).x;
    float  metallic     = tex_metallic.Sample(Samp, IN.uv).x;
    float  opacity      = tex_opacity.Sample(Samp, IN.uv).x;
    float  roughness    = tex_roughness.Sample(Samp, IN.uv).x;

    normal = NormalMapping(normal, IN.tangent, IN.bitangent, IN.normal);
    
    float3 color = PBR(IN.posWorld, normal, albedo, ao, metallic, roughness);
    
    return float4(color, opacity);
    */    
        
    
    
    float3 color = PBR(IN.posWorld, IN.normal, obj_albedo, obj_ao, obj_metallic, obj_roughness);
    return float4(color, 1.f);
      
}
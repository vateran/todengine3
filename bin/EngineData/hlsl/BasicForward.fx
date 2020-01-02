cbuffer TodBaseFrameCBuffer
{
    float4x4 matWVP;    
    float4x4 matWorld;
    float4x4 matInvWorld;
    float4x4 matView;
    float4x4 matInvView;
    float4x4 matProj;
    float3 vecViewPos;
    float fTime;
    uint iLightCount;
    float padding[3];
};

/*struct TodBaseLightCBuffer
{
    float intensity;
    float specularStrength;
    float attenuationLinear;
    float attenuationQuadratic;
};

struct Light
{
    float intensity;
};

cbuffer TodBaseLight
{
    Light lights[8];
};*/

cbuffer BoneTransformCBuffer
{
    float4x4 boneTransform[256];
};

struct NormalVSInput
{
    float3 vecPosition  : POSITION;
    float3 vecNormal    : NORMAL;
    float2 vecUV        : TEXCOORD;
};

struct OutlineVSInput
{
    float3 vecPosition  : POSITION;
    float3 vecNormal    : NORMAL;
};

struct SkinnedVSInput
{
    float3 vecPosition  : POSITION;
    float3 vecNormal    : NORMAL;
    float2 vecUV        : TEXCOORD;    
    uint4  boneIndex    : BLENDINDICES;
    float4 weight       : BLENDWEIGHTS;
};

struct PSInput
{
    float4 vecPosition      : SV_POSITION;
    float3 vecWorldPosition : POSITON;
    float3 vecNormal        : NORMAL;
    float2 vecUV            : TEXCOORD;
};

/*DepthBias = 0,
DepthBiasClamp = 0.0f,
FillMode = FillMode.Wireframe,
IsAntialiasedLineEnabled = false,
IsDepthClipEnabled = false,
IsFrontCounterclockwise = false,
IsMultisampleEnabled = false,
IsScissorEnabled = false,
SlopeScaledDepthBias = 0.0f*/

RasterizerState EnableBackCulling
{
    CullMode = BACK;
    FillMode = SOLID;
};

RasterizerState DisableCulling
{
    CullMode = NONE;
    FillMode = SOLID;
};

RasterizerState WireFrameMode
{
    CullMode = BACK;
    FillMode = WIREFRAME;
};

Texture2D texDiffuse;
Texture2D texSpecular;
Texture2D texNormal;
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    MaxLod = 3.402823466e+38f;
    ComparisonFunc = Less;
};

BlendState SrcAlphaBlendingAdd
{
    BlendEnable[0] = TRUE;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp[0] = ADD;
    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

PSInput NormalVS(NormalVSInput input)
{
    PSInput output;
    
    output.vecWorldPosition = mul(float4(input.vecPosition, 1), matWorld).xyz;
    output.vecPosition = mul(float4(output.vecWorldPosition, 1), matView);
    output.vecPosition = mul(output.vecPosition, matProj);

    output.vecUV = input.vecUV;
    output.vecNormal = mul(input.vecNormal, (float3x3)transpose(matInvWorld));

    return output;
}

PSInput SkinnedVS(SkinnedVSInput input)
{
    PSInput output = (PSInput)0;

    float4 p = float4(input.vecPosition, 1.0f);
    float3 n = input.vecNormal;

    float4 vecPosition = 0;
    float3 vecNormal = 0;
    for (int i = 0; i < 4; ++i)
    {
        uint boneIndex = input.boneIndex[i];
        float weight = input.weight[i];
        matrix m = boneTransform[boneIndex];
        vecPosition += weight * mul(p, m);
        vecNormal += weight * mul(n, (float3x3)m);
    }

    output.vecPosition = mul(vecPosition, matWVP);
    output.vecNormal = mul(vecNormal, (float3x3)(transpose(matInvWorld)));
    output.vecWorldPosition = mul(float4(input.vecPosition, 1), matWorld).xyz;
    output.vecUV = input.vecUV;

    return output;
}

float4 calcPhongLighting(float4 vecLightColor, float3 vecNormal, float3 vecLightDir, float3 vecViewDir, float3 vecReflect)
{
    float4 vecAmbient = float4(0.01, 0.01, 0.01, 1);
    float4 vecDiffuse = saturate(dot(vecNormal, vecLightDir));
    float4 vecSpecular = pow(saturate(dot(vecViewDir, vecReflect)), 256) * 0.4;

    return vecAmbient + (vecDiffuse + vecSpecular) * vecLightColor;
}

float4 PS(PSInput input) : SV_TARGET
{
    float3 vecLightDir = normalize(float3(0.25, -0.25, 0));

    input.vecNormal = normalize(input.vecNormal);
    float3 vecViewDir = normalize(vecViewPos - input.vecWorldPosition);
    float3 vecReflect = reflect(-vecLightDir, input.vecNormal);
    float4 vecTexColor = texDiffuse.Sample(SampleType, input.vecUV);
    
    return calcPhongLighting(float4(1, 1, 1, 1), input.vecNormal, vecLightDir, vecViewDir, vecReflect) * vecTexColor;

    //float gamma = 1.0 / 2.2;
    //float4 c = texDiffuse.Sample(SampleType, input.vecUV) * result;
    //return float4(pow(c.r, gamma), pow(c.g, gamma), pow(c.b, gamma), 1);
}

PSInput OutlineVS(OutlineVSInput input)
{
    PSInput output;

    //float3 vecNormal = mul(input.vecNormal, transpose((float3x3)matInvView));
    //vecNormal = mul(vecNormal, (float3x3)matProj);

    output.vecPosition = mul(float4(input.vecPosition, 1), matWVP);
    output.vecNormal = mul(input.vecNormal, (float3x3)(transpose(matInvWorld)));
    //output.vecPosition.xy += 1 * output.vecPosition.z * vecNormal.xy;

    return output;
}

float4 OutlinePS(PSInput input): SV_TARGET
{
    float3 cameraPos = normalize(vecViewPos - input.vecPosition);
    float rimLightColor = lerp(1.0f - 0.2f, 1.0f, 1 - max(0, dot(input.vecNormal, cameraPos)));

    return float4(1, 1, 1, 1) * 1;
}

technique11 Normal
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, NormalVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        SetBlendState(SrcAlphaBlendingAdd, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(EnableBackCulling);
    }
}

technique11 Skinned
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, SkinnedVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        SetRasterizerState(EnableBackCulling);
    }
}

technique11 Outline
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, OutlineVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, OutlinePS()));
        SetRasterizerState(EnableBackCulling);
    }
}


cbuffer CBuffer0
{
    matrix WVP;
};

struct VIn
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float3 uvw : TEXCOORD;
};

VOut main(VIn vin)
{
    VOut vout;
    
    vout.position = mul(WVP, vin.position);
    vout.uvw = vin.position.xyz;
    vout.color = vin.color;
    
    return vout;
}

cbuffer CBuffer0 : register(b0)
{
    matrix WVP;
    float4 cameraPos;
    float4 world_position;  
    matrix world_matrix;
    matrix inverse_world_matrix;
};

struct VIn
{
	float3 position : POSITION;
	float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct VOut
{
	float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 pos : TEXCOORD1;    
    float3 normal : TEXCOORD2;
    float3 world_pos : TEXCOORD3;
    float3 world_normal : TEXCOORD4;
    float3 cam_pos : TEXCOORD5;
};

VOut main(VIn vin)
{
	VOut vout;
    vout.position = mul(WVP, float4(vin.position, 1));
    vout.uv = vin.uv;
    
    vout.pos = world_position.xyz;
    vout.normal = float4(vin.norm, 1);
    vout.cam_pos = cameraPos.xyz;

    vout.world_pos = mul(world_matrix, float4(vin.position, 1));
    vout.world_normal = mul(inverse_world_matrix, float4(vin.norm, 1)).xyz;

	return vout;
}
#define MAX_LIGHTS 12

#define DIRECTIONAL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3

struct Material
{    
	//16 bytes
    float TextureID; // 4 bytes
    float3 Ambient; // 12 bytes
	
	// 16 bytes
    float3 Diffuse; // 12 bytes
    float padding1; // 4 bytes

	// 16 bytes
    float3 Specular; // 12 bytes
    float SpecularExponent; // 4 bytes
    
	// 16 bytes
    float3 Emissive; // 12 bytes
    float padding2; // 4 bytes
};

// 48 bytes
struct Light
{
    // 16 bytes
    int Type; // 4 bytes
    bool Enabled; // 4 bytes
    float Strength; // 4 bytes
    float SpotAngle; // 4 bytes
        
    // 16 bytes
    float4 Position; // 16 bytes
    
    // 16 bytes
    float4 Direction; // 16 bytes
    
    // 16 bytes
    float4 Colour; // 16 bytes
}; 

cbuffer PixelCBuffer : register(b0)
{
    Light AmbientLight; // 48 bytes
    Light Lights[MAX_LIGHTS]; // 48 * 12 = 576 bytes
    
    Material CurrentMaterial; // 64 bytes
};

Texture2D texture0; // diffuse
sampler sampler0;

struct LightInfo
{
    float4 Diffuse;
    float4 Specular;
};

float4 CalculateDiffuse(Light light, float3 L, float3 N)
{
    float diffuse = max(0, dot(L, N));
    return light.Colour * diffuse;
}

float4 CalculateSpecular(Light light, float3 L, float3 N, float3 V)
{
    float3 R = normalize(reflect(-L, N));
    float specular = pow(max(0, dot(R, V)), CurrentMaterial.SpecularExponent);
    return light.Colour * specular;
}

float CalculateAttenuation(Light light, float distance)
{
    return light.Strength / (light.Strength + distance * distance);
}

float CalculateSpotAngle(Light light, float3 L)
{
    // find current angle 
    float minCos = cos(light.SpotAngle);
    float maxCos = (minCos + 1.0f) / 2.0f;    
    float angle = dot(light.Direction.xyz, -L);
    
    return smoothstep(minCos, maxCos, angle);
}

LightInfo DirectionalLight(Light light, float3 N, float3 Ep, float3 Wp)
{    
    float3 L = light.Direction.xyz;
    float3 V = normalize(Ep - Wp);
    
    LightInfo lightInfo;    
    lightInfo.Diffuse = CalculateDiffuse(light, L, N);
    lightInfo.Specular = CalculateSpecular(light, L, N, V); //NEED DIRECTIONAL LIGHTING FIX
    return lightInfo;
}

LightInfo PointLight(Light light, float3 N, float3 Ep, float3 Wp)
{
    float3 L = (light.Position.xyz - Wp);
    float distance = length(L);
    L = L / distance;
    
    float3 V = normalize(Ep - Wp);
    
    float A = CalculateAttenuation(light, distance);
    
    LightInfo lightInfo;
        
    lightInfo.Diffuse = CalculateDiffuse(light, L, N) * A;
    lightInfo.Specular = CalculateSpecular(light, L, N, V) * A;
    
    return lightInfo;
}

LightInfo SpotLight(Light light, float3 N, float3 Ep, float3 Wp)
{
    float3 L = (light.Position.xyz - Wp);
    float distance = length(L);
    L = L / distance;
    float3 V = normalize(Ep - Wp);
    
    float Angle = CalculateSpotAngle(light, L);
    float A = CalculateAttenuation(light, distance);
        
    LightInfo lightInfo;
        
    lightInfo.Diffuse = CalculateDiffuse(light, L, N) * A * Angle;
    lightInfo.Specular = CalculateSpecular(light, L, N, V) * A * Angle;
    
    return lightInfo;
}

float4 main(float4 position : SV_Position, float4 color : COLOR, float2 uv : TEXCOORD,
            float3 pos : TEXCOORD1, float3 normal : TEXCOORD2, float3 world_pos : TEXCOORD3,
            float3 world_normal : TEXCOORD4, float3 cam_pos : TEXCOORD5) : SV_TARGET
{
    LightInfo totalInfo;
    totalInfo.Diffuse = float4(0, 0, 0, 0);
    totalInfo.Specular = float4(0, 0, 0, 0);
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (!Lights[i].Enabled)
            continue;        
        LightInfo info;
        info.Diffuse = float4(0, 0, 0, 0);
        info.Specular = float4(0, 0, 0, 0);

        switch ((int)Lights[i].Type)
        {
            case DIRECTIONAL_LIGHT:
                info = DirectionalLight(Lights[i], normal, cam_pos, world_pos);
                break;
            case POINT_LIGHT:
                info = PointLight(Lights[i], world_normal, cam_pos, world_pos);
                break;
            case SPOT_LIGHT:
                info = SpotLight(Lights[i], world_normal, cam_pos, world_pos);
                break;
        }
        
        totalInfo.Diffuse += info.Diffuse;
        totalInfo.Specular += info.Specular;
    }

    totalInfo.Diffuse = saturate(totalInfo.Diffuse);
    totalInfo.Specular = saturate(totalInfo.Specular);
           
    
    float3 e = CurrentMaterial.Emissive;
    float3 a = CurrentMaterial.Ambient * AmbientLight.Colour.xyz;
    float3 d = CurrentMaterial.Diffuse * totalInfo.Diffuse.xyz;
    float3 s = CurrentMaterial.Specular * totalInfo.Specular.xyz;
    
    float4 finalColor = float4(a + d + s + e, 1);
        
    if ((int)CurrentMaterial.TextureID == 1)
    {    
        float4 sampled = texture0.Sample(sampler0, uv);
        finalColor = finalColor * sampled;
    }
    
    return finalColor;
}
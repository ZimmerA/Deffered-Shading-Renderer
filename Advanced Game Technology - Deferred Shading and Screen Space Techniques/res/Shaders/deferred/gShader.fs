#version 330 core
layout (location = 0) out vec3 gPosition; // x,y,z = Position
layout (location = 1) out vec4 gNormalOccl; // x,y,z = Normal w = Occlusion
layout (location = 2) out vec4 gAlbedoMetal; // rgb = Albedo, a = Metallness
layout (location = 3) out vec4 gEmissiveRoughness; // rgb = emmisive, a = Roughness

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

struct Material {
    vec3 baseColorFactor;
    bool hasAlbedoMap;

    float metallicFactor;
    float roughnessFactor;
    bool hasMetalRoughnessMap;

    bool hasNormalMap;

    vec3 emissiveFactor;
    bool hasEmissiveMap;

    bool hasOcclusionMap;
};

uniform sampler2D uAlbedoMap;            // 0
uniform sampler2D uMetalRoughnessMap;    // 1
uniform sampler2D uNormalMap;            // 2
uniform sampler2D uEmmisiveMap;          // 3
uniform sampler2D uOcclusionMap;         // 4

uniform Material material;

void main()
{
    gPosition = FragPos;

    if(material.hasAlbedoMap)
    {
        gAlbedoMetal.rgb =  texture(uAlbedoMap, TexCoords).rgb * material.baseColorFactor;
    } else 
    {
        gAlbedoMetal.rgb = material.baseColorFactor;
    }

    if(material.hasMetalRoughnessMap)
    {
        gAlbedoMetal.a = texture(uMetalRoughnessMap, TexCoords).b * material.metallicFactor;
        gEmissiveRoughness.a = texture(uMetalRoughnessMap, TexCoords).g * material.roughnessFactor;
    } else {
        gAlbedoMetal.a = material.metallicFactor;
        gEmissiveRoughness.a = material.roughnessFactor;
    }

    if(material.hasEmissiveMap)
    {
        gEmissiveRoughness.rgb = texture(uEmmisiveMap, TexCoords).rgb * material.emissiveFactor;
    } else
    {
        gEmissiveRoughness.rgb = material.emissiveFactor;
    }
    
    if(material.hasNormalMap) {
        vec3 normal = texture(uNormalMap, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        gNormalOccl.rgb = normalize(TBN*normal);
    } else {
        gNormalOccl.rgb = normalize(Normal);
    }

    if(material.hasOcclusionMap)
    {
        gNormalOccl.a = texture(uOcclusionMap, TexCoords).r;
    } else 
    {
        gNormalOccl.a = 1.0;
    }
}
#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D gPos;                 // 0
uniform sampler2D gNormalOccl;          // 1
uniform sampler2D gAlbedoMetal;         // 2
uniform sampler2D gEmissiveRoughness;   // 3
uniform sampler2D uSSAOMap;             // 4
uniform sampler2DShadow uShadowMap;     // 5

uniform mat4 uLightViewProjMat;
uniform mat4 uViewMat;

uniform bool uSSAOOn;
uniform bool uDebugSSAO;
uniform bool uShadowOn;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uExposure;

const float PI = 3.14159265359;

float interleavedGradientNoise(vec2 v)
{
    vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(magic.z * dot(v, magic.xy));
}

vec2 vogelDiskSample(int sampleIndex, int samplesCount, float phi)
{
    const float goldenAngle = 2.4;
    
    float r = sqrt(sampleIndex + 0.5) / sqrt(samplesCount);
    float theta = sampleIndex * goldenAngle + phi;
    
    return r * vec2(cos(theta), sin(theta));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias = 0.005;    
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float shadow = 0.0;
    float noise = interleavedGradientNoise(gl_FragCoord.xy);

    for (int i = 0; i < 16; ++i)
    {
        vec2 sampleOffset = vogelDiskSample(i, 16, noise);
        shadow += texture(uShadowMap, vec3(projCoords.xy + sampleOffset * texelSize * 5.5, currentDepth-bias)) * (1.0 / 16.0);
    }

    return (1.0 - shadow);
}  


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Sample gBuffer
    vec3 fragmentViewPos = texture(gPos, UV).rgb;
    vec4 albedoMetal = texture(gAlbedoMetal,UV);
    vec3 albedo = pow(albedoMetal.rgb, vec3(2.2));
    float metallic = albedoMetal.a;
    vec4 normalOccl = texture(gNormalOccl, UV);
    vec3 N = normalize(normalOccl.rgb);
    float occlusion = normalOccl.a;

    // Cook-Torrance lighting (base code taken from learnopengl.com)
    // We're in view space so campos is 0
    vec3 V = normalize(-fragmentViewPos);
    vec4 emissiveRoughness = texture(gEmissiveRoughness, UV);
    float roughness = emissiveRoughness.a;
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 Lo = vec3(0.0);
    vec3 L = normalize(-uLightDirection);
    vec3 H = normalize(V + L);
    vec3 radiance = uLightColor;
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);   
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    float NdotL = max(dot(N, L), 0.0);        
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    // SSAO
    float ssao = 1.0;
    if(uSSAOOn)
    {
       ssao = texture(uSSAOMap, UV).r;
    }

    // Directional light shadow
    float shadow = 1.0;
    if(uShadowOn)
    {
        vec4 fragPosLightSpace = uLightViewProjMat * inverse(uViewMat) * vec4(fragmentViewPos, 1.0);
        shadow = ShadowCalculation(fragPosLightSpace);
    }

    vec3 ambient = vec3(0.03) * albedo * occlusion * ssao;
    vec3 color = (ambient + Lo * shadow);

    // Tone mapping
    color = vec3(1.0) - exp(-color * uExposure);

    // Gamma correction
    color = pow(color, vec3(1.0/2.2)); 
    if(uDebugSSAO) {
        FragColor = vec4(vec3(ssao), 1.0);
    } else {
        FragColor = vec4(color.xyz, 1.0);
    }

}
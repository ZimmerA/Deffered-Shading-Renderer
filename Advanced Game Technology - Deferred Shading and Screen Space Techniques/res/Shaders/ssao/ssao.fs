#version 330 core
out float FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormalOccl;
uniform sampler2D uNoiseSampler;

const int KERNEL_SIZE = 16;
uniform vec3 uFilterKernel[KERNEL_SIZE];
uniform vec2 uNoiseScale; 
uniform float uRadius;
uniform float uBias;
uniform mat4 uProjection;

void main()
{
    vec3 fragPos = texture(gPosition, UV).xyz;
    vec3 normal = normalize(texture(gNormalOccl, UV).rgb);
    vec3 noiseVec = normalize(texture(uNoiseSampler, UV * uNoiseScale).xyz);

    vec3 tangent = normalize(noiseVec - normal * dot(noiseVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for(int i = 0; i < KERNEL_SIZE; ++i)
    {
        vec3 sample = TBN * uFilterKernel[i];
        sample = fragPos + sample * uRadius; 

        vec4 offset = vec4(sample, 1.0);
        offset = uProjection * offset; 
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = texture(gPosition, offset.xy).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, uRadius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + uBias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    
    FragColor = occlusion;
}
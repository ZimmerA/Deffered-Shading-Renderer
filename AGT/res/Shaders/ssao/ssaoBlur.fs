#version 330 core
out float FragColor;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D uSSAOMap;

const float BLUR_KERNEL_RADIUS = 8.0;
uniform bool uHorizontal;
uniform int uFilterMode;

float crossBilateralWeight(float r, float d, float d0)
{
    const float BlurSigma = ( BLUR_KERNEL_RADIUS + 1.0) * 0.5;
    const float BlurFalloff = 1.0 / (2.0*BlurSigma*BlurSigma);

    float dz = d0 - d;
    return exp2(-r*r*BlurFalloff - dz*dz);
}

void main() {
    if(uFilterMode == 1)
    {
        // Gauss Blur (has to be done single pass)
        vec2 texelSize = 1.0 / vec2(textureSize(uSSAOMap, 0));
        float result = 0.0;
        for (int x = -2; x < 2; ++x) 
        {
            for (int y = -2; y < 2; ++y) 
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                result += texture(uSSAOMap, UV + offset).r;
            }
        }
        FragColor = result / (4.0 * 4.0);
    } else if(uFilterMode == 2)
    {
        // Edge preserving Cross Bilateral Filter (multi pass, uHorizontal/vertical axis)
        // Taken from Stable SSAO in Battlefield 3 with Selective Temporal Filtering
        vec2 texelSize = 1.0 / vec2(textureSize(uSSAOMap, 0));
        float result = 0.0;
        float weightSum = 0.0;
        float centerDepth = texture(gPosition, UV).z;

        float i = 0.0;

        // Blur first half of samples with point sampling
        for (; i < BLUR_KERNEL_RADIUS / 2.0; i++) 
        {
            vec2 offset = vec2(0,0);
            if(uHorizontal)
            {
                offset = vec2(float(i), 0.0) * texelSize;
            } else 
            {
                offset = vec2(0.0, float(i)) * texelSize;
            }

            float sampleDepth = texture(gPosition, UV - offset).z;
            float weight = crossBilateralWeight(i, sampleDepth, centerDepth);
            result += texture(uSSAOMap, UV - offset).r * weight;
            weightSum += weight;

            sampleDepth = texture(gPosition, UV + offset).z;
            weight = crossBilateralWeight(i, sampleDepth, centerDepth);
            result += texture(uSSAOMap, UV + offset).r * weight;
            weightSum += weight;
        }

        // Blur second half of samples with linear sampling
        for (; i < BLUR_KERNEL_RADIUS; i += 2.0) 
        {
            vec2 offset = vec2(0,0);
            if(uHorizontal)
            {
                offset = vec2(float(i) + 0.5, 0.0) * texelSize;
            } else 
            {
                offset = vec2(0.0, float(i) + 0.5) * texelSize;
            }

            float sampleDepth = texture(gPosition, UV - offset).z;
            float weight = crossBilateralWeight(i, sampleDepth, centerDepth);
            result += texture(uSSAOMap, UV - offset).r * weight;
            weightSum += weight;

            sampleDepth = texture(gPosition, UV + offset).z;
            weight = crossBilateralWeight(i, sampleDepth, centerDepth);
            result += texture(uSSAOMap, UV + offset).r * weight;
            weightSum += weight;
        }

        FragColor = result / weightSum;
    } else {
        // No Filter
        FragColor = texture(uSSAOMap, UV).r;
    }
}
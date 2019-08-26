#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aTangent;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out mat3 TBN;

uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uModel;

void main()
{
    // View Space gTextures
    vec3 biTangent = cross(aNormal, aTangent.xyz) * aTangent.w;
    vec3 T = normalize(vec3(uMV * vec4(aTangent.xyz, 0.0)));
    vec3 B = normalize(vec3(uMV * vec4(biTangent.xyz, 0.0)));
    Normal = normalize(vec3(uMV * vec4(aNormal, 0.0)));
    TBN = mat3(T, B, Normal);
    TexCoords = aUV;
    FragPos = vec3(uMV * vec4(aPos,1.0)).xyz;

    gl_Position = uMVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
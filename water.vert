#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout (location = 0) out b_VS_out
{
    vec3 worldPos;
    vec3 localPos;
    vec3 worldNorm;
    vec4 color;
    vec2 TexCoord;
} VS_out;

void main()
{
    vec4 worldPos = model * vec4(inPos, 1.0f);

    VS_out.worldPos = vec3(worldPos);
    VS_out.localPos = inPos;

    //VS_out.worldNorm = mat3(transpose(inverse(model))) * inNorm;
    VS_out.worldNorm = vec3(model * vec4(inNorm, 0));

    gl_Position = proj * view * worldPos;
    VS_out.TexCoord = aTexCoord;
    VS_out.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}
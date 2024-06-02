#version 460 core

struct Light
{
    vec3 position;
    vec3 diffuseColor;
    vec3 specularColor;
};

struct Material
{
    vec3 ka, kd, ks;
    float shininess;
};

uniform vec3 cameraPos;
uniform vec3 ambientColor;
uniform vec3 objectColor;
uniform Light light;
uniform Material material;
uniform sampler2D ourTexture;

layout (location = 0) in b_FS_in
{
    vec3 fragWorldPos;
    vec3 fragWorldNorm;
    vec4 color;
    vec2 TexCoord;
    vec3 tangent;
} FS_in;

out vec4 FragColor;

vec4 ComputePhongIllumination();

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//                 MAIN
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
void main()
{
    FragColor = ComputePhongIllumination();
} 
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*

vec4 ComputePhongIllumination()
{
    vec3 N = normalize(FS_in.fragWorldNorm);

    // ambient 
    vec3 ambient = material.ka * ambientColor;

    // diffuse 
    vec3 L = normalize(light.position - FS_in.fragWorldPos);
    vec3 diffuse = material.kd * light.diffuseColor * max(dot(N, L), 0.0f);

    // specular
    vec3 R = reflect(-L, N);
    vec3 V = normalize(cameraPos - FS_in.fragWorldPos);
    vec3 H = normalize(L + V);
    vec3 T = FS_in.tangent;
    //vec3 T = normalize(cross(N, vec3(1,0,0)));
    vec3 anisotropic = material.ks * light.specularColor * pow(sqrt(1-pow(dot(T, H),2)), material.shininess);

    //return vec4(1.0,0.5,1.0f,1.0f);
    vec4 c = texture(ourTexture, FS_in.TexCoord);
    return vec4((ambient + anisotropic) * c.xyz, 1.0f);
}
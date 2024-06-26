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
uniform samplerCube skybox;

layout (location = 0) in b_FS_in
{
    vec3 fragWorldPos;
    vec3 fragWorldNorm;
    vec4 color;
} FS_in;

out vec4 FragColor;

vec4 ComputePhongIllumination();

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//                 MAIN
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
void main()
{
    FragColor = texture(skybox, FS_in.fragWorldPos);
    //FragColor = ComputePhongIllumination();
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
    vec3 specular = material.ks * light.specularColor * pow(max(dot(R, V), 0.0f), material.shininess);

    return vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
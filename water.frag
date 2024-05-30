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
} FS_in;

out vec4 FragColor;

vec4 ComputePhongIllumination(vec3 N);
vec3 normalMapping(vec3 N, vec3 T, vec3 tn);

void main()
{
    vec3 N = normalize(FS_in.fragWorldNorm);
    vec3 dPdx = dFdx(FS_in.fragWorldPos);
    vec3 dPdy = dFdy(FS_in.fragWorldPos);
    vec2 dtdx = dFdx(FS_in.TexCoord);
    vec2 dtdy = dFdy(FS_in.TexCoord);
    vec3 T = normalize(-dPdx * dtdy.y + dPdy * dtdx.y);
    vec3 toCamera = normalize(cameraPos - FS_in.fragWorldPos);
    vec4 t_n = texture(ourTexture, FS_in.TexCoord)* 2.0 - 1.0;
    vec3 new_normal = normalMapping(N, T, t_n.xyz);
    FragColor = ComputePhongIllumination(new_normal);
} 

vec3 normalMapping(vec3 N, vec3 T, vec3 tn)
{
    N = normalize(N);
    T = normalize(T);

    vec3 B = normalize(cross(N, T));

    T = cross(B, N);

    mat3 TBN = mat3(T, B, N);
    vec3 world_normal = TBN * tn; // Matrix-vector multiplication
    return normalize(world_normal);
}

vec4 ComputePhongIllumination(vec3 N)
{
    //vec3 N = normalize(FS_in.fragWorldNorm);
    vec3 dPdx = dFdx(FS_in.fragWorldPos);
    vec3 dPdy = dFdy(FS_in.fragWorldPos);
    vec2 dtdx = dFdx(FS_in.TexCoord);
    vec2 dtdy = dFdy(FS_in.TexCoord);
    vec3 T = normalize(-dPdx * dtdy.y + dPdy * dtdx.y);
    vec3 toCamera = normalize(cameraPos - FS_in.fragWorldPos);
    vec4 t_n = texture(ourTexture, FS_in.TexCoord)* 2.0 - 1.0;
    vec3 new_normal = normalMapping(N, T, t_n.xyz);
    // ambient 
    vec3 ambient = material.ka * ambientColor;

    // diffuse 
    vec3 L = normalize(light.position - FS_in.fragWorldPos);
    vec3 diffuse = material.kd * light.diffuseColor * max(dot(N, L), 0.0f);

    // specular
    vec3 R = reflect(-L, N);
    vec3 V = normalize(cameraPos - FS_in.fragWorldPos);
    vec3 specular = material.ks * light.specularColor * pow(max(dot(R, V), 0.0f), material.shininess);

    //return vec4(1.0,0.5,1.0f,1.0f);
    //vec4 c = texture(ourTexture, FS_in.TexCoord);
    //return vec4(c.xyz, 1.0f);
    return vec4((ambient + diffuse + specular) * objectColor, 1.0f);
}
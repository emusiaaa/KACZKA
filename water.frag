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
uniform samplerCube skybox;

layout (location = 0) in b_FS_in
{
    vec3 fragWorldPos;
    vec3 fragLocalPos;
    vec3 fragWorldNorm;
    vec4 color;
    vec2 TexCoord;
} FS_in;

out vec4 FragColor;

vec4 ComputePhongIllumination(vec3 N, vec3 color);
vec3 normalMapping(vec3 N, vec3 T, vec3 tn);
vec3 intersectRay(vec3 p, vec3 d);
float fresnel(vec3 N, vec3 V, float n1, float n2);

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

    float n1 = 1.f, n2 = 4.f / 3.f;
    bool isUnderWater = dot(new_normal, toCamera) <= 0;
    float n = isUnderWater ? n2 / n1 : n1 / n2;
    if (isUnderWater) new_normal *= -1.f;
        
    vec3 odbity = reflect(-toCamera, new_normal);
    vec3 zalamany = refract(-toCamera, new_normal, n);
    
    float F = fresnel(new_normal, toCamera, n1, n2);

    vec3 color1 = texture(skybox, intersectRay(FS_in.fragLocalPos, odbity)).xyz;
    vec3 color2 = texture(skybox, intersectRay(FS_in.fragLocalPos, zalamany)).xyz;

    vec3 color = (zalamany.x != 0.0f || zalamany.y != 0.0f || zalamany.z != 0.0f) ? color2 + F * color1 : color1;
    
    FragColor = vec4((color.xyz), 1.0f);
    //FragColor = vec4(normalize(intersectRay(FS_in.fragLocalPos, odbity).xyz), 1.0f);
    //FragColor = ComputePhongIllumination(new_normal);
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

vec4 ComputePhongIllumination(vec3 N, vec3 color)
{
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

vec3 intersectRay(vec3 p, vec3 d)
{
    vec3 one = vec3(1.0, 1.0, 1.0);
    vec3 t1 = (one - p) / d;
    vec3 t2 = (-one - p) / d;
    
    vec3 tm = max(t1, t2);
    float t = min(min(tm.x, tm.y), tm.z);

    return p + t * d;
}

float fresnel(vec3 N, vec3 V, float n1, float n2)
{
    float F0 = pow((n2 - n1) / (n2 + n1), 2.0f);
    float cosTH = max(dot(N, V), 0.0f);
    return F0 + (1.0 - F0) * pow(1.0 - cosTH, 5.0f);
}
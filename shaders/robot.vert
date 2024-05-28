#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 norm;
  
out vec4 vertexColor;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gl_Position = proj * view * vec4(aPos, 1.0);
    vertexColor = vec4(0.5f, 0.5f, 0.f, 1.0f);
}
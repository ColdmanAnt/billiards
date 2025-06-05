#version 330 core
layout(location = 0) in vec3  aPos;   // видимые координаты (м)
layout(location = 1) in vec3  aNorm;  // нормаль

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vNormal;
out vec3 vPos;

void main()
{
    vec4 world = uModel * vec4(aPos, 1.0);
    vPos    = world.xyz;
    vNormal = mat3(uModel) * aNorm;
    gl_Position = uProj * uView * world;
}

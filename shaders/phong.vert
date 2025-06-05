#version 330 core

// Минимальный проходной вершинный шейдер.
// Просто принимает позицию и передаёт её дальше.
layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    // Задаём gl_Position напрямую
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
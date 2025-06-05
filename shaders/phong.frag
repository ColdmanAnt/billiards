#version 330 core
in vec3 vNormal;
in vec3 vPos;

uniform vec3 uLight = vec3( 0.0,  5.0, 5.0);
uniform vec3 uView  = vec3( 0.0,  5.0, 8.0);
uniform vec3 uColor = vec3(0.9, 0.9, 0.9);

out vec4 FragColor;

void main()
{
    vec3 N  = normalize(vNormal);
    vec3 L  = normalize(uLight - vPos);
    vec3 V  = normalize(uView  - vPos);
    vec3 R  = reflect(-L, N);

    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 32.0);

    vec3 final = 0.05 * uColor          // ambient
    + 0.85 * diff * uColor   // diffuse
    + 0.10 * spec;           // specular
    FragColor = vec4(final, 1.0);
}

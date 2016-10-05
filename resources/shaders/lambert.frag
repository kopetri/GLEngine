#version 330 core

#define NR_POINT_LIGHTS 1

out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

vec3 objectColor = vec3(0.2f, 0.8f, 0.2f);
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);


void main()
{
    vec3 N = normalize(Normal);
    vec3 L0 = normalize(vec3(2.0f, -0.5f, 1.0f) - WorldPos);

    float lambert = max(dot(N, L0), 0.0f);
    vec3 diffuse = objectColor.rgb * lambert * lightColor;

    diffuse += vec3(0.005);
    diffuse = pow(diffuse, vec3(1.0/2.2));

    FragColor = vec4(diffuse, 1.0);
}

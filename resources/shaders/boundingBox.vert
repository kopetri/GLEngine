#version 400 core

in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 viewPos;

void main()
{
    vec4 tmp = projection * view * model * vec4(pos, 1.0);
    viewPos = tmp.xyz;
    gl_Position = tmp;
}

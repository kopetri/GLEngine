#version 400 core

layout (location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    vec4 viewPos = projection * view * vec4(position, 1.0f);
    TexCoords = position;

    gl_Position = viewPos.xyww;
}

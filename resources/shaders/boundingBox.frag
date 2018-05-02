#version 400 core

layout (location = 4) out vec4 gBoundingBox;
void main()
{
    gBoundingBox = vec4(1.0f, 1.0f, 1.0f, 0.0f);
}

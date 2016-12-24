#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "shape.h"


class CubeMap
{
    public:
        GLfloat cameraAperture, cameraShutterSpeed, cameraISO;
        Texture texCubemap;
        Shape shapeCubemap;

        CubeMap();
        ~CubeMap();
        void setCubeMap(const char* texPath);
        void renderToShader(Shader& shaderCubemap, glm::mat4& projection, Camera& camera);
        void setExposure(GLfloat aperture, GLfloat shutterSpeed, GLfloat iso);
};


#endif

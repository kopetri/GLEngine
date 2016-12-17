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

#include "shader.h"
#include "camera.h"


class CubeMap
{
    public:
        GLuint cubemapTextureID, cubemapVAO, cubemapVBO;

        CubeMap();
        ~CubeMap();
        void setCubeMap(std::vector<const char*>& faces);
        void renderToShader(Shader& shaderCubemap, Shader& shaderLighting, glm::mat4& projection, Camera& camera);
};


#endif
